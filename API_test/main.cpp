#include "../include/Connection.h"
#include "../include/wang.h"
#include "../include/ConnectionPool.h"

int main(){
    // 开启连接池，但写这里不能放在lambda表达式中，因为lambda表达式中的变量是复制的
    // ConnectionPool *unique_connection_pool = ConnectionPool::ConnPoolInit();
    
    // 开启多个线程来请求mysql的服务
    thread t1([]{ // t1线程的入口函数，匿名函数写法
        // 不是说线程数和连接数一对一，一个线程可以请求好多个连接。这里先测试一个线程请求一个连接
        shared_ptr<Connection> conn = ConnectionPool::ConnPoolInit()->getConnection();
        try{
            conn->update("insert into users(name, age) values('jiumi','21')");
            cout<<"update success"<<endl;
        }catch(std::exception &e){
            cout<<e.what()<<endl;
        }
    });

    thread t2([]{ // t2线程的入口函数，匿名函数写法
        for(int i = 0; i < 3; ++i){
            shared_ptr<Connection> conn = ConnectionPool::ConnPoolInit()->getConnection();
            try{
                conn->update("insert into users(name, age) values('jiumi','21')");
                cout<<"update success"<<endl;
            }catch(std::exception &e){
                cout<<e.what()<<endl;
            }
        }
    });

    t1.join();
    t2.join();

    Connection conn;
    if(conn.connect("127.0.0.1", 3306, "wang","0812","chat")){
        LOG("connect success");
    }
    conn.update("insert into users(name, age) values('xiaohh','23')");
    MYSQL_RES* ret = conn.query("select * from users");
    if(ret != nullptr){
        int num = mysql_num_fields(ret);
        MYSQL_ROW row;
        while(row = mysql_fetch_row(ret)){
            for(int i = 0; i < num; ++i){
                if (row[i] == NULL) {
                    cout << "NULL | ";
                } else {
                    cout << row[i] << " | ";
                }
            }
            cout<<endl;
        }
    }

    return 0;
}