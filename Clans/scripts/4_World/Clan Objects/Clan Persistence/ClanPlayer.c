class ClanPlayer {
    private string clanId, playerPlainId;
    private int totalContributions;

    void ClanPlayer(string playerPlainId) {
        this.playerPlainId = playerPlainId;
    }
    
    void SetClanId(string clanId) {
        this.clanId = clanId;
    }

    void AddContributions(int amount) {
        totalContributions += Math.AbsInt(amount);
    }

    string GetClanId() {
        return clanId;
    }

    string GetPlainId() {
        return playerPlainId;
    }
}