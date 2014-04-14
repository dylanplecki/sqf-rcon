// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//

#pragma once

#define LOG_FILE "sqfrcon.log"
#define CONFIG_FILE "sqfrcon.cfg"

#define SOCKET_TIMEOUT 250 // Milliseconds
#define RESPONSIVE_TIMEOUT 1000 // Milliseconds

#define MAX_USER_LEN 64 // Bytes
#define MAX_PASS_LEN 64 // Bytes
#define MAX_CONTENT_LEN 262144 // Bytes

#include "version.h"

#define WIN32_LEAN_AND_MEAN		// Exclude rarely-used stuff from Windows headers

#include <windows.h>
#include <concurrent_unordered_map.h>
#include <concurrent_queue.h>

#include <iostream>
#include <fstream>
#include <string>
#include <atomic>
#include <mutex>
#include <condition_variable>
#include <thread>
#include <chrono>

#include <boost/log/core.hpp>
#include <boost/log/trivial.hpp>
#include <boost/log/expressions.hpp>
#include <boost/log/sinks/text_file_backend.hpp>
#include <boost/log/utility/setup/file.hpp>
#include <boost/log/utility/setup/common_attributes.hpp>
#include <boost/log/sources/severity_logger.hpp>

#include "base64.h"
#include <md5.h>
#include <sha.h>

#ifdef NDEBUG
#pragma comment(lib,"cryptlib.lib")
#else
#pragma comment(lib,"cryptlibd.lib")
#endif

#pragma comment(lib,"libconfig.lib")
#pragma comment(lib,"libconfig++.lib")