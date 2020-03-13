class ClanServerManager : PluginBase {
    private string baseModDir = ClanStatic.baseModDir;
    private string playerDirectory = ClanStatic.playerDirectory;
    private string clanDirectory = ClanStatic.clanDirectory;
    private string fileExtension = ClanStatic.fileExtension;
    private string languageFilter = ClanStatic.languageFilterName;
    private string languageFilterDir = ClanStatic.languageFilterDir;
    private ref array<ref ActiveClan> arrayActiveClans;

    void ClanServerManager() {
        CheckAndCreateDirectories();
        LoadConfig();
        LoadClans();
    }

    void CheckAndCreateDirectories() {
        if (!FileExist(baseModDir)) {
            MakeDirectory(baseModDir);
        }
        if (!FileExist(playerDirectory)) {
            MakeDirectory(playerDirectory);
        }
        if (!FileExist(clanDirectory)) {
            MakeDirectory(clanDirectory);
        }
        if (!FileExist(languageFilterDir)) {
            array<string> arrayLanguageFilter = new array<string>();
            // [REWRITE] Make a default class for this

            arrayLanguageFilter.Insert("Add bad words in here!");
            arrayLanguageFilter.Insert("Make sure there's a comma at the end of each word");
            arrayLanguageFilter.Insert("Limit to single words and words with more than 3 letters!");
            arrayLanguageFilter.Insert("And no comma at the end!");

            JsonFileLoader<array<string>>.JsonSaveFile(languageFilterDir, arrayLanguageFilter);
        }
    }

    private void LoadConfig() {
        ref ClanConfig config;

        if (FileExist(ClanStatic.serverConfigDir)) {
            JsonFileLoader<ClanConfig>.JsonLoadFile(ClanStatic.serverConfigDir, config);
        } else {
            config = new ClanConfig();
        }
        config.Verify();
        GetClanManager().SetConfig(config);
    }

    private void LoadClans() {
        string fileName, clanDir;
        FileAttr fileAttr;
        FindFileHandle clans = FindFile(clanDirectory + "*.clan", fileName, fileAttr, 0);
        array<string> arrayFileNames = new array<string>();

        delete arrayActiveClans;
        arrayActiveClans = new array<ref ActiveClan>();

        if (!clans) { return; }
        arrayFileNames.Insert(fileName);

        while (FindNextFile(clans, fileName, fileAttr)) {
            arrayFileNames.Insert(fileName);
        }
        CloseFindFile(clans);

        foreach (string name : arrayFileNames) {
            ActiveClan clan;
            clanDir = clanDirectory + name;

            if (FileExist(clanDir)) {
                JsonFileLoader<ActiveClan>.JsonLoadFile(clanDir, clan);
                if (clan.Verify()) {
                    GetClanManager().SortClan(clan);
                    arrayActiveClans.Insert(clan);
                } else {
                    DeleteClan(clan);
                }
            }
        }
        arrayActiveClans.Debug();
    }

    void SortClans() {
        auto emptyArray = new array<ref ClanBase>();
        GetClanManager().SetLeaderboard(emptyArray);
        foreach (ActiveClan activeClan : arrayActiveClans) {
            if (activeClan) {
                GetClanManager().SortClan(activeClan);
            }
        }
        auto leaderboard = new Param1<ref array<ref ClanBase>>(GetClanManager().GetClanLeaderboard());
        GetGame().RPCSingleParam(null, ClanRPCEnum.ClientUpdateLeaderboard, leaderboard, true);
    }

    /*
     * START Clan creation Management
     * 
     * Functions used to determine if clan creation is allowed
     * As well as handling actual creation of clans
     *
     */

    void CreateClan(PlayerBase player, string clanName) {
        PlayerIdentity playerIdentity = player.GetIdentity();
        ref ActiveClan newClan = new ActiveClan(clanName, playerIdentity.GetName(), playerIdentity.GetId(), playerIdentity.GetPlainId());

        newClan.CheckPlayerForActiveTracker(player);
        newClan.Save();
        arrayActiveClans.Insert(newClan);
        player.SetActiveClanId(newClan.GetClanId());
        SendClanRPC(player, newClan);
        UpdatePlayerDataFile(playerIdentity.GetPlainId(), newClan.GetClanId());
        SortClans();
    }
    
    void DeleteClan(ref ActiveClan clan) {
        array<ref ClanMember> clanMembers = clan.GetMembers();
        array<ref ClanMemberTracker> clanTrackers = clan.GetTrackers();

        foreach (ClanMember member : clanMembers) {
            if (member) {
                UpdatePlayerDataFile(member.GetPlayerPlainId(), string.Empty);
            }
        }
        foreach (ClanMemberTracker tracker : clanTrackers) {
            if (tracker && tracker.GetPlayer()) {
                GetGame().RPCSingleParam(tracker.GetPlayer(), ClanRPCEnum.ClientDeleteClan, null, true, tracker.GetIdentity());
            }
        }
        clan.Delete();
        arrayActiveClans.RemoveItem(clan);
        delete clan;
        SortClans();
    }
    
    bool CanCreateClan(PlayerBase player, string name, out int error) {
        Print(ClanStatic.debugPrefix + "Checking if clan can be created by name");
        if (name == string.Empty) {
            error = ClanCreateErrorEnum.INVALID_NAME;
            return false;
        }
        ref ActiveClan activeClan = GetClanByName(name);

        if (activeClan) {
            error = ClanCreateErrorEnum.CLAN_EXISTS;
            return false;
        }
        array<string> languageFilter;
        string playerDataDir = playerDirectory + player.GetIdentity().GetPlainId() + fileExtension;

        JsonFileLoader<array<string>>.JsonLoadFile(languageFilterDir, languageFilter);
        name.ToLower();

        if (languageFilter) {
            foreach (string badWord : languageFilter) {
                if (name.IndexOf(badWord) != -1) {
                    Print(ClanStatic.debugPrefix + "Clanname contains bad word! " + badWord);
                    error = ClanCreateErrorEnum.INVALID_NAME;
                    return false;
                }
            }
        }
        ClanPlayer clanPlayer = GetClanPlayerDataFile(player.GetIdentity().GetPlainId());

        if (clanPlayer) {
            string clanId = clanPlayer.GetClanId();

            if (clanId != string.Empty) {
                activeClan = GetClanById(clanPlayer.GetClanId());

                if (activeClan && activeClan.IsPlayerInClan(player.GetIdentity().GetId())) {
                    error = ClanCreateErrorEnum.IN_CLAN;
                    return false;
                } else {
                    clanPlayer.SetClanId(string.Empty);
                    SaveClanPlayer(clanPlayer);
                }
            }
            delete clanPlayer;
        }
        return true;
    }

    /*
     * END Clan creation Management
     * 
     */

    /*
     * START Functions to manage player data
     */

    void SendClanRPC(PlayerBase player, ActiveClan clan) {
        if (!player) { return; }
        auto params = new Param1<ActiveClan>(clan);

        GetGame().RPCSingleParam(player, ClanRPCEnum.ClientReceiveClan, params, true, player.GetIdentity());
    }

    private ClanPlayer GetClanPlayerDataFile(string playerPlainId) {
        string playerDataDir = playerDirectory + playerPlainId + ".json";

        Print(ClanStatic.debugPrefix + "Searching for clan player data file! " + playerDataDir);

        if (FileExist(playerDataDir)) {
            ClanPlayer player;
            Print(ClanStatic.debugPrefix + "File found! Loading!");
            JsonFileLoader<ClanPlayer>.JsonLoadFile(playerDataDir, player);
            return player;
        }
        return null;
    }

    void UpdatePlayerDataFile(string playerPlainId, string clanId) {
        ClanPlayer clanPlayer = GetClanPlayerDataFile(playerPlainId);

        if (!clanPlayer) {
            clanPlayer = new ClanPlayer(playerPlainId);
        }
        clanPlayer.SetClanId(clanId);
        SaveClanPlayer(clanPlayer);
        delete clanPlayer;
    }

    void AddPlayerContributions(string playerPlainId, int amount) {
        ClanPlayer clanPlayer = GetClanPlayerDataFile(playerPlainId);

        if (clanPlayer) {
            clanPlayer.AddContributions(amount);
            SaveClanPlayer(clanPlayer);
        }
        delete clanPlayer;
    }

    void SaveClanPlayer(ClanPlayer clanPlayer) {
        string dataDir = ClanStatic.playerDirectory + clanPlayer.GetPlainId() + ".json";
        
        JsonFileLoader<ClanPlayer>.JsonSaveFile(dataDir, clanPlayer);
    }

    /*
     * END Functions to manage player data
     */

    /*
     * START Clan Player Management Functions
     *
     * Functions to handle player management within a clan
     * These functions must be held here to ensure data is handled properly
     * Player data files must be manipulated as well as the clan itself
     * to ensure that the data is kept in-synch and so that a player will have
     * the correct data to rejoin with.
     *
     */

    void AddMemberToClan(ref ActiveClan clan, PlayerBase player) {
        UpdatePlayerDataFile(player.GetIdentity().GetPlainId(), clan.GetClanId());
        clan.AddMember(player.GetIdentity().GetName(), player.GetIdentity().GetId(), player.GetIdentity().GetPlainId());
        clan.CheckPlayerForActiveTracker(player);
        player.SetActiveClanId(clan.GetClanId());
        SendClanRPC(player, clan);
    }

    void RemoveMemberFromClan(ref ActiveClan clan, PlayerBase player) {
        RemovePlayerFromActiveClan(player);
        UpdatePlayerDataFile(player.GetIdentity().GetPlainId(), string.Empty);
        clan.RemoveMember(player.GetIdentity().GetId());
        player.SetActiveClanId(string.Empty);
    }

    void RemoveMemberFromClan(ref ActiveClan clan, string playerPlainId) {
        UpdatePlayerDataFile(playerPlainId, string.Empty);
        clan.RemoveMember(playerPlainId);
    }

    void RemovePlayerFromActiveClan(PlayerBase player) {
        ref ActiveClan clan = GetClanById(player.GetActiveClanId());

        if (clan) {
            clan.RemoveTracker(player.GetIdentity().GetId());
            UpdatePlayerDataFile(player.GetIdentity().GetPlainId(), clan.GetClanId());
        }
        if (player) {
            player.SetActiveClanId(string.Empty);
            GetGame().RPCSingleParam(player, ClanRPCEnum.ClientDeleteClan, null, true, player.GetIdentity());
        }
    }
    
    void InitializeClanPlayer(PlayerBase player) {
        string playerPlainId = player.GetIdentity().GetPlainId();
        ClanPlayer playerClanData = GetClanPlayerDataFile(playerPlainId);

        if (playerClanData) {
            string clanId = playerClanData.GetClanId();

            if (clanId != string.Empty) {
                ref ActiveClan activeClan = GetClanById(clanId);

                if (activeClan && activeClan.IsPlayerInClan(playerPlainId)) {
                    activeClan.UpdateMemberName(player.GetIdentity().GetId(), player.GetIdentity().GetName());
                    activeClan.CheckPlayerForActiveTracker(player);
                    player.SetActiveClanId(clanId);
                    SendClanRPC(player, activeClan);
                }
            }
        }
    }

    /*
     * END Clan Player Management Functions
     */

    ref ActiveClan GetClanById(string clanId) {
        foreach (ActiveClan clan : arrayActiveClans) {
            if (clan && clan.GetClanId() == clanId) {
                return clan;
            }
        }
        return null;
    }

    ref ActiveClan GetClanByName(string clanName) {
        clanName.ToLower();

        foreach (ActiveClan clan : arrayActiveClans) {
            if (clan && clan.GetCaseName() == clanName) {
                return clan;
            }
        }
        return null;
    }

    PlayerIdentity GetIdentityById(string playerId) {
        PlayerIdentity identity = NULL;
        array<Man> players = new array<Man>();
        GetGame().GetWorld().GetPlayerList(players);

        foreach (Man man : players) {
            if (man.GetIdentity()) {
                if (man.GetIdentity().GetId() == playerId || man.GetIdentity().GetPlainId() == playerId) {
                    identity = man.GetIdentity();
                    break;
                }
            }
        }
        return identity;
    }

    PlayerBase GetPlayerBaseById(string playerId) {
        PlayerBase player = null;

        if (playerId != string.Empty) {
            PlayerIdentity userIdentity = GetIdentityById(playerId);

            if (userIdentity) {
                int networkIdLowBits, networkIdHightBits;

                GetGame().GetPlayerNetworkIDByIdentityID(userIdentity.GetPlayerId(), networkIdLowBits, networkIdHightBits);
                player = PlayerBase.Cast(GetGame().GetObjectByNetworkId(networkIdLowBits, networkIdHightBits));
            }
        }
        return player;
    }

    /*
     * Functions to grab and remove player currency.
     */

    // [REWRITE]
    bool HasEnoughCurrency(PlayerBase player, int amount, out map<ItemBase, ref ClanConfigCurrency> mapCurrencyItems) {
        PlayerIdentity playerIdentity = player.GetIdentity();

        if (playerIdentity) {
            array<EntityAI> arrayInventoryItems;
            int totalAmount;

            arrayInventoryItems = new array<EntityAI>();
            mapCurrencyItems = new map<ItemBase, ref ClanConfigCurrency>();
            player.GetInventory().EnumerateInventory(InventoryTraversalType.PREORDER, arrayInventoryItems);

            foreach (EntityAI item : arrayInventoryItems) {
                ItemBase itemBase = ItemBase.Cast(item);

                if (itemBase) {
                    ref ClanConfigCurrency configCurrency;
                    string itemType;

                    itemType = itemBase.GetType();
                    configCurrency = GetClanManager().GetConfig().GetCurrencyByType(itemType);

                    if (configCurrency) {
                        int currencyValue = configCurrency.GetValue();

                        totalAmount += (QuantityConversions.GetItemQuantity(itemBase) * currencyValue);
                        mapCurrencyItems.Insert(itemBase, configCurrency);
                    }
                }
            }
            Print(ClanStatic.debugPrefix + "TOTAL CURRENCY ON PERSON:::" + totalAmount);
            if (totalAmount >= amount) {
                Print(ClanStatic.debugPrefix + "PLAYER HAS ENOUGH CURRENCY");
                return true;
            }
        }
        Print(ClanStatic.debugPrefix + "PLAYER DOES NOT HAVE ENOUGH CURRENCY");
        return false;
    }

    void DeductPlayerCurrency(PlayerBase player, int amountToDeduct, map<ItemBase, ref ClanConfigCurrency> mapCurrencyItems) {
        PlayerIdentity playerIdentity = player.GetIdentity();

        if (playerIdentity) {
            array<ItemBase> arrayCurrencyItems;
            array<ItemBase> arrayHigherValueCurrency;
            int amountDeducted;

            arrayCurrencyItems = mapCurrencyItems.GetKeyArray();
            arrayHigherValueCurrency = new array<ItemBase>();

            foreach (ItemBase currencyItem : arrayCurrencyItems) {
                if (amountDeducted >= amountToDeduct) { break; }
                if (currencyItem) {
                    ref ClanConfigCurrency configCurrency = mapCurrencyItems.Get(currencyItem);

                    if (configCurrency) {
                        Print(ClanStatic.debugPrefix + "Config Currency Found!");
                        int currencyValue = configCurrency.GetValue();
                        int itemQuantity = QuantityConversions.GetItemQuantity(currencyItem);
                        int currencyItemsToRemove = Math.Ceil((amountToDeduct - amountDeducted) / currencyValue);
                        Print(ClanStatic.debugPrefix + "Currency value=" + currencyValue + " || item quant=" + itemQuantity + " || items to remove=" + currencyItemsToRemove);

                        if (currencyItemsToRemove >= itemQuantity) {
                            Print(ClanStatic.debugPrefix + "More currency needs to be removed than available!");
                            amountDeducted += (currencyValue * itemQuantity);
                            GetGame().ObjectDelete(currencyItem);
                        } else {
                            Print(ClanStatic.debugPrefix + "More currency is available than needs to be deleted!");
                            int newItemQuantity = itemQuantity - currencyItemsToRemove;

                            amountDeducted += (currencyItemsToRemove * currencyValue);

                            Print(ClanStatic.debugPrefix + "SETTING NEW QUANTITY " + newItemQuantity + " || OLD QUANT " + itemQuantity);
                            SetItemQuantity(currencyItem, newItemQuantity);
                        }
                    }
                }
                Print(ClanStatic.debugPrefix + "TOTAL DEDUCTED=" + amountDeducted);
            }
            if (amountDeducted > amountToDeduct) {
                int amountToRefund = (amountDeducted - amountToDeduct);

                RefundCurrency(player, amountToRefund);

                Print(ClanStatic.debugPrefix + "More money was taken than necessary!!!! " + amountToRefund + " needs to be refunded!");
            }
        }
    }

    void RefundCurrency(PlayerBase player, int amountToRefund) {
        PlayerIdentity playerIdentity = player.GetIdentity();

        if (playerIdentity) {
            ref array<ref ClanConfigCurrency> arrayConfigCurrency;
            int amountRefunded;

            arrayConfigCurrency = GetClanManager().GetConfig().GetCurrency();

            foreach (ClanConfigCurrency configCurrency : arrayConfigCurrency) {
                if (amountRefunded >= amountToRefund) {
                    break;
                }
                if (configCurrency) {
                    Print(ClanStatic.debugPrefix + "FOUND CONFIG CURRENCY IN REFUND");

                    string currencyType = configCurrency.GetItemName();
                    int currencyValue = configCurrency.GetValue();
                    int itemsToCreate = Math.Floor((amountToRefund - amountRefunded) / currencyValue);

                    if (itemsToCreate > 0) {
                        Print(ClanStatic.debugPrefix + "Creating new item!!! " + currencyType);

                        ItemBase newItem = ItemBase.Cast(player.GetHumanInventory().CreateInInventory(currencyType));

                        if (newItem) {
                            Print(ClanStatic.debugPrefix + "New item created!!! " + newItem);
                            int maxQuant = GetItemMaxQuantity(newItem);

                            if (itemsToCreate > maxQuant) {
                                Print(ClanStatic.debugPrefix + "Items to create (" + itemsToCreate + ") is greater than max quant (" + maxQuant + ")");
                                int itemsLeftToCreate = (itemsToCreate - maxQuant);

                                Print(ClanStatic.debugPrefix + "items left to create=" + itemsLeftToCreate);

                                SetItemQuantity(newItem, maxQuant);

                                while (itemsLeftToCreate > 0) {
                                    newItem = ItemBase.Cast(player.GetHumanInventory().CreateInInventory(currencyType));

                                    if (!newItem) {
                                        newItem = ItemBase.Cast(GetGame().CreateObject(currencyType, player.GetPosition()));
                                    }
                                    if (itemsLeftToCreate >= maxQuant) {
                                        SetItemQuantity(newItem, maxQuant);
                                        itemsLeftToCreate -= maxQuant;
                                    } else {
                                        SetItemQuantity(newItem, itemsLeftToCreate);
                                        itemsLeftToCreate = 0;
                                    }
                                }
                            } else {
                                SetItemQuantity(newItem, itemsToCreate);
                            }
                            amountRefunded += (itemsToCreate * currencyValue);
                        }
                    }
                }
                Print(ClanStatic.debugPrefix + "TOTAL AMOUNT REFUNDED=" + amountRefunded);
            }
        }
    }

    void SetItemQuantity(ItemBase item, int quantity) {
        Magazine magItem = Magazine.Cast(item);

        if (magItem) {
            magItem.ServerSetAmmoCount(quantity);
        } else {
            item.SetQuantity(quantity);
        }
    }

    int GetItemMaxQuantity(ItemBase item) {
        Magazine magItem = Magazine.Cast(item);
        int maxQuant;

        if (magItem) {
            maxQuant = magItem.GetAmmoMax();
        } else {
            maxQuant = item.GetQuantityMax();
        }
        return maxQuant;
    }
}

ClanServerManager GetClanServerManager() {
    return ClanServerManager.Cast(GetPlugin(ClanServerManager));
}