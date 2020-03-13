class ClanClientSettings {
    int red, green, blue, clanMapTrackerUpdateInterval, clanMemberTrackerUpdateInterval;

    void ClanClientSettings() {
        red = 255;
        green = 255;
        blue = 255;
        clanMapTrackerUpdateInterval = 200;
        clanMemberTrackerUpdateInterval = 10;
    }

    void Verify() {
        if (red > 255 || red < 0) {
            red = 255;
        }
        if (green > 255 || green < 0) {
            green = 255;
        }
        if (blue > 255 || blue < 0) {
            blue = 255;
        }
        if (clanMapTrackerUpdateInterval < 0) {
            clanMapTrackerUpdateInterval = 200;
        }
        if (clanMemberTrackerUpdateInterval < 10) {
            clanMemberTrackerUpdateInterval = 10;
        }
    }

    void GetColorSettings(out int red, out int green, out int blue) {
        red = this.red;
        green = this.green;
        blue = this.blue;
    }

    void SetColors(int red, int green, int blue) {
        this.red = red;
        this.green = green;
        this.blue = blue;

        Save();
    }

    void Save() {
        JsonFileLoader<ClanClientSettings>.JsonSaveFile(ClanStatic.localSettingsDir, this);
    }
}