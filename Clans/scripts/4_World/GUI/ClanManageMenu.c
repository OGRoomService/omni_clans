class ClanManageMenu : ClanMenu {
	// For ranks there's just going to be a configurable map per int. So ranks will be stored as integers and displayed as strings through the correct key of the integer.

    override Widget Init() {
        wRoot = GetGame().GetWorkspace().CreateWidgets("Clans\\layouts\\ClanManage.layout");
        wRoot.Show(false);

        return wRoot;
    }

	override void OnShow() {
		super.OnShow();
		
		ref ActiveClan clan = GetClanClientManager().GetClan();
		TextWidget title = TextWidget.Cast(wRoot.FindAnyWidget("txtTitle"));
		TextWidget onNum = TextWidget.Cast(wRoot.FindAnyWidget("txtOnNum"));
		TextWidget fundsNum = TextWidget.Cast(wRoot.FindAnyWidget("txtFundNum"));

		title.SetText("" + clan.GetName());
		onNum.SetText("" + clan.CountOnline());
		fundsNum.SetText("" + clan.GetFunds());
	}

/* 	override bool OnClick(Widget w, int x, int y, int button) {
		super.OnClick(w, x, y, button);
		
		if (button == MouseState.LEFT) {
			string widgetName = w.GetName();

			if (widgetName == "btnCancelPin") {
				GetGame().GetUIManager().HideScriptedMenu(this);
				return true;
			}
		} else {
			int textLength = boxEditPin.GetText().Length();
			if (textLength < 3) {
				ShowCLMenuError("short");
				return true;
			}
		}
		return false;
	} */
}