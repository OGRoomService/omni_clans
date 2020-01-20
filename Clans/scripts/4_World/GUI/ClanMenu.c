class ClanMenu : UIScriptedMenu {
	protected ref Widget wRoot;
	protected PlayerBase player;

	override void OnShow() {
		super.OnShow();

		GetGame().GetMission().PlayerControlDisable(INPUT_EXCLUDE_ALL);
		GetGame().GetUIManager().ShowCursor(true);
		GetGame().GetMission().GetHud().Show(false);
		player = PlayerBase.Cast(GetGame().GetPlayer());
	}

	override void OnHide() {
		super.OnHide();

		GetGame().GetUIManager().ShowCursor(false);
		GetGame().GetMission().PlayerControlEnable(1);
		GetGame().GetMission().GetHud().Show(true);
	}

	void HandleClose() {
		Close();
	}

	void HandleError(string error) { }
}