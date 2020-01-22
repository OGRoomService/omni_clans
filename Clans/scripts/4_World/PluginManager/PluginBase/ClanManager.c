// Separate into client and server manager and have base functions a part of the clanmanager. Split things a part from there.

class ClanManager : PluginBase {
    private ref array<ref ClanUser> playerList;
    private ref ClanConfig config;
    // In here store shit that'll be used client/server side. For instance, if I need a player list, or clan leaderboard. That kind of shit

    void ClanManager() {
        playerList = new array<ref ClanUser>();
    }

    void SetPlayerList(ref array<ref ClanUser> list) {
        if (GetGame().IsServer() && GetGame().IsMultiplayer()) { return; }

        playerList = list;
        playerList.Debug();
    }

    void AddOnlinePlayer(string name = "", string id = "", PlayerBase player = NULL) {
        bool found = false;

        if (player && GetGame().IsServer() && GetGame().IsMultiplayer()) {
            id = player.GetIdentity().GetPlainId();
            name = player.GetIdentity().GetName();

            if (playerList.Count() > 0) {
                auto arrayParams = new Param1<ref array<ref ClanUser>>(playerList);
                GetGame().RPCSingleParam(player, ClanRPCEnum.ClientReceivePlayerList, arrayParams, true, player.GetIdentity());
            }
        }
        foreach (ClanUser user : playerList) {
            if (user.GetId() == id) {
                found = true;
                break;
            }
        }
        //if (!found) {
            ref ClanUser newUser = new ClanUser(name, id);
            playerList.Insert(newUser);
            //playerList.Debug();

            if (!GetGame().IsServer() || !GetGame().IsMultiplayer()) { return; }

            auto params = new Param2<string, string>(name, id);
            GetGame().RPCSingleParam(player, ClanRPCEnum.ClientAddOnlinePlayer, params, true);
        //}
    }

    void RemoveOnlinePlayer(string id = "", PlayerBase player = NULL) {
        if (player) {
            id = player.GetIdentity().GetPlainId();
        }
        foreach (ClanUser user : playerList) {
            if (user.GetId() == id) {
                playerList.RemoveItem(user);
                playerList.Debug();

                if (!GetGame().IsServer() || !GetGame().IsMultiplayer()) { return; }

                auto params = new Param1<string>(id);
                GetGame().RPCSingleParam(player, ClanRPCEnum.ClientRemoveOnlinePlayer, params, true);
            }
        }
    }

    void SetConfig(ref ClanConfig c) {
        config = c;
    }

    ref ClanConfig GetConfig() {
        return config;
    }

    ref array<ref ClanUser> GetPlayerList() {
        return playerList;
    }
}

ClanManager GetClanManager() {
    return ClanManager.Cast(GetPlugin(ClanManager));
}