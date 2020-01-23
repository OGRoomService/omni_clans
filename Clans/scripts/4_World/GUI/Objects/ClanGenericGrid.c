class ClanGenericGrid {
    private ref Widget wParent;
    private ref GridSpacerWidget grid;

    void ClanGenericGrid(Widget parent, string path) {
        grid = GridSpacerWidget.Cast(GetGame().GetWorkspace().CreateWidgets("Clans\\layouts\\" + path + ".layout", parent));
    }

    void ~ClanGenericGrid() {
        if (grid) {
            grid.Unlink();
        }
    }

    void SetColor(int color) {
        grid.SetColor(color);
    }

    Widget GetParent() {
        return wParent;
    }

    GridSpacerWidget GetGrid() {
        return grid;
    }
}