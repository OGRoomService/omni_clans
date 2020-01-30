class ActiveClan : Clan {
    private ref array<ref ClanMemberTracker> arrayTrackers;
    private ref array<string> arrayInvitations;

    void ActiveClan(string id, string pName, string name) {
        arrayTrackers = new array<ref ClanMemberTracker>();
        arrayInvitations = new array<string>();
    }

    void ~ActiveClan() {
        GetGame().GetCallQueue(CALL_CATEGORY_SYSTEM).Remove(SyncTrackers);
    }

    void Init() {
        if (GetGame().IsServer() && GetGame().IsMultiplayer()) {
            GetGame().GetCallQueue(CALL_CATEGORY_SYSTEM).Remove(SyncTrackers);
            GetGame().GetCallQueue(CALL_CATEGORY_SYSTEM).CallLater(SyncTrackers, GetClanManager().GetConfig().GetTrackerUpdateInterval() * 1000, true);
        } else {
            foreach (ClanMemberTracker tracker : arrayTrackers) {
                GetGame().GetCallQueue(CALL_CATEGORY_GAMEPLAY).RemoveByName(tracker, "ClientUpdatePlayerBase");
                GetGame().GetCallQueue(CALL_CATEGORY_GAMEPLAY).CallLaterByName(tracker, "ClientUpdatePlayerBase", 1000, true);
            }
        }
    }

    void AddTracker(PlayerBase player = NULL, string playerId = "", string playerPlainId = "", string playerName = "", float hp = 0, float blood = 0, float shock = 0, vector pos = "0 0 0") {
        Print("Adding player tracker!");
        Print("playerId=" + playerId);
        Print("playerPlainId=" + playerId);
        Print("playerName=" + playerId);
        Print("e");
        Print("e");
        Print("e");
        Print("e");
        Print("e");
        Print("e");
        if (GetGame().IsServer() && GetGame().IsMultiplayer()) {
            if (player.GetIdentity()) {
                PlayerIdentity identity = player.GetIdentity()
                playerId = identity.GetId();
                playerPlainId = identity.GetPlainId();
                playerName = identity.GetName();
            }
            pos = player.GetPosition();
            hp = player.GetHealth("", "Health");
            blood = player.GetHealth("", "Blood");
            shock = player.GetHealth("", "Shock");
        }
        ClanMemberTracker tracker = new ClanMemberTracker(playerId, playerPlainId, playerName, hp, blood, shock, pos);

        //RemoveTracker(playerId);
        arrayTrackers.Insert(tracker);
        
        if (!GetGame().IsServer() || !GetGame().IsMultiplayer()) {
            GetClanClientManager().GetHud().AddVisualTracker(tracker);
            GetGame().GetCallQueue(CALL_CATEGORY_GAMEPLAY).CallLaterByName(tracker, "ClientUpdatePlayerBase", 1000, true);
            return;
        }
        tracker.SetPlayer(player);
        tracker.Init();
        // Send RPC for client to do the same shit
        AddTrackerRPC(playerId, playerPlainId, playerName, hp, blood, shock, pos);
    }

    void RemoveTracker(string playerId) {
        Print("Removing Player Tracker!");
        string lastPlayerId;
        
        for (int i = 0; i < arrayTrackers.Count(); i++) {
            ref ClanMemberTracker tracker = arrayTrackers[i];
            string trackerPlayerId = tracker.GetId();

            if (lastPlayerId == trackerPlayerId || trackerPlayerId == playerId) {
                Print("Tracker found was either a duplicate, old, or nulled");
                if (!GetGame().IsServer() || !GetGame().IsMultiplayer()) {
                    GetClanClientManager().GetHud().RemoveVisualTracker(tracker);
                }
                arrayTrackers.Remove(i);
                delete tracker;
            }
            lastPlayerId = trackerPlayerId;
        }
    }

    void UpdateTracker(string playerId, float hp, float blood, float shock, vector pos) {
        if (GetGame().IsServer() && GetGame().IsMultiplayer()) { return; }
        foreach (ClanMemberTracker tracker : arrayTrackers) {
            if (tracker.GetId() == playerId) {
                tracker.SetValues(hp, blood, shock, pos);
                break;
            }
        }
    }

    void AddInvitation(string playerId) {
        if (playerId != string.Empty) {
            if (arrayInvitations.Find(playerId) == -1) {
                arrayInvitations.Insert(playerId);
                
                if (!GetGame().IsServer() || !GetGame().IsMultiplayer()) { return; }
                // send RPC for client to add invite
            }
        }
        arrayInvitations.Debug();
    }

    void RemoveInvitation(string playerId) {
        if (arrayInvitations.Find(playerId) != -1) {
            arrayInvitations.RemoveItem(playerId);

            if (!GetGame().IsServer() || !GetGame().IsMultiplayer()) { return; }
            // send RPC for client to remove invite
        }
        arrayInvitations.Debug();
    }

    void SyncTrackers() {
        if (!GetGame().IsServer() || !GetGame().IsMultiplayer()) { return; }
        foreach (ClanMemberTracker tracker : arrayTrackers) {
            if (tracker.NeedsClanSynch()) {
                // Send rpc to sync tracker
                Print("CLIENT NEEDS UPDATE FROM TRACKER");
                UpdateTrackerRPC(tracker.GetId(), tracker.GetHealth(), tracker.GetBlood(), tracker.GetShock(), tracker.GetPosition());
            }
        }
    }

    void AddTrackerRPC(string playerId, string playerPlainId, string playerName, float hp, float blood, float shock, vector pos) {
        if (!GetGame().IsServer() || !GetGame().IsMultiplayer()) { return; }
        foreach (ClanMemberTracker tracker : arrayTrackers) {
            if (tracker.GetIdentity()) {
                Print("sending add tracker RPC " + tracker.GetPlainId());
                auto params = new Param7<string, string, string, float, float, float, vector>(playerId, playerPlainId, playerName, hp, blood, shock, pos);
                GetGame().RPCSingleParam(tracker.GetPlayer(), ClanRPCEnum.ClientAddClanTracker, params, true, tracker.GetIdentity());
            }
        }
    }

    void RemoveTrackerRPC(string playerId) {
        if (!GetGame().IsServer() || !GetGame().IsMultiplayer()) { return; }
        foreach (ClanMemberTracker tracker : arrayTrackers) {
            if (tracker.GetIdentity()) {
                Print("sending remove tracker RPC " + tracker.GetPlainId());
                auto params = new Param1<string>(playerId);
                GetGame().RPCSingleParam(tracker.GetPlayer(), ClanRPCEnum.ClientRemoveClanTracker, params, true, tracker.GetIdentity());
            }
        }
    }

    void UpdateTrackerRPC(string playerId, float hp, float blood, float shock, vector pos) {
        if (!GetGame().IsServer() || !GetGame().IsMultiplayer()) { return; }
        foreach (ClanMemberTracker tracker : arrayTrackers) {
            if (tracker.GetIdentity()) {
                Print("sending update tracker RPC " + tracker.GetPlainId());
                auto params = new Param5<string, float, float, float, vector>(playerId, hp, blood, shock, pos);
                GetGame().RPCSingleParam(tracker.GetPlayer(), ClanRPCEnum.ClientUpdateClanTracker, params, true, tracker.GetIdentity());
            }
        }
    }

    void AddInvitationRPC(string playerId) {
        if (!GetGame().IsServer() || !GetGame().IsMultiplayer()) { return; }
        foreach (ClanMemberTracker tracker : arrayTrackers) {
            if (tracker.GetIdentity()) {
                Print("sending add invite RPC " + tracker.GetPlainId());
                auto params = new Param1<string>(playerId);
                GetGame().RPCSingleParam(tracker.GetPlayer(), ClanRPCEnum.ClientAddClanInvitation, params, true, tracker.GetIdentity());
            }
        }
    }

    void RemoveInvitiationRPC(string playerId) {
        if (!GetGame().IsServer() || !GetGame().IsMultiplayer()) { return; }
        foreach (ClanMemberTracker tracker : arrayTrackers) {
            if (tracker.GetIdentity()) {
                Print("sending remove invite RPC " + tracker.GetPlainId());
                auto params = new Param1<string>(playerId);
                GetGame().RPCSingleParam(tracker.GetPlayer(), ClanRPCEnum.ClientRemoveClanInvitation, params, true, tracker.GetIdentity());
            }
        }
    }

    void Test() {
        Print("[DEBUG] DEBUGGING TRACKER ARRAY!!!!!!");
        arrayTrackers.Debug();
        foreach (ClanMemberTracker tracker : arrayTrackers) {
            Print("player=" + tracker.GetPlayer());
            Print("playerId=" + tracker.GetPlainId());
            Print("pos=" + tracker.GetPlayer().GetPosition());
            Print("blood=" + tracker.GetBlood(true));
            Print("health=" + tracker.GetHealth(true));
            Print("shock=" + tracker.GetShock(true));
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
        return found;
    }

    ref array<ref ClanMemberTracker> GetTrackers() {
        return arrayTrackers;
    }

    int CountOnline() {
        return arrayTrackers.Count();
    }
}