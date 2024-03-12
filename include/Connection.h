// 实现mysql数据库的操作
#pragma once
#include "wang.h"

class Connection{
public:
    Connection();
    ~Connection();

    // 连接数据库
    bool connect(string ip, unsigned short port, string user, string password, string dbname);
    
    // 更新
    bool update(string sql);
    // 查询
    MYSQL_RES *query(string sql);

    // 刷新上一次连接有操作的时间
    void refreshAliveTime(){ _alivetime = clock(); };
    // 返回已经空闲了多久（预计超过一定时间限制就断开连接，这里的表现是从连接池中移除，连接池中保持_initSize个连接就好）
    clock_t getAliveTime() const { return (clock() - _alivetime);};

private:
    MYSQL* _conn;
    clock_t _alivetime; // 上一次连接有操作的时间(进入空闲状态)
};