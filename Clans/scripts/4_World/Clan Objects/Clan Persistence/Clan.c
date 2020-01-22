class Clan {
    protected int funds, rank, yearLastActive, monthLastActive, dayLastActive;
    protected string ownerName, ownerId, clanName;
    protected ref array<ref ClanMember> members;

    void Clan(string id, string pName, string name) {
        ref ClanMember member = new ClanMember(pName, id);
        members = new array<ref ClanMember>();
        ownerId = id;
        ownerName = pName;
        clanName = name;
        rank = 0;
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

    bool VerifyDate() {
        if (!GetGame().IsServer() || !GetGame().IsMultiplayer()) { return false; }

        int y, m, d;
        GetYearMonthDayUTC(y, m, d);

        if (y > 0 && m > 0 && d > 0) {
            int dayDiff = d - dayLastActive;
            int monthDiff = m - monthLastActive;

            /* Print("AHHHHHHHHHHHHHHHHHH = " + (y - yearLastActive));
            Print("SAVE YEAR = " + yearLastActive);
            Print("SAVE MONTH = " + monthLastActive);
            Print("SAVE DAY = " + dayLastActive);
            Print("YEAR = " + y);
            Print("MONTH = " + m);
            Print("DAY = " + d);
            Print("DAY DIFF = " + dayDiff);
            Print("MONTH DIFF = " + monthDiff);
            Print("DAY DAY  DIFF = " + (d - dayLastActive));
            Print("MONTH MONTH DIFF = " + (m - monthLastActive));
            Print("REEEEEEEE = " + (monthDiff + 12)); */

            // Deletion conditions. If this function returns false, it'll delete the clan
            if (y != yearLastActive) {
                if ((y - yearLastActive) > 1 || (monthDiff + 12) != 1 || dayDiff > 0) {
                    return false;
                }
            } else {
                if (monthDiff > 0 && dayDiff > 0) {
                    return false;
                }
            }
        }
        return true;
    }

    bool Verify() {
        if (!GetGame().IsServer() || !GetGame().IsMultiplayer()) { return false; }

        bool foundOwner = false;

        if (!VerifyDate()) {
            return false;
        }
        foreach (ClanMember member : members) {
            if (member.GetId() == ownerId) {
                foundOwner = true;
            } else if (member.GetId() != ownerId) {
                if (member.GetRank() == 0) {
                    // SET RANK TO LOWEST;
                    member.SetRank(12783);
                }
            }
        }
        if (!foundOwner) {
            ref ClanMember owner = new ClanMember(ownerName, ownerId);
            members.Insert(owner);
        }
        Save();
        return true;
    }

    void Save() {
        if (!GetGame().IsServer() || !GetGame().IsMultiplayer()) { return; }

        string clanDir = ClanStatic.clanDirectory + "\\" + clanName + ClanStatic.fileExtension;

        GetYearMonthDayUTC(yearLastActive, monthLastActive, dayLastActive);
        JsonFileLoader<Clan>.JsonSaveFile(clanDir, Clan.Cast(this));
    }

    void GetLastTimestamp(out int y, out int m, out int d) {
        y = yearLastActive;
        m = monthLastActive;
        d = dayLastActive;
    }

    void SetRank(int r) {
        rank = r;
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

    int GetRank() {
        return rank;
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