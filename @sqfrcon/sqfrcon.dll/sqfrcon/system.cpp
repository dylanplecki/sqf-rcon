#include "stdafx.h"
#include <libconfig.h++>
#include "system.h"

namespace sqfrcon
{
	bool ext_init = false;
	server* main_server = nullptr;

	std::string getDllPath()
	{
		char path[MAX_PATH];
		HMODULE hm = NULL;

		if (!GetModuleHandleExA(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS |
			GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT,
			(LPCSTR)&getDllPath,
			&hm))
		{
			int ret = GetLastError();
			BOOST_LOG_TRIVIAL(error) << "GetModuleHandle returned: " + std::to_string(ret);
		}
		GetModuleFileNameA(hm, path, sizeof(path));
		std::string ret_path = path;
		return ret_path.substr(0, ret_path.find_last_of("\\") + 1);
	}

	void init()
	{
		ext_init = true;
		boost::log::register_simple_formatter_factory<boost::log::trivial::severity_level, char>("Severity");
		boost::log::add_file_log
			(
			boost::log::keywords::file_name = (getDllPath() + LOG_FILE),
			boost::log::keywords::format = "[%TimeStamp%] <%Severity%> : %Message%",
			boost::log::keywords::auto_flush = true
			);
		boost::log::core::get()->set_filter
			(
			boost::log::trivial::severity >= boost::log::trivial::info
			);
		boost::log::add_common_attributes();
		BOOST_LOG_TRIVIAL(info) << "Working Directory: " + getDllPath();
		main_server = new server();
	}

	void loadConfig(server* serv, std::string path)
	{
		BOOST_LOG_TRIVIAL(info) << "Config: Loading file " + path;
		libconfig::Config cfg;
		// Read the config file
		try
		{
			cfg.readFile(path.c_str());
		}
		catch (const libconfig::FileIOException &fioex)
		{
			BOOST_LOG_TRIVIAL(error) << "Config: I/O error while reading file.";
		}
		catch (const libconfig::ParseException &pex)
		{
			BOOST_LOG_TRIVIAL(error) << "Parse error at " << pex.getFile() << ":" << pex.getLine() << " - " << pex.getError();
		}
		// Get port number
		try
		{
			serv->setPort(cfg.lookup("listening_port"));
		}
		catch (const libconfig::SettingNotFoundException &nfex)
		{
			serv->setPort("3302");
			BOOST_LOG_TRIVIAL(error) << "Config: Value 'listening_port' not found.";
		}
		// Get SSL certificate path
		try
		{
			serv->setSSLPath(cfg.lookup("ssl_certificate"));
		}
		catch (const libconfig::SettingNotFoundException &nfex)
		{
			serv->setSSLPath("");
			BOOST_LOG_TRIVIAL(error) << "Config: Value 'ssl_certificate' not found.";
		}
		// Get user list
		try
		{
			const libconfig::Setting &auth_list = cfg.getRoot()["authentication_list"];
			for (size_t i = 0; i < auth_list.getLength(); ++i)
			{
				const libconfig::Setting &user_auth = auth_list[i];
				std::string username, password;
				if (user_auth.lookupValue("username", username) && user_auth.lookupValue("password", password))
					serv->addUser(username, password);
				else
					BOOST_LOG_TRIVIAL(error) << "Config: Username or password not defined for a user.";
			}
		}
		catch (const libconfig::SettingNotFoundException &nfex)
		{
			BOOST_LOG_TRIVIAL(error) << "Config: Value 'authentication_list' not found.";
		}
	}
}
