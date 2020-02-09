class ClanServerManager : PluginBase {
    private string baseModDir = ClanStatic.baseModDir;
    private string playerDirectory = ClanStatic.playerDirectory;
    private string playerDataFile = ClanStatic.playerDataFile;
    private string clanDirectory = ClanStatic.clanDirectory;
    private string fileExtension = ClanStatic.fileExtension;
    private ref array<ref ActiveClan> arrayActiveClans;
    private ref map<PlayerIdentity, ref ActiveClan> mapPlayerActiveClan;

    void ClanServerManager() {
        arrayActiveClans = new array<ref ActiveClan>();
        mapPlayerActiveClan = new map<PlayerIdentity, ref ActiveClan>();
        CheckAndCreateDirectories();
        //CreateTestClans();
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
    }

    private void LoadConfig() {
        ref ClanConfig config;

        if (FileExist(baseModDir + "\\ClanConfig.json")) {
            JsonFileLoader<ClanConfig>.JsonLoadFile(baseModDir + "\\ClanConfig.json", config);
        } else {
            config = new ClanConfig();
        }
        config.Verify();
        GetClanManager().SetConfig(config);
    }

    private void LoadClans() {
        // FOR SORTING USE A RECURSIVE CALL THAT LOOPS OVER ARRAY AND COMPARES RANK. PLACE IT ACCORDINGLY
        string fileName, clanDir;
        FileAttr fileAttr;
        FindFileHandle clans = FindFile(clanDirectory + "\\*.clan", fileName, fileAttr, 0);
        ref array<string> arrayFileNames = new array<string>();

        if (!clans) { return; }
        arrayFileNames.Insert(fileName);

        while(FindNextFile(clans, fileName, fileAttr)) {
            arrayFileNames.Insert(fileName);
        }
        CloseFindFile(clans);
        
        foreach (string name : arrayFileNames) {
            Clan clan;
            clanDir = clanDirectory + "\\" + name;

            if (FileExist(clanDir)) {
                JsonFileLoader<Clan>.JsonLoadFile(clanDir, clan);
                if (clan.Verify()) {
                    GetClanManager().SortClan(ClanBase.Cast(clan));
                } else {
                    delete clan;
                    DeleteFile(clanDir);
                }
            }
        }
        GetClanManager().GetClanLeaderboard();
    }

    private void CreateTestClans() {
        for (int i = 0; i < 100; i++) {
            ActiveClan clan = new ActiveClan("yeet", "yeet", "" + Math.RandomInt(0, 999999999));
            clan.SetRank(i);
            clan.Save();
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

        MakeDirectory(playerDir);
        AddPlayerToActiveClan(clan, player);
        for (int i = 0; i < 10; i++) {
            int randoId = Math.RandomInt(111111111, 999999999);
            clan.AddMember("Member " + i, "" + randoId);
        }
        clan.Save();
        cPlayer.Save();
    }

    void CheckPlayerForClan(PlayerBase player) {
        string playerId = player.GetIdentity().GetPlainId();
        string playerDir = playerDirectory + "\\" + playerId;
        string playerFileDir = playerDir + playerDataFile + fileExtension;
        ClanPlayer cPlayer = GetClanPlayer(playerId);

        if (cPlayer) {
            string clanName = cPlayer.GetClanName();

            if (clanName != string.Empty) {
                ActiveClan clan = GetClanByName(clanName);

                if (clan && clan.IsMember(playerId)) {
                    AddPlayerToActiveClan(clan, player);
                }
            }
        }
    }
//AddPlayerToActiveClan
    void AddPlayerToActiveClan(ref ActiveClan clan, PlayerBase player) {
        bool found = false;
        foreach (ActiveClan c : arrayActiveClans) {
            if (c.GetCaseName() == clan.GetCaseName()) {
                clan = c;
                found = true;
                break;
            }
        }
        if (!found) {
            clan.Init();
            arrayActiveClans.Insert(clan);
        }
        PlayerBase testAI1 = PlayerBase.Cast(GetGame().CreateObject("SurvivorM_Mirek", "11993.6 140 12563.2"));
        PlayerBase testAI2 = PlayerBase.Cast(GetGame().CreateObject("SurvivorM_Boris", "11999.6 140 12566.2"));
        PlayerBase testAI3 = PlayerBase.Cast(GetGame().CreateObject("SurvivorM_Cyril", "11973.6 140 12569.2"));
        PlayerBase testAI4 = PlayerBase.Cast(GetGame().CreateObject("SurvivorM_Denis", "11983.6 140 12572.2"));
        PlayerBase testAI5 = PlayerBase.Cast(GetGame().CreateObject("SurvivorM_Niki", "12000.6 140 12576.2"));
        clan.AddTracker(player);
        clan.AddTracker(testAI1, "", "", testAI1.GetType());
        clan.AddTracker(testAI2, "", "", testAI2.GetType());
        clan.AddTracker(testAI3, "", "", testAI3.GetType());
        clan.AddTracker(testAI4, "", "", testAI4.GetType());
        clan.AddTracker(testAI5, "", "", testAI5.GetType());
        clan.Test();
        mapPlayerActiveClan.Set(player.GetIdentity(), clan);
        auto params = new Param1<ref ActiveClan>(clan);
        GetGame().RPCSingleParam(player, ClanRPCEnum.ClientReceiveClan, params, true, player.GetIdentity());
    }

    void RemovePlayerFromActiveClan(PlayerBase player) {
        ref ActiveClan clan;

        if (mapPlayerActiveClan.Find(player.GetIdentity(), clan)) {
            if (clan) {
                string playerId = player.GetIdentity().GetId()
                clan.RemoveTracker(playerId);

                if (clan.GetTrackers().Count() == 0) {
                    clan.Save();
                    arrayActiveClans.RemoveItem(clan);
                } else {
                    clan.RemoveTrackerRPC(playerId);
                }
            }
            mapPlayerActiveClan.Remove(player.GetIdentity());
        }
    }

    void RemoveAllPlayersFromActiveClan(ref array<ref ClanMemberTracker> clanTrackers) {
        foreach (ClanMemberTracker tracker : clanTrackers) {
            PlayerBase player = tracker.GetPlayer();

            if (player) {
                mapPlayerActiveClan.Remove(player.GetIdentity());
            }
        }
    }

    void RemoveActiveClan(ref ActiveClan clan) {
        arrayActiveClans.RemoveItem(clan);
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

    ref array<ref ActiveClan> GetActiveClans() {
        return arrayActiveClans;
    }

    ref ActiveClan GetActiveClanByPlayerIdentity(PlayerIdentity playerId) {
        ref ActiveClan activeClan = mapPlayerActiveClan.Get(playerId);

        return activeClan;
    }
    
    ref ActiveClan GetActiveClanByName(string name) {
        ref ActiveClan clan;
        name.ToLower();

        foreach (ActiveClan c : arrayActiveClans) {
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