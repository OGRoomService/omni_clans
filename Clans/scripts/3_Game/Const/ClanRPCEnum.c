enum ClanRPCEnum {
    ServerCreateClan = 1982392,
    ServerAddInvitation,
    ServerAcceptInvite,
    ServerManageInvite,
    
    ClientManageMenu,
    // START Clan Management Enums // All actions done on server are repeated on client.
        // Clan Management
    ClientReceiveClan,
        // Clan Tracker Management
    ClientAddClanTracker,
    ClientRemoveClanTracker,
    ClientUpdateClanTracker,
        // Clan Invitation Management
    ClientAddClanInvitation,
    ClientRemoveClanInvitation,
    // END Clan Management Enums
    ClientAddOnlinePlayer,
    ClientRemoveOnlinePlayer,
    ClientReceivePlayerList,
    ClientReceiveInvite,
    ClientInitialize
}