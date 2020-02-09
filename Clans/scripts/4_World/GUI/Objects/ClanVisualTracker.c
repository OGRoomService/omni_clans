class ClanVisualTracker {
    private ref Widget parentWidget, playerVisualTracker;
    private ref TextWidget txtPlayerName;
    private ref ImageWidget imgChevron;
    private ref ClanMemberTracker playerDataTracker;
    private bool isUpdatingVisualTracker = false;
    private bool relativeScreenSide = false; // false = left | true = right

    void ClanVisualTracker(ref ClanMemberTracker tracker, Widget parent) {
        string playerName = tracker.GetName();
        
        parentWidget = parent;
        playerDataTracker = tracker;
        playerVisualTracker = GetGame().GetWorkspace().CreateWidgets("Clans\\layouts\\ClanVisualTracker.layout", parent);
        txtPlayerName = TextWidget.Cast(playerVisualTracker.FindAnyWidget("txtPlayerName"));
        playerVisualTracker.Show(false);

        if (playerName == string.Empty) {
            playerName = "SOME RANDOM AI";
        }
        txtPlayerName.SetText(playerDataTracker.GetName());
        txtPlayerName.Show(false);

        GetGame().GetCallQueue(CALL_CATEGORY_GUI).Remove(this.CheckForPlayerBase);
        GetGame().GetCallQueue(CALL_CATEGORY_GUI).CallLater(this.CheckForPlayerBase, 1000, true);
    }

    void ~ClanVisualTracker() {
        Print("DELETING VISUAL TRACKER");
        if (playerVisualTracker) {
            playerVisualTracker.Unlink();
        }
    }

    void CheckForPlayerBase() {
        if (!playerDataTracker) {
            //Print("DELETING TRACKER, DATA TRACKER NOT FOUND");
            delete this;
            return;
        }
        if (playerDataTracker.GetPlainId() == GetClanClientManager().GetPlainId()) {
            //Print("Visual tracker is current clients tracker! Deleting visual and removing call queue");
            if (playerVisualTracker) {
                playerVisualTracker.Unlink();
                delete playerVisualTracker;
            }
            GetGame().GetCallQueue(CALL_CATEGORY_GUI).Remove(this.CheckForPlayerBase);
            return;
        } else {
            //Print("Searching for playerbase to show/hide visual tracker!");
            if (playerDataTracker.GetPlayer()) {
                //Print("Player base found on tracker!");
                if (!isUpdatingVisualTracker) {
                    playerVisualTracker.Show(true);
                    GetGame().GetCallQueue(CALL_CATEGORY_GUI).Remove(this.UpdateVisualTracker);
                    GetGame().GetCallQueue(CALL_CATEGORY_GUI).CallLater(this.UpdateVisualTracker, 10, true);
                }
            } else {
                //Print("Player base could not be found on tracker!");
                playerVisualTracker.Show(false);
                GetGame().GetCallQueue(CALL_CATEGORY_GUI).Remove(this.UpdateVisualTracker);
            }
        }
    }

    void UpdateVisualTracker() {
        if (!playerDataTracker.GetPlayer()) {
            isUpdatingVisualTracker = false;
            return;
        }
        if (!playerVisualTracker.IsVisible()) {
            playerVisualTracker.Show(true);
        }
        float posX, posY;
        vector playerHeadPos, relativeScreenPos, screenPos, angleVector;

        MiscGameplayFunctions.GetHeadBonePos(playerDataTracker.GetPlayer(), playerHeadPos);
        // Shift actual vector numbers instead of pixels. This makes the tracker stick to the target better at range
        playerHeadPos[1] = playerHeadPos[1] + 0.5;
        //playerHeadPos[0] = playerHeadPos[0] + 0.1;
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
                playerVisualTracker.SetColor(ARGB(180, 255, 255, 255));
                txtPlayerName.Show(true);
            } else {
                playerVisualTracker.SetColor(ARGB(255, 255, 255, 255));
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
}