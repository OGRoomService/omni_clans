class ActiveClan : Clan {
    private ref array<ref ClanMemberTracker> arrayTrackers;
    private ref array<string> arrayInvitations;

    void ActiveClan(string clanName, string ownerName, string ownerId, string ownerPlainId) {
        arrayTrackers = new array<ref ClanMemberTracker>();
        arrayInvitations = new array<string>();
    }

    void ~ActiveClan() {
        StopUpdateLoops();
    }

    void Init() {
        if (GetGame().IsServer() && GetGame().IsMultiplayer()) {
            StartUpdateLoops();
        }
        foreach (ClanMemberTracker tracker : arrayTrackers) {
            if (tracker) {
                if (!IsPlayerInClan(tracker.GetPlayerId())) {
                    RemoveTracker(tracker.GetPlayerId());
                    continue;
                }
                tracker.Init();
            }
        }
    }

    void StartUpdateLoops() {
        Print(ClanStatic.debugPrefix + "ActiveClan | StartUpdateLoops | Starting update loops");
        StopUpdateLoops();
        GetGame().GetCallQueue(CALL_CATEGORY_SYSTEM).CallLaterByName(this, "SyncTrackers", GetClanManager().GetConfig().GetTrackerPositionUpdateInterval() * 1000, true);
    }

    void StopUpdateLoops() {
        Print(ClanStatic.debugPrefix + "ActiveClan | StopUpdateLoops | Stopping update loops");
        GetGame().GetCallQueue(CALL_CATEGORY_SYSTEM).RemoveByName(this, "SyncTrackers");
    }

    /*
     * Critical functions for data management
     */

    override void AddFunds(int amount) {
        super.AddFunds(amount);

        if (GetGame().IsServer() && GetGame().IsMultiplayer()) {
            auto rpc_params = new array<ref Param>();
            auto params = new Param1<int>(amount);

            rpc_params.Insert(params);
            SendRPC(ClanRPCEnum.ClientAddClanFunds, rpc_params);
        }
    }

    override void RemoveFunds(int amount) {
        super.RemoveFunds(amount);

        if (GetGame().IsServer() && GetGame().IsMultiplayer()) {
            auto rpc_params = new array<ref Param>();
            auto params = new Param1<int>(amount);

            rpc_params.Insert(params);
            SendRPC(ClanRPCEnum.ClientRemoveClanFunds, rpc_params);
        }
    }

    override void UpgradeClan() {
        super.UpgradeClan();

        if (GetGame().IsServer() && GetGame().IsMultiplayer()) {
            auto rpc_params = new array<ref Param>();

            SendRPC(ClanRPCEnum.ClientUpgradeClan, rpc_params);
        }
    }

    override bool AddMemberContributions(string playerId, int contributionAmount) {
        if (super.AddMemberContributions(playerId, contributionAmount)) {
            if (GetGame().IsServer() && GetGame().IsMultiplayer()) {
                ref ClanMember member = GetClanMemberByPlayerId(playerId);
                auto rpc_params = new array<ref Param>();
                auto params = new Param2<string, int>(playerId, contributionAmount);

                rpc_params.Insert(params);
                SendRPC(ClanRPCEnum.ClientAddClanMemberContributions, rpc_params);
            }
        }
        return true;
    }

    /*
     * Critical functions for data management
     */

    /*
     * Critical functions for member management
     */

    override void AddMember(string playerName, string playerId, string playerPlainId) {
        super.AddMember(playerName, playerId, playerPlainId);

        if (GetGame().IsServer() && GetGame().IsMultiplayer()) {
            auto rpc_params = new array<ref Param>();
            auto params = new Param3<string, string, string>(playerName, playerId, playerPlainId);

            rpc_params.Insert(params);
            SendRPC(ClanRPCEnum.ClientAddClanMember, rpc_params);
        }
    }

    override bool RemoveMember(string playerId) {
        if (super.RemoveMember(playerId)) {
            if (GetGame().IsServer() && GetGame().IsMultiplayer()) {
                auto rpc_params = new array<ref Param>();
                auto params = new Param1<string>(playerId);

                rpc_params.Insert(params);
                SendRPC(ClanRPCEnum.ClientRemoveClanMember, rpc_params);
            }
        }
        return true;
    }

    override bool UpdateMemberName(string playerId, string playerName) {
        if (super.UpdateMemberName(playerId, playerName)) {
            if (GetGame().IsServer() && GetGame().IsMultiplayer()) {
                auto rpc_params = new array<ref Param>();
                auto params = new Param2<string, string>(playerId, playerName);

                rpc_params.Insert(params);
                SendRPC(ClanRPCEnum.ClientUpdateClanMember, rpc_params);
            }
        }
        return true;
    }

    override bool PromoteMember(string playerId) {
        if (super.PromoteMember(playerId)) {
            if (GetGame().IsServer() && GetGame().IsMultiplayer()) {
                auto rpc_params = new array<ref Param>();
                auto params = new Param1<string>(playerId);

                rpc_params.Insert(params);
                SendRPC(ClanRPCEnum.ClientPromoteClanMember, rpc_params);
            }
        }
        return true;
    }

    override bool DemoteMember(string playerId) {
        if (super.DemoteMember(playerId)) {
            if (GetGame().IsServer() && GetGame().IsMultiplayer()) {
                auto rpc_params = new array<ref Param>();
                auto params = new Param1<string>(playerId);

                rpc_params.Insert(params);
                SendRPC(ClanRPCEnum.ClientDemoteClanMember, rpc_params);
            }
        }
        return true;
    }

    /*
     * Critical functions for member management
     */

    /*
     * Critical functions for tracker management
     */

    void CheckPlayerForActiveTracker(PlayerBase player) {
        PlayerIdentity playerIdentity = player.GetIdentity();

        ClanMemberTracker tracker = GetTrackerByPlayerId(player.GetIdentity().GetId());

        Print(ClanStatic.debugPrefix + "ActiveClan | CheckPlayerForActiveTracker | Searching for old tracker | player id=" + player.GetIdentity().GetId());
        if (tracker) {
            Print(ClanStatic.debugPrefix + "ActiveClan | CheckPlayerForActiveTracker | Found old tracker | tracker id=" + tracker.GetPlayerId());
            tracker.SetPlayer(player);
        } else {
            auto rpc_params = new array<ref Param>();
            auto params = new Param3<string, string, vector>(player.GetIdentity().GetId(), player.GetIdentity().GetName(), player.GetPosition());
            tracker = new ClanMemberTracker(player.GetIdentity().GetName(), player.GetIdentity().GetId(), player.GetPosition());

            rpc_params.Insert(params);
            tracker.SetPlayer(player);
            SendRPC(ClanRPCEnum.ClientAddClanTracker, rpc_params);
            AddTracker(tracker);

            Print(ClanStatic.debugPrefix + "ActiveClan | CheckPlayerForActiveTracker | Adding new tracker | tracker count=" + arrayTrackers.Count());

            if (arrayTrackers.Count() == 1) {
                Print(ClanStatic.debugPrefix + "ActiveClan | CheckPlayerForActiveTracker | Starting update loops");
                StartUpdateLoops();
            }
        }
    }

    void AddTracker(string playerId, string playerName, vector pos) {
        ClanMemberTracker newTracker = new ClanMemberTracker(playerName, playerId, pos);

        AddTracker(newTracker);
    }

    void AddTracker(ClanMemberTracker trackerToAdd) {
        trackerToAdd.Init();
        arrayTrackers.Insert(trackerToAdd);
    }

    void RemoveTracker(string playerId) {
        ref ClanMemberTracker playerTracker = GetTrackerByPlayerId(playerId);

        if (playerTracker) {
            if (!GetGame().IsServer() || !GetGame().IsMultiplayer()) {
                GetClanClientManager().GetHud().RemoveVisualTracker(playerTracker.GetPlayerId());
            }
            arrayTrackers.RemoveItem(playerTracker);

            if (arrayTrackers.Count() == 0) {
                StopUpdateLoops();
                Save();
            } else {
                auto rpc_params = new array<ref Param>();
                auto params = new Param1<string>(playerId);

                rpc_params.Insert(params);
                SendRPC(ClanRPCEnum.ClientRemoveClanTracker, rpc_params);
            }
        }
    }

    void UpdateTracker(string playerId, vector playerPosition) {
        ref ClanMemberTracker tracker = GetTrackerByPlayerId(playerId);

        if (tracker) {
            tracker.SetPlayerPosition(playerPosition);
        }
    }

    void SyncTrackers() {
        Print(ClanStatic.debugPrefix + "ActiveClan | SyncTrackers | Checking trackers for update!");
        int trackerCount = arrayTrackers.Count();

        if (trackerCount == 0) {
            Print(ClanStatic.debugPrefix + "ActiveClan | SyncTrackers | No trackers found! Stopping update loops");
            StopUpdateLoops();
            return;
        }
        for (int i = (trackerCount - 1); i >= 0; i--) {
            ClanMemberTracker tracker = arrayTrackers[i];

            if (!tracker) {
                arrayTrackers.Remove(i);
                continue;
            } else if (!tracker.GetPlayer()) {
                RemoveTracker(tracker.GetPlayerId());
                continue;
            }
            if (tracker.IsPlayerPositionOOS()) {
                array<ref Param> rpc_params = new array<ref Param>();
                auto params = new Param2<string, vector>(tracker.GetPlayerId(), tracker.GetPosition());

                rpc_params.Insert(params);
                SendRPC(ClanRPCEnum.ClientUpdateClanTracker, rpc_params);
            }
        }
    }

    /*
     * Critical functions for tracker management
     */

    /*
     * Critical functions for invitation management
     */

    void AddInvitation(string playerId, string playerName = "") {
        if (!IsPlayerInvited(playerId)) {
            arrayInvitations.Insert(playerId);

            if (GetGame().IsServer() && GetGame().IsMultiplayer()) {
                array<ref Param> rpc_params = new array<ref Param>();
                auto params = new Param2<string, string>(playerId, playerName);

                rpc_params.Insert(params);
                SendRPC(ClanRPCEnum.ClientAddClanInvitation, rpc_params);
                GetGame().GetCallQueue(CALL_CATEGORY_SYSTEM).CallLater(this.RemoveInvitation, 30000, false, playerId);
            }
        }
        arrayInvitations.Debug();
    }

    void RemoveInvitation(string playerId) {
        if (IsPlayerInvited(playerId)) {
            arrayInvitations.RemoveItem(playerId);

            if (GetGame().IsServer() && GetGame().IsMultiplayer()) {
                array<ref Param> rpc_params = new array<ref Param>();
                auto params = new Param1<string>(playerId);
                PlayerBase targetPlayer = GetClanServerManager().GetPlayerBaseById(playerId);

                if (targetPlayer) {
                    PlayerIdentity targetPlayerIdentity = targetPlayer.GetIdentity();

                    GetGame().RPCSingleParam(targetPlayer, ClanRPCEnum.ClientDeleteInvite, null, true, targetPlayerIdentity);
                }
                rpc_params.Insert(params);
                SendRPC(ClanRPCEnum.ClientRemoveClanInvitation, rpc_params);
            }
        }
        arrayInvitations.Debug();
    }

    bool IsPlayerInvited(string playerId) {
        Print(ClanStatic.debugPrefix + "ActiveClan | IsPlayerInvited | Searching for invitation! " + playerId);
        foreach (string id : arrayInvitations) {
            if (id == playerId) {
                Print(ClanStatic.debugPrefix + "ActiveClan | IsPlayerInvited | Invitation found! " + id);
                return true;
            }
        }
        return false;
    }

    /*
     * Critical functions for invitation management
     */

    /*
     * Critical functions for RPC management
     */

    void SendRPC(int rpc_type, array<ref Param> rpc_params) {
        if (!GetGame().IsServer() || !GetGame().IsMultiplayer()) { return; }
        foreach (ClanMemberTracker tracker : arrayTrackers) {
            PlayerIdentity trackerIdentity = tracker.GetIdentity();

            if (trackerIdentity) {
                GetGame().RPC(tracker.GetPlayer(), rpc_type, rpc_params, true, trackerIdentity);
            }
        }
    }

    /*
     * Critical functions for RPC management
     */

    /*
     * Getters
     */

    ref array<ref ClanMemberTracker> GetTrackers() {
        return arrayTrackers;
    }

    ref ClanMemberTracker GetTrackerByPlayerId(string playerId) {
        foreach (ClanMemberTracker tracker : arrayTrackers) {
            if (tracker) {
                if (tracker.GetPlayerId() == playerId) {
                    return tracker;
                }
            }
        }
        return null;
    }

    int CountOnline() {
        return arrayTrackers.Count();
    }

    /*
     * Getters
     */
}