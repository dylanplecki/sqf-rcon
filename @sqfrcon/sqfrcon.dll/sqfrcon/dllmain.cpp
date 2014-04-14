// dllmain.cpp : Defines the entry point for the DLL application.
#include "stdafx.h"
#include "system.h"
#include "server.h"

BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
					 )
{
	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:
	case DLL_THREAD_ATTACH:
	case DLL_THREAD_DETACH:
	case DLL_PROCESS_DETACH:
		break;
	}
	return TRUE;
}

extern "C"
{
	__declspec(dllexport) void __stdcall RVExtension(char *output, int outputSize, const char *function);
};

void __stdcall RVExtension(char* container, int outputSize, const char* function)
{
	if (!sqfrcon::ext_init)
		sqfrcon::init();
	outputSize -= 1;
	std::string input = function;
	std::string output = "0";
	size_t io = atoi(input.substr(0,1).c_str());
	input = input.substr(1);
	if (sqfrcon::main_server != nullptr)
	{
		sqfrcon::main_server->responsiveCheck();
	}
	switch (io)
	{
	case 0: // Get
		output = sqfrcon::main_server->request(input, outputSize);
		break;
	case 1: // Post
		output = sqfrcon::main_server->store(input);
		break;
	case 2: // Info
		output = VER_FILE_VERSION_STR;
		break;
	case 3: // Start Server
		sqfrcon::loadConfig(sqfrcon::main_server, sqfrcon::getDllPath() + CONFIG_FILE);
		sqfrcon::main_server->start();
		output = "1";
		break;
	case 4: // Stop Server
		sqfrcon::main_server->stop();
		output = "1";
		break;
	case 5: // Test
		output = input;
		break;
	}
	strncpy(container, output.c_str(), outputSize);
}
