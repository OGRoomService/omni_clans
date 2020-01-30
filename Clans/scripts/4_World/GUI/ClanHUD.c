class ClanHud {
    private ref Widget wRoot;
    private ref GridSpacerWidget rootGrid;
    private ref ActiveClan clan;
    private ref array<ref ClanMemberTracker> arrayTrackers;
    private ref map<ref ClanMemberTracker, ref ClanVisualTracker> mapVisualTrackers;

    void ClanHud(ref ActiveClan activeClan) {
        arrayTrackers = new array<ref ClanMemberTracker>();
        mapVisualTrackers = new map<ref ClanMemberTracker, ref ClanVisualTracker>();

        wRoot = GetGame().GetWorkspace().CreateWidgets("Clans\\layouts\\ClanHud.layout", NULL);
        rootGrid = GridSpacerWidget.Cast(wRoot.FindAnyWidget("gridPlayerData"));
        clan = activeClan;
        arrayTrackers = clan.GetTrackers();

        InitVisualTrackers();
    }

    void InitVisualTrackers() {
        foreach (ClanMemberTracker tracker : arrayTrackers) {
            ClanVisualTracker visualTracker = new ClanVisualTracker(tracker, wRoot, rootGrid);
            mapVisualTrackers.Set(tracker, visualTracker);
        }
    }

    void Update() {
        // Not sure what I'll be doing here
    }

    void AddVisualTracker(ref ClanMemberTracker tracker) {
        Print("ADDING VISUAL TRACKER");
        RemoveVisualTracker(tracker);
        
        ClanVisualTracker visualTracker = new ClanVisualTracker(tracker, wRoot, rootGrid);
        mapVisualTrackers.Set(tracker, visualTracker);
    }

    void RemoveVisualTracker(ref ClanMemberTracker tracker) {
        ref ClanVisualTracker visualTracker;

        if (mapVisualTrackers.Find(tracker, visualTracker)) {
            Print("REMOVING VISUAL TRACKER");
            mapVisualTrackers.Remove(tracker);
            delete visualTracker;
        }
    }
}