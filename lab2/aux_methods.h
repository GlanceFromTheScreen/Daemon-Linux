#include <string>
#include <iostream>

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <fstream>
#include <semaphore.h>
#include <string>
#include <iostream>
#include <cstdio>
#include <future>
#include <thread>
#include <chrono>
#include <memory>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <cstring>
#include <fcntl.h>

void writeToLogFile(const std::string& message, std::ofstream& logFile);
bool check_file_for_updates(std::streampos& ex_pos, std::string& res);
std::string getAnswer();
bool wait_for_sem(sem_t *sem);


