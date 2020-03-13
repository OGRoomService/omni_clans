modded class MissionServer {
    void InitializeClanPlayerBase(PlayerIdentity identity, PlayerBase player) {
        if (identity) {
            Print(ClanStatic.debugPrefix + "Handling InitializeClanPlayerBase on MissionServer");
            auto rpc_params = new array<ref Param>();
            auto plainId = new Param1<string>(identity.GetPlainId());
            auto config = new Param1<ref ClanConfig>(GetClanManager().GetConfig());
            auto leaderboard = new Param1<ref array<ref ClanBase>>(GetClanManager().GetClanLeaderboard());
            auto playerList = new Param1<ref array<ref ClanUser>>(GetClanManager().GetPlayerList());

            rpc_params.Insert(plainId);
            rpc_params.Insert(config);
            rpc_params.Insert(leaderboard);
            rpc_params.Insert(playerList);

            GetGame().RPC(player, ClanRPCEnum.ClientInitialize, rpc_params, true, identity);
            GetClanServerManager().InitializeClanPlayer(player);

            if (!GetClanManager().HasUserInPlayerList(identity.GetId())) {
                GetClanManager().AddOnlinePlayer(identity.GetName(), identity.GetId(), identity.GetPlainId());
            }
        }
    }

    override void InvokeOnConnect(PlayerBase player, PlayerIdentity identity) {
        super.InvokeOnConnect(player, identity);
        InitializeClanPlayerBase(identity, player);
    }

    override void OnClientReconnectEvent(PlayerIdentity identity, PlayerBase player) {
        super.OnClientReconnectEvent(identity, player);
        InitializeClanPlayerBase(identity, player);
    }

    void OnClientDisconnectedEvent(PlayerIdentity identity, PlayerBase player, int logoutTime, bool authFailed) {
        bool disconnectNow = true;

        if (GetHive() && !authFailed) {
            if (player.IsAlive()) {
                if (!m_LogoutPlayers.Contains(player)) {
                    disconnectNow = false;
                }
            }
        }
        if (disconnectNow) {
            ClanPlayerDisconnected(player, identity, identity.GetId());
        }
        super.OnClientDisconnectedEvent(identity, player, logoutTime, authFailed);
    }

    override void UpdateLogoutPlayers() {
        for (int i = 0; i < m_LogoutPlayers.Count(); i++) {
            LogoutInfo info = m_LogoutPlayers.GetElement(i);

            if (GetGame().GetTime() >= info.param1) {
                PlayerBase player = m_LogoutPlayers.GetKey(i);
                PlayerIdentity identity;

                if (player) {
                    identity = player.GetIdentity();

                    if (identity) {
                        ClanPlayerDisconnected(player, identity, identity.GetId());
                    }
                }
            }
        }
        super.UpdateLogoutPlayers();
    }

    void ClanPlayerDisconnected(PlayerBase player, PlayerIdentity identity, string uid) {
        if (identity) {
            uid = identity.GetId();
        }
        if (GetClanManager().HasUserInPlayerList(uid)) {
            GetClanManager().RemoveOnlinePlayer(uid);
        }
        if (player && identity) {
            GetClanServerManager().RemovePlayerFromActiveClan(player);
        }
    }

    override void PlayerDisconnected(PlayerBase player, PlayerIdentity identity, string uid) {
        ClanPlayerDisconnected(player, identity, uid);

        super.PlayerDisconnected(player, identity, uid);
    }
}