modded class MissionServer {
    override void InvokeOnConnect(PlayerBase player, PlayerIdentity identity) {
        super.InvokeOnConnect(player, identity);

        if (player) {
            auto params = new Param3<string, ref ClanConfig, ref array<ref ClanBase>>(identity.GetPlainId(), GetClanManager().GetConfig(), GetClanManager().GetClanLeaderboard());
            
            GetGame().RPCSingleParam(player, ClanRPCEnum.ClientInitialize, params, true, identity);
            GetClanServerManager().AddPlayerToActiveClan(player);
            GetClanManager().AddOnlinePlayer("", "", player);
        }
        /* super.InvokeOnConnect(player, identity);

        array<Man> playerList = new array<Man>();
        GetGame().GetPlayers(playerList);
        playerList.Debug();
        auto params = new Param1<ref array<Man>>(playerList);

        GetGame().RPCSingleParam(player, 456412, params, true, identity); */
    }
    
    override void InvokeOnDisconnect(PlayerBase player) {
        super.InvokeOnDisconnect(player);

        if (player) {
            GetClanServerManager().RemoveFromActiveClan(player);
            GetClanManager().RemoveOnlinePlayer("", player);
        }
    }
}