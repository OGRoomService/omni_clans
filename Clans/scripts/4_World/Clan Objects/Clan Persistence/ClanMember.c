class ClanMember {
    private string playerName, playerId, playerPlainId;
    private int rank, contribution;

    void ClanMember(string playerName, string playerId, string playerPlainId, int rank) {
        this.playerName = playerName;
        this.playerId = playerId;
        this.playerPlainId = playerPlainId;
        this.rank = rank;
        this.contribution = 0;
    }

    void SetName(string playerName) {
        this.playerName = playerName;
    }

    void SetRank(int rank) {
        if (!GetGame().IsServer() || !GetGame().IsMultiplayer()) { return; }

        this.rank = rank;
    }

    void Promote() {
        ref array<ref ClanMemberRank> configRanks = GetClanManager().GetConfig().GetRanks();
        ref ClanMemberRank currentRank = GetClanManager().GetConfig().FindRankByInt(rank);
        int currentRankIndex = configRanks.Find(currentRank);

        if (currentRankIndex == -1) {
            rank = configRanks[configRanks.Count() - 1].GetRank();
        } else {
            if (currentRankIndex != 0) {
                rank = configRanks[currentRankIndex - 1].GetRank();
            }
        }
    }

    void Demote() {
        ref array<ref ClanMemberRank> configRanks = GetClanManager().GetConfig().GetRanks();
        ref ClanMemberRank currentRank = GetClanManager().GetConfig().FindRankByInt(rank);
        int configRanksCount = configRanks.Count();
        int currentRankIndex = configRanks.Find(currentRank);

        if (currentRankIndex == -1) {
            rank = configRanks[configRanksCount - 1].GetRank();
        } else {
            if (currentRankIndex != (configRanksCount - 1)) {
                rank = configRanks[currentRankIndex + 1].GetRank();
            }
        }
    }

    void AddContribution(int contribution) {
        this.contribution += contribution;
    }

    void RemoveContribution(int contribution) {
        this.contribution -= contribution;

        if (this.contribution < 0) {
            this.contribution = 0;
        }
    }

    string GetPlayerName() {
        return playerName;
    }

    string GetPlayerId() {
        return playerId;
    }

    string GetPlayerPlainId() {
        return playerPlainId;
    }

    int GetPlayerRank() {
        return rank;
    }

    int GetContribution() {
        return contribution;
    }
}