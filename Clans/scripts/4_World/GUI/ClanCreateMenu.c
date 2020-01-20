class ClanCreateMenu : ClanMenu {
	protected ref Widget inputRoot, createRoot, inviteRoot;
	protected ref TextWidget txtBox, txtPlayerList, txtClanName;
	protected ref EditBoxWidget editBox;
	protected ref ButtonWidget btnManageClan, btnJoinClan, btnInviteClan, btnCreateClan, btnAccept, btnDecline;
	protected ref GridSpacerWidget gridPlayerList, gridLeaderBoard;
	private ref array<ref ClanUser> playerList;
	private ref array<ref ClanGridSpacer> arrayGridPlayerList;
	private ref ClanTextWidget txtSelectedPlayer;

	override Widget Init() {
		arrayGridPlayerList = new array<ref ClanGridSpacer>();

		wRoot = GetGame().GetWorkspace().CreateWidgets("Clans\\layouts\\ClanCreateMenu.layout");
		inputRoot = wRoot.FindAnyWidget("rootInput");
		createRoot = wRoot.FindAnyWidget("rootCreate");
		inviteRoot = wRoot.FindAnyWidget("rootInvite");
		// For inputRoot
		editBox = EditBoxWidget.Cast(wRoot.FindAnyWidget("edtInput"));
		txtBox = TextWidget.Cast(wRoot.FindAnyWidget("txtEnter"));
		// For createRoot
		txtPlayerList = TextWidget.Cast(wRoot.FindAnyWidget("txtPlayerList"));
		btnCreateClan = ButtonWidget.Cast(wRoot.FindAnyWidget("btnCreateClan"));
		btnManageClan = ButtonWidget.Cast(wRoot.FindAnyWidget("btnManageClan"));
		btnJoinClan = ButtonWidget.Cast(wRoot.FindAnyWidget("btnJoinClan"));
		btnInviteClan = ButtonWidget.Cast(wRoot.FindAnyWidget("btnInviteToClan"));
		gridPlayerList = GridSpacerWidget.Cast(wRoot.FindAnyWidget("gridPlayerList"));
		gridLeaderBoard = GridSpacerWidget.Cast(wRoot.FindAnyWidget("gridLeaderBoard"));
		// For inviteRoot
		txtClanName = TextWidget.Cast(wRoot.FindAnyWidget("txtClanName"));
		btnAccept = ButtonWidget.Cast(wRoot.FindAnyWidget("btnAcceptInvite"));
		btnDecline = ButtonWidget.Cast(wRoot.FindAnyWidget("btnDeclineInvite"));
		wRoot.Show(false);

		return wRoot;
	}

	override bool OnClick(Widget w, int x, int y, int button) {
		super.OnClick(w, x, y, button);

		if (button == MouseState.LEFT) {
			string widgetName = w.GetName();
			string clanName;

			if (inputRoot.IsVisible()) {
				if (widgetName == "btnEnter") {
					clanName = editBox.GetText();
					auto params = new Param1<string>(clanName);

					GetGame().RPCSingleParam(player, ClanRPCEnum.ServerCreateClan, params, true);

					return true;
				}
			} else {
				if (w == btnCreateClan) {
					ToggleMenus();
					return true;
				} else if (inviteRoot.IsVisible()) {
					if (w.GetName() == "btnAcceptInvite" || w.GetName() == "btnDeclineInvite") {
						Print("Doing some bullshit with the invite");
						clanName = GetClanClientManager().GetInvite();
						
						if (clanName != string.Empty) {
							Print("clan invite exists");
							auto paramManageInvite = new Param2<string, string>(GetClanClientManager().GetInvite(), w.GetName());
							GetGame().RPCSingleParam(GetGame().GetPlayer(), ClanRPCEnum.ServerManageInvite, paramManageInvite, true);
							GetClanClientManager().DeleteInvitation();
						}
						inviteRoot.Show(false);
					}
				}
				if (GetClanClientManager().GetClan()) {
					if (w == btnManageClan) {
						GetGame().GetUIManager().HideScriptedMenu(this);
						// Call later to ensure the menu was closed properly before opening a new one.
						GetGame().GetCallQueue(CALL_CATEGORY_SYSTEM).CallLater(GetGame().GetUIManager().EnterScriptedMenu, 100, false, ClanMenuEnum.ManageMenu, null);
						return true;
					} else if (w.GetName() == "btnInviteToClan") {
						if (txtSelectedPlayer) {
							Print("sending invite rpc to server");
							auto paramsSendInvite = new Param2<string, string>(GetClanClientManager().GetClan().GetName(), txtSelectedPlayer.GetClanUser().GetId());
							GetGame().RPCSingleParam(GetGame().GetPlayer(), ClanRPCEnum.ServerAddInvitation, paramsSendInvite, true);
							return true;
						}
					}
				}
			}
			Print("MENU Clicked widgetName=" + widgetName);
		}
		return false;
	}

	bool OnMouseButtonUp(Widget w, int x, int y, int button) {
		super.OnMouseButtonUp(w, x, y, button);

		Print("Mouse Button Up Event: widgetname=" + w.GetName());

		if (button == MouseState.LEFT) {
			if (TextWidget.Cast(w)) {
				foreach (ClanGridSpacer spacer : arrayGridPlayerList) {
					if (spacer) {
						ref ClanTextWidget txtWidget = spacer.GetChild(w);

						if (txtWidget /*&& txtWidget.GetPlayerId() != GetClanClientManager().GetPlainId()*/) {
							if (txtSelectedPlayer) {
								txtSelectedPlayer.SetColor(ARGB(255, 190, 190, 190));
								txtSelectedPlayer.HideBackground();
							}
							Print("Found widget playerId=" + txtWidget.GetPlayerId());
							txtSelectedPlayer = txtWidget;
							txtWidget.SetColor(ARGB(255, 255, 255, 255));
							txtWidget.ShowBackground();
							return true;
						}
					}
				}
			}
		}
		return false;
	}

	override bool OnMouseEnter(Widget w, int x, int y) {
		super.OnMouseEnter(w, x, y);

		if (TextWidget.Cast(w)) {
			foreach (ClanGridSpacer spacer : arrayGridPlayerList) {
				if (spacer) {
					ref ClanTextWidget txtWidget = spacer.GetChild(w);

					if (txtWidget && txtWidget != txtSelectedPlayer) {
						txtWidget.SetColor(ARGB(255, 255, 255, 255));
						return true;
					}
				}
			}
		}
		return false;
	}

	override bool OnMouseLeave(Widget w, Widget enterW, int x, int y) {
		super.OnMouseLeave(w, enterW, x, y);

		if (TextWidget.Cast(w)) {
			foreach (ClanGridSpacer spacer : arrayGridPlayerList) {
				if (spacer) {
					ref ClanTextWidget txtWidget = spacer.GetChild(w);

					if (txtWidget && txtWidget != txtSelectedPlayer) {
						txtWidget.SetColor(ARGB(255, 190, 190, 190));
						return true;
					}
				}
			}
		}
		return false;
	}

	override void OnShow() {
		super.OnShow();

		CheckClan();
		CheckInvite();
		BuildPlayerList();
	}

	override void OnHide() {
		super.OnHide();
	}

	private void BuildPlayerList() {
		ClanGridSpacer newGrid;
		ClanTextWidget newText;
		int i = 0;
		string playerId = GetClanClientManager().GetPlainId();
		newGrid = new ClanGridSpacer(gridPlayerList);
		playerList = GetClanManager().GetPlayerList();
		txtPlayerList.SetText("Players Online: " + playerList.Count());

		arrayGridPlayerList.Insert(newGrid);

		foreach (ClanUser user : playerList) {
			string userId = user.GetId();

			if (i > 99) {
				i = 0;
				newGrid = new ClanGridSpacer(gridPlayerList);
				arrayGridPlayerList.Insert(newGrid);
			}
			newText = newGrid.AddTextWidget(user, user.GetName());

			if (userId == playerId) {
				newText.SetColor(ARGB(255, 255, 0, 0));
			}
			i++;
		}
	}

	private void CheckClan() {
		if (!GetClanClientManager().GetClan()) {
			btnManageClan.Show(false);
			btnInviteClan.Show(false);
		} else {
			btnJoinClan.Show(false);
			btnCreateClan.Show(false);
		}
	}
	
	void CheckInvite() {
		Print("Checking invite conditionals");
		string invite = GetClanClientManager().GetInvite();
		if (invite != string.Empty) {
			txtClanName.SetText(invite);
			inviteRoot.Show(true);
		}
	}

	override void HandleClose() {
		if (inputRoot.IsVisible()) {
			ToggleMenus();
		} else {
			super.HandleClose();
		}
	}

	private void ToggleMenus() {
		if (inputRoot.IsVisible()) {
			editBox.SetText(string.Empty);
			inputRoot.Show(false);
			createRoot.Show(true);
		} else {
			inputRoot.Show(true);
			createRoot.Show(false);
		}
	}

	override void HandleError(string error) {
		if (inputRoot.IsVisible()) {
			error.ToLower();

			switch (error) {
				case "exit":
					{
						ToggleMenus();
						break;
					}
				default:
					{
						txtBox.SetText(error);
						break;
					}
			}
		}
	}
}