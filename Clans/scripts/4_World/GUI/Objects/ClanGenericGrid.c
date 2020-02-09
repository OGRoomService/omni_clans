class ClanGenericGrid {
    private ref Widget wParent;
    private ref GridSpacerWidget grid;
    private ref array<ref ClanMemberGrid> clanMemberChildren;

    void ClanGenericGrid(Widget parent, string path) {
        clanMemberChildren = new array<ref ClanMemberGrid>();

        grid = GridSpacerWidget.Cast(GetGame().GetWorkspace().CreateWidgets("Clans\\layouts\\" + path + ".layout", parent));

        Print("Creating new generic grid");
    }

    void ~ClanGenericGrid() {
        Print("Deleting generic grid");
        grid.Unlink();
    }

    void SetColor(int color) {
        grid.SetColor(color);
    }

    void AddChild(ref ClanMemberGrid child) {
        grid.AddChild(child.GetRoot());
        clanMemberChildren.Insert(child);
    }

    int GetChildrenCount() {
        return clanMemberChildren.Count();
    }

    ref array<ref ClanMemberGrid> GetMemberChildren() {
        return clanMemberChildren;
    }

    Widget GetParent() {
        return wParent;
    }

    GridSpacerWidget GetGrid() {
        return grid;
    }
}