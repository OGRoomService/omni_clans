class ClanGridSpacer {
    private ref GridSpacerWidget grid;
    private ref array<ref ClanTextWidget> arrayTextWidgets;

    void ClanGridSpacer(Widget parent) {
        arrayTextWidgets = new array<ref ClanTextWidget>();

        grid = GridSpacerWidget.Cast(GetGame().GetWorkspace().CreateWidgets("Clans\\layouts\\GridSpacer.layout", parent));
    }

    void ~ClanGridSpacer() {
        Print("Deleting grid");
        if (grid) {
            grid.Unlink();
        }
    }

    GridSpacerWidget GetGrid() {
        return grid;
    }

    ClanTextWidget AddTextWidget(ref ClanUser user, string text) {
        ClanTextWidget newText = new ClanTextWidget(grid, user, text);
        arrayTextWidgets.Insert(newText);

        return newText;
    }

    ClanTextWidget GetChild(TextWidget w) {
        ClanTextWidget widget;

        if (w) {
            foreach (ClanTextWidget txtWidget : arrayTextWidgets) {
                if (txtWidget.GetWidget() == w) {
                    widget = txtWidget;
                    break;
                }
            }
        }
        return widget;
    }
}