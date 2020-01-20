class ClanClientManager : PluginBase {
    private ref ActiveClan activeClan;
    private string plainId, clanInviteName;

    ref ActiveClan GetClan() {
        return activeClan;
    }

    void SetActiveClan(ref ActiveClan clan) {
        if (activeClan) {
            delete activeClan;
        }
        activeClan = clan;
        Print("Set clan successfully activeClan=" + activeClan);
        activeClan.Test();
    }

    void SetClanInvite(string clanName) {
        if (clanInviteName != string.Empty) { return; }
        
        clanInviteName = clanName;

        GetGame().GetCallQueue(CALL_CATEGORY_SYSTEM).Remove(DeleteInvitation);
        GetGame().GetCallQueue(CALL_CATEGORY_SYSTEM).CallLater(DeleteInvitation, 30000, false);
    }

    void DeleteInvitation() {
        Print("removing clan invite");

        clanInviteName = string.Empty;
    }

    void SetPlainId(string id) {
        if (plainId == string.Empty) {
            Print("set plainid=" + id);
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