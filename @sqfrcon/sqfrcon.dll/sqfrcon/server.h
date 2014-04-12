#pragma once

#define SOCKET_TIMEOUT 250 // Milliseconds
#define RESPONSIVE_TIMEOUT 1000 // Milliseconds

#define MAX_USER_LEN 32 // Bytes
#define MAX_PASS_LEN 32 // Bytes
#define MAX_CONTENT_LEN (2^18) // Bytes

namespace sqfrcon
{
	class server
	{
	private:
		std::mutex mutex;
		std::condition_variable wd_wait;
		std::thread* wd_thread;
		bool running;
		bool responsive;
		struct mg_server *mongoose;
		concurrency::concurrent_queue<sqfrcon::package*> output_queue;
		concurrency::concurrent_unordered_map<size_t,sqfrcon::package*> input_storage;
		concurrency::concurrent_unordered_map<std::string, std::string> user_database;
		static int event_handler(struct mg_connection *conn, enum mg_event ev);
		void watchdog();
		void queue(sqfrcon::package* pkg);
	public:
		server(std::string port, std::string ssl_cert = "");
		~server();
		std::string request(std::string& input, size_t size);
		std::string store(std::string& input);
		void addUser(std::string user, std::string password);
		void responsiveCheck();
		bool stop();
	};
}
