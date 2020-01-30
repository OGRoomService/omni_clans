class ClanMemberTracker {
    private PlayerBase player;
    private vector playerPos;
    private float updateSens = GetClanManager().GetConfig().GetTrackerSynchSens();
    private float lastHPUpdate, lastBloodUpdate, lastShockUpdate;
    private float hp, blood, shock;
    private float maxHP, maxBlood, maxShock;
    private int posUpdateInterval = GetClanManager().GetConfig().GetTrackerPositionSens();
    private int posUpdateSens = GetClanManager().GetConfig().GetTrackerPositionSens();
    private int lastPosUpdate;
    private string playerId, playerPlainId, playerName;

    void ClanMemberTracker(string pId, string pPlainId, string pName, float initHP, float initBlood, float initShock, vector pos) {
        playerId = pId;
        playerPlainId = pPlainId;
        playerName = pName;
        playerPos = pos;
        hp = initHP;
        blood = initBlood;
        shock = initShock;
    }

    void Init() {
        if (!GetGame().IsServer() || !GetGame().IsMultiplayer()) { return; }
        maxHP = player.GetMaxHealth("", "Health");
        maxBlood = player.GetMaxHealth("", "Blood");
        maxShock = player.GetMaxHealth("", "Shock");

        UpdateValues();

        lastHPUpdate = (hp / maxHP);
        lastBloodUpdate = (blood / maxBlood);
        lastShockUpdate = (shock / maxShock);
    }

    void UpdateValues() {
        if (!GetGame().IsServer() || !GetGame().IsMultiplayer()) { return; }
        hp = player.GetHealth("", "Health");
        blood = player.GetHealth("", "Blood");
        shock = player.GetHealth("", "Shock");
        playerPos = player.GetPosition();

        Print("health=" + hp);
        Print("blood=" + blood);
        Print("shock=" + shock);
    }

    void SetPlayer(PlayerBase p) {
        player = p;
    }

    void SetValues(float updateHP, float updateBlood, float updateShock, vector updatePos) {
        if (GetGame().IsServer() && GetGame().IsMultiplayer()) { return; }
        hp = updateHP;
        blood = updateBlood;
        shock = updateShock;
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

    float GetHealth(bool normalize = false) {
        if (normalize) {
            return ((hp / maxHP) * 100);
        }
        return hp;
    }

    float GetBlood(bool normalize = false) {
        if (normalize) {
            return ((blood / maxBlood) * 100);
        }
        return blood;
    }

    float GetShock(bool normalize = false) {
        if (normalize) {
            return ((shock / maxShock) * 100);
        }
        return shock;
    }

    vector GetPosition() {
        return playerPos;
    }

    bool NeedsClanSynch() {
        if (!GetGame().IsServer() || !GetGame().IsMultiplayer()) { return false; }
        
        float currHP = (player.GetHealth("", "Health") / maxHP);
        float currBlood = (player.GetHealth("", "Blood") / maxBlood);
        float currShock  = (player.GetHealth("", "Shock") / maxShock);
        float diffHP = Math.AbsFloat(currHP - lastHPUpdate);
        float diffBlood = Math.AbsFloat(currBlood - lastBloodUpdate);
        float diffShock = Math.AbsFloat(currShock - lastShockUpdate);
        bool needsSync = false;
        vector pos;
        lastPosUpdate++;

        if (lastPosUpdate >= posUpdateInterval) {
            pos = player.GetPosition();

            if (vector.Distance(pos, playerPos) > posUpdateSens) {
                needsSync = true;
            }
            lastPosUpdate = 0;
        }
        if (diffHP > (updateSens / 100)) {
            lastHPUpdate = currHP;
            needsSync = true;
        }
        if (diffBlood > (updateSens / 100)) {
            lastBloodUpdate = currBlood;
            needsSync = true;
        }
        if (diffShock > (updateSens / 100)) {
            lastShockUpdate = currShock;
            needsSync = true;
        }
        if (needsSync) {
            UpdateValues();
            return true;
        }
        return false;
    }
}