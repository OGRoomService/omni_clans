class ClanConfig {
    private int TrackerPositionUpdateInterval;
    private int TrackerPositionDistanceSensitivity;
    private string ClanOwnerRankName;
    private ref array<ref ClanMemberRank> ClanMemberRanks;

    void ClanConfig() {
        ClanMemberRanks = new array<ref ClanMemberRank>();

        ClanOwnerRankName == "President";
        TrackerPositionUpdateInterval = 5;
        TrackerPositionDistanceSensitivity = 25;
    }

    void Verify() {
        array<ref ClanMemberRank> ranks = new array<ref ClanMemberRank>();
        ranks = ClanMemberRanks;
        ClanMemberRanks = new array<ref ClanMemberRank>();

        if (TrackerPositionUpdateInterval < 1) {
            TrackerPositionUpdateInterval = 5;
        }
        if (TrackerPositionDistanceSensitivity < 1) {
            TrackerPositionDistanceSensitivity = 25;
        }
        if (ClanOwnerRankName == string.Empty) {
            ClanOwnerRankName = "President";
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

    string GetOwnerRankName() {
        return ClanOwnerRankName;
    }

    string GetRankName(int num) {
        string name;
        ClanMemberRank rank = FindRank(num);

        if (rank) {
            return rank.GetName();
        }
        return name;
    }

    bool IsHighestRank(int num) {
        bool isHighestRank = false;
        int count = ClanMemberRanks.Count();

        for (int i = 0; i < count; i++) {
            ClanMemberRank r = ClanMemberRanks[i];
            
            if (r) {
                if (r.GetRank() == num) {
                    if (i == 0) {
                        isHighestRank = true;
                    }
                    break;
                }
            }
        }
        return isHighestRank;
    }

    bool IsLowestRank(int num) {
        bool isLowestRank = false;
        int count = ClanMemberRanks.Count();

        for (int i = 0; i < count; i++) {
            ClanMemberRank r = ClanMemberRanks[i];
            
            if (r) {
                if (r.GetRank() == num) {
                    if (i == (count - 1)) {
                        isLowestRank = true;
                    }
                    break;
                }
            }
        }
        return isLowestRank;
    }

    bool CanInviteMembers(int rank) {
        if (rank == 0) {
            return true;
        }
        ref ClanMemberRank rankObject = FindRank(rank);

        if (rankObject) {
            return rankObject.GetPermissions().CanInviteMembers();
        }
        return false;
    }

    bool CanKickMembers(int rank) {
        if (rank == 0) {
            return true;
        }
        ref ClanMemberRank rankObject = FindRank(rank);

        if (rankObject) {
            return rankObject.GetPermissions().CanKickMembers();
        }
        return false;
    }


    bool CanPromoteMembers(int rank) {
        if (rank == 0) {
            return true;
        }
        ref ClanMemberRank rankObject = FindRank(rank);

        if (rankObject) {
            return rankObject.GetPermissions().CanPromoteMembers();
        }
        return false;
    }


    bool CanDemoteMembers(int rank) {
        if (rank == 0) {
            return true;
        }
        ref ClanMemberRank rankObject = FindRank(rank);

        if (rankObject) {
            return rankObject.GetPermissions().CanDemoteMembers();
        }
        return false;
    }


    bool CanContributeFunds(int rank) {
        if (rank == 0) {
            return true;
        }
        ref ClanMemberRank rankObject = FindRank(rank);

        if (rankObject) {
            return rankObject.GetPermissions().CanContributeFunds();
        }
        return false;
    }


    int GetTrackerPositionUpdateInterval() {
        return TrackerPositionUpdateInterval;
    }

    int GetTrackerPositionSens() {
        return TrackerPositionDistanceSensitivity;
    }
}