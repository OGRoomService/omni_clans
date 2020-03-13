modded class MissionGameplay : MissionBase {
    void MissionGameplay() {
        GetClanClientManager().Init();
    }

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

    override void OnUpdate(float timeslice) {
        super.OnUpdate(timeslice);

        Input input;
        ClanMenu clanMenu;

        input = GetGame().GetInput();

        if (input.LocalPress("UAUIBack", false)) {
            clanMenu = ClanMenu.Cast(GetGame().GetUIManager().GetMenu());

            if (clanMenu) {
                clanMenu.HandleClose();
            }
        }
        if (input.LocalPress("OCToggleClanMenu", false)) {
            if (!g_Game.IsKeyboardBusy()) {
                clanMenu = ClanMenu.Cast(GetGame().GetUIManager().GetMenu());

                if (clanMenu && !clanMenu.IsInputDialogVisible()) {
                    clanMenu.HandleClose();
                } else {
                    GetUIManager().EnterScriptedMenu(ClanMenuEnum.Menu, null);
                }
            }
        }
    }
}