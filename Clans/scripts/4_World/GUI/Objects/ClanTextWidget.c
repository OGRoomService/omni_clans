class ClanTextWidget {
    private ref Widget wRoot, background;
    private ref TextWidget txtWidget;
    private ref ClanUser clanUser;

    void ClanTextWidget(Widget parent, ref ClanUser user, string text) {
        wRoot = GetGame().GetWorkspace().CreateWidgets("omni_clans\\gui\\layouts\\ListText.layout", parent);
		txtWidget = TextWidget.Cast(wRoot.FindAnyWidget("txtName"));
		background = wRoot.FindAnyWidget("pnlBackground");
        clanUser = user;

        SetColor(ClanColors.GRAY);
        SetText(text);
    }

    void ~ClanTextWidget() {
        txtWidget.Unlink();
    }

    ref ClanUser GetClanUser() {
        return clanUser;
    }

    TextWidget GetWidget() {
        return txtWidget;
    }

    string GetPlayerId() {
        return clanUser.GetId();
    }

    bool BGIsVisible() {
        return background.IsVisible();
    }

    void SetText(string text) {
        txtWidget.SetText(text);
    }

    void SetColor(int color) {
        txtWidget.SetColor(color);
    }

    void HideBackground() {
        background.Show(false);
    }

    void ShowBackground() {
        background.Show(true);
    }
}