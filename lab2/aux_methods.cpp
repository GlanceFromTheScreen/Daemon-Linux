#include "aux_methods.h"

void writeToLogFile(const std::string& message, std::ofstream& logFile) {
    if (logFile.is_open()) {
        logFile << "host: " << message << std::endl;        
    } else {
        std::cout << "Unable to open log file." << std::endl;
    }
}


bool check_file_for_updates(std::streampos& ex_pos, std::string& res) {
    std::ifstream inputFile("client_in.file");
    inputFile.seekg(0, std::ios::end);

    std::streampos tail = inputFile.tellg();

    if (ex_pos != tail) {
        inputFile.seekg(ex_pos);

        inputFile >> res;
        ex_pos = tail;
        inputFile.close();
        return true;

    }
    inputFile.close();
    return false;
}

std::string getAnswer()
{    
    std::string answer;
    std::getline(std::cin, answer);
    return answer;
}

bool wait_for_sem(sem_t *sem) {
    sem_wait(sem);
    return true;
}