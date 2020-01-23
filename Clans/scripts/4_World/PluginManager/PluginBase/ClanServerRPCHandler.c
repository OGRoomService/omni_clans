class ClanServerRPCHandler : PluginBase {
    void ClanServerRPCHandler() {
        GetDayZGame().Event_OnRPC.Insert(ServerRPCHandler);
    }

    void ~ClanServerRPCHandler() {
        GetDayZGame().Event_OnRPC.Remove(ServerRPCHandler);
    }

    void ServerRPCHandler(PlayerIdentity sender, Object target, int rpc_type, ParamsReadContext ctx) {
        if (!sender) { return; }

        ref ActiveClan clan;
        PlayerBase player, targetPlayer;
        string clanName, error, playerId;

        switch (rpc_type) {
            case ClanRPCEnum.ServerCreateClan:
                {
                    Param1<string> dataCreateClan;
                    if (!ctx.Read(dataCreateClan)) { return; }

                    player = PlayerBase.Cast(target);
                    clanName = dataCreateClan.param1;

                    if (player && clanName != string.Empty) {
                        if (GetClanServerManager().CanCreateClan(player, clanName, error)) {
                            error = "exit";
                            GetClanServerManager().CreateClan(player, clanName);
                        }
                        if (error == string.Empty) {
                            error = "unknown error occurred!";
                        }
                        auto params = new Param1<string>(error);

                        GetGame().RPCSingleParam(player, ClanRPCEnum.ClientManageMenu, params, true, sender);
                    }
                    break;
                }
            case ClanRPCEnum.ServerAddInvitation:
                {
                    Param2<string, string> dataAddInvitation;
                    if (!ctx.Read(dataAddInvitation)) { return; }

                    Print("Received rpc to attach invite");

                    player = PlayerBase.Cast(target);
                    clanName = dataAddInvitation.param1;
                    playerId = dataAddInvitation.param2;

                    if (player && clanName != string.Empty && playerId != string.Empty) {
                        clan = GetClanServerManager().GetActiveClanByName(clanName);

                        if (clan && clan.IsMember(player.GetIdentity().GetPlainId())) {
                            clan.AddInvitation(playerId);
                        }
                        targetPlayer = GetClanServerManager().GetPlayerBaseById(playerId);

                        if (targetPlayer) {
                            auto paramsInvite = new Param1<string>(clanName);
                            GetGame().RPCSingleParam(targetPlayer, ClanRPCEnum.ClientReceiveInvite, paramsInvite, true, targetPlayer.GetIdentity());
                        }
                    }
                    break;
                }
            case ClanRPCEnum.ServerManageInvite:
                {
                    Param2<string, string> dataManageInvite;
                    if (!ctx.Read(dataManageInvite)) { return; }

                    player = PlayerBase.Cast(target);
                    clanName = dataManageInvite.param1;
                    string selectedOption = dataManageInvite.param2;

                    Print("Received rpc to manage invite by player");

                    if (player && clanName != string.Empty) {
                        clan = GetClanServerManager().GetActiveClanByName(clanName);
                        playerId = player.GetIdentity().GetPlainId();

                        if (clan) {
                            if (selectedOption == "btnAcceptInvite" && clan.IsPlayerInvited(playerId)) {
                                clan.AddMember(player.GetIdentity().GetName(), playerId);
                            }
                            clan.AddTracker(player);
                            clan.RemoveInvitation(playerId);
                        }
                    }
                    break;
                }
        }
    }
}