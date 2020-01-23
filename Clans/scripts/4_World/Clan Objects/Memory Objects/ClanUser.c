class ClanUser {
    private string name, playerId;

    void ClanUser(string n, string id) {
        name = n;
        playerId = id;
    }

    string GetName() {
        return name;
    }

    string GetId() {
        return playerId;
    }
}