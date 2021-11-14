#include "connectionpool.hpp"
#include "connection.hpp"
#include <unistd.h>
#include <fcntl.h>
#include <strings.h>
#include <thread>
#include <functional>
#include <iostream>

ConnectionPool* ConnectionPool::getConnectionPool()
{
	static ConnectionPool connectionPool;
	return &connectionPool;
}


/*单例模式，构造函数私有化*/
ConnectionPool::ConnectionPool()
{
    loadConfigFile();
    for (int i = 0; i < initSize_; ++i) {
        Connection *p = new Connection();
        p->connnect(host_, user_, password_, db_, port_);
        p->refreshAliveTime();  /*刷新起始时间*/
        connectionQue_.push(p);
        countConnection_++;
    }

    std::thread produce(std::bind(&ConnectionPool::produceConnectionTask, this));
    produce.detach();

    std::thread scanner(std::bind(&ConnectionPool::scannerConnectionTask, this));
    scanner.detach();
}

ConnectionPool::~ConnectionPool()
{
    while (!connectionQue_.empty()) {
        Connection *p = connectionQue_.front();
        connectionQue_.pop();
        delete p;
    }
}

/*专门生产新连接的线程*/
void ConnectionPool::produceConnectionTask()
{
    while (1) {
        std::unique_lock<std::mutex> lock(queueMutex_);
        while (!connectionQue_.empty()) {
            cond_.wait(lock);
        }

        if (countConnection_ < maxSize_) {
            Connection *p = new Connection();
            p->connnect(host_, user_, password_, db_, port_);
            p->refreshAliveTime();  /*刷新开启的空闲时间*/
            countConnection_++;
        }
        cond_.notify_all(); /*唤醒消费者线程*/
    }
}

 /*扫描超过maxFreeTime时间的连接，对这些空闲连接进行回收*/
void ConnectionPool::scannerConnectionTask()
{
    while (1) {
        /*模拟定时效果*/
        sleep(maxFreeTime_);

        std::unique_lock<std::mutex> lock(queueMutex_);
        while (countConnection_ > initSize_) {
            Connection* p = connectionQue_.front();
            if (p->getAliveeTime() >= (maxFreeTime_ * CLOCKS_PER_SEC))  {
                connectionQue_.pop();
                countConnection_--;
                delete p;
            } else {
                /*队头的连接没有超过，其他连接也肯定没有超过*/
                break;
            }
        }
    }
}

/*获取connection连接，实质是消费者*/
std::shared_ptr<Connection> ConnectionPool::getConnection()
{
    std::unique_lock<std::mutex> lock(queueMutex_);
    while (connectionQue_.empty()) {
        if (cond_.wait_for(lock, std::chrono::milliseconds(connectionTimeout_)) == std::cv_status::timeout) {
            if (connectionQue_.empty()) {
                printf("连接失败，获取连接超时");
                return nullptr;
            }
        }
    }

    std::shared_ptr<Connection> sp(connectionQue_.front(), [&](Connection *p){
        /*自定义删除器，再使用完调用析构函数时，将这个connetion重新加入队列中，并且刷新空闲时间*/
        std::unique_lock<std::mutex> lock(queueMutex_);
        p->refreshAliveTime();
        connectionQue_.push(p);  
    });

    connectionQue_.pop();
    cond_.notify_all();

    return sp;
}


/*加载端口，用户，密码等信息*/
void ConnectionPool::loadConfigFile()
{
    FILE* file = fopen("mysql.txt", "r");
    char buf[30] = {0};

    std::cout << "go ahead" << std::endl;

    while (fgets(buf, sizeof(buf), file)) {
        std::string s = buf;
        int idx = s.find(":");
        std::string value = s.substr(idx + 1, s.size() - idx - 2);  /*换行符不要*/
        if (strncasecmp(buf, "ip", 2) == 0) {
            ip_ = value;
        } else if(strncasecmp(buf, "user", 4) == 0) {
            user_ = value;
        } else if (strncasecmp(buf, "host", 4) == 0) {
            host_ = value;
        } else if (strncasecmp(buf, "password", 8) == 0) {
            password_ = value;
        } else if (strncasecmp(buf, "db", 2) == 0) {
            db_ = value;
        } else if (strncasecmp(buf, "port", 4) == 0) {
            port_ = atoi(value.c_str());
        } else if (strncasecmp(buf, "initSize", 8) == 0) {
            initSize_ = atoi(value.c_str());
        } else if (strncasecmp(buf, "maxSize", 7) == 0) {
            maxSize_ = atoi(value.c_str());
        } else if (strncasecmp(buf, "maxfreeTime", 11) == 0) {
            maxFreeTime_ = atoi(value.c_str());
        } else if (strncasecmp(buf, "connectionTimeout", 17) == 0) {
            connectionTimeout_ = atoi(value.c_str());
        }
    }
}
