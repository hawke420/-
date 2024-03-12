#include "../include/ConnectionPool.h"
#include "../include/wang.h"

ConnectionPool* ConnectionPool::ConnPoolInit(){
    static ConnectionPool pool; // 延迟到需要时再实例化，C++11 保证了局部静态变量的初始化是线程安全的
	return &pool;
}

bool ConnectionPool::loadConfigFile(){
    FILE* config = fopen("../../conf/mysql.ini", "r");
    if(config == nullptr){
        LOG("config file open error");
    }
    else{
        // 解析配置文件，逐行读取
        char line[100] = {0};
        while(fgets(line, 100, config) != nullptr){
            string str = line;
            int idx = str.find("=");
            if(idx == -1){
                LOG("config format error");
                continue;
            }
            int end = str.find("\n");
            string key = str.substr(0, idx);
            string value = str.substr(idx+1,end-idx-1);
        
            if(key == "ip"){
                _ip = value;
            }
            else if(key == "port"){
                _port = stoi(value);
                // _port = atoi(value.c_str());
            }
            else if(key == "username"){
                _username = value;
            }
            else if(key == "password"){
                _password = value;
            }
            else if(key == "dbname"){
                _dbname = value;
            }
            else if(key == "initSize"){
                _initSize = stoi(value);
            }
            else if(key == "maxSize"){
                _maxSize = stoi(value);
            }
            else if(key == "patient"){
                _patient = stoi(value);
            }
            else if(key == "onBusy"){
                _onBusy = stoi(value);
            }
            LOG("parse success! "+key+":"+value);
        }
    }

    return true;
}

ConnectionPool::ConnectionPool(){
    if(!loadConfigFile()){
        return;
    }

    // 构建初始数量的连接
    for(auto i=0;i<_initSize;++i){
        Connection *pConn = new Connection();
        pConn->connect(_ip.c_str(), _port, _username.c_str(), _password.c_str(), _dbname.c_str());
        pConn->refreshAliveTime();
        _connectionQueue.push(pConn);
        _currentCnt++;
    }

    // 启动一个线程来负责生产(从这里开始才需要考虑线程安全)
    thread produceTd = thread(bind(&ConnectionPool::produceConnectionTask, this));
    produceTd.detach();

    // 启动一个线程来负责扫描超过patient时间的空闲连接，进行连接回收
    thread scannerTd = thread(bind(&ConnectionPool::scannerConnectionTask, this));
    scannerTd.detach();
}

// 理解这里和一般生产者逻辑不同的地方：一般的生产者是队列不满就可以生产，这里我们设计成只有队列为空才生产并一直生产到队列满
void ConnectionPool::produceConnectionTask(){
    while(1){
        unique_lock<mutex> lock(_queueMutex);
        while(!_connectionQueue.empty()){
            _cv.wait(lock); // cv.wait()会释放锁并阻塞当前线程，直到消费者那边（其他线程）cv.notify()被调用锁才会被唤醒并重新获取锁
        }
        if(_currentCnt < _maxSize){ // 一直生产到队列满
            Connection *pConn = new Connection();
            pConn->connect(_ip.c_str(), _port, _username.c_str(), _password.c_str(), _dbname.c_str());
            pConn->refreshAliveTime();
            _connectionQueue.push(pConn);
            _currentCnt++;
        }
        _cv.notify_all(); // 通知消费者线程
    }    
}

// 作为消费者线程，提供给外部（mysql的客户端，但也是本程序的服务端），这个线程入口函数是来自服务器（同时也是mysql的客户）创建的线程要调用的
shared_ptr<Connection> ConnectionPool::getConnection(){
    unique_lock<mutex> lock(_queueMutex); // 为了实现同步原语
    // 队列为空就要一直阻塞等待生产者线程生产。但是如果阻塞时间超过了onBusy时间，就要告知这个获取连接失败（连接池繁忙）
    while(_connectionQueue.empty()){
        if(std::cv_status::timeout == _cv.wait_for(lock, std::chrono::milliseconds(_onBusy))){
            if(_connectionQueue.empty()){
                LOG("connection pool is busy! connect to mysql failed!")
                return nullptr;
            }
        }
    }

    // 从队列中取出一个连接
    shared_ptr<Connection> ret(_connectionQueue.front(),[&](Connection* pConn){
        // 这个队列对服务器的应用线程均可见，因此要考虑队列的线程安全
        unique_lock<mutex> lock(_queueMutex);
        pConn->refreshAliveTime();
        _connectionQueue.push(pConn);
    });
    /* 这里智能指针的析构函数，也就是会delete里面的指针。但是不希望智能指针的生命周期决定资源的生命周期
    （执行里面资源类的析构函数，但Connection类析构的话就close了，这不是想要的。希望的是回归连接池空闲）
     所以要自定义智能指针的删除器（智能指针的第二个参数，匿名函数的使用）*/
    
    _connectionQueue.pop();
    _cv.notify_all(); // 通知生产者线程
    return ret;
}

void ConnectionPool::scannerConnectionTask(){
    // patient决定连接资源是否真正释放（结束生命周期），也就是析构函数中close掉
    // 当当前连接数超过初始大小时扫描超过patient时间的空闲连接，进行连接回收
    while(1){
        unique_lock<mutex> lock(_queueMutex);
        while(_initSize < _currentCnt){
            Connection *old = _connectionQueue.front();
            if(old->getAliveTime() > _patient){
                _connectionQueue.pop();
                _currentCnt--;
                delete old;
            }
            else{
                break;
            }
        }
    }
}