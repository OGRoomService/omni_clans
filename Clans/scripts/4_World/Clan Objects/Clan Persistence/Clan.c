class Clan : ClanBase {
    protected int clanFunds, yearLastActive, monthLastActive, dayLastActive;
    protected string ownerName, ownerId, ownerPlainId;
    protected ref array<ref ClanMember> arrayClanMembers;

    void Clan(string clanName, string ownerName, string ownerId, string ownerPlainId) {
        ref ClanMember memberOwner = new ClanMember(ownerName, ownerId, ownerPlainId, 0);

        this.clanLevel = 1;
        this.clanName = clanName;
        this.arrayClanMembers = new array<ref ClanMember>();
        this.ownerName = ownerName;
        this.ownerId = ownerId;
        this.ownerPlainId = ownerPlainId;
        this.arrayClanMembers.Insert(memberOwner);
    }
    
    /*
     * Critical functions for data management
    */

    bool VerifyDate() {
        if (!GetGame().IsServer() || !GetGame().IsMultiplayer()) { return false; }

        int y, m, d;
        GetYearMonthDayUTC(y, m, d);

        if (y > 0 && m > 0 && d > 0) {
            int dayDiff = d - dayLastActive;
            int monthDiff = m - monthLastActive;

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

        ref array<ref ClanMemberRank> arrayConfigRanks;
        ref array<ref ClanMember> arrayMembersToDelete = new array<ref ClanMember>();
        ref ClanMember owner, previousMember;
        int clanPlayerCap, currentMemberCount;

        arrayConfigRanks = GetClanManager().GetConfig().GetRanks();
        clanPlayerCap = GetClanManager().GetConfig().GetClanPlayerCap(clanLevel);
        currentMemberCount = 1;

        Print(ClanStatic.debugPrefix + "CLAN PLAYER CAP=" + clanPlayerCap);

        if (!VerifyDate()) {
            return false;
        }
        for (int i = (arrayClanMembers.Count() - 1); i >= 0; i--) {
            ref ClanMember clanMember = arrayClanMembers[i];
            currentMemberCount++;

            if (!clanMember) {
                arrayClanMembers.Remove(i);
                continue;
            }
            if (previousMember && previousMember.GetPlayerId() == clanMember.GetPlayerId()) {
                arrayClanMembers.Remove(i);
                continue;
            }
            previousMember = clanMember;

            if (currentMemberCount > clanPlayerCap) {
                Print(ClanStatic.debugPrefix + "CURRENT COUNT GREATER THAN PLAYER CAP");
                arrayClanMembers.Remove(i);
                continue;
            }
            if (clanMember.GetPlayerId() == ownerId) {
                owner = clanMember;
                currentMemberCount--;
            } else {
                int memberRank = clanMember.GetPlayerRank();
                int lowestRank = GetClanManager().GetConfig().GetLowestRank().GetRank();

                if (memberRank == 0) {
                    clanMember.SetRank(lowestRank);
                } else if (!GetClanManager().GetConfig().FindRankByInt(memberRank)) {
                    bool rankSet = false;

                    foreach (ClanMemberRank rank : arrayConfigRanks) {
                        if (rank) {
                            if (memberRank < rank.GetRank()) {
                                clanMember.SetRank(rank.GetRank());
                                rankSet = true;
                                break;
                            }
                        }
                    }
                    if (!rankSet) {
                        clanMember.SetRank(lowestRank);
                    }
                }
            }
        }
        if (!owner) {
            Print(ClanStatic.debugPrefix + "OWNER NOT FOUND");
            owner = new ClanMember(ownerName, ownerId, ownerPlainId, 0);
            arrayClanMembers.Insert(owner);
        } else {
            if (owner.GetPlayerRank() != 0) {
                owner.SetRank(0);
            }
        }
        if (clanLevel < 1) {
            clanLevel = 1;
        }
        Save();
        return true;
    }

    void Save() {
        if (!GetGame().IsServer() || !GetGame().IsMultiplayer()) { return; }
        string clanDir = ClanStatic.clanDirectory + ownerPlainId + ClanStatic.fileExtension;

        GetYearMonthDayUTC(yearLastActive, monthLastActive, dayLastActive);
        JsonFileLoader<Clan>.JsonSaveFile(clanDir, Clan.Cast(this));
    }

    void Delete() {
        if (!GetGame().IsServer() || !GetGame().IsMultiplayer()) { return; }
        string clanDir = ClanStatic.clanDirectory + ownerPlainId + ClanStatic.fileExtension;

        DeleteFile(clanDir);
    }

    void AddFunds(int amount) {
        clanFunds += Math.AbsInt(amount);
        Save();
    }

    void RemoveFunds(int amount) {
        clanFunds -= Math.AbsInt(amount);

        if (clanFunds < 0) {
            clanFunds = 0;
        }
        Save();
    }

    void UpgradeClan() {
        clanLevel += 1;
        
        Save();
    }

    bool AddMemberContributions(string playerId, int contributionAmount) {
        ref ClanMember clanMember = GetClanMemberByPlayerId(playerId);

        if (clanMember) {
            clanMember.AddContribution(contributionAmount);
            Save();
            return true;
        }
        return false;
    }

    bool CanUpgradeClan(out int upgradeCost) {
        upgradeCost = GetUpgradePrice();
        
        if (upgradeCost <= clanFunds) {
            return true;
        }
        return false;
    }

    int GetUpgradePrice() {
        ref ClanConfig config;
        int startingUpgradePrice, upgradeCost;
        float perLevelMultiplier;

        config = GetClanManager().GetConfig();
        startingUpgradePrice = config.GetStartingClanUpgradePrice();
        perLevelMultiplier = config.GetClanPerLevelMultiplier();

        if ((clanLevel - 1) < 1) {
            upgradeCost = startingUpgradePrice;
        } else {
            upgradeCost = Math.Ceil(((clanLevel - 1) * perLevelMultiplier * startingUpgradePrice));
        }
        return upgradeCost;
    }
    
    /*
     * Critical functions for data management
    */
    
    /*
     * Critical functions for member management
    */

    void AddMember(string playerName, string playerId, string playerPlainId) {
        ref ClanMember newMember = new ClanMember(playerName, playerId, playerPlainId, GetClanManager().GetConfig().GetLowestRank().GetRank());

        arrayClanMembers.Insert(newMember);
        Save();
    }

    bool RemoveMember(string playerId) {
        //if (playerId == ownerId) { return; }
        ref ClanMember clanMember = GetClanMemberByPlayerId(playerId);

        if (clanMember) {
            arrayClanMembers.RemoveItem(clanMember);
            Save();
            return true;
        }
        return false;
    }

    bool UpdateMemberName(string playerId, string playerName) {
        ref ClanMember clanMember = GetClanMemberByPlayerId(playerId);

        if (clanMember) {
            if (clanMember.GetPlayerName() != playerName) {
                clanMember.SetName(playerName);
                Save();
                return true;
            }
        }
        return false;
    }

    bool PromoteMember(string playerId) {
        //if (playerId == ownerId) { return; }
        ref ClanMember clanMember = GetClanMemberByPlayerId(playerId);

        if (clanMember) {
            clanMember.Promote();
            Save();
        }
        return true;
    }

    bool DemoteMember(string playerId) {
        //if (playerId == ownerId) { return; }
        ref ClanMember clanMember = GetClanMemberByPlayerId(playerId);

        if (clanMember) {
            clanMember.Demote();
            Save();
            return true;
        }
        return false;
    }
    
    /*
     * Critical functions for member management
    */
    
    /*
     * Getters
    */

    ref array<ref ClanMember> GetMembers() {
        return arrayClanMembers;
    }

    ref ClanMember GetClanMemberByPlayerId(string playerId) {
        foreach (ClanMember clanMember : arrayClanMembers) {
            if (clanMember) {
                if (clanMember.GetPlayerId() == playerId || clanMember.GetPlayerPlainId() == playerId) {
                    return clanMember;
                }
            }
        }
        return null;
    }

    int GetFunds() {
        return clanFunds;
    }

    string GetClanId() {
        return ownerPlainId;
    }

    string GetOwnerId() {
        return ownerId;
    }

    string GetOwnerPlainId() {
        return ownerPlainId;
    }

    bool IsPlayerInClan(string playerId) {
        if (GetClanMemberByPlayerId(playerId)) {
            return true;
        }
        return false;
    }
    
    /*
     * Getters
    */
}