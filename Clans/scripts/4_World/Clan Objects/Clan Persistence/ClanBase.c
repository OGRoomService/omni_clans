class ClanBase {
    protected int rank;
    protected string clanName;

    void SetRank(int r) {
        if (!GetGame().IsServer() || !GetGame().IsMultiplayer()) { return; }

        rank = r;
    }

    int GetRank() {
        return rank;
    }

    string GetName() {
        return clanName;
    }

    string GetCaseName() {
        string caseName = clanName;
        caseName.ToLower();
        return caseName;
    }
}