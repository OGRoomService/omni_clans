class ActiveClan : Clan {
    private ref array<ref ClanMemberTracker> arrayTrackers;
    private ref array<string> arrayInvitations;

    void ActiveClan(string id, string pName, string name) {
        arrayTrackers = new array<ref ClanMemberTracker>();
        arrayInvitations = new array<string>();
    }

    void ~ActiveClan() {
        GetGame().GetCallQueue(CALL_CATEGORY_SYSTEM).Remove(SynchMembers);
    }

    void InitTicker() {
        if (GetGame().IsServer() && GetGame().IsMultiplayer()) {
            GetGame().GetCallQueue(CALL_CATEGORY_SYSTEM).Remove(SynchMembers);
            GetGame().GetCallQueue(CALL_CATEGORY_SYSTEM).CallLater(SynchMembers, GetClanManager().GetConfig().GetTrackerUpdateInterval() * 1000, true);
        }
    }

    // Revisit these. Maybe it's better to have the client complete these actions as well instead of constantly sending over the entire clan

    void AddTracker(PlayerBase player) {
        if (!GetGame().IsServer() || !GetGame().IsMultiplayer()) { return; }
        
        //RemoveTracker(player);
        arrayTrackers.Insert(new ClanMemberTracker(player));
        SendRPC();
    }

    void RemoveTracker(PlayerBase player) {
        if (!GetGame().IsServer() || !GetGame().IsMultiplayer()) { return; }

        string playerId = player.GetIdentity().GetId();
        string lastPlayerId;
        
        for (int i = 0; i < arrayTrackers.Count(); i++) {
            ref ClanMemberTracker tracker = arrayTrackers[i];
            PlayerBase trackerPlayer = tracker.GetPlayer();
            string trackerPlayerId = tracker.GetId();

            if (!trackerPlayer || lastPlayerId == trackerPlayerId || trackerPlayerId == playerId) {
                Print("Tracker found was either a duplicate, old, or nulled");
                arrayTrackers.Remove(i);
                delete tracker;
            }
            lastPlayerId = trackerPlayerId;
        }
    }

    void AddInvitation(string playerId) {
        if (!GetGame().IsServer() || !GetGame().IsMultiplayer()) { return; }
        if (playerId != string.Empty) {
            if (arrayInvitations.Find(playerId) == -1) {
                arrayInvitations.Insert(playerId);
                // Only store the playerId for 30 seconds;
                GetGame().GetCallQueue(CALL_CATEGORY_SYSTEM).CallLater(RemoveInvitation, 30000, false, playerId);
                SendRPC();
            }
        }
        arrayInvitations.Debug();
    }

    void RemoveInvitation(string playerId) {
        if (!GetGame().IsServer() || !GetGame().IsMultiplayer()) { return; }
        if (arrayInvitations.Find(playerId) != -1) {
            arrayInvitations.RemoveItem(playerId);
            SendRPC();
        }
        arrayInvitations.Debug();
    }

    void SynchMembers() {
        if (!GetGame().IsServer() || !GetGame().IsMultiplayer()) { return; }

        bool needsSynch = false;

        // I need to add a position check for this. Once every 5 cycles instead of 1 cycle.

        foreach (ClanMemberTracker tracker : arrayTrackers) {
            if (tracker.NeedsClanSynch()) {
                needsSynch = true;
            }
        }
        if (needsSynch) {
            Print("Client needs clan update");
            SendRPC();
        }
    }

    void SendRPC() {
        if (!GetGame().IsServer() || !GetGame().IsMultiplayer()) { return; }
        
        foreach (ClanMemberTracker tracker : arrayTrackers) {
            auto params = new Param1<ref ActiveClan>(this);
            GetGame().RPCSingleParam(tracker.GetPlayer(), ClanRPCEnum.ClientUpdateClan, params, true, tracker.GetIdentity());
        }
    }

    void Test() {
        foreach (ClanMemberTracker tracker : arrayTrackers) {
            Print("pos=" + tracker.GetPlayer().GetPosition());
            Print("blood=" + tracker.GetCVal("blood", true));
            Print("health=" + tracker.GetCVal("health", true));
            Print("shock=" + tracker.GetCVal("shock", true));
        }
    }

    bool IsPlayerInvited(string playerId) {
        bool found = false;

        Print("Checking if a player is invited to the clan");
        arrayInvitations.Debug();

        foreach (string id : arrayInvitations) {
            if (id != string.Empty) {
                if (id == playerId) {
                    found = true;
                }
            } else {
                arrayInvitations.RemoveItem(id);
            }
        }
        arrayInvitations.Debug();
        return found;
    }

    bool IsPlayerActive(PlayerBase player) {
        foreach (ClanMemberTracker tracker : arrayTrackers) {
            if (tracker.GetPlayer() == player) {
                return true;
            }
        }
        return false;
    }

    ref array<ref ClanMemberTracker> GetTrackers() {
        return arrayTrackers;
    }

    int CountOnline() {
        return arrayTrackers.Count();
    }
}