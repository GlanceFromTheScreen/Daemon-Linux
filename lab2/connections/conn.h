#include <iostream>
#include <string>
#pragma once

class AbstractConnection {
public:

    virtual void connOpen() = 0;
    virtual void connRead(bool is_host, std::ofstream* logFile = nullptr) = 0;
    virtual void connWrite(std::string answer) = 0;
    virtual void connClose() = 0;

    char* shared_variable;
};