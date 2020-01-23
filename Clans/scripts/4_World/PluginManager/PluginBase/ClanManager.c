// Separate into client and server manager and have base functions a part of the clanmanager. Split things a part from there.

class ClanManager : PluginBase {
    private ref array<ref ClanUser> arrayPlayerList;
    private ref array<ref ClanBase> arrayClanLeaderboard;
    private ref ClanConfig config;
    // In here store shit that'll be used client/server side. For instance, if I need a player list, or clan leaderboard. That kind of shit

    void ClanManager() {
        arrayPlayerList = new array<ref ClanUser>();
        arrayClanLeaderboard = new array<ref ClanBase>();
    }

    void SetPlayerList(ref array<ref ClanUser> list) {
        if (GetGame().IsServer() && GetGame().IsMultiplayer()) { return; }

        arrayPlayerList = list;
        arrayPlayerList.Debug();
    }

    void AddOnlinePlayer(string name = "", string id = "", PlayerBase player = NULL) {
        bool found = false;

        if (player && GetGame().IsServer() && GetGame().IsMultiplayer()) {
            id = player.GetIdentity().GetPlainId();
            name = player.GetIdentity().GetName();

            if (arrayPlayerList.Count() > 0) {
                auto arrayParams = new Param1<ref array<ref ClanUser>>(arrayPlayerList);
                GetGame().RPCSingleParam(player, ClanRPCEnum.ClientReceivePlayerList, arrayParams, true, player.GetIdentity());
            }
        }
        foreach (ClanUser user : arrayPlayerList) {
            if (user.GetId() == id) {
                found = true;
                break;
            }
        }
        //if (!found) {
            ref ClanUser newUser = new ClanUser(name, id);
            arrayPlayerList.Insert(newUser);
            //arrayPlayerList.Debug();

            if (!GetGame().IsServer() || !GetGame().IsMultiplayer()) { return; }

            auto params = new Param2<string, string>(name, id);
            GetGame().RPCSingleParam(player, ClanRPCEnum.ClientAddOnlinePlayer, params, true);
        //}
    }

    void RemoveOnlinePlayer(string id = "", PlayerBase player = NULL) {
        if (player) {
            id = player.GetIdentity().GetPlainId();
        }
        foreach (ClanUser user : arrayPlayerList) {
            if (user.GetId() == id) {
                arrayPlayerList.RemoveItem(user);
                arrayPlayerList.Debug();

                if (!GetGame().IsServer() || !GetGame().IsMultiplayer()) { return; }

                auto params = new Param1<string>(id);
                GetGame().RPCSingleParam(player, ClanRPCEnum.ClientRemoveOnlinePlayer, params, true);
            }
        }
    }

    void SetConfig(ref ClanConfig c) {
        config = c;
    }

    void SetLeaderboard(ref array<ref ClanBase> leaderboard) {
        arrayClanLeaderboard = leaderboard;
        //arrayClanLeaderboard.Debug();
    }

    void SortClan(ClanBase clan) {
        if (!GetGame().IsServer() || !GetGame().IsMultiplayer()) { return; }

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
}

ClanManager GetClanManager() {
    return ClanManager.Cast(GetPlugin(ClanManager));
}