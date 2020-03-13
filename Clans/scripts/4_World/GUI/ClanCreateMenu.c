class ClanCreateMenu : ClanMenu {
	private static const string STATE_CREATE_CLAN = "createclan";
	private static const string STATE_ACCEPT_INVITE = "acceptinvite";
	private static const string STATE_CONFIRM_CREATION = "confirmcreation";
	private static const string STATE_STATUS_NO_FUNDS = "nofunds";

	protected ref Widget rootFrame, inviteRoot;
	protected ref TextWidget txtPlayerList, txtClanName;
	protected ref ButtonWidget btnManageClan, btnInviteClan, btnCreateClan, btnAccept, btnDecline, btnCloseMenu;
	protected ref GridSpacerWidget gridPlayerList, gridLeaderBoard;
	private ref array<ref ClanUser> playerList;
	private ref array<ref ClanPlayerListGrid> arrayGridPlayerList;
	private ref array<ref ClanGenericGrid> arrayGenericGrids;
	private ref ClanTextWidget txtSelectedPlayer;

	override Widget Init() {
		float pnlW, pnlH, pnlX, pnlY;

		arrayGridPlayerList = new array<ref ClanPlayerListGrid>();
		arrayGenericGrids = new array<ref ClanGenericGrid>();

		rootFrame = GetGame().GetWorkspace().CreateWidgets("omni_clans\\gui\\layouts\\ClanCreateMenu.layout");
		wRoot = rootFrame.FindAnyWidget("rootCreate");
		inviteRoot = rootFrame.FindAnyWidget("rootInvite");

		// For dialogs
		InitInputDialog();
		InitConfirmationDialog();
		InitStatusDialog();

		// For createRoot
		txtPlayerList = TextWidget.Cast(rootFrame.FindAnyWidget("txtPlayerList"));
		btnCreateClan = ButtonWidget.Cast(rootFrame.FindAnyWidget("btnCreateClan"));
		btnManageClan = ButtonWidget.Cast(rootFrame.FindAnyWidget("btnManageClan"));
		btnInviteClan = ButtonWidget.Cast(rootFrame.FindAnyWidget("btnInviteToClan"));
		btnCloseMenu = ButtonWidget.Cast(rootFrame.FindAnyWidget("btnClose"));
		gridPlayerList = GridSpacerWidget.Cast(rootFrame.FindAnyWidget("gridPlayerList"));
		gridLeaderBoard = GridSpacerWidget.Cast(rootFrame.FindAnyWidget("gridLeaderBoard"));

		// For inviteRoot
		txtClanName = TextWidget.Cast(rootFrame.FindAnyWidget("txtClanName"));
		btnAccept = ButtonWidget.Cast(rootFrame.FindAnyWidget("btnAcceptInvite"));
		btnDecline = ButtonWidget.Cast(rootFrame.FindAnyWidget("btnDeclineInvite"));
		rootFrame.Show(false);

		return rootFrame;
	}

	override bool OnClick(Widget w, int x, int y, int button) {
		super.OnClick(w, x, y, button);

		if (button == MouseState.LEFT) {
			string widgetName = w.GetName();
			string clanName;

			switch (w) {
				case btnStatusOK:
					{
						HideStatusDialog();
						break;
					}
				case btnConfirmationYes:
					{
						switch (confirmationDialogState) {
							case STATE_ACCEPT_INVITE:
								{
									Print(ClanStatic.debugPrefix + "ACCEPTING INVITE");
									ManageInvitation(true);
									HideConfirmationDialog();
									break;
								}
							case STATE_CONFIRM_CREATION:
								{
									Print(ClanStatic.debugPrefix + "ACCEPTING CLAN CREATION COST");
									ShowInputDialog(STATE_CREATE_CLAN);
									HideConfirmationDialog();
									break;
								}
						}
						break;
					}
				case btnConfirmationNo:
					{
						HideConfirmationDialog();
						CheckInvite();
						break;
					}
				case btnAccept:
					{
						if (GetClanClientManager().GetClan()) {
							ShowConfirmationDialog(STATE_ACCEPT_INVITE);
							inviteRoot.Show(false);
						} else {
							ManageInvitation(true);
						}
						break;
					}
				case btnDecline:
					{
						ManageInvitation();
						break;
					}
				case btnDialogConfirm:
					{
						if (!IsRateLimited()) {
							clanName = edtDialogBox.GetText();
							auto params = new Param1<string>(clanName);

							SetRateLimited();
							GetGame().RPCSingleParam(player, ClanRPCEnum.ServerCreateClan, params, true);
						}
						break;
					}
				case btnCreateClan:
					{
						ShowConfirmationDialog(STATE_CONFIRM_CREATION);
						break;
					}
				case btnManageClan:
					{
						GetGame().GetUIManager().HideScriptedMenu(this);
						GetGame().GetUIManager().EnterScriptedMenu(ClanMenuEnum.ManageMenu, null);
						break;
					}
				case btnInviteClan:
					{
						if (txtSelectedPlayer) {
							ref ActiveClan clan = GetClanClientManager().GetClan();
							string targetPlayerId = txtSelectedPlayer.GetPlayerId();

							if (!clan.IsPlayerInvited(targetPlayerId)) {
								if (!IsRateLimited()) {
									auto paramsSendInvite = new Param1<string>(txtSelectedPlayer.GetPlayerId());
									GetGame().RPCSingleParam(player, ClanRPCEnum.ServerAddInvitation, paramsSendInvite, true);
									SetRateLimited();
								}
							}
						}
						break;
					}
				case btnCloseMenu:
					{
						GetGame().GetUIManager().HideScriptedMenu(this);
						break;
					}
			}
		}
		return false;
	}

	bool OnMouseButtonUp(Widget w, int x, int y, int button) {
		super.OnMouseButtonUp(w, x, y, button);

		if (button == MouseState.LEFT) {
			if (TextWidget.Cast(w)) {
				foreach (ClanPlayerListGrid spacer : arrayGridPlayerList) {
					if (spacer) {
						ref ClanTextWidget txtWidget = spacer.GetChild(w);
						ref ActiveClan clan = GetClanClientManager().GetClan();

						if (txtWidget && txtWidget.GetPlayerId() != GetGame().GetPlayer().GetIdentity().GetId()) {
							if (clan && clan.IsPlayerInClan(txtWidget.GetPlayerId())) { return false; }
							if (txtSelectedPlayer) {
								txtSelectedPlayer.SetColor(ClanColors.GRAY);
								txtSelectedPlayer.HideBackground();
							}
							txtSelectedPlayer = txtWidget;
							txtWidget.SetColor(ClanColors.WHITE);
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
			foreach (ClanPlayerListGrid spacer : arrayGridPlayerList) {
				if (spacer) {
					ref ActiveClan clan = GetClanClientManager().GetClan();
					ref ClanTextWidget txtWidget = spacer.GetChild(w);

					if (txtWidget && txtWidget != txtSelectedPlayer && txtWidget.GetPlayerId() != GetGame().GetPlayer().GetIdentity().GetId()) {
						if (clan && clan.IsPlayerInClan(txtWidget.GetPlayerId())) { return false; }
						txtWidget.SetColor(ClanColors.WHITE);
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
			foreach (ClanPlayerListGrid spacer : arrayGridPlayerList) {
				if (spacer) {
					ref ActiveClan clan = GetClanClientManager().GetClan();
					ref ClanTextWidget txtWidget = spacer.GetChild(w);

					if (txtWidget && txtWidget != txtSelectedPlayer && txtWidget.GetPlayerId() != GetGame().GetPlayer().GetIdentity().GetId()) {
						if (clan && clan.IsPlayerInClan(txtWidget.GetPlayerId())) { return false; }
						txtWidget.SetColor(ClanColors.GRAY);
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
		BuildLeaderboard();
	}

	override void OnHide() {
		super.OnHide();
	}

	void RefreshPlayerList() {
		string previousPlayerId;

		if (txtSelectedPlayer) {
			previousPlayerId = txtSelectedPlayer.GetPlayerId();
		}
		arrayGridPlayerList = new array<ref ClanPlayerListGrid>();
		txtSelectedPlayer = null;

		BuildPlayerList(previousPlayerId);
	}

	void BuildPlayerList(string previousPlayerId = "") {
		ref ActiveClan clan;
		ClanPlayerListGrid newGrid;
		ClanTextWidget newText;
		int i;
		string playerId;

		i = 0;
		clan = GetClanClientManager().GetClan();
		newGrid = new ClanPlayerListGrid(gridPlayerList);
		playerList = GetClanManager().GetPlayerList();
		txtPlayerList.SetText("Players Online: " + playerList.Count());

		arrayGridPlayerList.Insert(newGrid);

		foreach (ClanUser user : playerList) {
			if (i > 99) {
				i = 0;
				newGrid = new ClanPlayerListGrid(gridPlayerList);
				arrayGridPlayerList.Insert(newGrid);
			}
			newText = newGrid.AddTextWidget(user, user.GetName());

			if (user.GetId() == previousPlayerId) {
				txtSelectedPlayer = newText;
				txtSelectedPlayer.SetColor(ClanColors.WHITE);
				txtSelectedPlayer.ShowBackground();
			}
			if (user.GetId() == GetGame().GetPlayer().GetIdentity().GetId() || clan && clan.IsPlayerInClan(user.GetId())) {
				newText.SetColor(ClanColors.RED);
			}
			i++;
		}
	}

	void BuildLeaderboard() {
		ClanGenericGrid newGrid;
		int i;
		ref array<ref ClanBase> clans = new array<ref ClanBase>();
		clans = GetClanManager().GetClanLeaderboard();
		delete arrayGenericGrids;
		arrayGenericGrids = new array<ref ClanGenericGrid>();

		foreach (ClanBase c : clans) {
			if (i >= 15) { break; }
			if (c) {
				TextWidget clanName, clanRank;
				GridSpacerWidget grid;

				newGrid = new ClanGenericGrid(gridLeaderBoard, "ClanLeaderGrid");
				grid = newGrid.GetGrid();
				clanName = TextWidget.Cast(grid.FindAnyWidget("clanName"));
				clanRank = TextWidget.Cast(grid.FindAnyWidget("clanRank"));
				clanName.SetText("" + c.GetName());
				clanRank.SetText("" + c.GetRank());
				arrayGenericGrids.Insert(newGrid);

				if ((i % 2) == 1) {
					grid.SetColor(ClanColors.DARK_GRAY);
				}
			}
			i++;
		}
	}

	void CheckClan() {
		ref ActiveClan clan = GetClanClientManager().GetClan();

		btnCreateClan.Show(true);
		btnManageClan.Show(true);
		btnInviteClan.Show(true);
		inputRoot.Show(false);

		if (!clan) {
			btnManageClan.Show(false);
			btnInviteClan.Show(false);
		} else {
			ref ClanMember clientMember = clan.GetClanMemberByPlayerId(GetGame().GetPlayer().GetIdentity().GetId());

			if (clientMember) {
				ref ClanConfig config = GetClanManager().GetConfig();
				int clientMemberRank = clientMember.GetPlayerRank();

				if (!config.CanInviteMembers(clientMemberRank)) {
					btnInviteClan.Show(false);
				}
			} else {
				GetGame().GetUIManager().HideScriptedMenu(this);
			}
			btnCreateClan.Show(false);
			btnManageClan.Show(true);
			btnInviteClan.Show(true);
		}
	}

	void CheckInvite() {
		if (GetClanClientManager().GetInvite()) {
			txtClanName.SetText(GetClanClientManager().GetInvite().GetClanName());
			inviteRoot.Show(true);
		} else {
			inviteRoot.Show(false);
		}
	}

	private void ManageInvitation(bool acceptedInvite = false) {
		if (GetClanClientManager().GetInvite()) {
			auto paramManageInvite = new Param2<string, bool>(GetClanClientManager().GetInvite().GetClanId(), acceptedInvite);
			GetGame().RPCSingleParam(player, ClanRPCEnum.ServerManageInvite, paramManageInvite, true);
			GetClanClientManager().DeleteInvitation();
		}
		inviteRoot.Show(false);
	}

	override void ShowInputDialog(string type) {
		super.ShowInputDialog(type);

		switch (type) {
			case STATE_CREATE_CLAN:
				{
					inputDialogState = type;
					txtDialogHeader.SetText("ENTER CLAN NAME");
					btnDialogConfirm.SetText("CREATE");
					inputRoot.Show(true);
				}
		}
		FixDialogSize(inputDialog, txtDialogHeader, edtDialogBox);
	}

	override void ShowConfirmationDialog(string state) {
		super.ShowConfirmationDialog(state);

		string questionText;
		confirmationDialogState = state;
		txtConfirmationHeader.SetText("ARE YOU SURE YOU WANT TO");

		switch (state) {
			case STATE_ACCEPT_INVITE:
				{
					txtConfirmationHeader.SetText("ARE YOU SURE YOU WANT TO ACCEPT CLAN INVITE?");
					questionText = "THIS WILL REMOVE YOU FROM YOUR CURRENT CLAN!";
					break;
				}
			case STATE_CONFIRM_CREATION:
				{
					txtConfirmationHeader.SetText("ARE YOU SURE YOU WANT TO CREATE A CLAN?");
					questionText = "THIS WILL COST: " + GetClanManager().GetConfig().GetClanCreationCost() + "!";
					break;
				}
		}
		questionText.ToUpper();
		txtConfirmationQuestion.SetText(questionText);
		FixDialogSize(confirmationDialog, txtConfirmationHeader, txtConfirmationQuestion);
	}

	override void HandleError(int error) {
		switch (error) {
			case ClanCreateErrorEnum.CLAN_EXISTS:
				{
					txtDialogHeader.SetText("CLAN BY THAT NAME ALREADY EXISTS!");
					break;
				}
			case ClanCreateErrorEnum.IN_CLAN:
				{
					txtDialogHeader.SetText("YOU ARE ALREADY IN A CLAN!");
					break;
				}
			case ClanCreateErrorEnum.INVALID_NAME:
				{
					txtDialogHeader.SetText("YOU CAN'T CREATE A CLAN BY THAT NAME!");
					break;
				}
			case ClanCreateErrorEnum.NO_FUNDS:
				{
					HideInputDialog();
					ShowStatusDialog(STATE_STATUS_NO_FUNDS);
					return;
					break;
				}
		}
		FixDialogSize(inputDialog, txtDialogHeader, edtDialogBox);
	}

	override void ShowStatusDialog(string state) {
		super.ShowStatusDialog(state);

		string line1, line2;

		switch (state) {
			case STATE_STATUS_NO_FUNDS:
				{
					line1 = "CANNOT CREATE CLAN!";
					line2 = "NOT ENOUGH FUNDS!";
					break;
				}
		}
		txtStatusDialogLine1.SetText(line1);
		txtStatusDialogLine2.SetText(line2);
		FixDialogSize(menuStatusDialog, txtStatusDialogLine1, txtStatusDialogLine2);
	}
}