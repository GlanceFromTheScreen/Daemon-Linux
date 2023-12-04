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
#include "../connections/conn_shm.h"
#include "../aux_methods.h"
#include "../connections/conn_mq.h"
#include "../connections/conn_fifo.h"
#include <csignal>

std::string n1 = "/3999write_semaphore";
std::string n2 = "/3999host_read_semaphore";
std::string n3 = "/3999client_read_semaphore";

class Host {
public:

    static inline bool is_running = true;
    sem_t *write_semaphore;
    sem_t *host_read_semaphore;
    sem_t *client_read_semaphore;
    AbstractConnection* abstr = nullptr;

    Host(char* conn_type) {
   
        if (std::string(conn_type) == "shm") {
            std::cout << "no1\n";
        abstr = new SEG;
        }
        else if (std::string(conn_type) == "fifo") {
            std::cout << "no2\n";
            abstr = new FIFO;
        } else {
            std::cout << "no3\n";
            abstr = new MQ;
        }
        
        write_semaphore = sem_open(n1.c_str(), O_CREAT, S_IRWXU | S_IRWXG | S_IRWXO, 1);
        host_read_semaphore = sem_open(n2.c_str(), O_CREAT, S_IRWXU | S_IRWXG | S_IRWXO, 0);
        client_read_semaphore = sem_open(n3.c_str(), O_CREAT, S_IRWXU | S_IRWXG | S_IRWXO, 0);
    }

    void run() {

        // Opening connection
        abstr->connOpen();
        
        pid_t pid = fork();

        if (pid < 0) {
            perror("fork failed");
            exit(EXIT_FAILURE);
        }

        if (pid > 0) {
            // Parent process
            printf("Parent process. Child PID: %d\n", pid);

            std::future<std::string> future = std::async(std::launch::async, getAnswer);
            std::string answer;
            std::future<bool> future2 = std::async(std::launch::async, wait_for_sem, host_read_semaphore);

            struct sigaction sa_int;
            sa_int.sa_handler = sigint_handler;
            sigaction(SIGINT, &sa_int, nullptr);

            while(is_running) {
                std::chrono::seconds timeout(1);
                if (future.wait_for(timeout) == std::future_status::ready)
                {
                    answer = future.get();
                    future = std::async(getAnswer);  // update future
                    
                    sem_wait(write_semaphore);
                    answer += "\n";
                    abstr->connWrite(answer);
                    sem_post(client_read_semaphore);
                }
                
                if (future2.wait_for(timeout) == std::future_status::ready) {
                    abstr->connRead(true);
                    sem_post(write_semaphore);
                    future2 = std::async(std::launch::async, wait_for_sem, host_read_semaphore);
                }
            }

            // Закрываем и удаляем межпроцессовые семафоры
            sem_close(write_semaphore);
            sem_close(host_read_semaphore);
            sem_close(client_read_semaphore);
            sem_unlink(n1.c_str());
            sem_unlink(n2.c_str());
            sem_unlink(n3.c_str());

            abstr->connClose();

            delete abstr;
            system("pkill gnome-terminal");
            // std::cout << "FINITO\n";

        
        
        } else {
            // Дочерний процесс
            setsid();
            std::ofstream log;
            log.open("client_out.file");
            system("gnome-terminal -e \"bash -c 'cp /dev/stdin client_in.file | tail -f client_out.file ;'\""); // открыли терминал, out которого соединен с дескриптором файла log.file, а stdin с дескриптором файла myfile.txt
            log << "You are in child process\n";
            log.close();

            std::ofstream logFile("client_out.file", std::ios::app);  // std::ios::app - открывает файл для добавления в конец
            std::future<bool> future = std::async(std::launch::async, wait_for_sem, client_read_semaphore);

            std::streampos tail = 0;
            std::string ans = "";

            struct sigaction sa_int;
            sa_int.sa_handler = sigint_handler;
            sigaction(SIGINT, &sa_int, nullptr);
            
            while(is_running) {
                std::chrono::seconds timeout(3);

                if (check_file_for_updates(tail, ans))
                {   
                    sem_wait(write_semaphore);
                    ans = "client: " + ans + "\n";
                    abstr->connWrite(ans);
                    sem_post(host_read_semaphore);

                }
        
                if (future.wait_for(timeout) == std::future_status::ready) {
                    abstr->connRead(0, &logFile);            
                    sem_post(write_semaphore);
                    future = std::async(std::launch::async, wait_for_sem, client_read_semaphore);
                }
            }  
            logFile.close();
            abstr->connClose();

        } 
    }


    static void sigint_handler(int signal_number) {
        is_running = false;
    }

};



int main(int argc, char* argv[]) {

    Host host(argv[1]);
    host.run();
   
    return 0;
}
