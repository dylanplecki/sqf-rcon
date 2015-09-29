

class sqfrcon
{
	/* General Information */
	name = "SQF RCON";
	dir = "@sqfrcon";
	version = "1.0.1.4";
	author[] = {"Naught"};
	authorUrl = "https://github.com/dylanplecki/sqf-rcon"; // URL
	
	/* Requirements */
	requiredAddons[] = {};
	requiredVersion = 1;
	
	/* Includes */
	units[] = {};
	weapons[] = {};
	
	/* Display */
	hideName = "false";
	hidePicture = "true";
	//picture = "";
	
	/* Extra */
	actionName = "Website";
	action = "http://bitbucket.org/dylanplecki/sqf-rcon"; // URL
	description = "";
};

class CfgPatches {
	class sqfrcon_main : sqfrcon {};
};

class CfgMods {
	class sqfrcon_mod : sqfrcon {};
};

class CfgFunctions
{
	class sqfrcon
	{
		class main
		{
			class startServer
			{
				file = "\x\naught\addons\sqfrcon\fnc_startServer.sqf";
				preInit = 1; // Arma 3 only
			};
		};
	};
};

class Extended_PreInit_EventHandlers {
	sqfrcon = "call compile preprocessFileLineNumbers '\x\naught\addons\sqfrcon\fnc_startServer.sqf'"; // Arma 2 or 3 (requires CBA)
};
