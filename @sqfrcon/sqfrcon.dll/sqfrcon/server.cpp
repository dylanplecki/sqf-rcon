#include "stdafx.h"
#include "mongoose.h"
#include "server.h"

namespace sqfrcon
{
	int server::event_handler(struct mg_connection *conn, enum mg_event ev)
	{
		sqfrcon::server* server = static_cast<sqfrcon::server*>(conn->server_param);
		if (ev == MG_AUTH)
			return MG_TRUE;
		else if (ev == MG_REQUEST)
		{
			// Set MIME type
			mg_send_header(conn, "Content-Type", "text/plain");
			// Allow cross-domain and cross-port javascript access
			mg_send_header(conn, "Access-Control-Allow-Origin", "*");
			mg_send_header(conn, "Access-Control-Allow-Methods", "GET, POST, OPTIONS");
			const char* ac_requestHeaders = mg_get_header(conn, "Access-Control-Request-Headers");
			if (ac_requestHeaders != NULL)
				mg_send_header(conn, "Access-Control-Allow-Headers", ac_requestHeaders);
			// Check request
			if (server->responsive)
			{
				char user[MAX_USER_LEN];
				char pass[MAX_USER_LEN];
				if ((mg_get_var(conn, "user", user, MAX_USER_LEN) > 0) && (mg_get_var(conn, "pass", pass, MAX_PASS_LEN) > 0))
				{
					std::string digest;
					CryptoPP::SHA256 hash;
					CryptoPP::StringSource passb64(pass, true,
						new CryptoPP::HashFilter(hash,
						new CryptoPP::Base64Encoder(
						new CryptoPP::StringSink(digest), false)));
					if (server->user_database.at(user) == digest)
					{
						char content[MAX_CONTENT_LEN];
						if (mg_get_var(conn, "script", content, MAX_CONTENT_LEN) > 0)
						{
							sqfrcon::package* pkg = new package();
							pkg->fill(content);
							conn->connection_param = pkg;
							server->queue(pkg);
							BOOST_LOG_TRIVIAL(info) << "HTTP 200 OK: SQF data request queued for execution.";
							BOOST_LOG_TRIVIAL(debug) << "Request Data: " + std::string(content) + ".";
							return MG_MORE;
						}
						mg_send_status(conn, 400); // 400 Bad Request
						mg_printf_data(conn, "No Script Data Was Recieved.");
						BOOST_LOG_TRIVIAL(info) << "HTTP 400 Bad Request: No script data was sent.";
						return MG_TRUE;
					}
					mg_send_status(conn, 403); // 403 Forbidden
					mg_printf_data(conn, "Access Denied for '%s'@'%s'.", user, digest);
					BOOST_LOG_TRIVIAL(info) << "HTTP 403 Forbidden: Access denied for user '" + std::string(user) + "'@'" + digest + "'.";
					return MG_TRUE;
				}
				mg_send_status(conn, 403); // 400 Bad Request
				mg_printf_data(conn, "Username or Password Not Given.");
				BOOST_LOG_TRIVIAL(info) << "HTTP 400 Bad Request: Username or password not given.";
				return MG_TRUE;
			}
			mg_send_status(conn, 503); // 503 Service Unavailable
			mg_printf_data(conn, "SQF Service Temporarily Unresponsive.");
			BOOST_LOG_TRIVIAL(info) << "HTTP 503 Service Unavailable: SQF service is temporarily unresponsive.";
			return MG_TRUE;
		}
		else if (ev == MG_POLL)
		{
			if (conn->connection_param != nullptr)
			{
				sqfrcon::package* pkg;
				pkg = static_cast<sqfrcon::package*>(conn->connection_param);
				if (pkg->check())
				{
					// Send back data
					mg_printf_data(conn, pkg->content().c_str());
					server->input_storage.unsafe_erase(pkg->id());
					BOOST_LOG_TRIVIAL(info) << "HTTP Connection Closed: Response formed and sent.";
					BOOST_LOG_TRIVIAL(debug) << "HTTP Content: " + pkg->content() + ".";
					mg_write(conn, "0\r\n\r\n", 5); // Write last chunk
					delete pkg;
					return MG_TRUE;
				}
				BOOST_LOG_TRIVIAL(trace) << "Connection waiting for SQF return call.";
				return MG_MORE;
			}
		}
		return MG_FALSE;
	}

	void server::watchdog()
	{
		BOOST_LOG_TRIVIAL(debug) << "Watchdog thread started.";
		while (running)
		{
			if (responsive)
			{
				std::unique_lock<std::mutex> lk(mutex);
				responsive = false;
				wd_wait.wait_for(lk, std::chrono::milliseconds(RESPONSIVE_TIMEOUT), [this](){ return (responsive || !running); });
			}
			if (running)
				mg_poll_server(mongoose, SOCKET_TIMEOUT);
		}
		BOOST_LOG_TRIVIAL(debug) << "Watchdog thread ended.";
	}

	void server::queue(sqfrcon::package* pkg)
	{
		output_queue.push(pkg);
	}

	server::server() : running(false), responsive(false), port("8080"), ssl_certificate_path("")
	{
		BOOST_LOG_TRIVIAL(debug) << "Web server created.";
	}

	server::~server()
	{
		BOOST_LOG_TRIVIAL(debug) << "Web server deleted.";
	}

	std::string server::request(std::string& input, size_t size)
	{
		if (!running)
		{
			BOOST_LOG_TRIVIAL(error) << "Request failed: Server is not running.";
			return (std::string("3") + "Request failed: Server is not running.");
		}
		if (input.length() > 0) // Request package chunk
		{
			package* pkg = input_storage.at(atoi(input.c_str()));
			char trunc = '1';
			std::string content;
			pkg->take();
			if (pkg->partial(content, size))
			{
				trunc = '0';
			}
			pkg->release();
			BOOST_LOG_TRIVIAL(trace) << "Package chunk requested, found, and sent.";
			return (std::string("1") + trunc + content);
		}
		else // Request new package
		{
			package* pkg;
			if (output_queue.try_pop(pkg))
			{
				char trunc = '0';
				std::string content;
				pkg->take();
				size_t pid = pkg->id();
				if (!pkg->partial(content, size))
				{
					trunc = '1';
				}
				input_storage[pid] = pkg;
				pkg->release();
				BOOST_LOG_TRIVIAL(debug) << "New package returned to SQF. PID: " + std::to_string(pid) + ".";
				return (std::string("1") + trunc + std::to_string(pid) + '@' + content);
			}
			else
			{
				BOOST_LOG_TRIVIAL(trace) << "No new packages to return.";
				return "0";
			}
		}
	}

	std::string server::store(std::string& input)
	{
		if (!running)
		{
			BOOST_LOG_TRIVIAL(error) << "Storage failed: Server is not running.";
			return (std::string("3") + "Storage failed: Server is not running.");
		}
		size_t sep = input.find_first_of('@');
		size_t pid = atoi(input.substr(0, sep).c_str());
		try {
			package* pkg = input_storage.at(pid);
			if (pkg != nullptr)
			{
				pkg->take();
				pkg->fill(input.substr(sep + 1));
				pkg->close();
				pkg->release();
				BOOST_LOG_TRIVIAL(debug) << "SQF return call stored in package. PID: " + std::to_string(pid) + ".";
				return "0";
			} else
				BOOST_LOG_TRIVIAL(error) << "Storage failed: Package found but deleted. PID: " + std::to_string(pid) + ".";
				return (std::string("3") + "Storage failed: Package found but deleted.");
		}
		catch (std::out_of_range ret)
		{
			BOOST_LOG_TRIVIAL(error) << "Storage failed: No relative package found. PID: " + std::to_string(pid) + ".";
			return (std::string("3") + "Storage failed: No relative package found.");
		};
	}

	void server::addUser(std::string user, std::string password)
	{
		user_database[user] = password;
		BOOST_LOG_TRIVIAL(debug) << "Added user '" + user + "'@'" + password + "'.";
	}

	void server::responsiveCheck() {
		std::unique_lock<std::mutex> lk(mutex);
		responsive = true;
		wd_wait.notify_one();
		BOOST_LOG_TRIVIAL(trace) << "SQF responsive check activated.";
	}

	void server::start()
	{
		BOOST_LOG_TRIVIAL(info) << "Web server started on port " + port + ".";
		running = true;
		mongoose = mg_create_server(this, event_handler);
		mg_set_option(mongoose, "ssl_certificate", ssl_certificate_path.c_str());
		mg_set_option(mongoose, "listening_port", port.c_str());
		wd_thread = new std::thread(&sqfrcon::server::watchdog, this);
	}

	bool server::stop()
	{
		if (!running)
			return false;
		running = false;
		mg_destroy_server(&mongoose);
		wd_wait.notify_one();
		wd_thread->join();
		delete wd_thread;
		sqfrcon::package* pkg;
		while (output_queue.try_pop(pkg))
		{
			delete pkg;
		}
		for (concurrency::concurrent_unordered_map<size_t, sqfrcon::package*>::iterator it = input_storage.begin(); it != input_storage.end(); ++it)
		{
			delete it->second;
		}
		input_storage.clear();
		user_database.clear();
		BOOST_LOG_TRIVIAL(info) << "Web server stopped.";
		return true;
	}
}
