modded class PluginManager {
	ref array<string> clanPluginList;

	void PluginManager() {
		clanPluginList = new array<string>();
	}

	override void Init() {
		super.Init();

		clanPluginList.Insert("ClanManager");
		clanPluginList.Insert("ClanServerRPCHandler");
		clanPluginList.Insert("ClanServerManager");
		clanPluginList.Insert("ClanClientRPCHandler");
		clanPluginList.Insert("ClanClientManager");
		//----------------------------------------------------------------------
		// Register modules
		//----------------------------------------------------------------------
		//				Module Class Name 						Client	Server
		//----------------------------------------------------------------------
		RegisterPlugin("ClanManager", true, true);
		RegisterPlugin("ClanServerRPCHandler", false, true);
		RegisterPlugin("ClanServerManager", false, true);
		RegisterPlugin("ClanClientRPCHandler", true, false);
		RegisterPlugin("ClanClientManager", true, false);
	}
	//=================================
	// RegisterPlugin Except this one fucking works
	//=================================
	protected void RegisterPlugin(string plugin_class_name, bool reg_on_client, bool reg_on_server, bool reg_on_release = true) {
		if (clanPluginList.Find(plugin_class_name) != -1) {
			if (!reg_on_client) {
				if (!GetGame().IsServer() || !GetGame().IsMultiplayer()) { return; }
			}
			if (!reg_on_server) {
				if (GetGame().IsServer() && GetGame().IsMultiplayer()) { return; }
			}
			if (!reg_on_release) {
				if (!GetGame().IsDebug()) { return; }
			}
			m_PluginRegister.Insert(plugin_class_name.ToType());
		} else {
			super.RegisterPlugin(plugin_class_name, reg_on_client, reg_on_server, reg_on_release);
		}
	}
}