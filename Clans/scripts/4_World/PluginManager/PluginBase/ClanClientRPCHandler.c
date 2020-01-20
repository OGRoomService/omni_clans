class ClanClientRPCHandler : PluginBase {
    void ClanClientRPCHandler() {
        if (GetGame().IsServer() && GetGame().IsMultiplayer()) { return; }

        GetDayZGame().Event_OnRPC.Insert(ClientRPCHandler);
    }

    void ~ClanClientRPCHandler() {
        GetDayZGame().Event_OnRPC.Remove(ClientRPCHandler);
    }

    void ClientRPCHandler(PlayerIdentity sender, Object target, int rpc_type, ParamsReadContext ctx) {
        if (GetGame().IsServer() && GetGame().IsMultiplayer()) {
            delete this;
            return;
        }
        string error, clanName;

        switch (rpc_type) {
            case ClanRPCEnum.ClientSetPlainId:
                {
                    Print("Set Plain ID RPC Received");
                    Param1<string> dataPlainId;
                    if (!ctx.Read(dataPlainId)) { return; }

                    GetClanClientManager().SetPlainId(dataPlainId.param1);
                    break;
                }
            case ClanRPCEnum.ClientManageMenu:
                {
                    Print("Manage Menu RPC Received");
                    Param1<string> dataManageMenu;
                    if (!ctx.Read(dataManageMenu)) { return; }

                    ClanMenu menu = ClanMenu.Cast(GetGame().GetUIManager().GetMenu());
                    error = dataManageMenu.param1;

                    if (menu) {
                        menu.HandleError(error);
                    }
                    break;
                }
            case ClanRPCEnum.ClientUpdateClan:
                {
                    Print("Client clan update received");
                    Param1<ref ActiveClan> dataUpdateClan;
                    if (!ctx.Read(dataUpdateClan)) { return; }

                    ref ActiveClan clan = dataUpdateClan.param1;
                    Print("clan=" + clan);

                    if (clan) {
                        GetClanClientManager().SetActiveClan(clan);
                    }
                    break;
                }
            case ClanRPCEnum.ClientReceivePlayerList:
                {
                    Param1<ref array<ref ClanUser>> dataPlayerList;
                    if (!ctx.Read(dataPlayerList)) { return; }

                    Print("Receiving player list");
                    GetClanManager().SetPlayerList(dataPlayerList.param1);
                    break;
                }
            case ClanRPCEnum.ClientAddOnlinePlayer:
                {
                    Param2<string, string> dataAddOnlinePlayer;
                    if (!ctx.Read(dataAddOnlinePlayer)) { return; }

                    Print("Adding player from list");
                    for (int i = 0; i < 250; i++) {
                        GetClanManager().AddOnlinePlayer(dataAddOnlinePlayer.param1, dataAddOnlinePlayer.param2);
                    }
                    break;
                }
            case ClanRPCEnum.ClientRemoveOnlinePlayer:
                {
                    Param1<string> dataRemoveOnlinePlayer;
                    if (!ctx.Read(dataRemoveOnlinePlayer)) { return; }

                    Print("Removing player from list");
                    GetClanManager().RemoveOnlinePlayer(dataRemoveOnlinePlayer.param1);
                    break;
                }
            case ClanRPCEnum.ClientReceiveInvite:
                {
                    Param1<string> dataReceiveInvite;
                    if (!ctx.Read(dataReceiveInvite)) { return; }

                    ClanCreateMenu createMenu = ClanCreateMenu.Cast(GetGame().GetUIManager().GetMenu());
                    clanName = dataReceiveInvite.param1;

                    GetClanClientManager().SetClanInvite(clanName);
                    NotificationSystem.AddNotificationExtended(5, "Clan Invite Received!", "You have been invited to the " + clanName + " clan!");


                    if (createMenu) {
                        createMenu.CheckInvite();
                    }
                }
        }
    }
}