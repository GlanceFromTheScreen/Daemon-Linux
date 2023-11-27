#include "conn_fifo.h"

#include "conn_mq.h"
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
#include <mqueue.h>
#include <cstring>
#include <cstdlib>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>


void FIFO::connOpen() {
    // Создаем именованный канал
    char* FIFO_NAME = "FIFO2023";

    shared_variable = FIFO_NAME;

    unlink(FIFO_NAME);
    if (mkfifo(FIFO_NAME, 0666) == -1) {
        perror("mkfifo");
        exit(EXIT_FAILURE);
    }
}

void FIFO::connRead(bool is_host, std::ofstream* logFile) {
    int fd = open(shared_variable, O_RDONLY);
    if (fd == -1) {
        perror("open");
        exit(EXIT_FAILURE);
    }

    char buffer[100];
    ssize_t bytesRead = read(fd, buffer, sizeof(buffer));
    if (bytesRead == -1) {
        perror("read");
        exit(EXIT_FAILURE);
    }

    if(is_host) {
        std::cout << std::string(buffer);
    }
    else {
        std::string abc(buffer);
        writeToLogFile(abc, *logFile);
    }
}

void FIFO::connWrite(std::string answer) {

    int fd = open(shared_variable, O_RDWR);
    if (fd == -1) {
        perror("open");
        exit(EXIT_FAILURE);
    }

    const char* message = answer.c_str();
    if (write(fd, message, sizeof(message)) == -1) {
        perror("write");
        exit(EXIT_FAILURE);
    }

    
    
}

void FIFO::connClose() {
    int fd = open(shared_variable, O_WRONLY);
    if (fd == -1) {
        perror("open");
        exit(EXIT_FAILURE);
    }

    close(fd);
}

