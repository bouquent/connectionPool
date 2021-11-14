/*#include <fcntl.h>
#include <unistd.h>
#include <iostream>
#include <time.h>
#include "connectionpool.hpp"


int main()
{
    clock_t begin = clock();
    for (int i = 0; i < 10000; ++i) {
        ConnectionPool* connPtr = ConnectionPool::getConnectionPool();
        char sql[1024] = {0};
        sprintf(sql, "insert into student(id, name, sex) values(%d, 'czz', 'man')", i);
	auto conn = connPtr->getConnection();
        conn->update(sql);
    }
    clock_t end = clock();
    std::cout << (end - begin) / 1000 << "ms" << std::endl;
    printf("over!\n");
    return 0;
}*/



#include <fcntl.h>
#include <unistd.h>
#include <iostream>
#include <time.h>
#include "connection.hpp"


int main()
{
    clock_t begin = clock();
    for (int i = 0; i < 1000; ++i) {
        Connection conn;
        char sql[1024] = {0};
        sprintf(sql, "insert into student(id, name, sex) values(%d, 'czz', 'man')", i);
        conn.connnect("127.0.0.1", "root", "chenzezheng666", "test", 3306);
        conn.update(sql);
    }
    clock_t end = clock();
    std::cout << (end - begin) / 1000 << "ms" << std::endl;
    return 0;
}
