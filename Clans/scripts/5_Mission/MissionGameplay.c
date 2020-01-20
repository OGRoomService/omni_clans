modded class MissionGameplay : MissionBase {
    override UIScriptedMenu CreateScriptedMenu(int id) {
        UIScriptedMenu menu = null;
        menu = super.CreateScriptedMenu(id);

        if (!menu) {
            switch (id) {
                case ClanMenuEnum.ManageMenu:
                    {
                        menu = new ClanManageMenu();
                        break;
                    }
                case ClanMenuEnum.Menu:
                    {
                        menu = new ClanCreateMenu();
                        break;
                    }
            }
            if (menu) {
                menu.SetID(id);
            }
        }
        return menu;
    }

    override void OnKeyPress(int key) {
        switch (key) {
            case KeyCode.KC_NUMPADENTER:
                GetUIManager().EnterScriptedMenu(ClanMenuEnum.ManageMenu, null);
                break;
            case KeyCode.KC_SEMICOLON:
                GetUIManager().EnterScriptedMenu(ClanMenuEnum.Menu, null);
                break;
            case KeyCode.KC_ESCAPE:
                ClanMenu menu = ClanMenu.Cast(GetGame().GetUIManager().GetMenu());

                if (menu) {
                    menu.HandleClose();
                    return;
                }
                break;
        }
        super.OnKeyPress(key);
        m_Hud.KeyPress(key);
    }
}