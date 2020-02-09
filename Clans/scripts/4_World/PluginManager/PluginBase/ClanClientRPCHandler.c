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
        ClanManageMenu manageMenu;
        ref ActiveClan clan;
        string playerId, playerPlainId, playerName, error, clanName;
        float playerHp, playerBlood, playerShock;
        int funds;
        vector playerPos

        switch (rpc_type) {
            case ClanRPCEnum.ClientInitialize:
                {
                    Print("Set Plain ID RPC Received");
                    Param3<string, ref ClanConfig, ref array<ref ClanBase>> dataIntialize;
                    if (!ctx.Read(dataIntialize)) { return; }

                    GetClanClientManager().SetPlainId(dataIntialize.param1);
                    GetClanManager().SetConfig(dataIntialize.param2);
                    GetClanManager().SetLeaderboard(dataIntialize.param3);
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
        // START Active Clan Management
            // Initial Receival of Clan
            case ClanRPCEnum.ClientReceiveClan:
                {
                    Print("Client clan received");
                    Param1<ref ActiveClan> dataUpdateClan;
                    if (!ctx.Read(dataUpdateClan)) { return; }

                    clan = dataUpdateClan.param1;

                    if (clan) {
                        GetClanClientManager().SetActiveClan(clan);
                    }
                    break;
                }
            // Add tracker to active clan
            case ClanRPCEnum.ClientAddClanTracker:
                {
                    Print("Client add tracker rpc received");
                    Param4<string, string, string, vector> dataAddClanTracker;
                    if (!ctx.Read(dataAddClanTracker)) { return; }
                    
                    playerId = dataAddClanTracker.param1;
                    playerPlainId = dataAddClanTracker.param2;
                    playerName = dataAddClanTracker.param3;
                    playerPos = dataAddClanTracker.param4;
                    clan = GetClanClientManager().GetClan();

                    if (clan) {
                        clan.AddTracker(NULL, playerId, playerPlainId, playerName, playerPos);
                    }
                    break;
                }
            // Remove tracker from active clan
            case ClanRPCEnum.ClientRemoveClanTracker:
                {
                    Print("Client remove tracker rpc received");
                    Param1<string> dataRemoveClanTracker;
                    if (!ctx.Read(dataRemoveClanTracker)) { return; }

                    playerId = dataRemoveClanTracker.param1;
                    clan = GetClanClientManager().GetClan();

                    if (clan) {
                        clan.RemoveTracker(playerId);
                    }
                    break;
                }
            // Update existing tracker data
            case ClanRPCEnum.ClientUpdateClanTracker:
                {
                    Print("Client clan update tracker received");
                    Param2<string, vector> dataUpdateClanTracker;
                    if (!ctx.Read(dataUpdateClanTracker)) { return; }

                    playerId = dataUpdateClanTracker.param1;
                    playerPos = dataUpdateClanTracker.param2;
                    clan = GetClanClientManager().GetClan();

                    if (clan) {
                        clan.UpdateTracker(playerId, playerPos);
                    }
                    break;
                }
            // Add invitation
            case ClanRPCEnum.ClientAddClanInvitation:
                {
                    Print("Client clan add invite received");
                    Param1<string> dataAddClanInvite;
                    if (!ctx.Read(dataAddClanInvite)) { return; }

                    playerId = dataAddClanInvite.param1;
                    clan = GetClanClientManager().GetClan();

                    if (clan) {
                        clan.AddInvitation(playerId);
                    }
                    break;
                }
            // Remove invitation
            case ClanRPCEnum.ClientRemoveClanInvitation:
                {
                    Print("Client clan remove invitation received");
                    Param1<string> dataRemoveClanInvite;
                    if (!ctx.Read(dataRemoveClanInvite)) { return; }
                    
                    playerId = dataRemoveClanInvite.param1;
                    clan = GetClanClientManager().GetClan();

                    if (clan) {
                        clan.RemoveInvitation(playerId);
                    }
                    break;
                }
            // Add Member
            case ClanRPCEnum.ClientAddClanMember:
                {
                    Print("Client add member received");
                    Param2<string, string> dataAddClanMember;
                    if (!ctx.Read(dataAddClanMember)) { return; }
                    
                    playerName = dataAddClanMember.param1;
                    playerId = dataAddClanMember.param2;
                    clan = GetClanClientManager().GetClan();

                    if (clan) {
                        clan.AddMember(playerName, playerId);
                        RefreshManageMenu();
                    }
                    break;
                }
            // Remove Member
            case ClanRPCEnum.ClientRemoveClanMember:
                {
                    Print("Client clan remove member received");
                    Param1<string> dataRemoveClanMember;
                    if (!ctx.Read(dataRemoveClanMember)) { return; }
                    
                    playerId = dataRemoveClanMember.param1;
                    clan = GetClanClientManager().GetClan();

                    if (clan) {
                        clan.RemoveMember(playerId);
                        RefreshManageMenu();
                    }
                    break;
                }
            // Promote Member
            case ClanRPCEnum.ClientPromoteClanMember:
                {
                    Print("Client clan promote member received");
                    Param1<string> dataPromoteClanMember;
                    if (!ctx.Read(dataPromoteClanMember)) { return; }
                    
                    playerId = dataPromoteClanMember.param1;
                    clan = GetClanClientManager().GetClan();

                    if (clan) {
                        clan.PromoteMember(playerId);
                        RefreshManageMenu();
                    }
                    break;
                }
            // Demote Member
            case ClanRPCEnum.ClientDemoteClanMember:
                {
                    Print("Client clan demote member received");
                    Param1<string> dataDemoteClanMember;
                    if (!ctx.Read(dataDemoteClanMember)) { return; }
                    
                    playerId = dataDemoteClanMember.param1;
                    clan = GetClanClientManager().GetClan();

                    if (clan) {
                        clan.DemoteMember(playerId);
                        RefreshManageMenu();
                    }
                    break;
                }
            // Set Funds
            case ClanRPCEnum.ClientSetClanFunds:
                {
                    Print("Client clan remove invitation received");
                    Param1<int> dataSetClanFunds;
                    if (!ctx.Read(dataSetClanFunds)) { return; }
                    
                    funds = dataSetClanFunds.param1;
                    clan = GetClanClientManager().GetClan();

                    if (clan) {
                        clan.SetFunds(funds);
                    }
                    break;
                }
        // END Active Clan Management
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
            case ClanRPCEnum.ClientDeleteClan:
                {
                    GetClanClientManager().DeleteClan();
                }
        }
    }

    void RefreshManageMenu() {
        ClanManageMenu manageMenu = ClanManageMenu.Cast(GetGame().GetUIManager().GetMenu());

        if (manageMenu) {
            manageMenu.Refresh();
        }
    }
}