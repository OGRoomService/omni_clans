class CfgPatches
{
	class Clans
	{
		requiredAddons[] = {};
	};
};

class CfgMods
{
	class Clans
	{
        type = "mod";
        author = "RoomService";
        dir = "Clans";
        name = "Clans";
		dependencies[] = {"Game","World","Mission"};

	    class defs
	    {
            class gameScriptModule
            {
                files[] = {"Clans\scripts\3_Game"};
            };
            class worldScriptModule
            {
                files[] = {"Clans\scripts\4_World"};
            };
            class missionScriptModule
            {
                files[] = {"Clans\Scripts\5_Mission"};
            };
        };
    };
};