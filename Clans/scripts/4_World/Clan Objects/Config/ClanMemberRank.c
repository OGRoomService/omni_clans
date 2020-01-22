class ClanMemberRank {
    private int rank;
    private string name;
    private ref ClanRankPerms perms;

    void ClanMemberRank(int r, string n) {
        perms = new ClanRankPerms();
        rank = r;
        name = n;
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