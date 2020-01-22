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
        if (ClanMemberRanks.Count() < 1) {
            for (int i = 0; i < 2; i++) {
                ClanMemberRank rank = new ClanMemberRank(i, "EXAMPLE, CHANGE ME");
                ClanMemberRanks.Insert(rank);
            }
        }
        Save();
    }

    void Save() {
        JsonFileLoader<ClanConfig>.JsonSaveFile(ClanStatic.baseModDir + "\\ClanConfig.json", this);
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