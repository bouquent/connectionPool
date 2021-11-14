#include "connection.hpp"
#include <stdio.h>

Connection::Connection()
{
    conn_ = mysql_init(nullptr);
    if (conn_ == nullptr) {
        printf("mysql_init wrong!, error:[%s]", mysql_error(conn_));
        exit(-1);
    }
    printf("connection::connection!\n");
}
Connection::~Connection()
{
    mysql_close(conn_);
    printf("connection::~connection!\n");
}

bool Connection::connnect(std::string host, 
                        std::string user, 
                        std::string password, 
                        std::string db, 
                        unsigned int port)    
{
    MYSQL *p = mysql_real_connect(conn_, host.c_str(), user.c_str(), password.c_str(), db.c_str(), port, NULL, 0);
    if (conn_ == nullptr) {
        printf("mysql_real_connect wrong!, error:[%s]\n", mysql_error(conn_));
        return false;
    }
    return true;
}
bool Connection::update(const std::string& sql)      /*增删改操作*/
{
    int ret = mysql_query(conn_, sql.c_str());
    if (ret != 0) {
        printf("mysql_query wrong sql is %s, [%s]\n", sql.c_str(), mysql_error(conn_));
        return false;
    }
    return true;
}


MYSQL_RES* Connection::query(const std::string& sql) /*查询语句*/
{
    int ret = mysql_query(conn_, sql.c_str());
    if (ret != 0) {
        printf("mysql_query wrong sql is %s, [%s]\n", sql.c_str(), mysql_error(conn_));
        return NULL;
    }

    return mysql_store_result(conn_);
}
