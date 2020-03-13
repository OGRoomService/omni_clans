class ClanManageMenu : ClanMenu {
	private static const string STATE_CONTRIBUTE = "contribute";
	private static const string STATE_LEAVE = "leave";
	private static const string STATE_KICK = "kick";
	private static const string STATE_DELETE_FIRST = "deletefirst";
	private static const string STATE_DELETE_SECOND = "deletesecond";
	private static const string STATE_UPGRADE = "upgrade";
	private static const string STATE_NO_FUNDS = "nofunds";
	private static const int KEYCODE_MIN_NUM = 48;
	private static const int KEYCODE_MAX_NUM = 57;

	private ref GridSpacerWidget rootGrid;
	private ref ClanMemberGrid selectedGrid;
	private ref array<ref ClanGenericGrid> arrayAllGrids;
	private ref array<ref ClanMember> arraySortedMembers;
	private ref TextWidget txtTitle, txtOnline, txtFunds;
	private ref ActiveClan clan;
	private ref ButtonWidget btnContribute, btnLeave, btnPromote, btnDemote, btnKick, btnUpgrade, btnDisband, btnClose, btnSettings;

	override Widget Init() {
		arrayAllGrids = new array<ref ClanGenericGrid>();
		arraySortedMembers = new array<ref ClanMember>();

		wRoot = GetGame().GetWorkspace().CreateWidgets("omni_clans\\gui\\layouts\\ClanManage.layout");
		rootGrid = GridSpacerWidget.Cast(wRoot.FindAnyWidget("gridMembers"));
		txtTitle = TextWidget.Cast(wRoot.FindAnyWidget("txtTitle"));
		txtOnline = TextWidget.Cast(wRoot.FindAnyWidget("txtOnNum"));
		txtFunds = TextWidget.Cast(wRoot.FindAnyWidget("txtFundNum"));
		btnContribute = ButtonWidget.Cast(wRoot.FindAnyWidget("btnContribute"));
		btnLeave = ButtonWidget.Cast(wRoot.FindAnyWidget("btnLeave"));
		btnPromote = ButtonWidget.Cast(wRoot.FindAnyWidget("btnPromote"));
		btnDemote = ButtonWidget.Cast(wRoot.FindAnyWidget("btnDemote"));
		btnUpgrade = ButtonWidget.Cast(wRoot.FindAnyWidget("btnUpgrade"));
		btnDisband = ButtonWidget.Cast(wRoot.FindAnyWidget("btnDisband"));
		btnKick = ButtonWidget.Cast(wRoot.FindAnyWidget("btnKick"));
		btnClose = ButtonWidget.Cast(wRoot.FindAnyWidget("btnClose"));
		btnSettings = ButtonWidget.Cast(wRoot.FindAnyWidget("btnSettings"));
		clan = GetClanClientManager().GetClan();

		// For inputRoot
		InitInputDialog();
		InitConfirmationDialog();
		InitSettingsDialog();
		InitStatusDialog();

		// Other stuff
		wRoot.Show(false);
		btnContribute.Show(false);
		btnLeave.Show(false);
		btnPromote.Show(false);
		btnDemote.Show(false);
		btnKick.Show(false);
		btnUpgrade.Show(false);
		btnDisband.Show(false);

		return wRoot;
	}

	bool OnMouseButtonUp(Widget w, int x, int y, int button) {
		super.OnMouseButtonUp(w, x, y, button);

		if (button == MouseState.LEFT) {
			foreach (ClanGenericGrid genericGrid : arrayAllGrids) {
				ref array<ref ClanMemberGrid> gridChildren = genericGrid.GetMemberChildren();

				if (gridChildren && gridChildren.Count() > 0) {
					foreach (ClanMemberGrid memberGrid : gridChildren) {
						Widget memberGridRoot = memberGrid.GetRoot();

						if (memberGridRoot && memberGridRoot == w) {
							if (selectedGrid) {
								selectedGrid.HideBackground();
							}
							if (memberGrid.GetMember().GetPlayerId() == GetGame().GetPlayer().GetIdentity().GetId()) {
								if (selectedGrid) {
									selectedGrid = null;
								}
								return true;
							}
							selectedGrid = memberGrid;
							selectedGrid.ShowBackground();
							return true;
						}
					}
				}
			}
		}
		return false;
	}

	override void OnShow() {
		super.OnShow();

		BuildUI();
	}

	void BuildUI() {
		HideButtons();
		RefreshText();
		Refresh();
	}

	void HideButtons() {
		ref ClanMember clientMember = clan.GetClanMemberByPlayerId(GetGame().GetPlayer().GetIdentity().GetId());

		if (clientMember) {
			ref ClanConfig config = GetClanManager().GetConfig();
			int clientMemberRank = clientMember.GetPlayerRank();

			if (clan.GetOwnerId() == GetGame().GetPlayer().GetIdentity().GetId()) {
				btnPromote.Show(true);
				btnDemote.Show(true);
				btnKick.Show(true);
				btnUpgrade.Show(true);
				btnDisband.Show(true);
				btnContribute.Show(true);
			} else {
				btnLeave.Show(true);

				if (config.CanKickMembers(clientMemberRank)) {
					btnKick.Show(true);
				}
				if (config.CanPromoteMembers(clientMemberRank)) {
					btnPromote.Show(true);
				}
				if (config.CanDemoteMembers(clientMemberRank)) {
					btnDemote.Show(true);
				}
				if (config.CanContributeFunds(clientMemberRank)) {
					btnContribute.Show(true);
				}
				if (config.CanUpgradeClan(clientMemberRank)) {
					btnUpgrade.Show(true);
				}
			}
		} else {
			GetGame().GetUIManager().HideScriptedMenu(this);
		}
	}

	void RefreshText() {
		txtTitle.SetText("" + clan.GetName() + " Level: " + clan.GetRank());
		txtOnline.SetText("" + clan.CountOnline() + "/" + GetClanManager().GetConfig().GetClanPlayerCap(clan.GetRank()));
		txtFunds.SetText("" + clan.GetFunds());
	}

	void Refresh() {
		string previousSelectionId;
		ref array<ref ClanMember> clanMembers;

		if (selectedGrid) {
			previousSelectionId = selectedGrid.GetMember().GetPlayerId();
		}
		clanMembers = new array<ref ClanMember>();
		arraySortedMembers = new array<ref ClanMember>();
		arrayAllGrids = new array<ref ClanGenericGrid>();
		clanMembers = clan.GetMembers();

		foreach (ClanMember member : clanMembers) {
			SortClanMember(member);
		}
		BuildLeaderboard(previousSelectionId);
	}

	void SortClanMember(ref ClanMember member) {
		int count = arraySortedMembers.Count();
		bool inserted = false;

		if (count < 1) {
			arraySortedMembers.Insert(member);
		} else {
			for (int i = 0; i < count; i++) {
				ref ClanMember sortedMember = arraySortedMembers[i];

				if (sortedMember.GetPlayerRank() > member.GetPlayerRank()) {
					arraySortedMembers.InsertAt(member, i);
					inserted = true;
					break;
				}
			}
			if (!inserted) {
				arraySortedMembers.Insert(member);
			}
		}
	}

	void BuildLeaderboard(string previousSelectionId) {
		ClanGenericGrid newGrid;
		ClanMemberGrid memberGrid;
		int color;
		int i;

		clan = GetClanClientManager().GetClan();

		foreach (ClanMember member : arraySortedMembers) {
			if (i >= 99 || !newGrid) {
				i = 0;
				newGrid = CreateNewGrid();
			}
			if (i % 2 == 1) {
				color = ClanColors.DARK_GRAY;
			} else {
				color = ClanColors.WHITE;
			}
			memberGrid = new ClanMemberGrid(newGrid.GetGrid(), member, color);
			newGrid.AddChild(memberGrid);

			if (member.GetPlayerId() == previousSelectionId) {
				selectedGrid = memberGrid;
				selectedGrid.ShowBackground();
			}
			i++;
		}
	}

	ClanGenericGrid CreateNewGrid() {
		ClanGenericGrid newGrid = new ClanGenericGrid(rootGrid, "GridSpacer");
		arrayAllGrids.Insert(newGrid);

		return newGrid;
	}

	override bool OnKeyPress(Widget w, int x, int y, int key) {
		if (w == edtDialogBox) {
			if (inputDialogState == STATE_CONTRIBUTE) {
				string boxText = edtDialogBox.GetText();

				if (boxText.Length() >= 9 || key < KEYCODE_MIN_NUM || key > KEYCODE_MAX_NUM) {
					edtDialogBox.SetText(boxText);
					return true;
				}
			}
		}
		return false;
	}

	override bool OnClick(Widget w, int x, int y, int button) {
		super.OnClick(w, x, y, button);

		if (button == MouseState.LEFT) {
			string widgetName = w.GetName();
			Param1<string> params;

			switch (w) {
				case btnDialogConfirm:
					{
						HideInputDialog();

						switch (inputDialogState) {
							case STATE_CONTRIBUTE:
								{
									int amountToContribute = edtDialogBox.GetText().ToInt();

									if (amountToContribute > 0) {
										auto contributeParams = new Param1<int>(amountToContribute);
										GetGame().RPCSingleParam(player, ClanRPCEnum.ServerContributeFundsToClan, contributeParams, true);
									}
								}
						}
						return true;
						break;
					}
				case btnConfirmationYes:
					{
						HideConfirmationDialog();

						switch (confirmationDialogState) {
							case STATE_KICK:
								{
									if (selectedGrid) {
										if (!IsRateLimited()) {
											SetRateLimited();

											Print(ClanStatic.debugPrefix + "Kicking member...");
											params = new Param1<string>(selectedGrid.GetMember().GetPlayerId());
											GetGame().RPCSingleParam(player, ClanRPCEnum.ServerKickFromClan, params, true);
										}
									} else {
										ShowStatusDialog(STATE_STATUS_NO_SELECTION_KICK);
									}
									break;
								}
							case STATE_LEAVE:
								{
									GetGame().RPCSingleParam(player, ClanRPCEnum.ServerLeaveClan, null, true);
									GetClanClientManager().DeleteClan();
									GetGame().GetUIManager().HideScriptedMenu(this);
									break;
								}
							case STATE_DELETE_FIRST:
								{
									ShowConfirmationDialog(STATE_DELETE_SECOND);
									break;
								}
							case STATE_DELETE_SECOND:
								{
									GetGame().RPCSingleParam(player, ClanRPCEnum.ServerDeleteClan, null, true);
									GetClanClientManager().DeleteClan();
									GetGame().GetUIManager().HideScriptedMenu(this);
									break;
								}
							case STATE_UPGRADE:
								{
									if (!IsRateLimited()) {
										SetRateLimited();
										
										GetGame().RPCSingleParam(player, ClanRPCEnum.ServerUpgradeClan, null, true);
									}
								}
						}
						break;
					}
				case btnConfirmationNo:
					{
						HideConfirmationDialog();
						break;
					}
				case btnContribute:
					{
						ShowInputDialog(STATE_CONTRIBUTE);
						break;
					}
				case btnLeave:
					{
						ShowConfirmationDialog(STATE_LEAVE);
						break;
					}
				case btnPromote:
					{
						if (selectedGrid) {
							if (!IsRateLimited()) {
								Print(ClanStatic.debugPrefix + "promoting user...");
								params = new Param1<string>(selectedGrid.GetMember().GetPlayerId());
								GetGame().RPCSingleParam(player, ClanRPCEnum.ServerPromoteClanMember, params, true);
								SetRateLimited();
							}
						} else {
							ShowStatusDialog(STATE_STATUS_NO_SELECTION_PROMOTE);
						}
						break;
					}
				case btnDemote:
					{
						if (selectedGrid) {
							if (!IsRateLimited()) {
								Print(ClanStatic.debugPrefix + "demoting user...");
								params = new Param1<string>(selectedGrid.GetMember().GetPlayerId());
								GetGame().RPCSingleParam(player, ClanRPCEnum.ServerDemoteClanMember, params, true);
								SetRateLimited();
							}
						} else {
							ShowStatusDialog(STATE_STATUS_NO_SELECTION_DEMOTE);
						}
						break;
					}
				case btnKick:
					{
						if (selectedGrid) {
							ShowConfirmationDialog(STATE_KICK);
						} else {
							ShowStatusDialog(STATE_STATUS_NO_SELECTION_KICK);
						}
						break;
					}
				case btnUpgrade:
					{
						if (clan.CanUpgradeClan(clanUpgradeCost)) {
							ShowConfirmationDialog(STATE_UPGRADE);
						} else {
							ShowStatusDialog(STATE_STATUS_UPGRADE_FAIL);
						}
						break;
					}
				case btnDisband:
					{
						ShowConfirmationDialog(STATE_DELETE_FIRST);
						break;
					}
				case btnClose:
					{
						GetGame().GetUIManager().HideScriptedMenu(this);
						break;
					}
				case btnSettings:
					{
						ShowSettingsDialog();
						break;
					}
				case btnStatusOK:
					{
						HideStatusDialog();
						break;
					}
			}
		}
		return false;
	}

	override void ShowInputDialog(string type) {
		super.ShowInputDialog(type);

		switch (type) {
			case STATE_CONTRIBUTE:
				{
					inputDialogState = type;
					txtDialogHeader.SetText("ENTER AMOUNT TO CONTRIBUTE");
					btnDialogConfirm.SetText("ENTER");
					inputRoot.Show(true);
				}
		}
	}

	override void ShowConfirmationDialog(string state) {
		super.ShowConfirmationDialog(state);

		string questionText;
		confirmationDialogState = state;
		txtConfirmationHeader.SetText("ARE YOU SURE YOU WANT TO");

		switch (state) {
			case STATE_KICK:
				{
					questionText = "KICK '" + selectedGrid.GetMember().GetPlayerName() + "'?";
					break;
				}
			case STATE_LEAVE:
				{
					questionText = "LEAVE YOUR CLAN?";
					break;
				}
			case STATE_DELETE_FIRST:
				{
					questionText = "DELETE YOUR CLAN?";
					break;
				}
			case STATE_DELETE_SECOND:
				{
					questionText = "DELETE YOUR CLAN? IT CANNOT BE RECOVERED!";
					break;
				}
			case STATE_UPGRADE:
				{
					txtConfirmationHeader.SetText("ARE YOU SURE?");
					questionText = "UPGRADE COST: " + clanUpgradeCost;
					break;
				}
		}
		questionText.ToUpper();
		txtConfirmationQuestion.SetText(questionText);
		FixDialogSize(confirmationDialog, txtConfirmationHeader, txtConfirmationQuestion);
	}

	override void HandleError(int error) {
		switch(error) {
			case ClanCreateErrorEnum.NO_FUNDS:
				{
					ShowStatusDialog(STATE_NO_FUNDS);
					break;
				}
		}
	}

	override void ShowStatusDialog(string state) {
		super.ShowStatusDialog(state);

		string line1, line2;

		switch (state) {
			case STATE_STATUS_NO_SELECTION_KICK:
				{
					line1 = "CANNOT KICK!";
					line2 = "NO MEMBER WAS SELECTED!";
					break;
				}
			case STATE_STATUS_NO_SELECTION_DEMOTE:
				{
					line1 = "CANNOT DEMOTE!";
					line2 = "NO MEMBER WAS SELECTED!";
					break;
				}
			case STATE_STATUS_NO_SELECTION_PROMOTE:
				{
					line1 = "CANNOT PROMOTE!";
					line2 = "NO MEMBER WAS SELECTED!";
					break;
				}
			case STATE_STATUS_UPGRADE_FAIL:
				{
					line1 = "CANNOT UPGRADE CLAN!";
					line2 = "UPGRADE COST: " + clanUpgradeCost;
					break;
				}
			case STATE_NO_FUNDS:
				{
					line1 = "CANNOT CONTRIBUTE!";
					line2 = "NOT ENOUGH FUNDS!";
					break;
				}
		}
		txtStatusDialogLine1.SetText(line1);
		txtStatusDialogLine2.SetText(line2);
		FixDialogSize(menuStatusDialog, txtStatusDialogLine1, txtStatusDialogLine2);
	}
}