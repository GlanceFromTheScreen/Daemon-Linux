#include "conn.h"

class SEG: public AbstractConnection {
private:
    int shmid;
public:
    void connOpen() override;

    void connRead(bool is_host, std::ofstream* logFile = nullptr) override;

    void connWrite(std::string answer) override;

    void connClose() override;
 
};