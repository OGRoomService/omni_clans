modded class MissionServer {
    override void InvokeOnConnect(PlayerBase player, PlayerIdentity identity) {
        super.InvokeOnConnect(player, identity);

        if (player) {
            auto params = new Param2<string, ref ClanConfig>(identity.GetPlainId(), GetClanManager().GetConfig());
            GetClanServerManager().AddPlayerToActiveClan(player);
            GetClanManager().AddOnlinePlayer("", "", player);
            GetGame().RPCSingleParam(player, ClanRPCEnum.ClientInitialize, params, true, identity);
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