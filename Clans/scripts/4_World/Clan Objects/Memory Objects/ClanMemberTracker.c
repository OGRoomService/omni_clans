class ClanMemberTracker {
    private PlayerBase player;
    private vector playerPos;
    private int posUpdateInterval = GetClanManager().GetConfig().GetTrackerPositionSens();
    private int posUpdateSens = GetClanManager().GetConfig().GetTrackerPositionSens();
    private int lastPosUpdate;
    private string playerId, playerPlainId, playerName;

    void ClanMemberTracker(string pId, string pPlainId, string pName, vector pos) {
        playerId = pId;
        playerPlainId = pPlainId;
        playerName = pName;
        playerPos = pos;
    }

    void Init() {
        UpdateValues();
    }

    void UpdateValues() {
        if (!GetGame().IsServer() || !GetGame().IsMultiplayer()) { return; }
        playerPos = player.GetPosition();
    }

    void SetPlayer(PlayerBase p) {
        player = p;
    }

    void SetValues(vector updatePos) {
        if (GetGame().IsServer() && GetGame().IsMultiplayer()) { return; }
        playerPos = updatePos;
    }

    void ClientUpdatePlayerBase() {
        if (playerPlainId == GetClanClientManager().GetPlainId()) {
            GetGame().GetCallQueue(CALL_CATEGORY_GAMEPLAY).RemoveByName(this, "ClientUpdatePlayerBase");
            return;
        }
        if (player) { return; }
        array<Man> playerList = new array<Man>();
        playerList = ClientData.m_PlayerBaseList;
        
        foreach (Man man : playerList) {
            PlayerBase playerBase = PlayerBase.Cast(man);

            if (playerBase) {
                if (!playerBase.GetIdentity()) {
                    if (playerBase.GetType() == playerName) {
                        SetPlayer(playerBase);
                        break;
                    }
                } else {
                    if (playerBase.GetIdentity().GetId() == playerId) {
                        SetPlayer(playerBase)
                        break;
                    }
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

    string GetPlainId() {
        return playerPlainId;
    }

    string GetId() {
        return playerId;
    }

    string GetName() {
        return playerName;
    }

    vector GetPosition() {
        return playerPos;
    }

    bool NeedsClanSynch() {
        if (!GetGame().IsServer() || !GetGame().IsMultiplayer()) { return false; }
        bool needsSync = false;
        vector pos = player.GetPosition();
        
        if (vector.Distance(pos, playerPos) > posUpdateSens) {
            UpdateValues();
            return true;
        }
        return false;
    }
}