// dllmain.cpp : Defines the entry point for the DLL application.
#include "stdafx.h"
#include "package.h"
#include "server.h"

bool ext_init;
sqfrcon::server* main_server = nullptr;

// Namespace shortcuts
namespace logging = boost::log;
namespace src = boost::log::sources;
namespace sinks = boost::log::sinks;
namespace keywords = boost::log::keywords;

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

void init()
{
	ext_init = true;
	logging::add_file_log
	(
		keywords::file_name = "sqfrcon.log",
		keywords::auto_flush = true
	);
	logging::core::get()->set_filter
	(
		logging::trivial::severity >= logging::trivial::debug
	);
	logging::add_common_attributes();
}

void __stdcall RVExtension(char* container, int outputSize, const char* function)
{
	if (!ext_init)
		init();
	outputSize -= 1;
	std::string input = function;
	std::string output = "0";
	size_t io = atoi(input.substr(0,1).c_str());
	input = input.substr(1);
	BOOST_LOG_TRIVIAL(trace) << "Input from call: " + input;
	if (main_server != nullptr)
	{
		main_server->responsiveCheck();
	}
	switch (io)
	{
	case 0: // Get
		output = main_server->request(input, outputSize);
		break;
	case 1: // Post
		output = main_server->store(input);
		break;
	case 2: // Info
		output = "1.0.0.1";
		break;
	case 3: // Start Server
		if (main_server == nullptr)
		{
			// Debug code
			main_server = new sqfrcon::server("3302");
			main_server->addUser("admin", "A6xnQhbz4Vx2HuGl4lXwZ5U2I8iziLRFnhP5eNfIRvQ=");
			// End debug code
			output = "1";
		}
		break;
	case 4: // Stop Server
		if (main_server != nullptr)
		{
			main_server->stop();
			delete main_server;
			output = "1";
		}
		break;
	case 5: // Test
		output = input;
		break;
	}
	strncpy(container, output.c_str(), outputSize);
}
