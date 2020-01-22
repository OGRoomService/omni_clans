class ClanMember {
    private string name, playerId;
    private int rank, contribution;

    void ClanMember(string n, string id) {
        name = n;
        playerId = id;
        contribution = 0;
        // set rank to lowest possible rank... 
        // Should I have this in ascending order? 
        // Definitely not becuase if more ranks are added then all of a sudden the owner is no longer owner.... 
        // but then how do I ensure that when ranks are added the lowest ranked players aren't all of a sudden the 
        // highest ranked? I guess server owners are just going to have to manage that correctly by assigning numbers 
        // far apart so that they don't run into this conflict. I'll let hollow know the deal with that when this is finished.
    }

    // Descending order hierarchy. 0 is top of chain = owner. Do not allow assignment to any other member
    void SetRank(int r) {
        rank = r;
    }

    void AddContribution(int amount) {
        contribution += amount;
    }

    void RemoveContribution(int amount) {
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