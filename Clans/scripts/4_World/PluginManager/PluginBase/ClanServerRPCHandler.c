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
        ref ClanMember clanMember, targetClanMember;
        PlayerBase player, targetPlayer;
        string clanName, error, playerId, targetPlayerId;
        int clanMemberRank, targetClanMemberRank;

        switch (rpc_type) {
            case ClanRPCEnum.ServerCreateClan:
                {
                    Param1<string> dataCreateClan;
                    if (!ctx.Read(dataCreateClan)) { return; }

                    // Add filter here. Check a file against the name. Load the entire file as an array of string
                    // and loop through it and check if the string contains and of the elements, don't allow it to be created
                    // if it does.

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
                    Param1<string> dataAddInvitation;
                    if (!ctx.Read(dataAddInvitation)) { return; }

                    Print("Received rpc to attach invite");

                    player = PlayerBase.Cast(target);
                    targetPlayerId = dataAddInvitation.param1;

                    if (player && targetPlayerId != string.Empty) {
                        clan = GetClanServerManager().GetActiveClanByPlayerIdentity(player.GetIdentity());

                        if (clan && clan.IsMember(player.GetIdentity().GetPlainId())) {
                            targetPlayer = GetClanServerManager().GetPlayerBaseById(targetPlayerId);
                            
                            if (targetPlayer) {
                                clan.AddInvitation(targetPlayerId);

                                auto paramsInvite = new Param1<string>(clanName);
                                GetGame().RPCSingleParam(targetPlayer, ClanRPCEnum.ClientReceiveInvite, paramsInvite, true, targetPlayer.GetIdentity());
                            }
                        }
                    }
                    break;
                }
            case ClanRPCEnum.ServerManageInvite:
                {
                    Param1<string> dataManageInvite;
                    if (!ctx.Read(dataManageInvite)) { return; }

                    player = PlayerBase.Cast(target);
                    string selectedOption = dataManageInvite.param1;

                    Print("Received rpc to manage invite by player");
                    // This is fucked. Not even sure what's happening
                    // FIX THIS It's currently only allowing players already in the clan to be invited
                    // FIX THIS
                    // FIX THIS
                    // FIX THIS
                    // FIX THIS
                    // FIX THIS
                    // FIX THIS
                    // FIX THIS
                    // FIX THIS
                    // FIX THIS

                    if (player) {
                        clan = GetClanServerManager().GetActiveClanByPlayerIdentity(player.GetIdentity());
                        playerId = player.GetIdentity().GetPlainId();

                        if (clan && clan.IsMember(playerId)) {
                            // Get the clan member, find their permissions, and go from there... If they can actually invite a user, allow it
                            if (selectedOption == "btnAcceptInvite" && clan.IsPlayerInvited(playerId)) {
                                clan.AddMember(player.GetIdentity().GetName(), playerId);
                                clan.AddTracker(player);
                            }
                            clan.RemoveInvitation(playerId);
                        }
                    }
                    break;
                }
            case ClanRPCEnum.ServerPromoteClanMember:
                {
                    Print("Received promotion RPC");
                    Param1<string> dataPromoteMember;
                    if (!ctx.Read(dataPromoteMember)) { return; }

                    player = PlayerBase.Cast(target);
                    playerId = player.GetIdentity().GetPlainId();
                    targetPlayerId = dataPromoteMember.param1;

                    if (player && targetPlayerId != string.Empty) {
                        clan = GetClanServerManager().GetActiveClanByPlayerIdentity(player.GetIdentity());

                        if (clan) {
                            clanMember = clan.GetMember(playerId);

                            if (clanMember) {
                                targetClanMember = clan.GetMember(targetPlayerId);

                                if (targetClanMember) {
                                    clanMemberRank = clanMember.GetRank();
                                    targetClanMemberRank = targetClanMember.GetRank();

                                    if (clanMemberRank < targetClanMemberRank && !GetClanManager().GetConfig().IsLowestRank(targetClanMemberRank) && !GetClanManager().GetConfig().IsHighest(targetClanMemberRank)) {
                                        if (GetClanManager().GetConfig().CanPromoteMembers(clanMemberRank)) {
                                            clan.PromoteMember(targetPlayerId);
                                        }
                                    }
                                }
                            } else {
                                GetClanServerManager().RemovePlayerFromActiveClan(player);
                            }
                        }
                    }
                    break;
                }
            case ClanRPCEnum.ServerDemoteClanMember:
                {
                    Print("Received demotion RPC");
                    Param1<string> dataDemoteMember;
                    if (!ctx.Read(dataDemoteMember)) { return; }

                    player = PlayerBase.Cast(target);
                    playerId = player.GetIdentity().GetPlainId();
                    targetPlayerId = dataDemoteMember.param1;

                    if (player && targetPlayerId != string.Empty) {
                        clan = GetClanServerManager().GetActiveClanByPlayerIdentity(player.GetIdentity());

                        if (clan) {
                            clanMember = clan.GetMember(playerId);

                            if (clanMember) {
                                targetClanMember = clan.GetMember(targetPlayerId);

                                if (targetClanMember) {
                                    clanMemberRank = clanMember.GetRank();
                                    targetClanMemberRank = targetClanMember.GetRank();

                                    if (clanMemberRank < targetClanMemberRank && !GetClanManager().GetConfig().IsLowestRank(targetClanMemberRank) && !GetClanManager().GetConfig().IsHighest(targetClanMemberRank)) {
                                        if (GetClanManager().GetConfig().CanDemoteMembers(clanMemberRank)) {
                                            clan.DemoteMember(targetPlayerId);
                                        }
                                    }
                                }
                            } else {
                                GetClanServerManager().RemovePlayerFromActiveClan(player);
                            }
                        }
                    }
                    break;
                }
            case ClanRPCEnum.ServerDeleteClan:
                {
                    Print("Received clan deletion RPC");
                    player = PlayerBase.Cast(target);

                    if (player) {
                        clan = GetClanServerManager().GetActiveClanByPlayerIdentity(player.GetIdentity());
                        playerId = player.GetIdentity().GetPlainId();

                        if (clan && clan.GetOwnerId() == playerId) {
                            string clanDir = ClanStatic.clanDirectory + "\\" + clan.GetName() + ClanStatic.fileExtension;
                            ref array<ref ClanMemberTracker> clanTrackers = clan.GetTrackers();
                            
                            clan.DeleteClanRPC();
                            GetClanServerManager().RemoveAllPlayersFromActiveClan(clanTrackers);
                            GetClanServerManager().RemoveActiveClan(clan);

                            if (FileExist(clanDir)) {
                                DeleteFile(clanDir);
                            }
                        }
                    }
                    break;
                }
        }
    }
}