class ClanInvitation {
    string clanId, clanName;

    void ClanInvitation (string clanId, string clanName) {
        this.clanId = clanId;
        this.clanName = clanName;
    }

    string GetClanId() {
        return clanId;
    }

    string GetClanName() {
        return clanName;
    }
}