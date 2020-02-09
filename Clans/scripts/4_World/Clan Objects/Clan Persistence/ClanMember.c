class ClanMember {
    private string name, playerId;
    private int rank, contribution;

    void ClanMember(string n, string id, int r) {
        name = n;
        playerId = id;
        contribution = 0;
        rank = r;
        // set rank to lowest possible rank... 
        // Should I have this in ascending order? 
        // Definitely not becuase if more ranks are added then all of a sudden the owner is no longer owner.... 
        // but then how do I ensure that when ranks are added the lowest ranked players aren't all of a sudden the 
        // highest ranked? I guess server owners are just going to have to manage that correctly by assigning numbers 
        // far apart so that they don't run into this conflict. I'll let hollow know the deal with that when this is finished.
    }

    void SetRank(int r) {
        if (!GetGame().IsServer() || !GetGame().IsMultiplayer()) { return; }

        rank = r;
    }

    // Other logic required for promotions/demotions can be done in server-rpc handler or clan
    // Descending order hierarchy. 0 is top of chain = owner. Do not allow assignment to any other member
    void Promote() {
        // PROMOTE THE USER BASED ON NEXT LOWEST IN CONFIG;
        ref array<ref ClanMemberRank> configRanks = GetClanManager().GetConfig().GetRanks();
        ref ClanMemberRank currentRank = GetClanManager().GetConfig().FindRank(rank);
        int currentRankIndex = configRanks.Find(currentRank);

        if (currentRankIndex == -1) {
            // For some reason they have a non existant rank, reset to lowest rank
            rank = configRanks[configRanks.Count() - 1].GetRank();
        } else {
            if (currentRankIndex != 0) {
                rank = configRanks[currentRankIndex - 1].GetRank();
            }
        }
    }

    void Demote() {
        // DEMOTE THE USER BASED ON NEXT LOWEST IN CONFIG;
        ref array<ref ClanMemberRank> configRanks = GetClanManager().GetConfig().GetRanks();
        ref ClanMemberRank currentRank = GetClanManager().GetConfig().FindRank(rank);
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

    void AddContribution(int amount) {
        if (!GetGame().IsServer() || !GetGame().IsMultiplayer()) { return; }

        contribution += amount;
    }

    void RemoveContribution(int amount) {
        if (!GetGame().IsServer() || !GetGame().IsMultiplayer()) { return; }
        
        contribution -= amount;

        if (contribution < 0) {
            contribution = 0;
        }
    }

    string GetName() {
        return name;
    }

    string GetId() {
        return playerId;
    }

    int GetRank() {
        return rank;
    }

    int GetContribution() {
        return contribution;
    }
}