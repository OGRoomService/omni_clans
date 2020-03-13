class ClanClientRPCHandler : PluginBase {
    void ClanClientRPCHandler() {
        if (GetGame().IsServer() && GetGame().IsMultiplayer()) { return; }

        GetDayZGame().Event_OnRPC.Insert(ClientRPCHandler);
    }

    void ~ClanClientRPCHandler() {
        GetDayZGame().Event_OnRPC.Remove(ClientRPCHandler);
    }

    void ClientRPCHandler(PlayerIdentity sender, Object target, int rpc_type, ParamsReadContext ctx) {
        if (GetGame().IsServer() && GetGame().IsMultiplayer()) { return; }
        ClanMember clanMember;
        ClanManageMenu manageMenu;
        ref ActiveClan clan;
        string playerId, playerPlainId, playerName, clanName, clanId;
        float playerHp, playerBlood, playerShock;
        int funds, clanRank, createMenuError, contributions;
        vector playerPos

        switch (rpc_type) {
            case ClanRPCEnum.ClientInitialize:
                {
                    Print(ClanStatic.debugPrefix + "Client Initialization RPC Received");
                    Param1<string> dataPlainId;
                    Param1<ref ClanConfig> dataConfig;
                    Param1<ref array<ref ClanBase>> dataLeaderboard;
                    Param1<ref array<ref ClanUser>> dataPlayerList;
                    if (!ctx.Read(dataPlainId) || !ctx.Read(dataConfig) || !ctx.Read(dataLeaderboard) || !ctx.Read(dataPlayerList)) { return; }

                    GetClanClientManager().SetPlainId(dataPlainId.param1);
                    GetClanManager().SetConfig(dataConfig.param1);
                    GetClanManager().SetLeaderboard(dataLeaderboard.param1);
                    GetClanManager().SetPlayerList(dataPlayerList.param1);
                    break;
                }
            case ClanRPCEnum.ClientUpdateLeaderboard:
                {
                    Print(ClanStatic.debugPrefix + "ClanClientRPCHandler | ClientUpdateLeaderboard | Client Update Leaderboard RPC Received");
                    Param1<ref array<ref ClanBase>> dataUpdateLeaderboard;

                    if (!ctx.Read(dataUpdateLeaderboard)) { return; }
                    ClanCreateMenu createMenu = ClanCreateMenu.Cast(GetGame().GetUIManager().GetMenu());

                    Print(ClanStatic.debugPrefix + "ClanClientRPCHandler | ClientUpdateLeaderboard | Data Read");
                    GetClanManager().SetLeaderboard(dataUpdateLeaderboard.param1);

                    if (createMenu) {
                        createMenu.BuildLeaderboard();
                    }
                    break;
                }
            case ClanRPCEnum.ClientAddOnlinePlayer:
                {
                    Param3<string, string, string> dataAddOnlinePlayer;
                    if (!ctx.Read(dataAddOnlinePlayer)) { return; }

                    GetClanManager().AddOnlinePlayer(dataAddOnlinePlayer.param1, dataAddOnlinePlayer.param2, dataAddOnlinePlayer.param3);
                    RefreshCreateMenu(true);
                    break;
                }
            case ClanRPCEnum.ClientRemoveOnlinePlayer:
                {
                    Param1<string> dataRemoveOnlinePlayer;
                    if (!ctx.Read(dataRemoveOnlinePlayer)) { return; }

                    GetClanManager().RemoveOnlinePlayer(dataRemoveOnlinePlayer.param1);
                    RefreshCreateMenu(true);
                    break;
                }
                // START Active Clan Management
                // Initial Receival of Clan
            case ClanRPCEnum.ClientReceiveClan:
                {
                    Print(ClanStatic.debugPrefix + "Client clan received");
                    Param1<ref ActiveClan> dataUpdateClan;
                    if (!ctx.Read(dataUpdateClan)) { return; }

                    clan = dataUpdateClan.param1;

                    if (clan) {
                        GetClanClientManager().SetActiveClan(clan);
                        RefreshCreateMenu();
                    }
                    break;
                }
                // Add tracker to active clan
            case ClanRPCEnum.ClientAddClanTracker:
                {
                    Print(ClanStatic.debugPrefix + "Client add tracker rpc received");
                    Param3<string, string, vector> dataAddClanTracker;
                    if (!ctx.Read(dataAddClanTracker)) { return; }

                    playerId = dataAddClanTracker.param1;
                    playerName = dataAddClanTracker.param2;
                    playerPos = dataAddClanTracker.param3;
                    clan = GetClanClientManager().GetClan();

                    if (clan) {
                        clan.AddTracker(playerId, playerName, playerPos);
                    }
                    break;
                }
                // Remove tracker from active clan
            case ClanRPCEnum.ClientRemoveClanTracker:
                {
                    Print(ClanStatic.debugPrefix + "Client remove tracker rpc received");
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
                    Print(ClanStatic.debugPrefix + "Client clan update tracker received");
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
                    Print(ClanStatic.debugPrefix + "Client clan add invite received");
                    Param2<string, string> dataAddClanInvite;
                    if (!ctx.Read(dataAddClanInvite)) { return; }

                    playerId = dataAddClanInvite.param1;
                    playerName = dataAddClanInvite.param2;
                    clan = GetClanClientManager().GetClan();

                    if (clan) {
                        NotificationSystem.AddNotificationExtended(3, "Player Invited!", playerName + " has been invited to the clan!");
                        clan.AddInvitation(playerId);
                    }
                    break;
                }
                // Remove invitation
            case ClanRPCEnum.ClientRemoveClanInvitation:
                {
                    Print(ClanStatic.debugPrefix + "Client clan remove invitation received");
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
                    Print(ClanStatic.debugPrefix + "Client add member received");
                    Param3<string, string, string> dataAddClanMember;
                    if (!ctx.Read(dataAddClanMember)) { return; }

                    playerName = dataAddClanMember.param1;
                    playerId = dataAddClanMember.param2;
                    playerPlainId = dataAddClanMember.param3;
                    clan = GetClanClientManager().GetClan();

                    if (clan) {
                        clan.AddMember(playerName, playerId, playerPlainId);
                        NotificationSystem.AddNotificationExtended(3, "Player Joined!", playerName + " has joined the clan!");
                        RefreshManageMenu();
                    }
                    break;
                }
                // Remove Member
            case ClanRPCEnum.ClientRemoveClanMember:
                {
                    Print(ClanStatic.debugPrefix + "Client clan remove member received");
                    Param1<string> dataRemoveClanMember;
                    if (!ctx.Read(dataRemoveClanMember)) { return; }

                    playerId = dataRemoveClanMember.param1;
                    clan = GetClanClientManager().GetClan();

                    if (clan) {
                        clanMember = clan.GetClanMemberByPlayerId(playerId);

                        if (clanMember) {
                            NotificationSystem.AddNotificationExtended(3, "Player Left", clanMember.GetPlayerName() + " has left the clan!");
                            clan.RemoveMember(playerId);
                            RefreshManageMenu();
                        }
                    }
                    break;
                }
                // Update Member
            case ClanRPCEnum.ClientUpdateClanMember:
                {
                    Print(ClanStatic.debugPrefix + "Client clan remove member received");
                    Param2<string, string> dataUpdateClanMember;
                    if (!ctx.Read(dataUpdateClanMember)) { return; }

                    playerId = dataUpdateClanMember.param1;
                    playerName = dataUpdateClanMember.param2;
                    clan = GetClanClientManager().GetClan();

                    if (clan) {
                        clan.UpdateMemberName(playerId, playerName);
                        RefreshManageMenu();
                    }
                    break;
                }
                // Promote Member
            case ClanRPCEnum.ClientPromoteClanMember:
                {
                    Print(ClanStatic.debugPrefix + "Client clan promote member received");
                    Param1<string> dataPromoteClanMember;
                    if (!ctx.Read(dataPromoteClanMember)) { return; }

                    playerId = dataPromoteClanMember.param1;
                    clan = GetClanClientManager().GetClan();

                    if (clan) {
                        clan.PromoteMember(playerId);
                        clanMember = clan.GetClanMemberByPlayerId(playerId);
                        NotificationSystem.AddNotificationExtended(3, "Member Promoted!", clanMember.GetPlayerName() + " has been promoted to " + GetClanManager().GetConfig().FindRankByInt(clanMember.GetPlayerRank()).GetName() + "!");
                        RefreshManageMenu();
                    }
                    break;
                }
                // Demote Member
            case ClanRPCEnum.ClientDemoteClanMember:
                {
                    Print(ClanStatic.debugPrefix + "Client clan demote member received");
                    Param1<string> dataDemoteClanMember;
                    if (!ctx.Read(dataDemoteClanMember)) { return; }

                    playerId = dataDemoteClanMember.param1;
                    clan = GetClanClientManager().GetClan();

                    if (clan) {
                        clan.DemoteMember(playerId);
                        clanMember = clan.GetClanMemberByPlayerId(playerId);
                        NotificationSystem.AddNotificationExtended(3, "Member Demoted!", clanMember.GetPlayerName() + " has been demoted to " + GetClanManager().GetConfig().FindRankByInt(clanMember.GetPlayerRank()).GetName() + "!");
                        RefreshManageMenu();
                    }
                    break;
                }
                // Add Funds
            case ClanRPCEnum.ClientAddClanFunds:
                {
                    Print(ClanStatic.debugPrefix + "Client add clan funds received");
                    Param1<int> dataAddClanFunds;
                    if (!ctx.Read(dataAddClanFunds)) { return; }

                    funds = dataAddClanFunds.param1;
                    clan = GetClanClientManager().GetClan();

                    if (clan) {
                        clan.AddFunds(funds);

                        manageMenu = ClanManageMenu.Cast(GetGame().GetUIManager().GetMenu());

                        if (manageMenu) {
                            manageMenu.RefreshText();
                        }
                    }
                    break;
                }
                // Remove funds
            case ClanRPCEnum.ClientRemoveClanFunds:
                {
                    Print(ClanStatic.debugPrefix + "Client remove clan funds received");
                    Param1<int> dataRemoveClanFunds;
                    if (!ctx.Read(dataRemoveClanFunds)) { return; }

                    funds = dataRemoveClanFunds.param1;
                    clan = GetClanClientManager().GetClan();

                    if (clan) {
                        clan.RemoveFunds(funds);

                        manageMenu = ClanManageMenu.Cast(GetGame().GetUIManager().GetMenu());

                        if (manageMenu) {
                            manageMenu.RefreshText();
                        }
                    }
                    break;
                }
                // Upgrade clan
            case ClanRPCEnum.ClientUpgradeClan:
                {
                    Print(ClanStatic.debugPrefix + "Client upgrade clan rpc received");
                    clan = GetClanClientManager().GetClan();

                    if (clan) {
                        clan.UpgradeClan();

                        manageMenu = ClanManageMenu.Cast(GetGame().GetUIManager().GetMenu());

                        if (manageMenu) {
                            manageMenu.RefreshText();
                        }
                    }
                    break;
                }
                // Add Contributions
            case ClanRPCEnum.ClientAddClanMemberContributions:
                {
                    Print(ClanStatic.debugPrefix + "Client add member contributions received");
                    Param2<string, int> dataAddClanMemberContributions;
                    if (!ctx.Read(dataAddClanMemberContributions)) { return; }

                    playerId = dataAddClanMemberContributions.param1;
                    contributions = dataAddClanMemberContributions.param2;
                    clan = GetClanClientManager().GetClan();

                    if (clan) {
                        clan.AddMemberContributions(playerId, contributions);
                        RefreshManageMenu();
                    }
                    break;
                }
                // END Active Clan Management
            case ClanRPCEnum.ClientReceiveInvite:
                {
                    Param2<string, string> dataReceiveInvite;
                    if (!ctx.Read(dataReceiveInvite)) { return; }

                    clanId = dataReceiveInvite.param1;
                    clanName = dataReceiveInvite.param2;

                    GetClanClientManager().SetClanInvite(clanId, clanName);
                    NotificationSystem.AddNotificationExtended(3, "Clan Invite Received!", "You have been invited to the " + clanName + " clan!");
                    RefreshCreateMenu();
                    break;
                }
            case ClanRPCEnum.ClientDeleteInvite:
                {
                    Print("Received RPC to delete active invite!");

                    if (GetClanClientManager().GetInvite()) {
                        NotificationSystem.AddNotificationExtended(3, "Invite Expired!", "Your invite for the " + GetClanClientManager().GetInvite().GetClanName() + " clan has expired!");
                        GetClanClientManager().DeleteInvitation();
                        RefreshCreateMenu();
                    }
                    break;
                }
            case ClanRPCEnum.ClientManageMenu:
                {
                    Print(ClanStatic.debugPrefix + "Manage Menu RPC Received");
                    Param1<int> dataManageMenu;
                    if (!ctx.Read(dataManageMenu)) { return; }

                    ClanMenu menu = ClanMenu.Cast(GetGame().GetUIManager().GetMenu());
                    createMenuError = dataManageMenu.param1;

                    if (menu) {
                        menu.HandleError(createMenuError);
                    }
                    break;
                }
            case ClanRPCEnum.ClientDeleteClan:
                {
                    Print(ClanStatic.debugPrefix + "Received RPC to delete local clan");
                    GetClanClientManager().DeleteClan();
                    RefreshCreateMenu();
                    break;
                }
        }
    }

    void RefreshManageMenu() {
        ClanManageMenu manageMenu = ClanManageMenu.Cast(GetGame().GetUIManager().GetMenu());

        if (manageMenu) {
            manageMenu.BuildUI();
        }
    }

    void RefreshCreateMenu(bool includePlayerList = false) {
        ClanCreateMenu createMenu = ClanCreateMenu.Cast(GetGame().GetUIManager().GetMenu());

        if (createMenu) {
            if (includePlayerList) {
                createMenu.RefreshPlayerList();
            }
            createMenu.CheckClan();
            createMenu.CheckInvite();
        }
    }
}