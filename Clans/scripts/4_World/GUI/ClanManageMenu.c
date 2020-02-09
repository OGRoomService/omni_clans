class ClanManageMenu : ClanMenu {
	private ref ScrollWidget scrollRoot;
	private ref GridSpacerWidget rootGrid;
	private ref ClanMemberGrid selectedGrid;
	private ref array<ref ClanGenericGrid> arrayAllGrids;
	private ref array<ref ClanMember> arraySortedMembers;
	private ref TextWidget txtTitle, txtOnline, txtFunds;
	private ref ActiveClan clan;

    override Widget Init() {
		arrayAllGrids = new array<ref ClanGenericGrid>();
		arraySortedMembers = new array<ref ClanMember>();

        wRoot = GetGame().GetWorkspace().CreateWidgets("Clans\\layouts\\ClanManage.layout");
		rootGrid = GridSpacerWidget.Cast(wRoot.FindAnyWidget("gridMembers"));
		txtTitle = TextWidget.Cast(wRoot.FindAnyWidget("txtTitle"));
		txtOnline = TextWidget.Cast(wRoot.FindAnyWidget("txtOnNum"));
		txtFunds = TextWidget.Cast(wRoot.FindAnyWidget("txtFundNum"));
		clan = GetClanClientManager().GetClan();

		txtTitle.SetText("" + clan.GetName());
		txtOnline.SetText("" + clan.CountOnline());
		txtFunds.SetText("" + clan.GetFunds());
        wRoot.Show(false);

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
							if (memberGrid.GetMember().GetId() == GetClanClientManager().GetPlainId()) {
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

		Refresh();
	}

	void Refresh() {
		ref array<ref ClanMember> clanMembers = new array<ref ClanMember>();
		arraySortedMembers = new array<ref ClanMember>();
		arrayAllGrids = new array<ref ClanGenericGrid>();
		clanMembers = clan.GetMembers();

		foreach (ClanMember member : clanMembers) {
			SortClanMember(member);
		}
		BuildLeaderboard();
	}

	void SortClanMember(ref ClanMember member) {
		int count = arraySortedMembers.Count();
		bool inserted = false;

		if (count < 1) {
			arraySortedMembers.Insert(member);
		} else {
			for (int i = 0; i < count; i++) {
				ref ClanMember sortedMember = arraySortedMembers[i];

				if (sortedMember.GetRank() > member.GetRank()) {
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

	void BuildLeaderboard() {
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
				color = ARGB(255, 125, 125, 125);
			} else {
				color = ARGB(255, 255, 255, 255);
			}
			memberGrid = new ClanMemberGrid(newGrid.GetGrid(), member, color);
			newGrid.AddChild(memberGrid);
			i++;
		}
	}

	ClanGenericGrid CreateNewGrid() {
		ClanGenericGrid newGrid = new ClanGenericGrid(rootGrid, "GridSpacer");
		arrayAllGrids.Insert(newGrid);

		return newGrid;
	}

	override bool OnClick(Widget w, int x, int y, int button) {
		super.OnClick(w, x, y, button);
		
		if (button == MouseState.LEFT) {
			string widgetName = w.GetName();
			Param1<string> params;

			switch (widgetName) {
				case "btnPromote":
					{
						if (selectedGrid) {
							Print("promoting user...");
							params = new Param1<string>(selectedGrid.GetMember().GetId());
							GetGame().RPCSingleParam(player, ClanRPCEnum.ServerPromoteClanMember, params, true);
						}
						break;
					}
				case "btnDemote":
					{
						if (selectedGrid) {
							Print("promoting user...");
							params = new Param1<string>(selectedGrid.GetMember().GetId());
							GetGame().RPCSingleParam(player, ClanRPCEnum.ServerDemoteClanMember, params, true);
						}
						break;
					}
			}
		}
		return false;
	}
}