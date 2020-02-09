class Clan : ClanBase {
    protected int funds, yearLastActive, monthLastActive, dayLastActive;
    protected string ownerName, ownerId;
    protected ref array<ref ClanMember> members;

    void Clan(string id, string pName, string name) {
        ref ClanMember member = new ClanMember(pName, id, 0);
        members = new array<ref ClanMember>();
        ownerId = id;
        ownerName = pName;
        clanName = name;
        rank = 0;
        members.Insert(member);
    }

    void AddFunds(int amount) {
        if (!GetGame().IsServer() || !GetGame().IsMultiplayer()) { return; }

        funds += amount;
    }

    void RemoveFunds(int amount) {
        if (!GetGame().IsServer() || !GetGame().IsMultiplayer()) { return; }
        
        funds -= amount;

        if (funds < 0) {
            funds = 0;
        }
    }

    void AddMember(string name, string id) {
        if (/*!IsMember(id)*/true) {
            int randoRankIndex = Math.RandomInt(0, (GetClanManager().GetConfig().GetRanks().Count() - 1));
            int randoRank = GetClanManager().GetConfig().GetRanks()[randoRankIndex].GetRank();
            ref ClanMember member = new ClanMember(name, id, randoRank);

            members.Insert(member);
            Save();
        }
    }

    void RemoveMember(string id) {
        if (id == ownerId) { return; }

        ref ClanMember member = GetMember(id);

        if (member) {
            members.RemoveItem(member);
            Save();
        }
    }

    void PromoteMember(string playerId) {
        if (playerId == ownerId) { return; }

        ref ClanMember member = GetMember(playerId);

        if (member) {
            member.Promote();
            Save();
        }
    }

    void DemoteMember(string playerId) {
        if (playerId == ownerId) { return; }

        ref ClanMember member = GetMember(playerId);

        if (member) {
            member.Demote();
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
        ref array<ref ClanMemberRank> ranks = GetClanManager().GetConfig().GetRanks();

        if (!VerifyDate()) {
            return false;
        }
        foreach (ClanMember member : members) {
            if (member.GetId() == ownerId) {
                foundOwner = true;
            } else {
                int mRank = member.GetRank();
                int lowestRank = ranks[ranks.Count() - 1].GetRank();

                if (mRank == 0) {
                    member.SetRank(lowestRank);
                } else {
                    if (!GetClanManager().GetConfig().FindRank(mRank)) {
                        bool rankSet = false;

                        foreach (ClanMemberRank r : ranks) {
                            if (r) {
                                if (mRank < r.GetRank()) {
                                    member.SetRank(r.GetRank());
                                    rankSet = true;
                                    break;
                                }
                            }
                        }
                        if (!rankSet) {
                            member.SetRank(lowestRank);
                        }
                    }
                }
            }
        }
        if (!foundOwner) {
            ref ClanMember owner = new ClanMember(ownerName, ownerId, 0);
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

    ref ClanMember GetMember(string id) {
        ref ClanMember member;

        foreach (ClanMember m : members) {
            if (m.GetId() == id) {
                member = m;
                break;
            }
        }
        return member;
    }

    ref array<ref ClanMember> GetMembers() {
        return members;
    }

    bool IsMember(string playerId) {
        if (GetMember(playerId)) {
            return true;
        }
        return false;
    }

    int GetFunds() {
        return funds;
    }

    string GetOwnerId() {
        return ownerId;
    }
}