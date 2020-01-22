class ClanPlayer {
    private string clanName, playerId;
    private int totalContributions;

    void ClanPlayer(string name, string id) {
        clanName = name;
        playerId = id;
    }

    void Save() {
        string playerDir = ClanStatic.playerDirectory + "\\" + playerId + ClanStatic.playerDataFile + ClanStatic.fileExtension;
        JsonFileLoader<ClanPlayer>.JsonSaveFile(playerDir, this);
    }

    string GetClanName() {
        return clanName;
    }
}