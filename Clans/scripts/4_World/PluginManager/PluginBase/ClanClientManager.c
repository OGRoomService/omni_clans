class ClanClientManager : PluginBase {
    private ref ActiveClan activeClan;
    private ref ClanHud activeClanHud;
    private ref ClanClientSettings clientSettings;
    private ref ClanInvitation clanInvitation;
    private string plainId, clanInviteName;

    void ClanClientManager() {
        if (!FileExist(ClanStatic.baseModDir)) {
            MakeDirectory(ClanStatic.baseModDir);
        }
        LoadClientSettings();
        Init();
    }

    ref ActiveClan GetClan() {
        return activeClan;
    }

    ref ClanHud GetHud() {
        return activeClanHud;
    }

    ref ClanClientSettings GetClientSettings() {
        return clientSettings;
    }

    void Init() {
        activeClan = null;
        activeClanHud = null;
        plainId = string.Empty;
        clanInviteName = string.Empty;
    }

    void LoadClientSettings() {
        if (FileExist(ClanStatic.localSettingsDir)) {
            JsonFileLoader<ClanClientSettings>.JsonLoadFile(ClanStatic.localSettingsDir, clientSettings);
        } else {
            clientSettings = new ClanClientSettings();
        }
        clientSettings.Verify();
        clientSettings.Save();
    }

    void SetActiveClan(ref ActiveClan clan) {
        DeleteClan();
        
        activeClan = clan;
        activeClanHud = new ClanHud(activeClan);
        activeClan.Init();
    }

    void DeleteClan() {
        if (activeClan) {
            delete activeClan;
        }
        if (activeClanHud) {
            delete activeClanHud;
        }
    }

    void SetClanInvite(string clanId, string clanName) {
        clanInvitation = new ClanInvitation(clanId, clanName);
    }

    void DeleteInvitation() {
        delete clanInvitation;
    }

    void SetPlainId(string id) {
        if (plainId == string.Empty) {
            plainId = id;
        }
    }

    ref ClanInvitation GetInvite() {
        return clanInvitation;
    }

    string GetPlainId() {
        return plainId;
    }
}

ClanClientManager GetClanClientManager() {
    return ClanClientManager.Cast(GetPlugin(ClanClientManager));
}