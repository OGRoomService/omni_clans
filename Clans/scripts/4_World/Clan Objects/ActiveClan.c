class ActiveClan : Clan {
    private ref array<ref ClanMemberTracker> onlinePlayers;
    private ref array<string> playerInvitations;

    void ActiveClan(string id, string pName, string name) {
        onlinePlayers = new array<ref ClanMemberTracker>();
        playerInvitations = new array<string>();
    }

    void ~ActiveClan() {
        GetGame().GetCallQueue(CALL_CATEGORY_SYSTEM).Remove(SynchMembers);
    }

    void InitTicker() {
        if (GetGame().IsServer() && GetGame().IsMultiplayer()) {
            GetGame().GetCallQueue(CALL_CATEGORY_SYSTEM).Remove(SynchMembers);
            GetGame().GetCallQueue(CALL_CATEGORY_SYSTEM).CallLater(SynchMembers, 1000, true);
        }
    }

    void AddActivePlayer(PlayerBase player) {
        if (!GetGame().IsServer() || !GetGame().IsMultiplayer()) { return; }

        string playerId = player.GetIdentity().GetId();
        string lastId;
        bool found = false;

        for (int i = 0; i < onlinePlayers.Count(); i++) {
            ref ClanMemberTracker tracker = onlinePlayers[i];
            PlayerBase p = tracker.GetPlayer();
            string pId = tracker.GetId();

            if (!p || lastId == pId || pId == playerId) {
                Print("Tracker found was either a duplicate, old, or nulled");
                delete tracker;
                onlinePlayers.Remove(i);
            }
        }
        onlinePlayers.Insert(new ClanMemberTracker(player));
        SendRPC();
    }

    void RemoveActivePlayer(PlayerBase player) {
    }

    void AddPlayerInvitation(string playerId) {
        if (!GetGame().IsServer() || !GetGame().IsMultiplayer()) { return; }
        if (playerId != string.Empty) {
            if (playerInvitations.Find(playerId) == -1) {
                playerInvitations.Insert(playerId);
                // Only store the playerId for 30 seconds;
                GetGame().GetCallQueue(CALL_CATEGORY_SYSTEM).CallLater(RemovePlayerInvitation, 30000, false, playerId);
                SendRPC();
            }
        }
        playerInvitations.Debug();
    }

    void RemovePlayerInvitation(string playerId) {
        if (!GetGame().IsServer() || !GetGame().IsMultiplayer()) { return; }
        Print("REMOVING PLAYER INVITE");
        if (playerInvitations.Find(playerId) != -1) {
            playerInvitations.RemoveItem(playerId);
            SendRPC();
        }
        playerInvitations.Debug();
    }

    void SynchMembers() {
        if (!GetGame().IsServer() || !GetGame().IsMultiplayer()) { return; }

        bool needsSynch = false;

        // I need to add a position check for this. Once every 5 cycles instead of 1 cycle.

        foreach (ClanMemberTracker tracker : onlinePlayers) {
            if (tracker.NeedsClanSynch()) {
                Print("Yay it's working");
                needsSynch = true;
            }
        }
        if (needsSynch) {
            Print("Client needs clan update");
            Print("Client needs clan update");
            Print("Client needs clan update");
            Print("Client needs clan update");
            Print("Client needs clan update");
            Print("Client needs clan update");
            Print("Client needs clan update");
            Print("Client needs clan update");
            Print("Client needs clan update");
            Print("Client needs clan update");
            SendRPC();
        }
    }

    void SendRPC() {
        if (!GetGame().IsServer() || !GetGame().IsMultiplayer()) { return; }
        foreach (ClanMemberTracker tracker : onlinePlayers) {
            auto params = new Param1<ref ActiveClan>(this);
            GetGame().RPCSingleParam(tracker.GetPlayer(), ClanRPCEnum.ClientUpdateClan, params, true, tracker.GetIdentity());
        }
    }

    void Test() {
        foreach (ClanMemberTracker tracker : onlinePlayers) {
            Print("pos=" + tracker.GetPlayer().GetPosition());
            Print("blood=" + tracker.GetCVal("blood", true));
            Print("health=" + tracker.GetCVal("health", true));
            Print("shock=" + tracker.GetCVal("shock", true));
        }
    }

    bool IsPlayerInvited(string playerId) {
        bool found = false;

        Print("Checking if a player is invited to the clan");
        playerInvitations.Debug();

        foreach (string id : playerInvitations) {
            if (id != string.Empty) {
                if (id == playerId) {
                    found = true;
                }
            } else {
                playerInvitations.RemoveItem(id);
            }
        }
        playerInvitations.Debug();
        return found;
    }

    ref array<ref ClanMemberTracker> GetOnlinePlayers() {
        return onlinePlayers;
    }

    int CountOnline() {
        return onlinePlayers.Count();
    }
}