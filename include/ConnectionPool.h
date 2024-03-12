#pragma once
#include "wang.h"
#include "Connection.h"

class ConnectionPool{
public:
  static ConnectionPool* ConnPoolInit();
  // 对外的接口
  shared_ptr<Connection> getConnection();

private:
  ConnectionPool(); // 单例模式--懒汉版
  
  // 从配置文件中加载配置项（已测试）
  bool loadConfigFile();
  // 运行在独立的线程中，专门负责生产新连接
  void produceConnectionTask();
  // 扫描超过patient时间的空闲连接，进行对于的连接回收
  void scannerConnectionTask();
  
  // 队列中构建连接的时候需要传递的参数
  string _ip;
  unsigned short _port;
  string _username;
  string _password;
  string _dbname;
  
  unsigned int _initSize; // 队列初始大小
  unsigned int _maxSize; // 队列最大长度
  unsigned int _onBusy;// 新连接的最大阻塞时间
  unsigned int _patient; // 小小连接池不养闲人，不繁忙时initSize个连接就够了，繁忙时最多maxSize个连接
  queue<Connection *> _connectionQueue;
  
  // 线程安全控制
  mutex _queueMutex;
  atomic_int _currentCnt; // 当前连接计数（队列中连接数目）
  // 生产者-消费者模型
  condition_variable _cv;
};

// ConnectionPool *unique_connection_pool = ConnectionPool::ConnPoolInit();