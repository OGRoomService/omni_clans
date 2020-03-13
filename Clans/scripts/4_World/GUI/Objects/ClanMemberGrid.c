class ClanMemberGrid {
    private ref Widget parent, root, background;
    private ref GridSpacerWidget grid;
    private ref TextWidget txtName, txtRank, txtContributions;
    private ref ClanMember member;
    private int unselectedBGColor;

    void ClanMemberGrid(Widget parent, ref ClanMember member, int unselectedBGColor) {
        int memberRank = member.GetPlayerRank();
        string memberRankName;

        root = GetGame().GetWorkspace().CreateWidgets("omni_clans\\gui\\layouts\\ManageMenuLeaderWidget.layout", parent);
        grid = GridSpacerWidget.Cast(root.FindAnyWidget("grid"));
        background = root.FindAnyWidget("pnlBackground");
        txtName = TextWidget.Cast(root.FindAnyWidget("txtName"));
        txtRank = TextWidget.Cast(root.FindAnyWidget("txtRank"));
        txtContributions = TextWidget.Cast(root.FindAnyWidget("txtContributions"));

        if (memberRank == 0) {
            memberRankName = GetClanManager().GetConfig().GetOwnerRankName();
        } else {
            memberRankName = GetClanManager().GetConfig().GetRankName(member.GetPlayerRank());
        }
        if (unselectedBGColor != ClanColors.WHITE) {
            background.Show(true);
            background.SetColor(unselectedBGColor);
        }
        txtName.SetText("" + member.GetPlayerName());
        txtRank.SetText("" + memberRankName);
        txtContributions.SetText("" + member.GetContribution());
        this.parent = parent;
        this.member = member;
        this.unselectedBGColor = unselectedBGColor;
    }

    void ~ClanMemberGrid() {
        Print(ClanStatic.debugPrefix + "Deleting member grid");
        root.Unlink();
    }

    ref ClanMember GetMember() {
        return member;
    }

    Widget GetRoot() {
        return root;
    }

    void ShowBackground() {
        background.Show(true);
        background.SetColor(ClanColors.DARK_RED);
    }

    void HideBackground() {
        if (unselectedBGColor == ClanColors.WHITE) {
            background.Show(false);
        } else {
            background.SetColor(unselectedBGColor);
        }
    }
}