class ClanMemberTracker {
    private PlayerBase player;
    private float updateSens = 1;
    private float lastHPUpdate, lastBloodUpdate, lastShockUpdate;
    private float hp, blood, shock;
    private float maxHP, maxBlood, maxShock;
    private string pId, pPlainId;

    void ClanMemberTracker(PlayerBase p) {
        player = p;
        pId = player.GetIdentity().GetId();
        pPlainId = player.GetIdentity().GetPlainId();
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

        Print("health=" + hp);
        Print("blood=" + blood);
        Print("shock=" + shock);
    }

    PlayerBase GetPlayer() {
        return player;
    }

    PlayerIdentity GetIdentity() {
        return player.GetIdentity();
    }

    string GetPlainId() {
        return pPlainId;
    }

    string GetId() {
        return pId;
    }

    bool NeedsClanSynch() {
        if (!GetGame().IsServer() || !GetGame().IsMultiplayer()) { return false; }
        
        float currHP = (player.GetHealth("", "Health") / maxHP);
        float diff = Math.AbsFloat(currHP - lastHPUpdate);
        bool needsSynch = false;

        if (diff > (updateSens / 100)) {
            lastHPUpdate = currHP;
            needsSynch = true;
        }
        float currBlood = (player.GetHealth("", "Blood") / maxBlood);
        diff = Math.AbsFloat(currBlood - lastBloodUpdate);

        if (diff > (updateSens / 100)) {
            lastBloodUpdate = currBlood;
            needsSynch = true;
        }
        float currShock  = (player.GetHealth("", "Shock") / maxShock);
        diff = Math.AbsFloat(currShock - lastShockUpdate);
        
        if (diff > (updateSens / 100)) {
            lastShockUpdate = currShock;
            needsSynch = true;
        }
        if (needsSynch) {
            UpdateValues();
            return true;
        }
        return false;
    }

    float GetCVal(string type, bool normal = false) {
        type.ToLower();

        switch(type) {
            case "health":
            {
                if (normal) {
                    return ((hp / maxHP) * 100);
                }
                return hp;
                break;
            }
            case "blood":
            {
                if (normal) {
                    return ((blood / maxBlood) * 100);
                }
                return blood;
                break;
            }
            case "shock":
            {
                if (normal) {
                    return ((shock / maxShock) * 100);
                }
                return shock;
                break;
            }
        }
        return -1;
    }
}