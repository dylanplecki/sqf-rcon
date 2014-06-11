
if (isNil 'sqfrcon_server_handler') then {
	sqfrcon_server_handler = false;
	[] execFSM '\x\naught\addons\sqfrcon\server_handler.fsm';
	sqfrcon_server_handler = true;
};
