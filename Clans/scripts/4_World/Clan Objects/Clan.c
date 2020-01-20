class Clan {
    protected int funds;
    protected string ownerName, ownerId, clanName;
    protected ref array<ref ClanMember> members;

    void Clan(string id, string pName, string name) {
        ref ClanMember member = new ClanMember(pName, id);
        members = new array<ref ClanMember>();
        ownerId = id;
        ownerName = pName;
        clanName = name;
        member.SetRank(0);
        members.Insert(member);
    }

    void AddFunds(int amount) {
        funds += amount;
    }

    void RemoveFunds(int amount) {
        funds -= amount;

        if (funds < 0) {
            funds = 0;
        }
    }

    void AddMember(string name, string id) {
        if (!GetGame().IsServer() || !GetGame().IsMultiplayer()) { return; }
        if (/*!IsMember(id)*/true) {
            ref ClanMember member = new ClanMember(name, id);

            members.Insert(member);
            Save();
        }
    }

    void RemoveMember(string id) {
        if (!GetGame().IsServer() || !GetGame().IsMultiplayer()) { return; }
        if (id == ownerId) { return; }

        ref ClanMember member = GetMember(id);

        if (member) {
            members.RemoveItem(member);
            Save();
        }
    }

    void Validate() {
        if (!GetGame().IsServer() || !GetGame().IsMultiplayer()) { return; }

        bool foundOwner = false;

        foreach (ClanMember member : members) {
            if (member.GetId() == ownerId) {
                foundOwner = true;
            }
        }
        if (!foundOwner) {
            ref ClanMember owner = new ClanMember(ownerName, ownerId);
            members.Insert(owner);
            Save();
        }
    }

    void Save() {
        if (!GetGame().IsServer() || !GetGame().IsMultiplayer()) { return; }

        string clanDir = ClanStatic.clanDirectory + "\\" + clanName + ClanStatic.fileExtension;
        JsonFileLoader<Clan>.JsonSaveFile(clanDir, Clan.Cast(this));
    }

    private ref ClanMember GetMember(string id) {
        ref ClanMember member;

        foreach (ClanMember m : members) {
            if (member.GetId() == id) {
                member = m;
                break;
            }
        }
        return member;
    }

    bool IsMember(string playerId) {
        bool found = false;
        bool foundOwner = false;

        foreach (ClanMember member : members) {
            if (member.GetId() == playerId) {
                found = true;
                break;
            } else if (playerId == ownerId) {
                foundOwner = true;
            }
        }
        if (!foundOwner) {
            ref ClanMember owner = new ClanMember(ownerName, ownerId);
        }
        return found;
    }

    int GetFunds() {
        return funds;
    }

    string GetOwnerId() {
        return ownerId;
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