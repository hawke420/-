#include "../include/Connection.h"
#include "../include/wang.h"

Connection::Connection(){
    _conn = mysql_init(nullptr);
    if(_conn == nullptr){
        LOG("mysql_init error");
    }
}

Connection::~Connection(){
    if(_conn != nullptr){ 
        mysql_close(_conn);
    }
}

bool Connection::connect(string ip, unsigned short port, string user, string password, string dbname){
    MYSQL* ret = mysql_real_connect(_conn, ip.c_str(), user.c_str(), password.c_str(), dbname.c_str(), port, nullptr, 0);
    if(ret == nullptr){
        LOG("connect error");
        return false;
    }
    return true;
}

bool Connection::update(string sql){
    int ret = mysql_query(_conn,sql.c_str());
    if(ret){
        LOG(sql+"[update error]");
        return false;
    }
    return true;
}

MYSQL_RES* Connection::query(string sql){
    int ret = mysql_query(_conn,sql.c_str());
    if(ret){
        LOG(sql+"[query error]");
        return nullptr;
    }
    return mysql_use_result(_conn);
}