class ClanHud {
    private ref Widget wRoot;
    private ref array<ref ClanVisualTracker> arrayVisualTrackers;

    void ClanHud(ref ActiveClan activeClan) {
        arrayVisualTrackers = new array<ref ClanVisualTracker>();
        wRoot = GetGame().GetWorkspace().CreateWidgets("omni_clans\\gui\\layouts\\ClanHud.layout", NULL);

        InitVisualTrackers();
    }

    void InitVisualTrackers() {
        delete arrayVisualTrackers;
        arrayVisualTrackers = new array<ref ClanVisualTracker>();

        foreach (ClanMemberTracker tracker : GetClanClientManager().GetClan().GetTrackers()) {
            if (tracker && tracker.GetPlayerId() != GetGame().GetPlayer().GetIdentity().GetId()) {
                AddVisualTracker(tracker);
            }
        }
    }

    void Update() {
        // Not sure what I'll be doing here
    }

    void UpdateTrackerColors() {
        foreach (ClanVisualTracker visualTracker : arrayVisualTrackers) {
            if (visualTracker) {
                visualTracker.UpdateChevronColor();
            }
        }
    }

    void AddVisualTracker(ref ClanMemberTracker tracker) {
        if (tracker.GetPlayerId() != GetGame().GetPlayer().GetIdentity().GetId()) {
            Print(ClanStatic.debugPrefix + "ADDING VISUAL TRACKER");
            ClanVisualTracker newVisualTracker = new ClanVisualTracker(tracker, wRoot);
            arrayVisualTrackers.Insert(newVisualTracker)
        }
    }

    void RemoveVisualTracker(string playerId) {
        foreach (ClanVisualTracker visualTracker : arrayVisualTrackers) {
            if (visualTracker.GetPlayerId() == playerId) {
                Print(ClanStatic.debugPrefix + "REMOVING VISUAL TRACKER " + playerId + " || visual tracker " + visualTracker);
                arrayVisualTrackers.RemoveItem(visualTracker);
                delete visualTracker;
                return;
            }
        }
    }

    bool DoesTrackerExist(string playerId) {
        foreach (ClanVisualTracker visualTracker : arrayVisualTrackers) {
            if (visualTracker && visualTracker.GetPlayerId() == playerId) {
                return true;
            }
        }
        return false;
    }
}