/*
	This file is part of Naught.Net.

	Copyright © 2013 Dylan Plecki <dylanplecki@gmail.com>
	All rights reserved.

	Naught.Net is free software: you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation, either version 3 of the License, or
	(at your option) any later version.

	Naught.Net is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

class sqfrcon
{
	/* General Information */
	name = "SQF RCON";
	dir = "@sqfrcon";
	version = "1.0.0.1";
	author[] = {"Naught"};
	authorUrl = ""; // URL
	
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
	action = ""; // URL
	description = "";
};

class CfgPatches {
	class sqfrcon_main : sqfrcon {};
};

class CfgMods {
	class sqfrcon_mod : sqfrcon {};
};

class Extended_PreInit_EventHandlers {
	sqfrcon = "[] execFSM '\x\naught\addons\sqfrcon\server_handler.fsm'";
};
