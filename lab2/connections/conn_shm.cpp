#include "conn_shm.h"
#include "../aux_methods.h"

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


void SEG::connOpen() {

    // Создаем ключ для разделяемой памяти
    key_t key = ftok("/tmp", 'b');
    // Создаем сегмент разделяемой памяти
    shmid = shmget(key, 256, IPC_CREAT | 0666);
    // Присоединяем сегмент к адресному пространству процесса
    shared_variable = (char*)shmat(shmid, NULL, 0);
    // Инициализируем строку
    std::string message = "Hello, shared memory!";
    strcpy(shared_variable, message.c_str());
    
}

void SEG::connRead(bool is_host, std::ofstream* logFile) {
    if(is_host) {
        std::cout << shared_variable;
    }
    else {
        std::string abc(shared_variable);
        writeToLogFile(abc, *logFile);
    }
}

void SEG::connWrite(std::string answer) {
    strcpy(shared_variable, answer.c_str());
}

void SEG::connClose() {
    shmdt(shared_variable);
    shmctl(shmid, IPC_RMID, NULL);
}

