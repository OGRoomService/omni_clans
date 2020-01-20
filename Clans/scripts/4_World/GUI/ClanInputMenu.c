/* class ClanInputMenu : ClanMenu {
    protected ref EditBoxWidget editBox;
    protected ref TextWidget txtBox;

    override Widget Init() {
        wRoot = GetGame().GetWorkspace().CreateWidgets("Clans\\layouts\\ClanCreateMenu.layout", null);
        editBox = EditBoxWidget.Cast(wRoot.FindAnyWidget("edtInput"));
        txtBox = TextWidget.Cast(wRoot.FindAnyWidget("txtEnter"));

        return wRoot;
    }

    override bool OnClick(Widget w, int x, int y, int button) {
        super.OnClick(w, x, y, button);

        if (button == MouseState.LEFT) {
            string widgetName = w.GetName();

            if (widgetName == "btnEnter") {
                string clanName = editBox.GetText();
                auto params = new Param1<string>(clanName);

                GetGame().RPCSingleParam(player, ClanRPCEnum.ServerCreateClan, params, true);

                return true;
            }
        }
        return false;
    }

    override void HandleError(string error) {
        switch (error) {
            default: {
                txtBox.SetText(error);
            }
        }
    }
} */