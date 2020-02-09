enum ClanRPCEnum {
    ServerCreateClan = 1982392,
    ServerAddInvitation,
    ServerManageInvite,
    ServerPromoteClanMember,
    ServerDemoteClanMember,
    ServerUpgradeClan,
    ServerContributeFundsToClan,
    ServerKickFromClan,
    ServerLeaveClan,
    ServerDeleteClan,
    
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
        // Clan promotion Management
    ClientPromoteClanMember,
    ClientDemoteClanMember,
        // Clan Member Adding/Removing Management
    ClientAddClanMember,
    ClientRemoveClanMember,
        // Clan Funds Management
    ClientSetClanFunds,
    // END Clan Management Enums
    ClientAddOnlinePlayer,
    ClientRemoveOnlinePlayer,
    ClientReceivePlayerList,
    ClientReceiveInvite,
    ClientDeleteClan,
    ClientInitialize
}