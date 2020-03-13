class ClanRankPerms {
    string CanInviteMembers, CanKickMembers, CanPromoteMembers, CanDemoteMembers, CanContributeFunds, CanUpgradeClan;

    void ClanRankPerms() {
        CanInviteMembers = "false";
        CanKickMembers = "false";
        CanPromoteMembers = "false";
        CanDemoteMembers = "false";
        CanContributeFunds = "false";
        CanUpgradeClan = "false";
    }

    bool CanInviteMembers() {
        string loweredString = CanInviteMembers;
        loweredString.ToLower();

        if (loweredString == "true") {
            return true;
        }
        return false;
    }

    bool CanKickMembers() {
        string loweredString = CanKickMembers;
        loweredString.ToLower();

        if (loweredString == "true") {
            return true;
        }
        return false;
    }

    bool CanPromoteMembers() {
        string loweredString = CanPromoteMembers;
        loweredString.ToLower();

        if (loweredString == "true") {
            return true;
        }
        return false;
    }

    bool CanDemoteMembers() {
        string loweredString = CanDemoteMembers;
        loweredString.ToLower();

        if (loweredString == "true") {
            return true;
        }
        return false;
    }

    bool CanContributeFunds() {
        string loweredString = CanContributeFunds;
        loweredString.ToLower();

        if (loweredString == "true") {
            return true;
        }
        return false;
    }

    bool CanUpgradeClan() {
        string loweredString = CanUpgradeClan;
        loweredString.ToLower();

        if (loweredString == "true") {
            return true;
        }
        return false;
    }
}