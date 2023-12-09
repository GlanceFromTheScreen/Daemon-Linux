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


void MQ::connOpen() {
    char* QUEUE_NAME = "/my_message_queue317651";
    const int MAX_MSG_SIZE = 256;
    const int QUEUE_PERMISSIONS = 0666;

    mq_unlink(QUEUE_NAME);

    shared_variable = QUEUE_NAME;
    mqd_t mq = mq_open(QUEUE_NAME, O_CREAT | O_RDWR | O_EXCL, QUEUE_PERMISSIONS, nullptr);


    if (mq == (mqd_t)-1) {
        perror("mq_open");
        exit(EXIT_FAILURE);
    }

    // Очищаем очередь
    // const char* message = "";
    // if (mq_send(mq, message, strlen(message) + 1, 0) == -1) {
    //     perror("mq_send");
    //     exit(EXIT_FAILURE);
    // }
}

void MQ::connRead(bool is_host, std::ofstream* logFile) {
    // Открываем существующую очередь сообщений
    mqd_t mq = mq_open(shared_variable, O_RDWR);
    if (mq == (mqd_t)-1) {
        perror("mq_open");
        exit(EXIT_FAILURE);
    }

    // Получаем атрибуты очереди, чтобы узнать размер сообщения
    struct mq_attr attr;
    if (mq_getattr(mq, &attr) == -1) {
        perror("mq_getattr");
        exit(EXIT_FAILURE);
    }

    // Выделяем буфер под сообщение
    char buffer[attr.mq_msgsize];

    // Читаем сообщение из очереди
    ssize_t bytesRead = mq_receive(mq, buffer, attr.mq_msgsize, nullptr);
    if (bytesRead == -1) {
        perror("mq_receive");
        exit(EXIT_FAILURE);
    }

    if(is_host) {
        std::cout << buffer;
    }
    else {
        std::string abc(buffer);
        writeToLogFile(abc, *logFile);
    }
}

void MQ::connWrite(std::string answer) {

    mqd_t mq = mq_open(shared_variable, O_RDWR);
    if (mq == (mqd_t)-1) {
        perror("mq_open");
        exit(EXIT_FAILURE);
    }

    // Отправляем сообщение в очередь
    const char* message = answer.c_str();
    if (mq_send(mq, message, strlen(message) + 1, 0) == -1) {
        perror("mq_send");
        exit(EXIT_FAILURE);
    }
    
}

void MQ::connClose() {
    // Открываем существующую очередь сообщений
    mqd_t mq = mq_open(shared_variable, O_RDWR);
    if (mq == (mqd_t)-1) {
        perror("mq_open");
        exit(EXIT_FAILURE);
    }


    // Закрываем очередь сообщений
    if (mq_close(mq) == -1) {
        perror("mq_close");
        exit(EXIT_FAILURE);
    }

    // Удаляем очередь сообщений
    if (mq_unlink(shared_variable) == -1) {
        perror("mq_unlink");
        exit(EXIT_FAILURE);
    }
}

