class ClanMemberRank {
    private int rank;
    private string name;
    private ref ClanRankPerms perms;

    void ClanMemberRank(int rank, string name) {
        perms = new ClanRankPerms();
        this.rank = rank;
        this.name = name;
    }

    ref ClanRankPerms GetPermissions() {
        return perms;
    }

    int GetRank() {
        return rank;
    }

    string GetName() {
        return name;
    }
}