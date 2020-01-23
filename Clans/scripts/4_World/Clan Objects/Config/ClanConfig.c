class ClanConfig {
    private int TrackerSynchSensitivity;
    private int TrackerUpdateInterval;
    private int TrackerPositionUpdateInterval;
    private int TrackerPositionDistanceSensitivity;
    private ref array<ref ClanMemberRank> ClanMemberRanks;

    void ClanConfig() {
        ClanMemberRanks = new array<ref ClanMemberRank>();

        TrackerSynchSensitivity = 1;
        TrackerUpdateInterval = 1;
        TrackerPositionUpdateInterval = 5;
        TrackerPositionDistanceSensitivity = 25;
    }

    void Verify() {
        array<ref ClanMemberRank> ranks = new array<ref ClanMemberRank>();
        ranks = ClanMemberRanks;
        ClanMemberRanks = new array<ref ClanMemberRank>();

        if (TrackerSynchSensitivity < 1) {
            TrackerSynchSensitivity = 1;
        }
        if (TrackerUpdateInterval < 1) {
            TrackerUpdateInterval = 1;
        }
        if (TrackerPositionUpdateInterval < 1) {
            TrackerPositionUpdateInterval = 5;
        }
        if (TrackerPositionDistanceSensitivity < 1) {
            TrackerPositionDistanceSensitivity = 25;
        }
        foreach (ClanMemberRank r : ranks) {
            if (r) {
                SortRank(r);
            }
        }
        if (ClanMemberRanks.Count() < 1) {
            for (int i = 0; i < 2; i++) {
                ClanMemberRank rank = new ClanMemberRank(i, "EXAMPLE, CHANGE ME");
                ClanMemberRanks.Insert(rank);
            }
        }
        Save();
    }

    private void SortRank(ref ClanMemberRank rank) {
        int count = ClanMemberRanks.Count();
        bool inserted = false;

        if (count < 1) {
            ClanMemberRanks.Insert(rank);
        } else {
            for (int i = 0; i < count; i++) {
                ref ClanMemberRank r = ClanMemberRanks[i];

                if (r.GetRank() > rank.GetRank()) {
                    ClanMemberRanks.InsertAt(rank, i);
                    inserted = true;
                    break;
                }
            }
            if (!inserted) {
                ClanMemberRanks.Insert(rank);
            }
        }
    }

    void Save() {
        JsonFileLoader<ClanConfig>.JsonSaveFile(ClanStatic.baseModDir + "\\ClanConfig.json", this);
    }

    ref array<ref ClanMemberRank> GetRanks() {
        return ClanMemberRanks;
    }

    ref ClanMemberRank FindRank(int num) {
        ClanMemberRank rank;

        for (int i = 0; i < ClanMemberRanks.Count(); i++) {
            ClanMemberRank r = ClanMemberRanks[i];
            
            if (r) {
                if (r.GetRank() == num) {
                    rank = r;
                    break;
                }
            }
        }
        return rank;
    }

    int GetTrackerSynchSens() {
        return TrackerSynchSensitivity;
    }

    int GetTrackerUpdateInterval() {
        return TrackerUpdateInterval;
    }

    int GetTrackerPositionUpdateInterval() {
        return TrackerPositionUpdateInterval;
    }

    int GetTrackerPositionSens() {
        return TrackerPositionDistanceSensitivity;
    }
}