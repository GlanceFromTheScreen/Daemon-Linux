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
#include <filesystem>
#include <vector>
#include <variant>

class Daemon {
public:
    static Daemon& get_instance() {
        static Daemon instance;
        return instance;
    }

    std::vector<std::string> read_config() {
        std::string filename = "config.txt";
        std::ifstream file(filename);

        if (!file.is_open()) {
            // error to syslog
        }
        else {
            std::vector<std::string> config_data;
            std::string line;
            while (std::getline(file, line)) {
                config_data.push_back(line);
            }
            file.close();
            return config_data;
        }
    }

    int get_file_age(std::string file_path) {
        std::filesystem::file_time_type file_time = std::filesystem::last_write_time(file_path);
        std::filesystem::file_time_type now = std::filesystem::file_time_type::clock::now();
        std::filesystem::file_time_type::duration age = now - file_time;
        int age_seconds = std::chrono::duration_cast<std::chrono::duration<double>>(age).count();
        return age_seconds;
    }

    void move_files(std::string& source, std::string& target, double time, int sign) {
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
            syslog(LOG_INFO, "ERROR WITH OPENING THE DIRECTORY");
        }
    }

    

    void folders_processing() {

        std::vector<std::string> data = read_config();
        std::string in = data[0];
        std::string out = data[1];
        double time1 = std::stod(data[2]);
        double time2 = std::stod(data[3]);

        int i = 0;
        openlog("my_daemon_5005", LOG_PID, LOG_DAEMON);
        while (i < 5) {
            move_files(in, out, time1, 1);
            move_files(out, in, time2, -1);  // if a > b => -a < -b 
            // пробегаемся по каталогу 2, перемещаем файлы в каталог 2
            // syslog(LOG_INFO, данные перемещены);
            
            //syslog(LOG_INFO, data[0].c_str());
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


int main() {
    Daemon& my_daemon = Daemon::get_instance();
    my_daemon.folders_processing();
    return 0;
}