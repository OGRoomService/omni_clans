class ClanVisualTracker {
    private ref Widget parentWidget, playerVisualTracker;
    private ref TextWidget txtPlayerName;
    private ref ImageWidget imgChevron;
    private ref ClanMemberTracker playerDataTracker;
    private bool isUpdatingVisualTracker = false;
    private bool relativeScreenSide = false; // false = left | true = right

    void ClanVisualTracker(ref ClanMemberTracker tracker, Widget parent) {
        int red, green, blue;
        string playerName = tracker.GetName();
        
        parentWidget = parent;
        playerDataTracker = tracker;
        playerVisualTracker = GetGame().GetWorkspace().CreateWidgets("omni_clans\\gui\\layouts\\ClanVisualTracker.layout", parent);
        txtPlayerName = TextWidget.Cast(playerVisualTracker.FindAnyWidget("txtPlayerName"));
        imgChevron = ImageWidget.Cast(playerVisualTracker.FindAnyWidget("imgChevron"));

		GetClanClientManager().GetClientSettings().GetColorSettings(red, green, blue);
        imgChevron.SetColor(ARGB(255, red, green, blue));
        txtPlayerName.SetText(playerDataTracker.GetName());
        txtPlayerName.Show(false);
        playerVisualTracker.Show(false);

        InitPlayerCheckLoop();
    }

    void ~ClanVisualTracker() {
        Print(ClanStatic.debugPrefix + "DELETING VISUAL TRACKER");
        if (playerVisualTracker) {
            playerVisualTracker.Unlink();
        }
    }

    void InitPlayerCheckLoop() {
        RemovePlayerCheckLoop();
        GetGame().GetCallQueue(CALL_CATEGORY_GUI).CallLaterByName(this, "CheckForPlayerBase", 1000, true);
    }

    void RemovePlayerCheckLoop() {
        GetGame().GetCallQueue(CALL_CATEGORY_GUI).RemoveByName(this, "CheckForPlayerBase");
    }

    void InitUpdateLoop() {
        GetGame().GetCallQueue(CALL_CATEGORY_GUI).CallLaterByName(this, "UpdateVisualTracker", 10, true);
    }

    void RemoveUpdateLoop() {
        GetGame().GetCallQueue(CALL_CATEGORY_GUI).RemoveByName(this, "UpdateVisualTracker");
    }

    void CheckForPlayerBase() {
        if (!playerDataTracker || playerDataTracker.GetPlayerId() == GetGame().GetPlayer().GetIdentity().GetId()) {
            RemovePlayerCheckLoop();
            Print("DELETING TRACKER, DATA TRACKER NOT FOUND");
            delete this;
            return;
        }
        Print(ClanStatic.debugPrefix + "Searching for playerbase to show/hide visual tracker!");
        PlayerBase trackerPlayer = playerDataTracker.GetPlayer();

        Print(ClanStatic.debugPrefix + "Player on tracker = " + trackerPlayer);
        if (trackerPlayer && trackerPlayer.IsAlive()) {
            Print(ClanStatic.debugPrefix + "Player base found on tracker! ");
            if (!playerVisualTracker.IsVisible()) {
                playerVisualTracker.Show(true);
                InitUpdateLoop();
            }
        } else {
            Print(ClanStatic.debugPrefix + "Player base could not be found on tracker!");
            playerVisualTracker.Show(false);
            RemoveUpdateLoop();
        }
    }

    void UpdateChevronColor() {
        int red, green, blue;

		GetClanClientManager().GetClientSettings().GetColorSettings(red, green, blue);
        imgChevron.SetColor(ARGB(255, red, green, blue));
    }

    void UpdateVisualTracker() {
        if (!playerDataTracker.GetPlayer()) {
            Print(ClanStatic.debugPrefix + "Cannot find playerbase! Removing chevron update loop!");
            RemoveUpdateLoop();
            playerVisualTracker.Show(false);
            return;
        }
        float posX, posY;
        vector playerHeadPos, relativeScreenPos, screenPos, angleVector;

        playerVisualTracker.Show(true);
        MiscGameplayFunctions.GetHeadBonePos(playerDataTracker.GetPlayer(), playerHeadPos);
        // Shift actual vector numbers instead of pixels. This makes the tracker stick to the target better at range
        //playerHeadPos[0] = playerHeadPos[0] + 0.;
        playerHeadPos[1] = playerHeadPos[1] + 0.5;
        relativeScreenPos = GetGame().GetScreenPosRelative(playerHeadPos);
        screenPos = GetGame().GetScreenPos(playerHeadPos);
        posY = Math.Ceil(screenPos[1]);
        isUpdatingVisualTracker = true;
        angleVector = relativeScreenPos.VectorToAngles();

        if (relativeScreenPos[0] > 0 && relativeScreenPos[0] < 1 && relativeScreenPos[1] > 0 && relativeScreenPos[1] < 1 && relativeScreenPos[2] > 0) {
            if (relativeScreenPos[0] > 0 && relativeScreenPos[0] < 0.5) {
                relativeScreenSide = false;
            } else if (relativeScreenPos[0] >= 0.5 && relativeScreenPos[0] < 1) {
                relativeScreenSide = true;
            }
            if (relativeScreenPos[0] > 0.489 && relativeScreenPos[0] < 0.51 && relativeScreenPos[1] > 0.43 && relativeScreenPos[1] < 0.48) {
                playerVisualTracker.SetColor(ClanColors.TRANS_WHITE);
                txtPlayerName.Show(true);
            } else {
                playerVisualTracker.SetColor(ClanColors.WHITE);
                txtPlayerName.Show(false);
            }
            screenPos = GetGame().GetScreenPos(playerHeadPos);
            posX = Math.Ceil(screenPos[0]);
            playerVisualTracker.SetPos(posX, posY);
        } else {
            // This will attach the tracker to whichever side of the screen the player is closest
            // instead of hiding it. Could be useful for combat.

            int screenW, screenH;

            GetScreenSize(screenW, screenH);

            if (relativeScreenPos[0] > 0 && relativeScreenPos[0] < 0.5) {
                relativeScreenSide = true;
            } else if (relativeScreenPos[0] < 1 && relativeScreenPos[0] >= 0.5) {
                relativeScreenSide = false;
            }
            if (relativeScreenSide) {
                playerVisualTracker.SetPos(screenW - 30, posY);
            } else {
                playerVisualTracker.SetPos(0, posY);
            }
        }
    }

    string GetPlayerId() {
        return playerDataTracker.GetPlayerId();
    }
}