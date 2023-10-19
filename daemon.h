#ifndef DAEMON_H
#define DAEMON_H
#include <vector>
#include <iostream>
#include <cstdlib>
#include <unistd.h>

class Daemon {
public:
    static Daemon& get_instance();
    void daemon_launch();
private:
    static inline std::vector<std::string> data = {};
    static inline bool is_it_time_to_finish = 0;

    Daemon();
    ~Daemon() {}
    Daemon(const Daemon&) = delete;
    Daemon& operator= (const Daemon&) = delete;

    static void read_config();
    static double read_pid();
    static void write_pid(std::string out_pid);

    void move_files(std::string& source, std::string& target, double time, int sign);
    int get_file_age(std::string file_path);

    bool check_if_process_exists(pid_t pid);
    bool reset_new_process();
    
    static void sigterm_handler(int signal_number);
    static void sighup_handler(int signal_number);    
};

#endif