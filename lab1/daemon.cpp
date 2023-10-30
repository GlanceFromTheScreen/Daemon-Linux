#include "daemon.h"
#include <fstream>
#include <chrono>
#include <thread>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/prctl.h>
#include <syslog.h>
#include <filesystem>
#include <variant>
#include <csignal>


Daemon::Daemon() {
    data = {};
    pid_t pid = fork();
    if (pid < 0) {exit(1);}  
    else if (pid > 0) {exit(0);}  // exit parent process
    else {  //  now we are in child process
        prctl(PR_SET_NAME, "dmn1", 0, 0, 0);
        umask(0);
        
        close(STDIN_FILENO);
        close(STDOUT_FILENO);
        close(STDERR_FILENO);

        setsid();  
    }
}


Daemon& Daemon::get_instance() {
    static Daemon instance;
    return instance;
}


void Daemon::daemon_launch() {
    if (!reset_new_process()) {
        syslog(LOG_INFO, "ERROR IN KILLING (EX)PROCESS. EXIT.");
        return;
    }
    if (!write_pid(std::to_string(getpid()))) {
        syslog(LOG_INFO, "DAEMON CANT WRITE PID IN PID FILE. EXIT.");
        return;
    }
    if (!read_config()) {
        syslog(LOG_INFO, "DAEMON CANT READ CONFIG FILE. EXIT.");
        return;
    }
    
    std::string process_name = "my_daemon_" + std::to_string(getpid());
    openlog(process_name.c_str(), LOG_PID, LOG_DAEMON);
    syslog(LOG_INFO, "DAEMON LAUNCHED");

    // asinc methods sigaction (from csignal) provides instant signals processing
    struct sigaction sa_term;
    sa_term.sa_handler = sigterm_handler;
    sigaction(SIGTERM, &sa_term, nullptr);

    struct sigaction sa_hup;
    sa_hup.sa_handler = sigterm_handler;
    sigaction(SIGTERM, &sa_hup, nullptr);

    // Main Loop
    while (is_it_time_to_finish == 0) {  // while no commands recieved from SIGTERM

        // main functionality: moving files from one folder to another
        move_files(data[0], data[1], std::stod(data[2]), 1);
        move_files(data[1], data[0], std::stod(data[3]), -1);  // if a > b => -a < -b 

        // Wait 30 sec or until signal comes and changes is_it_time_to_finish
        clever_sleep(); 
    }
    syslog(LOG_INFO, "DAEMON KILLED");
    closelog();
}


bool Daemon::read_config() {
    std::string filename = "config.txt";
    std::ifstream file(filename);

    if (!file.is_open()) {
        return false;
    }
    else {
        std::vector<std::string> config_data;
        std::string line;
        while (std::getline(file, line)) {
            config_data.push_back(line);
        }
        file.close();
        data = config_data;
        return true;
    }
}


double Daemon::read_pid() {
    std::string filename = "pid.txt";
    std::ifstream file(filename);

    if (!file.is_open()) {
        // 2nd error type (processed in reset_new_process method)
        return -2;  
    }
    else {
        std::string line;
        if (!std::getline(file, line)) {
            file.close();
            return -1;
        }
        file.close();
        return std::stod(line);
    }
}


bool Daemon::write_pid(std::string out_pid) {
    std::string filePath = "pid.txt";
    std::ofstream outputFile(filePath, std::ios::trunc);  // clean file and then write

    if (outputFile.is_open()) {
        outputFile << out_pid;
        outputFile.close();
        return true;
    } else {
        return false;
    }
}


int Daemon::get_file_age(std::string file_path) {  // part of main functionality
    std::filesystem::file_time_type file_time = std::filesystem::last_write_time(file_path);
    std::filesystem::file_time_type now = std::filesystem::file_time_type::clock::now();
    std::filesystem::file_time_type::duration age = now - file_time;
    int age_seconds = std::chrono::duration_cast<std::chrono::duration<double>>(age).count();
    return age_seconds;
}


void Daemon::move_files(std::string& source, std::string& target, double time, int sign) {
    try {
        for (const auto& item: std::filesystem::directory_iterator(source)) {
            if (std::filesystem::is_regular_file(item) && sign * get_file_age(item.path()) < time * sign) {  
                std::filesystem::path s_file = item.path();
                std::filesystem::path t_file = target / s_file.filename();
                std::filesystem::rename(s_file, t_file);

                char buffer[256];
                snprintf(buffer, sizeof(buffer), "FILE %s MOVED FROM %s TO %s ", (s_file.filename()).c_str(), source.c_str(), target.c_str());
                syslog(LOG_INFO, "%s", buffer);
            }
        }
        syslog(LOG_INFO, "FILES ARE IN RELEVANT STATE");
    } catch (const std::filesystem::filesystem_error& e) {
        syslog(LOG_INFO, "ERROR WITH OPENING THE DIRECTORY WHILE MOVING FILES");
    }
} 


bool Daemon::check_if_process_exists(pid_t pid) {
    std::string path = "/proc/" + std::to_string(pid);
    std::ifstream dir(path);
    if (dir) {return true;}
    return false;
}


void Daemon::sighup_handler(int signal_number) {
    read_config();
}


void Daemon::sigterm_handler(int signal_number) {
    is_it_time_to_finish = 1;
}


bool Daemon::reset_new_process() {
    double ex_pid = read_pid();
    if (ex_pid == -2) {return 0;}
    if (!check_if_process_exists(ex_pid)) {
        return 1;  // process does not exist => ok
    }
    //  finish prev process
    int response = kill(ex_pid, SIGTERM);
    if (response == 0) {return 1;}  // ex process is killed
    else {return 0;}
}


void Daemon::clever_sleep() {
    int i = 0;
    while (i < 30 && is_it_time_to_finish == 0) {
        i++;
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }
}

