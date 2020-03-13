modded class PlayerBase {
    protected string activeClanId;

    void ~PlayerBase() {
        if (GetGame().IsServer() && GetGame().IsMultiplayer()) {
            Print(ClanStatic.debugPrefix + "Removing player from playerlist! | Destroying player!");
            PlayerIdentity identity = GetIdentity();
            
            if (identity) {
                Print(ClanStatic.debugPrefix + "Player Identity Found | Destroying player!");
                if (GetClanManager().HasUserInPlayerList(identity.GetId())) {
                    Print(ClanStatic.debugPrefix + "Player found in list, removing | Destroying player!");
                    GetClanManager().RemoveOnlinePlayer(identity.GetId());
                }
                GetClanServerManager().RemovePlayerFromActiveClan(this);
            }
        }
    }

    void SetActiveClanId(string activeClanId) {
        Print(ClanStatic.debugPrefix + "Setting active clan id on player base! " + activeClanId);
        this.activeClanId = activeClanId;
    }

    string GetActiveClanId() {
        return activeClanId;
    }
}