class ClanUser {
    private string name, playerId;

    void ClanUser(string name, string playerId) {
        this.name = name;
        this.playerId = playerId;
    }

    string GetName() {
        return name;
    }

    string GetId() {
        return playerId;
    }
}