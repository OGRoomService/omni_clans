class ClanServerManager : PluginBase {
    // I might be storing all clans in memory. Or, I might filter them on server start and remove unnecessary clans from memory
    // for leaderboard purposes.
    private string baseModDir = ClanStatic.baseModDir;
    private string playerDirectory = ClanStatic.playerDirectory;
    private string playerDataFile = ClanStatic.playerDataFile;
    private string clanDirectory = ClanStatic.clanDirectory;
    private string fileExtension = ClanStatic.fileExtension;
    private ref array<ref ActiveClan> activeClans;

    void ClanServerManager() {
        activeClans = new array<ref ActiveClan>();
        CheckAndCreateDirectories();
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
    }

    void CreateClan(PlayerBase player, string name) {
        string error;
        name.ToLower();

        if (!CanCreateClan(player, name, error)) { return; }

        string playerId = player.GetIdentity().GetPlainId();
        string playerDir = playerDirectory + "\\" + playerId;
        ActiveClan clan = new ActiveClan(playerId, player.GetIdentity().GetName(), name);
        ClanPlayer cPlayer = new ClanPlayer(name, playerId);

        Print("CREATING CLAN BY NAME {" + name + "} BY PLAYER {" + playerId + "}");

        MakeDirectory(playerDir);
        AddActiveClan(clan, player);
        clan.Save();
        cPlayer.Save();
    }

    void AddPlayerToActiveClan(PlayerBase player) {
        string playerId = player.GetIdentity().GetPlainId();
        string playerDir = playerDirectory + "\\" + playerId;
        string playerFileDir = playerDir + playerDataFile + fileExtension;
        ClanPlayer cPlayer = GetClanPlayer(playerId);

        if (cPlayer) {
            string clanName = cPlayer.GetClanName();

            if (clanName != string.Empty) {
                ActiveClan clan = GetClanByName(clanName);

                if (clan && clan.IsMember(playerId)) {
                    AddActiveClan(clan, player);
                }
            }
        }
    }

    void AddActiveClan(ref ActiveClan clan, PlayerBase player) {
        bool found = false;
        foreach (ActiveClan c : activeClans) {
            if (c.GetCaseName() == clan.GetCaseName()) {
                clan = c;
                found = true;
            }
        }
        if (!found) {
            clan.InitTicker();
            activeClans.Insert(clan);
        }
        clan.AddActivePlayer(player);
    }

    void AddPlayerInvitation(string clanName, string playerId) {

    }

    void RemoveFromActiveClan(PlayerBase player) { }

    private void RemoveActiveClan(ref Clan clan) {
        activeClans.RemoveItem(clan);
    }

    private ClanPlayer GetClanPlayer(string playerId) {
        string playerDataDir = playerDirectory + "\\" + playerId + playerDataFile + fileExtension;
        ClanPlayer player;

        if (FileExist(playerDataDir)) {
            JsonFileLoader<ClanPlayer>.JsonLoadFile(playerDataDir, player);
        }
        return player;
    }

    private ref ActiveClan GetClanByName(string name) {
        ActiveClan clan;
        string clanFileDir = clanDirectory + "\\" + name + fileExtension;

        if (FileExist(clanFileDir)) {
            JsonFileLoader<ActiveClan>.JsonLoadFile(clanFileDir, clan);
        }
        return clan;
    }

    bool CanCreateClan(PlayerBase player, string name, out string error) {
        string playerId = player.GetIdentity().GetPlainId();
        string playerDataDir = playerDirectory + "\\" + playerId + playerDataFile + fileExtension;

        name.ToLower();

        if (FileExist(clanDirectory + "\\" + name + fileExtension)) {
            error = "Clan already exists!";
            return false;
        }
        ClanPlayer cPlayer = GetClanPlayer(playerId);

        if (cPlayer) {
            string clanName = cPlayer.GetClanName();

            if (clanName != string.Empty) {
                Clan clan = GetClanByName(clanName);

                if (clan && clan.IsMember(playerId)) {
                    error = "You're already in a clan!";
                    return false;
                }
            }
        }
        return true;
    }

    ref array<ref ActiveClan> GetActiveClans() {
        return activeClans;
    }
    
    ref ActiveClan GetActiveClanByName(string name) {
        ref ActiveClan clan;
        name.ToLower();

        foreach (ActiveClan c : activeClans) {
            if (c.GetCaseName() == name) {
                clan = c;
                break;
            }
        }
        return clan;
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
        PlayerBase player = NULL;

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
}

ClanServerManager GetClanServerManager() {
    return ClanServerManager.Cast(GetPlugin(ClanServerManager));
}