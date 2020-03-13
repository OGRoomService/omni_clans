class ClanConfigCurrency {
    string ItemName;
    int value;

    void ClanConfigCurrency(string ItemName, int value) {
        this.ItemName = ItemName;
        this.value = value;
    }

    string GetItemName() {
        return ItemName;
    }

    int GetValue() {
        return value;
    }
}