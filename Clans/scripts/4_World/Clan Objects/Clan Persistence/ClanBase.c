class ClanBase {
    protected int clanLevel;
    protected string clanName;

    void SetRank(int clanLevel) {
        this.clanLevel = clanLevel;
    }

    int GetRank() {
        return clanLevel;
    }

    string GetName() {
        return clanName;
    }

    string GetCaseName() {
        string lowercaseName = clanName;

        lowercaseName.ToLower()

        return lowercaseName;
    }
}