class ClanManager : PluginBase {
    private ref array<ref ClanUser> arrayPlayerList;
    private ref array<ref ClanBase> arrayClanLeaderboard;
    private ref ClanConfig config;

    void ClanManager() {
        arrayPlayerList = new array<ref ClanUser>();
        arrayClanLeaderboard = new array<ref ClanBase>();
    }

    void AddOnlinePlayer(string playerName, string playerId, string playerPlainId) {
        ref ClanUser newUser = new ClanUser(playerName, playerId);
        arrayPlayerList.Insert(newUser);

        if (GetGame().IsServer() && GetGame().IsMultiplayer()) {
            auto params = new Param3<string, string, string>(playerName, playerId, playerPlainId);
            GetGame().RPCSingleParam(null, ClanRPCEnum.ClientAddOnlinePlayer, params, true);
        }
    }

    void RemoveOnlinePlayer(string playerId) {
        foreach (ClanUser user : arrayPlayerList) {
            if (user) {
                if (playerId == user.GetId()) {
                    arrayPlayerList.RemoveItem(user);
                    break;
                }
            }
        }
        if (GetGame().IsServer() && GetGame().IsMultiplayer()) {
            auto params = new Param1<string>(playerId);
            GetGame().RPCSingleParam(null, ClanRPCEnum.ClientRemoveOnlinePlayer, params, true);
        }
    }

    void SetConfig(ref ClanConfig config) {
        delete this.config;
        this.config = config;
    }

    void SetPlayerList(ref array<ref ClanUser> arrayPlayerList) {
        delete this.arrayPlayerList;
        this.arrayPlayerList = new array<ref ClanUser>();
        this.arrayPlayerList = arrayPlayerList;
    }

    void SetLeaderboard(ref array<ref ClanBase> arrayClanLeaderboard) {
        delete this.arrayClanLeaderboard;
        this.arrayClanLeaderboard = new array<ref ClanBase>();
        this.arrayClanLeaderboard = arrayClanLeaderboard;
    }
    
    void SortClan(ClanBase clan) {
        int count = arrayClanLeaderboard.Count();
        bool inserted = false;

        if (count < 1) {
            arrayClanLeaderboard.Insert(clan);
        } else {
            for (int i = 0; i < count; i++) {
                ref ClanBase c = arrayClanLeaderboard[i];

                if (c.GetRank() < clan.GetRank()) {
                    arrayClanLeaderboard.InsertAt(clan, i);
                    inserted = true;
                    break;
                }
            }
            if (!inserted) {
                arrayClanLeaderboard.Insert(clan);
            }
        }
    }

    ref ClanConfig GetConfig() {
        return config;
    }

    ref array<ref ClanUser> GetPlayerList() {
        return arrayPlayerList;
    }

    ref array<ref ClanBase> GetClanLeaderboard() {
        return arrayClanLeaderboard;
    }

    bool HasUserInPlayerList(string playerId) {
        foreach (ClanUser user : arrayPlayerList) {
            if (user) {
                if (playerId == user.GetId()) {
                    return true;
                }
            }
        }
        return false;
    }
}

ClanManager GetClanManager() {
    return ClanManager.Cast(GetPlugin(ClanManager));
}