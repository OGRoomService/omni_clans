class CfgPatches
{
	class omni_clans
	{
		requiredAddons[] = {
            "DZ_Data", 
            "VanillaPlusPlus"
        };
	};
};

class CfgMods
{
	class omni_clans
	{
		dir="omni_clans";
		hideName=0;
		hidePicture=0;
		name="Omnicidal Clans";
		credits="RoomService";
		author="RoomService";
		authorID="999999";
		inputs="omni_clans\modded_Inputs.xml";
		version="version 1.0";
		extra=0;
		type="mod";
		dependencies[] = {"Game","World","Mission"};

	    class defs
	    {
            class gameScriptModule
            {
                files[] = {"omni_clans\scripts\3_Game"};
            };
            class worldScriptModule
            {
                files[] = {"omni_clans\scripts\4_World"};
            };
            class missionScriptModule
            {
                files[] = {"omni_clans\Scripts\5_Mission"};
            };
        };
    };
};