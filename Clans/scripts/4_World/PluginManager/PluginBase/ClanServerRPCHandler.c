class ClanServerRPCHandler : PluginBase {
    void ClanServerRPCHandler() {
        GetDayZGame().Event_OnRPC.Insert(ServerRPCHandler);
    }

    void ~ClanServerRPCHandler() {
        GetDayZGame().Event_OnRPC.Remove(ServerRPCHandler);
    }

    // [REWRITE] Entire file. Make this way easier to read.
    void ServerRPCHandler(PlayerIdentity sender, Object target, int rpc_type, ParamsReadContext ctx) {
        if (!sender) { return; }

        ref ActiveClan clan, previousClan;
        ref ClanMember clanMember, targetClanMember;
        ref ClanConfig clanConfig;
        PlayerBase player, targetPlayer;
        string clanName, clanId, playerId, playerPlainId, targetPlayerId, targetPlayerName;
        int clanMemberRank, targetClanMemberRank, fundsToContribute, clanUpgradeCost, clanMenuError;
        map<ItemBase, ref ClanConfigCurrency> mapCurrencyItems;

        switch (rpc_type) {
            case ClanRPCEnum.ServerCreateClan:
                {
                    Print(ClanStatic.debugPrefix + "RPC Create Clan | RPC Received");
                    Param1<string> dataCreateClan;
                    if (!ctx.Read(dataCreateClan)) { return; }

                    player = PlayerBase.Cast(target);
                    clanName = dataCreateClan.param1;

                    Print(ClanStatic.debugPrefix + "RPC Create Clan | Data Read | player=" + player.GetIdentity().GetId() + " | clan name=" + clanName);
                    if (player && clanName != string.Empty) {
                        Param1<int> params;
                        int clanCreationCost = GetClanManager().GetConfig().GetClanCreationCost();
                        Print(ClanStatic.debugPrefix + "RPC Create Clan | Player exists and clanname is not empty!");

                        if (GetClanServerManager().HasEnoughCurrency(player, clanCreationCost, mapCurrencyItems)) {
                            Print(ClanStatic.debugPrefix + "RPC Create Clan | Player has enough currency to create a clan!");
                            if (GetClanServerManager().CanCreateClan(player, clanName, clanMenuError)) {
                                Print(ClanStatic.debugPrefix + "RPC Create Clan | Player can create clan, creating!");
                                GetClanServerManager().CreateClan(player, clanName);
                                GetClanServerManager().DeductPlayerCurrency(player, clanCreationCost, mapCurrencyItems);
                            } else {
                                params = new Param1<int>(clanMenuError);
                                GetGame().RPCSingleParam(player, ClanRPCEnum.ClientManageMenu, params, true, sender);
                            }
                        } else {
                            params = new Param1<int>(ClanCreateErrorEnum.NO_FUNDS);
                            GetGame().RPCSingleParam(player, ClanRPCEnum.ClientManageMenu, params, true, sender);
                        }
                    }
                    break;
                }
            case ClanRPCEnum.ServerAddInvitation:
                {
                    Print(ClanStatic.debugPrefix + "RPC Add Invitation | RPC Received!");
                    Param1<string> dataAddInvitation;
                    if (!ctx.Read(dataAddInvitation)) { return; }

                    player = PlayerBase.Cast(target);
                    targetPlayerId = dataAddInvitation.param1;

                    Print(ClanStatic.debugPrefix + "RPC Add Invitation | Data Read! | player=" + player.GetIdentity().GetId() + " | target player=" + targetPlayerId);

                    if (player && targetPlayerId != string.Empty && targetPlayerId != player.GetIdentity().GetId()) {
                        clan = GetClanServerManager().GetClanById(player.GetActiveClanId());

                        Print(ClanStatic.debugPrefix + "RPC Add Invitation | Conditionals met, searching for clan! | clan id=" + player.GetActiveClanId() + " | clan=" + clan);
                        if (clan && clan.IsPlayerInClan(player.GetIdentity().GetId()) && clan.GetMembers().Count() < GetClanManager().GetConfig().GetClanPlayerCap(clan.GetRank()) && !clan.IsPlayerInvited(targetPlayerId) && !clan.IsPlayerInClan(targetPlayerId)) {
                            Print(ClanStatic.debugPrefix + "RPC Add Invitation | Second Conditionals met, checking permissions! rank=" + clan.GetClanMemberByPlayerId(player.GetIdentity().GetId()).GetPlayerRank());
                            if (GetClanManager().GetConfig().CanInviteMembers(clan.GetClanMemberByPlayerId(player.GetIdentity().GetId()).GetPlayerRank())) {
                                targetPlayer = GetClanServerManager().GetPlayerBaseById(targetPlayerId);
                                Print(ClanStatic.debugPrefix + "RPC Add Invitation | Permissions validated, searching for player! | id=" + targetPlayerId + " | player=" + player.GetIdentity().GetId());

                                if (targetPlayer && targetPlayer.GetIdentity()) {
                                    Print(ClanStatic.debugPrefix + "RPC Add Invitation | Player found, sending invite!");
                                    auto paramsInvite = new Param2<string, string>(clan.GetClanId(), clan.GetName());
                                    
                                    clan.AddInvitation(targetPlayerId, targetPlayer.GetIdentity().GetName());
                                    GetGame().RPCSingleParam(targetPlayer, ClanRPCEnum.ClientReceiveInvite, paramsInvite, true, targetPlayer.GetIdentity());
                                }
                            }
                        }
                    }
                    break;
                }
            case ClanRPCEnum.ServerManageInvite:
                {
                    Print(ClanStatic.debugPrefix + "RPC Manage Invite | RPC Received");
                    Param2<string, bool> dataManageInvite;
                    if (!ctx.Read(dataManageInvite)) { return; }

                    bool clientAcceptedInvite = dataManageInvite.param2;
                    player = PlayerBase.Cast(target);
                    clanId = dataManageInvite.param1;

                    Print(ClanStatic.debugPrefix + "RPC Manage Invite | Data read | accepted=" + clientAcceptedInvite + " | player=" + player.GetIdentity().GetId() + " | clan id=" + clanId);

                    if (player) {
                        Print(ClanStatic.debugPrefix + "RPC Manage Invite | Player found");
                        clan = GetClanServerManager().GetClanById(clanId);
                        Print(ClanStatic.debugPrefix + "RPC Manage Invite | Searching for clan " + clan);

                        if (clan && clan.IsPlayerInvited(player.GetIdentity().GetId()) && clan.GetMembers().Count() < GetClanManager().GetConfig().GetClanPlayerCap(clan.GetRank())) {
                            Print(ClanStatic.debugPrefix + "RPC Manage Invite | Clan found and player is invited | " + clan.GetClanId());
                            if (clientAcceptedInvite) {
                                previousClan = GetClanServerManager().GetClanById(player.GetActiveClanId());

                                if (previousClan) {
                                    Print(ClanStatic.debugPrefix + "RPC Manage Invite | Previous clan from player found! " + previousClan.GetOwnerId());
                                    if (previousClan.GetOwnerId() == player.GetIdentity().GetId()) {
                                        Print(ClanStatic.debugPrefix + "RPC Manage Invite | Player is owner of previous clan, deleting!");
                                        GetClanServerManager().DeleteClan(previousClan);
                                    } else {
                                        Print(ClanStatic.debugPrefix + "RPC Manage Invite | Removing member from previous clan!");
                                        GetClanServerManager().RemoveMemberFromClan(previousClan, player);
                                    }
                                }
                                Print(ClanStatic.debugPrefix + "RPC Manage Invite | Adding member to clan!");
                                GetClanServerManager().AddMemberToClan(clan, player);
                            }
                            clan.RemoveInvitation(player.GetIdentity().GetId());
                        }
                    }
                    break;
                }
            case ClanRPCEnum.ServerPromoteClanMember:
                {
                    Print(ClanStatic.debugPrefix + "RPC Promote Member | RPC Received");
                    Param1<string> dataPromoteMember;
                    if (!ctx.Read(dataPromoteMember)) { return; }

                    player = PlayerBase.Cast(target);
                    targetPlayerId = dataPromoteMember.param1;

                    Print(ClanStatic.debugPrefix + "RPC Promote Member | Data read | player=" + player.GetIdentity().GetId() + " | target player=" + targetPlayerId);
                    if (player && targetPlayerId != string.Empty) {
                        clan = GetClanServerManager().GetClanById(player.GetActiveClanId());
                        Print(ClanStatic.debugPrefix + "RPC Promote Member | Conditionals met, searching for clan | clan id=" + player.GetActiveClanId() + " | clan=" + clan);

                        if (clan && clan.IsPlayerInClan(player.GetIdentity().GetId()) && clan.IsPlayerInClan(targetPlayerId)) {
                            Print(ClanStatic.debugPrefix + "RPC Promote Member | Second conditionals met, validating permissions");
                            clanMemberRank = clan.GetClanMemberByPlayerId(player.GetIdentity().GetId()).GetPlayerRank();
                            clanConfig = GetClanManager().GetConfig();

                            if (clanConfig.CanPromoteMembers(clanMemberRank)) {
                                Print(ClanStatic.debugPrefix + "RPC Promote Member | Permissions validated");
                                int clanMemberRankIndex, targetClanMemberRankIndex;

                                targetClanMemberRank = clan.GetClanMemberByPlayerId(targetPlayerId).GetPlayerRank();
                                clanMemberRankIndex = clanConfig.GetRankIndex(clanMemberRank);
                                targetClanMemberRankIndex = clanConfig.GetRankIndex(targetClanMemberRank);

                                if ((clanMemberRankIndex != -1 && targetClanMemberRankIndex != -1 && (targetClanMemberRankIndex - 1) > clanMemberRankIndex && !clanConfig.IsHighestRank(targetClanMemberRank)) || clan.GetOwnerId() == player.GetIdentity().GetId()) {
                                    Print(ClanStatic.debugPrefix + "RPC Promote Member | User is still a rank lower after promotion, promoting");
                                    clan.PromoteMember(targetPlayerId);
                                }
                            }
                        }
                    }
                    break;
                }
            case ClanRPCEnum.ServerDemoteClanMember:
                {
                    Print(ClanStatic.debugPrefix + "RPC Demote Member | RPC Received");
                    Param1<string> dataDemoteMember;
                    if (!ctx.Read(dataDemoteMember)) { return; }

                    player = PlayerBase.Cast(target);
                    targetPlayerId = dataDemoteMember.param1;

                    Print(ClanStatic.debugPrefix + "RPC Demote Member | Data read | player=" + player.GetIdentity().GetId() + " | target player=" + targetPlayerId);
                    if (player && targetPlayerId != string.Empty) {
                        clan = GetClanServerManager().GetClanById(player.GetActiveClanId());

                        Print(ClanStatic.debugPrefix + "RPC Demote Member | Conditionals met, searching for clan | clan id=" + player.GetActiveClanId() + " | clan=" + clan);
                        if (clan && clan.IsPlayerInClan(player.GetIdentity().GetId()) && clan.IsPlayerInClan(targetPlayerId)) {
                            clanMemberRank = clan.GetClanMemberByPlayerId(player.GetIdentity().GetId()).GetPlayerRank();

                            Print(ClanStatic.debugPrefix + "RPC Promote Member | Second conditionals met, validating permissions");
                            if (GetClanManager().GetConfig().CanDemoteMembers(clanMemberRank)) {
                                targetClanMemberRank = clan.GetClanMemberByPlayerId(targetPlayerId).GetPlayerRank();

                                if (clanMemberRank < targetClanMemberRank && !GetClanManager().GetConfig().IsLowestRank(targetClanMemberRank)) {
                                    Print(ClanStatic.debugPrefix + "RPC Demote Member | Demoting member");
                                    clan.DemoteMember(targetPlayerId);
                                }
                            }
                        }
                    }
                    break;
                }
            case ClanRPCEnum.ServerContributeFundsToClan:
                {
                    Print(ClanStatic.debugPrefix + "RPC Contribute Funds | RPC Received!");
                    Param1<int> dataContributeFunds;
                    if (!ctx.Read(dataContributeFunds)) { return; }

                    player = PlayerBase.Cast(target);
                    fundsToContribute = dataContributeFunds.param1;

                    Print(ClanStatic.debugPrefix + "RPC Contribute Funds | Data read | player=" + player.GetIdentity().GetId() + " | funds amount=" + fundsToContribute);
                    if (player) {
                        clan = GetClanServerManager().GetClanById(player.GetActiveClanId());

                        Print(ClanStatic.debugPrefix + "RPC Contribute Funds | Conditionals met, searching for clan | clan id=" + player.GetActiveClanId() + " | clan=" + clan);
                        if (clan && clan.IsPlayerInClan(player.GetIdentity().GetId())) {
                            Print(ClanStatic.debugPrefix + "RPC Contribute Funds | Second conditionals met, validating permissions!");
                            if (GetClanManager().GetConfig().CanContributeFunds(clan.GetClanMemberByPlayerId(player.GetIdentity().GetId()).GetPlayerRank())) {
                                if (GetClanServerManager().HasEnoughCurrency(player, fundsToContribute, mapCurrencyItems)) {
                                    Print(ClanStatic.debugPrefix + "RPC Contribute Funds | Player has enough currency, contributing!");
                                    GetClanServerManager().DeductPlayerCurrency(player, fundsToContribute, mapCurrencyItems);
                                    GetClanServerManager().AddPlayerContributions(player.GetIdentity().GetPlainId(), fundsToContribute);
                                    clan.AddFunds(fundsToContribute);
                                    clan.AddMemberContributions(player.GetIdentity().GetId(), fundsToContribute);
                                } else {
                                    Print(ClanStatic.debugPrefix + "RPC Contribute Funds | Player does not have enough currency!");
                                    auto paramsManageMenu = new Param1<int>(ClanCreateErrorEnum.NO_FUNDS);

                                    GetGame().RPCSingleParam(player, ClanRPCEnum.ClientManageMenu, paramsManageMenu, true, player.GetIdentity());
                                }
                            }
                        }
                    }
                    break;
                }
            case ClanRPCEnum.ServerKickFromClan:
                {
                    Print(ClanStatic.debugPrefix + "RPC Kick Member | RPC Received!");
                    Param1<string> dataKickFromClan;
                    if (!ctx.Read(dataKickFromClan)) { return; }

                    player = PlayerBase.Cast(target);
                    targetPlayerId = dataKickFromClan.param1;

                    Print(ClanStatic.debugPrefix + "RPC Kick Member | Data read | player=" + player.GetIdentity().GetId() + " | target player=" + targetPlayerId);
                    if (player) {
                        clan = GetClanServerManager().GetClanById(player.GetActiveClanId());

                        Print(ClanStatic.debugPrefix + "RPC Kick Member | Conditionals met, searching for clan | clan id=" + player.GetActiveClanId() + " | clan=" + clan);
                        if (clan && clan.IsPlayerInClan(player.GetIdentity().GetId()) && clan.IsPlayerInClan(targetPlayerId)) {
                            clanMemberRank = clan.GetClanMemberByPlayerId(player.GetIdentity().GetId()).GetPlayerRank();

                            Print(ClanStatic.debugPrefix + "RPC Kick Member | Second conditionals met, validating permissions!");
                            if (GetClanManager().GetConfig().CanKickMembers(clanMemberRank)) {
                                targetClanMemberRank = clan.GetClanMemberByPlayerId(targetPlayerId).GetPlayerRank();

                                Print(ClanStatic.debugPrefix + "RPC Kick Member | Third conditionals met, checking hierarchy!");
                                if (clanMemberRank < targetClanMemberRank) {
                                    targetPlayer = GetClanServerManager().GetPlayerBaseById(targetPlayerId);

                                    Print(ClanStatic.debugPrefix + "RPC Kick Member | Rank is lower, searching for active player!");
                                    if (targetPlayer) {
                                        Print(ClanStatic.debugPrefix + "RPC Kick Member | Player found, removing!");
                                        GetClanServerManager().RemoveMemberFromClan(clan, targetPlayer);
                                    } else {
                                        Print(ClanStatic.debugPrefix + "RPC Kick Member | Player not found, removing!");
                                        GetClanServerManager().RemoveMemberFromClan(clan, targetPlayerId);
                                    }
                                }
                            }
                        }
                    }
                    break;
                }
            case ClanRPCEnum.ServerLeaveClan:
                {
                    Print(ClanStatic.debugPrefix + "RPC Leave Clan | RPC Received!");
                    player = PlayerBase.Cast(target);

                    Print(ClanStatic.debugPrefix + "RPC Leave Clan | Data Read | player=" + player.GetIdentity().GetId());
                    if (player) {
                        clan = GetClanServerManager().GetClanById(player.GetActiveClanId());

                        Print(ClanStatic.debugPrefix + "RPC Leave Clan | Conditionals met, searching for clan | clan id=" + player.GetActiveClanId() + " | clan=" + clan);
                        if (clan) {
                            Print(ClanStatic.debugPrefix + "RPC Leave Clan | Clan found! | owner id=" + clan.GetOwnerId());
                            if (clan.GetOwnerId() == player.GetIdentity().GetId()) {
                                Print(ClanStatic.debugPrefix + "RPC Leave Clan | Owner left, deleting!");
                                GetClanServerManager().DeleteClan(clan);
                            } else {
                                Print(ClanStatic.debugPrefix + "RPC Leave Clan | Player left, removing member!");
                                GetClanServerManager().RemoveMemberFromClan(clan, player);
                            }
                        }
                    }
                    break;
                }
            case ClanRPCEnum.ServerUpgradeClan:
                {
                    Print(ClanStatic.debugPrefix + "RPC Upgrade Clan | RPC Received!");
                    player = PlayerBase.Cast(target);

                    Print(ClanStatic.debugPrefix + "RPC Upgrade Clan | Data Read! | player=" + player.GetIdentity().GetId());
                    if (player) {
                        clan = GetClanServerManager().GetClanById(player.GetActiveClanId());

                        Print(ClanStatic.debugPrefix + "RPC Upgrade Clan | Conditionals met, searching for clan | clan id=" + player.GetActiveClanId() + " | clan=" + clan);
                        if (clan && clan.IsPlayerInClan(player.GetIdentity().GetId())) {
                            Print(ClanStatic.debugPrefix + "RPC Upgrade Clan | Scond conditionals met, validating permissions!");
                            if (GetClanManager().GetConfig().CanUpgradeClan(clan.GetClanMemberByPlayerId(player.GetIdentity().GetId()).GetPlayerRank())) {
                                if (clan.CanUpgradeClan(clanUpgradeCost)) {
                                    Print(ClanStatic.debugPrefix + "RPC Upgrade Clan | Enough funds found, upgrading clan!");
                                    clan.UpgradeClan();
                                    clan.RemoveFunds(clanUpgradeCost);
                                    GetClanServerManager().SortClans();
                                }
                            }
                        }
                    }
                    break;
                }
            case ClanRPCEnum.ServerDeleteClan:
                {
                    Print(ClanStatic.debugPrefix + "RPC Delete Clan | RPC Received!");
                    player = PlayerBase.Cast(target);

                    Print(ClanStatic.debugPrefix + "RPC Delete Clan | Data Read! | player=" + player.GetIdentity().GetId());
                    if (player) {
                        clan = GetClanServerManager().GetClanById(player.GetActiveClanId());
                        
                        Print(ClanStatic.debugPrefix + "RPC Delete Clan | Conditionals met, searching for clan | clan id=" + player.GetActiveClanId() + " | clan=" + clan + " | owner id=" + clan.GetOwnerId());
                        if (clan && clan.GetOwnerId() == player.GetIdentity().GetId()) {
                            Print(ClanStatic.debugPrefix + "RPC Delete Clan | Owner sent RPC, deleting clan!");
                            GetClanServerManager().DeleteClan(clan);
                            player.SetActiveClanId(string.Empty);
                        }
                    }
                    break;
                }
        }
    }
}