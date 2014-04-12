#pragma once

namespace sqfrcon
{
	class package
	{
	private:
		size_t pid;
		static std::atomic<size_t> pid_counter;
		std::mutex pkg_lock;
		bool finished;
		std::string contents;
		size_t placeholder;
	public:
		package() : placeholder(0), pid(pid_counter), finished(false) { ++pid_counter; };
		void take() { pkg_lock.lock(); };
		void release() { pkg_lock.unlock(); };
		std::string content() { return contents; };
		void empty() { contents.empty(); };
		void fill(std::string newContent) { contents = newContent; };
		size_t id() { return pid; };
		bool partial(std::string& container, size_t amount);
		bool check() { return finished; };
		void close() { finished = true; };
	};
};

