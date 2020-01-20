/* modded class PlayerBase {
    ref array<Man> allPlayers;

	override void OnRPC(PlayerIdentity sender, int rpc_type, ParamsReadContext ctx)
	{
		super.OnRPC(sender, rpc_type, ctx);

        if (rpc_type == 456412) {
            if (GetGame().IsServer() && GetGame().IsMultiplayer()) { return; }

            Param1<ref array<Man>> data;
            if (!ctx.Read(data)) { return; }

            allPlayers = data.param1;

            allPlayers.Debug();

            Print(allPlayers[0].GetIdentity().GetPlainId());
        }
    }

    override void OnConnect() {
        super.OnConnect();

        if (!GetGame().IsServer() || !GetGame().IsMultiplayer()) { return; }

        array<Man> playerList = new array<Man>();

        GetGame().GetPlayers(playerList);

        playerList.Debug();
    }
} */

/* modded class PlayerBase {
    private float clanSynchSens = 1;
    private float cLastHP, cLastBlood, cLastShock, cMaxHP, cMaxBlood, cMaxShock;

    override void Init() {
        super.Init();

        if (!GetGame().IsServer() || !GetGame().IsMultiplayer()) { return; }

        cMaxHP = GetMaxHealth("", "Health");
        cMaxBlood = GetMaxHealth("", "Blood");
        cMaxShock = GetMaxHealth("", "Shock");

        UpdateValues();

        Print("max health=" + cMaxHP);
        Print("max blood=" + cMaxBlood);
        Print("max shock=" + cMaxShock);
    }

    void UpdateValues() {
        if (!GetGame().IsServer() || !GetGame().IsMultiplayer()) { return; }

        cLastHP = GetHealth("", "Health");
        cLastBlood = GetHealth("", "Blood");
        cLastShock = GetHealth("", "Shock");

        Print("health=" + cLastHP);
        Print("blood=" + cLastBlood);
        Print("shock=" + cLastShock);
    }

    bool NeedsClanSynch() {
        if (!GetGame().IsServer() || !GetGame().IsMultiplayer()) { return; }
        
        float diff;
        float healthCurr = (GetHealth("", "Health") / cMaxHP);
        diff = Math.AbsFloat(healthCurr - cLastHP);

        Print("synchsens=" + clanSynchSens / 100);
        Print("healthCurr=" + healthCurr);
        Print("diff1=" + diff);
        if (diff > (clanSynchSens / 100)) {
            UpdateValues();
            return true;
        }
        float bloodCurr = (GetHealth("", "Blood") / cMaxBlood);
        diff = Math.AbsFloat(bloodCurr - cLastBlood);

        Print("bloodCurr=" + bloodCurr);
        Print("diff2=" + diff);
        if (diff > (clanSynchSens / 100)) {
            UpdateValues();
            return true;
        }
        float shockCurr = (GetHealth("", "Shock") / cMaxShock);
        diff = Math.AbsFloat(shockCurr - cLastShock);

        Print("shockCurr=" + shockCurr);
        Print("diff3=" + diff);
        if (diff > (clanSynchSens / 100)) {
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
                    return cLastHP / cMaxHP;
                }
                return cLastHP;
                break;
            }
            case "blood":
            {
                if (normal) {
                    return cLastBlood / cMaxBlood;
                }
                return cLastBlood;
                break;
            }
            case "shock":
            {
                if (normal) {
                    return cLastShock / cMaxShock;
                }
                return cLastShock;
                break;
            }
        }
        return -1;
    }
} */