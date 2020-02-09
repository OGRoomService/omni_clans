class ClanClientManager : PluginBase {
    private ref ActiveClan activeClan;
    private ref ClanHud activeClanHud;
    private string plainId, clanInviteName;

    ref ActiveClan GetClan() {
        return activeClan;
    }

    ref ClanHud GetHud() {
        return activeClanHud;
    }

    void SetActiveClan(ref ActiveClan clan) {
        DeleteClan();
        
        activeClan = clan;
        activeClan.Test();
        activeClan.Init();
        activeClanHud = new ClanHud(activeClan);
    }

    void DeleteClan() {
        if (activeClan) {
            delete activeClan;
        }
        if (activeClanHud) {
            delete activeClanHud;
        }
    }

    void SetClanInvite(string clanName) {
        if (clanInviteName != string.Empty) { return; }
        
        clanInviteName = clanName;

        GetGame().GetCallQueue(CALL_CATEGORY_SYSTEM).Remove(DeleteInvitation);
        GetGame().GetCallQueue(CALL_CATEGORY_SYSTEM).CallLater(DeleteInvitation, 30000, false);
    }

    void DeleteInvitation() {
        clanInviteName = string.Empty;
    }

    void SetPlainId(string id) {
        if (plainId == string.Empty) {
            plainId = id;
        }
    }

    string GetInvite() {
        return clanInviteName;
    }

    string GetPlainId() {
        return plainId;
    }
}

ClanClientManager GetClanClientManager() {
    return ClanClientManager.Cast(GetPlugin(ClanClientManager));
}