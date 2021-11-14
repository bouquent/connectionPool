#ifndef CONNECTIONPOOL_H
#define CONNECTIONPOOL_H

#include "connection.hpp"
#include <string>
#include <queue>
#include <atomic>
#include <mutex>
#include <condition_variable>
#include <memory>

class ConnectionPool
{
public:
    static ConnectionPool* getConnectionPool();
    ~ConnectionPool();

    /*从连接池中获取一个Connection*/
    std::shared_ptr<Connection> getConnection();
private:
    /*单例模式，构造函数私有化*/
    ConnectionPool();

    /*加载端口，用户，密码等信息*/
    void loadConfigFile();

    /*专门生产新连接的线程*/
    void produceConnectionTask();

    /*扫描超过maxFreeTime时间的连接，对这些空闲连接进行回收*/
    void scannerConnectionTask();

private:    
    std::string ip_;
    std::string host_;
    std::string user_;
    std::string password_;
    std::string db_;
    unsigned int port_;

    int initSize_;           /*初始的连接量*/
    int maxSize_;            /*最大连接量*/
    int maxFreeTime_;        /*连接池最大空闲时间*/
    int connectionTimeout_;  /*请求连接超时时间*/

    std::queue<Connection*> connectionQue_;
    std::mutex queueMutex_;              /*队列锁*/
    std::condition_variable cond_;       /*配合互斥锁的条件变量*/
    std::atomic_int64_t countConnection_; /*记录connection的总数量*/
};

#endif
