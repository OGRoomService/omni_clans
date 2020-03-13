class ClanConfig {
    // Constants
    private static const int TRACKER_POSITION_INTERVAL = 5;
    private static const int TRACKER_POSITION_SENSITIVITY = 25;
    private static const int CLAN_START_UPGRADE_PRICE = 50000;
    private static const int CLAN_CREATION_COST = 15000;
    private static const int CLAN_START_SLOTS = 5;
    private static const float CLAN_UPGRADE_MULTIPLIER = 2.5;
    private static const int CLAN_UPGRADE_SLOTS = 1;

    // Config variables
    private int ClanCreationCost;
    private int StartingClanUpgradePrice;
    private float ClanPerLevelUpgradeMultiplier;
    private int StartingClanPlayerSlots;
    private int ClanPerLevelSlots;
    private string ClanOwnerRankName;
    private int TrackerPositionUpdateInterval;
    private int TrackerPositionDistanceSensitivity;
    private ref array<ref ClanConfigCurrency> ContributableCurrencyandValue;
    private ref array<ref ClanMemberRank> ClanMemberRanks;

    void ClanConfig() {
        ContributableCurrencyandValue = new array<ref ClanConfigCurrency>();
        ClanMemberRanks = new array<ref ClanMemberRank>();

        ClanOwnerRankName == "President";
        ClanCreationCost = CLAN_CREATION_COST;
        StartingClanUpgradePrice = CLAN_START_UPGRADE_PRICE;
        ClanPerLevelUpgradeMultiplier = CLAN_UPGRADE_MULTIPLIER;
        StartingClanPlayerSlots = CLAN_START_SLOTS;
        ClanPerLevelSlots = CLAN_UPGRADE_SLOTS;
        TrackerPositionUpdateInterval = TRACKER_POSITION_INTERVAL;
        TrackerPositionDistanceSensitivity = TRACKER_POSITION_SENSITIVITY;
    }

    void Verify() {
        int i;
        array<ref ClanMemberRank> arrayRanks = new array<ref ClanMemberRank>();
        array<ref ClanConfigCurrency> arrayCurrency = new array<ref ClanConfigCurrency>();
        arrayRanks = ClanMemberRanks;
        arrayCurrency = ContributableCurrencyandValue;
        ClanMemberRanks = new array<ref ClanMemberRank>();
        ContributableCurrencyandValue = new array<ref ClanConfigCurrency>();

        if (ClanCreationCost < 1) {
            ClanCreationCost = CLAN_CREATION_COST;
        }
        if (TrackerPositionUpdateInterval < 1) {
            TrackerPositionUpdateInterval = TRACKER_POSITION_INTERVAL;
        }
        if (TrackerPositionDistanceSensitivity < 1) {
            TrackerPositionDistanceSensitivity = TRACKER_POSITION_SENSITIVITY;
        }
        if (StartingClanUpgradePrice < 1) {
            StartingClanUpgradePrice = CLAN_START_UPGRADE_PRICE;
        }
        if (ClanPerLevelUpgradeMultiplier <= 0.0) {
            ClanPerLevelUpgradeMultiplier = CLAN_UPGRADE_MULTIPLIER;
        } 
        if (StartingClanPlayerSlots < 1) {
            StartingClanPlayerSlots = CLAN_START_SLOTS;
        }
        if (ClanPerLevelSlots < 1) {
            ClanPerLevelSlots = CLAN_UPGRADE_SLOTS;
        }
        if (ClanOwnerRankName == string.Empty) {
            ClanOwnerRankName = "President";
        }
        foreach (ClanMemberRank rank : arrayRanks) {
            if (rank) {
                SortRank(rank);
            }
        }
        foreach (ClanConfigCurrency currency : arrayCurrency) {
            if (currency) {
                SortCurrency(currency);
            }
        }
        if (ContributableCurrencyandValue.Count() < 1) {
            for (i = 0; i < 2; i++) {
                ClanConfigCurrency newCurrency = new ClanConfigCurrency("EXAMPLE, CHANGE ME", (i + 1) * 5);
                ContributableCurrencyandValue.Insert(newCurrency);
            }
        }
        if (ClanMemberRanks.Count() < 1) {
            for (i = 0; i < 2; i++) {
                ClanMemberRank newRank = new ClanMemberRank(i + 1, "EXAMPLE, CHANGE ME");
                ClanMemberRanks.Insert(newRank);
            }
        }
        Save();
    }

    private void SortRank(ref ClanMemberRank newRank) {
        int count = ClanMemberRanks.Count();
        bool inserted = false;

        if (count < 1) {
            ClanMemberRanks.Insert(newRank);
        } else {
            for (int i = 0; i < count; i++) {
                ref ClanMemberRank rank = ClanMemberRanks[i];

                if (rank.GetRank() > newRank.GetRank()) {
                    ClanMemberRanks.InsertAt(newRank, i);
                    inserted = true;
                    break;
                }
            }
            if (!inserted) {
                ClanMemberRanks.Insert(newRank);
            }
        }
    }

    private void SortCurrency(ref ClanConfigCurrency newCurrency) {
        int count = ContributableCurrencyandValue.Count();
        bool inserted = false;

        if (count < 1) {
            ContributableCurrencyandValue.Insert(newCurrency);
        } else {
            for (int i = 0; i < count; i++) { 
                ref ClanConfigCurrency currency = ContributableCurrencyandValue[i];

                if (currency.GetValue() < newCurrency.GetValue()) {
                    ContributableCurrencyandValue.InsertAt(newCurrency, i);
                    inserted = true;
                    break;
                }
            }
            if (!inserted) {
                ContributableCurrencyandValue.Insert(newCurrency);
            }
        }
    }

    void Save() {
        JsonFileLoader<ClanConfig>.JsonSaveFile(ClanStatic.serverConfigDir, this);
    }

    ref array<ref ClanMemberRank> GetRanks() {
        return ClanMemberRanks;
    }

    ref array<ref ClanConfigCurrency> GetCurrency() {
        return ContributableCurrencyandValue;
    }

    ref ClanMemberRank FindRankByInt(int rankInt) {
        ref ClanMemberRank foundRank;

        foreach (ClanMemberRank searchRank : ClanMemberRanks) {
            if (searchRank && searchRank.GetRank() == rankInt) {
                foundRank = searchRank;
                break;
            }
        }
        return foundRank;
    }

    ref ClanMemberRank GetHighestRank() {
        ref ClanMemberRank highestRank = ClanMemberRanks[0];

        return highestRank;
    }

    ref ClanMemberRank GetLowestRank() {
        ref ClanMemberRank lowestRank = ClanMemberRanks[ClanMemberRanks.Count() - 1];

        return lowestRank;
    }

    ref ClanConfigCurrency GetCurrencyByType(string itemType) {
        ClanConfigCurrency currencyItem;

        foreach (ClanConfigCurrency currency : ContributableCurrencyandValue) {
            if (currency) {
                string currencyType = currency.GetItemName();

                currencyType.ToLower();
                itemType.ToLower();

                if (currencyType == itemType) {
                    currencyItem = currency;
                    break;
                }
            }
        }
        return currencyItem;
    }

    string GetOwnerRankName() {
        return ClanOwnerRankName;
    }

    string GetRankName(int num) {
        string name;
        ClanMemberRank rank = FindRankByInt(num);

        if (rank) {
            name = rank.GetName();
        }
        return name;
    }

    bool IsHighestRank(int rankInt) {
        ref ClanMemberRank highestRank = GetHighestRank();

        if (highestRank.GetRank() == rankInt) {
            return true;
        }
        return false;
    }

    bool IsLowestRank(int rankInt) {
        ref ClanMemberRank lowestRank = GetLowestRank();

        if (lowestRank.GetRank() == rankInt) {
            return true;
        }
        return false;
    }

    bool CanInviteMembers(int rank) {
        if (rank == 0) { return true; }
        ref ClanMemberRank rankObject = FindRankByInt(rank);

        if (rankObject) {
            return rankObject.GetPermissions().CanInviteMembers();
        }
        return false;
    }

    bool CanKickMembers(int rank) {
        if (rank == 0) { return true; }
        ref ClanMemberRank rankObject = FindRankByInt(rank);

        if (rankObject) {
            return rankObject.GetPermissions().CanKickMembers();
        }
        return false;
    }

    bool CanPromoteMembers(int rank) {
        if (rank == 0) { return true; }
        ref ClanMemberRank rankObject = FindRankByInt(rank);

        if (rankObject) {
            return rankObject.GetPermissions().CanPromoteMembers();
        }
        return false;
    }

    bool CanDemoteMembers(int rank) {
        if (rank == 0) { return true; }
        ref ClanMemberRank rankObject = FindRankByInt(rank);

        if (rankObject) {
            return rankObject.GetPermissions().CanDemoteMembers();
        }
        return false;
    }

    bool CanContributeFunds(int rank) {
        if (rank == 0) { return true; }
        ref ClanMemberRank rankObject = FindRankByInt(rank);

        if (rankObject) {
            return rankObject.GetPermissions().CanContributeFunds();
        }
        return false;
    }

    bool CanUpgradeClan(int rank) {
        if (rank == 0) { return true; }
        ref ClanMemberRank rankObject = FindRankByInt(rank);

        if (rankObject) {
            return rankObject.GetPermissions().CanUpgradeClan();
        }
        return false;
    }

    int GetRankIndex(int rankInt) {
        int rankIndex = -1;

        for (int i = 0; i < ClanMemberRanks.Count(); i++) {
            ref ClanMemberRank clanRank = ClanMemberRanks[i];

            if (clanRank) {
                if (clanRank.GetRank() == rankInt) {
                    rankIndex = i;
                    break;
                }
            }
        }
        return rankIndex;
    }

    int GetClanCreationCost() {
        return ClanCreationCost;
    }

    int GetTrackerPositionUpdateInterval() {
        return TrackerPositionUpdateInterval;
    }

    int GetTrackerPositionSens() {
        return TrackerPositionDistanceSensitivity;
    }

    int GetStartingClanUpgradePrice() {
        return StartingClanUpgradePrice;
    }

    int GetClanStartingPlayerSlots() {
        return StartingClanPlayerSlots;
    }

    int GetClanPerLevelPlayerSlots() {
        return ClanPerLevelSlots;
    }

    int GetClanPlayerCap(int clanRank) {
        int clanPlayerCap = StartingClanPlayerSlots + ((clanRank - 1) * ClanPerLevelSlots);

        return clanPlayerCap;
    }

    float GetClanPerLevelMultiplier() {
        return ClanPerLevelUpgradeMultiplier;
    }
}