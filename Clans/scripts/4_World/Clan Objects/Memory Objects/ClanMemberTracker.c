class ClanMemberTracker {
    private PlayerBase player;
    private vector playerPosition;
    private int posUpdateInterval = GetClanManager().GetConfig().GetTrackerPositionSens();
    private int posUpdateSens = GetClanManager().GetConfig().GetTrackerPositionSens();
    private int lastPosUpdate;
    private string playerId, playerName;

    void ClanMemberTracker(string playerName, string playerId, vector playerPosition) {
        this.playerName = playerName;
        this.playerId = playerId;
        this.playerPosition = playerPosition;
    }

    // Add client settings for this bullshit

    void Init() {
        if (!GetGame().IsServer() || !GetGame().IsMultiplayer()) {
            GetGame().GetCallQueue(CALL_CATEGORY_GAMEPLAY).RemoveByName(this, "ClientUpdatePlayerBase");

            if (playerId != GetGame().GetPlayer().GetIdentity().GetId()) {
                GetGame().GetCallQueue(CALL_CATEGORY_GAMEPLAY).CallLaterByName(this, "ClientUpdatePlayerBase", 1000, true);
                if (!GetClanClientManager().GetHud().DoesTrackerExist(playerId)) {
                    GetClanClientManager().GetHud().AddVisualTracker(this);
                }
            }
        } else {
            UpdatePlayerPosition();
        }
    }

    void UpdatePlayerPosition() {
        playerPosition = player.GetPosition();
    }

    void SetPlayer(PlayerBase player) {
        this.player = player;
    }

    void SetPlayerPosition(vector playerPosition) {
        this.playerPosition = playerPosition;
    }

    void ClientUpdatePlayerBase() {
        if (!player || !player.IsAlive()) {
            array<Man> playerList = new array<Man>();
            playerList = ClientData.m_PlayerBaseList;
            
            foreach (Man man : playerList) {
                PlayerBase playerBase = PlayerBase.Cast(man);
                PlayerIdentity playerIdentity = playerBase.GetIdentity();

                if (playerIdentity && playerBase && playerBase.IsAlive() && playerIdentity.GetId() == playerId) {
                    SetPlayer(playerBase);
                    break;
                }
            }
        }
    }

    PlayerBase GetPlayer() {
        return player;
    }

    PlayerIdentity GetIdentity() {
        return player.GetIdentity();
    }

    string GetPlayerId() {
        return playerId;
    }

    string GetName() {
        return playerName;
    }

    vector GetPosition() {
        return playerPosition;
    }

    bool IsPlayerPositionOOS() {
        if (player && player.GetIdentity()) {
            vector pos = player.GetPosition();
            
            if (vector.Distance(pos, playerPosition) > posUpdateSens) {
                UpdatePlayerPosition();
                return true;
            }
        }
        return false;
    }
}