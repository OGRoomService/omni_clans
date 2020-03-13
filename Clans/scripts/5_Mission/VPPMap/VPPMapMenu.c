modded class VPPMapMenu {
    void SetMenuOpen(bool isMenuOpen) {
        super.SetMenuOpen (isMenuOpen);

        if (m_IsMenuOpen) {
            GetGame().GetCallQueue(CALL_CATEGORY_GAMEPLAY).CallLaterByName(this, "RefreshAllMapMarkers", 300, true);
        }
    }

    void RefreshAllMapMarkers() {
        if (!m_IsMenuOpen) {
            GetGame().GetCallQueue(CALL_CATEGORY_GAMEPLAY).RemoveByName(this, "RefreshAllMapMarkers");
            return;
        }
        DisplayClientMarkers();
        DisplayServerMarkers();
        DisplayClanTrackers();
    }

    void DisplayClanTrackers() {
        ref ActiveClan clan = GetClanClientManager().GetClan();

        if (clan) {
            ref array<ref ClanMemberTracker> arrayTrackers = clan.GetTrackers();

            foreach (ClanMemberTracker tracker : arrayTrackers) {
                if (!tracker || tracker.GetPlayerId() == GetGame().GetPlayer().GetIdentity().GetId()) { continue; }

                PlayerBase player = tracker.GetPlayer();
                vector playerPos;
                int red, green, blue;

                if (player && player.IsAlive()) {
                    playerPos = player.GetPosition();
                } else {
                    playerPos = tracker.GetPosition();
                }
		        GetClanClientManager().GetClientSettings().GetColorSettings(red, green, blue);
                m_MapWidget.AddUserMark(playerPos, tracker.GetName(), ARGB(255, red, green, blue), "omni_clans\\gui\\img\\circle.paa");
            }
        }
    }
}