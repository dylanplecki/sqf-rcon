#pragma once

#include "server.h"

namespace sqfrcon
{
	extern bool ext_init;
	extern server* main_server;

	std::string getDllPath();
	void init();
	void loadConfig(server* serv, std::string path);
}
