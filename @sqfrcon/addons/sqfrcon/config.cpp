
class sqfrcon
{
	/* General Information */
	name = "SQF RCON";
	dir = "@sqfrcon";
	version = "1.0.1.4";
	author[] = {"Naught"};
	authorUrl = "http://bitbucket.org/dylanplecki/sqf-rcon"; // URL
	
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
		class core
		{
			class server_handler
			{
				preInit = 1;
				file = "\x\naught\addons\sqfrcon\server_handler.fsm";
			};
		};
	};
};
