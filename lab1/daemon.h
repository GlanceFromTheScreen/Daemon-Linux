#ifndef DAEMON_H
#define DAEMON_H
#include <vector>
#include <iostream>
#include <cstdlib>
#include <unistd.h>

// Singleton Daemon class
class Daemon {
public:
    static Daemon& get_instance();  // creates instance
    void daemon_launch();  // main method
private:
    static inline std::vector<std::string> data = {};  // data written from config.txt
    static inline bool is_it_time_to_finish = 0;  // catches SIGTERM signal

    Daemon();
    ~Daemon() {}
    Daemon(const Daemon&) = delete;
    Daemon& operator= (const Daemon&) = delete;

    // working with reading from and writing to files
    static bool read_config();
    static double read_pid();
    static bool write_pid(std::string out_pid);

    // main functionality
    void move_files(std::string& source, std::string& target, double time, int sign);
    int get_file_age(std::string file_path); 

    // next 2 functions are run in the begining of demon launching
    bool check_if_process_exists(pid_t pid);  // prevents 2 deamons running (looks for folder <pid> in /proc/)
    bool reset_new_process();

    void clever_sleep();
    
    // signals processing
    static void sigterm_handler(int signal_number);
    static void sighup_handler(int signal_number);    
};

#endif