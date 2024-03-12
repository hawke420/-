# pragma once
#include <iostream>
#include <stdio.h>
#include <queue>
#include <mutex>
#include <atomic>
#include <condition_variable>
#include <iostream>
#include <string>
#include <ctime>
#include <mysql/mysql.h>
#include <thread>
#include <functional>
#include <chrono>
#include <exception>

using std::shared_ptr;
using std::queue;
using std::mutex;
using std::condition_variable;
using std::atomic_int;
using std::cout;
using std::string;
using std::endl;
using std::bind;
using std::thread;
using std::unique_lock;

#define LOG(str) cout<<__FILE__<<":"<<__LINE__<<" "<<str<<endl;