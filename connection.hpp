#ifndef CONNECTION_H
#define CONNECTION_H
#include <mysql/mysql.h>
#include <string>
#include <time.h>

class Connection
{
public:
    Connection();
    ~Connection();

    bool connnect(std::string host, 
                std::string user, 
                std::string password, 
                std::string db, 
                unsigned int port);    /*连接数据库*/

    bool update(const std::string& sql);      /*增删改操作*/

    MYSQL_RES* query(const std::string& sql); /*查询语句*/

    	//刷新一下连接的起始的空闲时间点
	void refreshAliveTime() { alivetime_ = clock(); }
	//返回存活的时间
	clock_t getAliveeTime()const { return clock() - alivetime_; }
private:
    MYSQL *conn_;
    clock_t alivetime_;
};

#endif
