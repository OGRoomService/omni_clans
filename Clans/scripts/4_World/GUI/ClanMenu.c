class ClanMenu : UIScriptedMenu {
	protected static const string STATE_STATUS_NO_SELECTION_KICK = "statusnoselectionkick";
	protected static const string STATE_STATUS_NO_SELECTION_DEMOTE = "statusnoselectiondemote";
	protected static const string STATE_STATUS_NO_SELECTION_PROMOTE = "statusnoselectionpromote";
	protected static const string STATE_STATUS_UPGRADE_FAIL = "statuscannotupgrade";

	protected ref Widget wRoot;
	protected ref Widget inputRoot, inputDialog;
	protected ref Widget confirmationRoot, confirmationDialog;
	protected ref Widget clientSettingsRoot, imgSettingsChevron;
	protected ref Widget menuStatusRoot, menuStatusDialog;
	protected PlayerBase player;
	protected ref TextWidget txtDialogHeader;
	protected ref TextWidget txtConfirmationHeader, txtConfirmationQuestion;
	protected ref TextWidget txtSettingsSldrRed, txtSettingsSldrGreen, txtSettingsSldrBlue;
	protected ref TextWidget txtStatusDialogLine1, txtStatusDialogLine2;
	protected ref EditBoxWidget edtDialogBox;
	protected ref ButtonWidget btnDialogConfirm;
	protected ref ButtonWidget btnConfirmationYes, btnConfirmationNo;
	protected ref ButtonWidget btnSettingsConfirm, btnSettingsCancel;
	protected ref ButtonWidget btnStatusOK;
	protected ref SliderWidget sldrSettingsRed, sldrSettingsGreen, sldrSettingsBlue;
	protected string inputDialogState, confirmationDialogState;
	protected int clanUpgradeCost;
	protected bool rateLimited;

	protected void InitInputDialog() {
		inputRoot = GetGame().GetWorkspace().CreateWidgets("omni_clans\\gui\\layouts\\ClanInputDialog.layout", wRoot);
		inputDialog = inputRoot.FindAnyWidget("rootInput");
		edtDialogBox = EditBoxWidget.Cast(inputRoot.FindAnyWidget("edtInput"));
		txtDialogHeader = TextWidget.Cast(inputRoot.FindAnyWidget("txtHeader"));
		btnDialogConfirm = ButtonWidget.Cast(inputRoot.FindAnyWidget("btnEnter"));

		FixBlur(inputRoot);
		inputRoot.Show(false);
	}

	protected void InitConfirmationDialog() {
		confirmationRoot = GetGame().GetWorkspace().CreateWidgets("omni_clans\\gui\\layouts\\ClanConfirmationDialog.layout", wRoot);
		confirmationDialog = confirmationRoot.FindAnyWidget("rootConfirmation");
		txtConfirmationHeader = TextWidget.Cast(confirmationRoot.FindAnyWidget("txtHeader"));
		txtConfirmationQuestion = TextWidget.Cast(confirmationRoot.FindAnyWidget("txtQuestion"));
		btnConfirmationYes = ButtonWidget.Cast(confirmationRoot.FindAnyWidget("btnYes"));
		btnConfirmationNo = ButtonWidget.Cast(confirmationRoot.FindAnyWidget("btnNo"));

		FixBlur(confirmationRoot);
		confirmationRoot.Show(false);
	}

	protected void InitSettingsDialog() {
		int red, green, blue;

		clientSettingsRoot = GetGame().GetWorkspace().CreateWidgets("omni_clans\\gui\\layouts\\ClanColorSettingsDialog.layout", wRoot);
		btnSettingsConfirm = ButtonWidget.Cast(clientSettingsRoot.FindAnyWidget("btnSave"));
		btnSettingsCancel = ButtonWidget.Cast(clientSettingsRoot.FindAnyWidget("btnCancel"));
		imgSettingsChevron = clientSettingsRoot.FindAnyWidget("imgChevron");
		sldrSettingsRed = SliderWidget.Cast(clientSettingsRoot.FindAnyWidget("sliderRed"));
		sldrSettingsGreen = SliderWidget.Cast(clientSettingsRoot.FindAnyWidget("sliderGreen"));
		sldrSettingsBlue = SliderWidget.Cast(clientSettingsRoot.FindAnyWidget("sliderBlue"));
		txtSettingsSldrRed = TextWidget.Cast(clientSettingsRoot.FindAnyWidget("txtRed"));
		txtSettingsSldrGreen = TextWidget.Cast(clientSettingsRoot.FindAnyWidget("txtGreen"));
		txtSettingsSldrBlue = TextWidget.Cast(clientSettingsRoot.FindAnyWidget("txtBlue"));

		FixBlur(clientSettingsRoot);
		GetClanClientManager().GetClientSettings().GetColorSettings(red, green, blue);
		sldrSettingsRed.SetCurrent(red);
		sldrSettingsGreen.SetCurrent(green);
		sldrSettingsBlue.SetCurrent(blue);
		txtSettingsSldrRed.SetText("" + red);
		txtSettingsSldrGreen.SetText("" + green);
		txtSettingsSldrBlue.SetText("" + blue);
		imgSettingsChevron.SetColor(ARGB(255, red, green, blue));

		clientSettingsRoot.Show(false);
	}

	protected void InitStatusDialog() {
		int red, green, blue;

		menuStatusRoot = GetGame().GetWorkspace().CreateWidgets("omni_clans\\gui\\layouts\\ClanStatusDialog.layout", wRoot);
		menuStatusDialog = menuStatusRoot.FindAnyWidget("statusDialog");
		btnStatusOK = ButtonWidget.Cast(menuStatusRoot.FindAnyWidget("btnOK"));
		txtStatusDialogLine1 = TextWidget.Cast(menuStatusRoot.FindAnyWidget("txtLine1"));
		txtStatusDialogLine2 = TextWidget.Cast(menuStatusRoot.FindAnyWidget("txtLine2"));

		FixBlur(menuStatusRoot);
		menuStatusRoot.Show(false);
	}

	protected void FixBlur(Widget rootWidget) {
		Widget blurPanel;
		float pnlW, pnlH, pnlX, pnlY;

		blurPanel = rootWidget.FindAnyWidget("pnlBlur");

		wRoot.GetSize(pnlW, pnlH);
		wRoot.GetPos(pnlX, pnlY);
		blurPanel.SetSize(pnlW, pnlH);
		blurPanel.SetPos(pnlX, pnlY);
	}

	override bool OnClick(Widget w, int x, int y, int button) {
		super.OnClick(w, x, y, button);

		switch (w) {
			case btnSettingsConfirm:
				{
					GetClanClientManager().GetClientSettings().SetColors(sldrSettingsRed.GetCurrent(), sldrSettingsGreen.GetCurrent(), sldrSettingsBlue.GetCurrent());
					GetClanClientManager().GetHud().UpdateTrackerColors();
					HideSettingsDialog();
					break;
				}
			case btnSettingsCancel:
				{
					HideSettingsDialog();
					break;
				}
		}
		return false;
	}

	override bool OnChange(Widget w, int x, int y, bool finished) {
		if (SliderWidget.Cast(w)) {
			int r, g, b;
			r = sldrSettingsRed.GetCurrent();
			g = sldrSettingsGreen.GetCurrent();
			b = sldrSettingsBlue.GetCurrent();

			imgSettingsChevron.SetColor(ARGB(255, r, g, b));

			switch (w) {
				case sldrSettingsRed:
					{
						txtSettingsSldrRed.SetText("" + r);
						break;
					}
				case sldrSettingsGreen:
					{
						txtSettingsSldrGreen.SetText("" + g);
						break;
					}
				case sldrSettingsBlue:
					{
						txtSettingsSldrBlue.SetText("" + b);
						break;
					}
			}
		}
		return super.OnChange(w, x, y, finished);
	}

	override void OnShow() {
		super.OnShow();

		GetGame().GetMission().PlayerControlDisable(INPUT_EXCLUDE_ALL);
		GetGame().GetUIManager().ShowCursor(true);
		GetGame().GetMission().GetHud().Show(false);
		player = PlayerBase.Cast(GetGame().GetPlayer());
		inputDialogState = string.Empty;
		confirmationDialogState = string.Empty;
	}

	override void OnHide() {
		super.OnHide();

		GetGame().GetUIManager().ShowCursor(false);
		GetGame().GetMission().PlayerControlEnable(1);
		GetGame().GetMission().GetHud().Show(true);
	}

	void HandleClose() {
		if (inputRoot && inputRoot.IsVisible()) {
			HideInputDialog();
		} else if (confirmationRoot && confirmationRoot.IsVisible()) {
			HideConfirmationDialog();
		} else if (menuStatusRoot && menuStatusRoot.IsVisible()) {
			HideStatusDialog();
		} else {
			Close();
		}
	}

	protected void ShowInputDialog(string type) {
		edtDialogBox.SetText(string.Empty);
	}

	protected void HideInputDialog() {
		inputRoot.Show(false);
	}

	protected void ShowConfirmationDialog(string state) {
		confirmationRoot.Show(true);
	}

	protected void HideConfirmationDialog() {
		confirmationRoot.Show(false);
	}

	protected void ShowStatusDialog(string state) {
		menuStatusRoot.Show(true);
	}

	protected void HideStatusDialog() {
		menuStatusRoot.Show(false);
	}

	protected void ShowSettingsDialog() {
		clientSettingsRoot.Show(true);
	}

	protected void HideSettingsDialog() {
		clientSettingsRoot.Show(false);
	}

	void HandleError(int error) { }

	void FixDialogSize(Widget dialog, Widget w1, Widget w2) {
		TextWidget txt1, txt2;
		int txt1W, txt1H, txt2W, txt2H;
		float rootW, rootH, widget1W, widget1H, widget2W, widget2H;

		Print("Fixing dialog size " + w1 + " || " + w2);

		if (w1) {
			txt1 = TextWidget.Cast(w1);

			if (txt1) {
				txt1.GetTextSize(txt1W, txt1H);
			} else {
				w1.GetSize(widget1W, widget1H);
			}
		}
		if (w2) {
			txt2 = TextWidget.Cast(w2);
		
			if (txt2) {
				txt2.GetTextSize(txt2W, txt2H);
			} else {
				w2.GetSize(widget2W, widget2H);
			}
		}
		dialog.GetSize(rootW, rootH);
		
		if (txt1 && txt2) {
			if (txt1W > txt2W) {
				dialog.SetSize((txt1W + 20), rootH);
			} else {
				dialog.SetSize((txt2W + 20), rootH);
			}
		}
		if (txt1 && !txt2) {
			if (txt1W > widget2W) {
				dialog.SetSize((txt1W + 20), rootH);
			} else {
				dialog.SetSize((widget2W + 20), rootH);
			}
		} else if (!txt2 && txt2) {
			if (txt2W > widget1W) {
				dialog.SetSize((txt2W + 20), rootH);
			} else {
				dialog.SetSize((widget1W + 20), rootH);
			}
		} else if (!txt1 && !txt2) {
			if (widget1W > widget2W) {
				dialog.SetSize((widget1W + 20), rootH);
			} else {
				dialog.SetSize((widget2W + 20), rootH);
			}
		}
	}

	bool IsInputDialogVisible() {
		return inputRoot.IsVisible();
	}

	void SetRateLimited() {
		if (rateLimited) { return; }
		rateLimited = true;

		GetGame().GetCallQueue(CALL_CATEGORY_GUI).RemoveByName(this, "RemoveRateLimit");
		GetGame().GetCallQueue(CALL_CATEGORY_GUI).CallLaterByName(this, "RemoveRateLimit", 1000, true);
	}

	void RemoveRateLimit() {
		rateLimited = false;
	}

	bool IsRateLimited() {
		return rateLimited;
	}
}