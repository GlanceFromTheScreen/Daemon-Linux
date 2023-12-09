#include "conn.h"
#include <mqueue.h>

class MQ: public AbstractConnection {
// private:
//     mqd_t mq;
public:
    void connOpen() override;

    void connRead(bool is_host, std::ofstream* logFile = nullptr) override;

    void connWrite(std::string answer) override;

    void connClose() override;
 
};