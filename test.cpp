#include <iostream>
#include <fstream>
#include <chrono>
#include <ctime>
#include <thread>
#include <cstdlib>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/prctl.h>
#include <syslog.h>

class Daemon {
public:
    static Daemon& get_instance() {
        static Daemon instance;
        return instance;
    }

    // void my_method() {
    //     std::cout << "my deamon 2\n";
    // }

    void folders_processing() {
        int i = 0;
        openlog("my_daemon_5005", LOG_PID, LOG_DAEMON);
        while (i < 5) {
            syslog(LOG_INFO, "Моя попытка номер пять!!!");
            // Ожидаем 5 секунд
            std::this_thread::sleep_for(std::chrono::seconds(5));
            i++;
        }
        closelog();
    }

private:
    static bool is_valid;
    Daemon() {
        pid_t pid = fork();
        if (pid < 0) {}  // is_valid = false; tracking an error in fork function
        else if (pid > 0) {exit(0);}  // exit parent process
        else {  //  now we are in child process
            // is_valid = 1;
            prctl(PR_SET_NAME, "dmn1", 0, 0, 0);
            umask(0);
            

            close(STDIN_FILENO);
            close(STDOUT_FILENO);
            close(STDERR_FILENO);

            setsid();
        }

        
    }
    ~Daemon() {}
    Daemon(const Daemon&) = delete;
    Daemon& operator= (const Daemon&) = delete;

};

void my_fun() {
    pid_t one = fork();
    if (one < 0) {
        std::cout << "Error\n";
    }
    if (one == 0) {
        std::cout << "Child\n";
    }
    if (one > 0) {
        std::cout << "Parent\n";
        exit(0);
    }
}

int main() {

    // pid_t pid = fork();

    // if (pid < 0) {std::cerr << "Error with deamon creating\n"; return 1;}
    // if (pid > 0) {exit(0);}

    // prctl(PR_SET_NAME, "my_deamon_1", 0, 0, 0);
    // umask(0);
    // setsid();

    // std::ofstream outputFile("file.txt");

    // if (!outputFile.is_open()) {
    //     std::cerr << "Ошибка открытия файла." << std::endl;
    //     return 1;
    // }

    // int  i = 0;
    // while (i < 5) {
    //     // Получаем текущее время
    //     auto currentTime = std::chrono::system_clock::now();
    //     std::time_t time = std::chrono::system_clock::to_time_t(currentTime);

    //     // Записываем текущее время в файл
    //     outputFile << "Текущее время: " << std::ctime(&time);
    //     outputFile.flush();

    //     // Ожидаем 10 секунд
    //     std::this_thread::sleep_for(std::chrono::seconds(10));
    //     i++;
    // }

    // outputFile.close(); // Этот код никогда не будет выполнен
    
    // my_fun();
    // std::cout << "MAIN\n";

    Daemon& my_daemon = Daemon::get_instance();
    my_daemon.folders_processing();
    // std::cout << "this msg must be only once\n";
    return 0;
}