#include "../include/Connection.h"
#include "../include/wang.h"
#include "../include/ConnectionPool.h"

int main()
{
	Connection conn;
	conn.connect("127.0.0.1", 3306, "wang", "0812", "chat");

	clock_t begin = clock();
	
	thread t1([]() {
		ConnectionPool *cp = ConnectionPool::ConnPoolInit();
		for (int i = 0; i < 2500; ++i)
		{
			char sql[1024] = { 0 };
			sprintf(sql, "insert into users(name,age,sex) values('%s',%d,'%s')",
				"zhang san", 20, "male");
			shared_ptr<Connection> sp = cp->getConnection();
			sp->update(sql);
		}
		cout<<"t1 end"<<endl;
	});
	thread t2([]() {
		ConnectionPool *cp = ConnectionPool::ConnPoolInit();
		for (int i = 0; i < 2500; ++i)
		{
			char sql[1024] = { 0 };
			sprintf(sql, "insert into users(name,age,sex) values('%s',%d,'%s')",
				"zhang san", 20, "male");
			shared_ptr<Connection> sp = cp->getConnection();
			sp->update(sql);
		}
		cout<<"t2 end"<<endl;
	});
	thread t3([]() {
		ConnectionPool *cp = ConnectionPool::ConnPoolInit();
		for (int i = 0; i < 2500; ++i)
		{
			char sql[1024] = { 0 };
			sprintf(sql, "insert into users(name,age,sex) values('%s',%d,'%s')",
				"zhang san", 20, "male");
			shared_ptr<Connection> sp = cp->getConnection();
			sp->update(sql);
			
		}
		cout<<"t3 end"<<endl;
	});
	thread t4([]() {
		ConnectionPool *cp = ConnectionPool::ConnPoolInit();
		for (int i = 0; i < 2500; ++i)
		{
			char sql[1024] = { 0 };
			sprintf(sql, "insert into users(name,age,sex) values('%s',%d,'%s')",
				"zhang san", 20, "male");
			shared_ptr<Connection> sp = cp->getConnection();
			sp->update(sql);
		}
		cout<<"t4 end"<<endl;
	});

	t1.join();
	t2.join();
	t3.join();
	t4.join();

	clock_t end = clock();
	cout << (end - begin) << "ms" << endl;

	conn.update("delete from users");
	return 0;
}
