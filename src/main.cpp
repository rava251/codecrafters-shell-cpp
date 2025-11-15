#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <sstream>
#include <unistd.h>
#include <pwd.h>
#include <cstdlib>
#include <signal.h>
#include <sys/stat.h>

class Shell {
private:
    std::vector<std::string> history;
    std::string history_file;

    void loadHistory() {
        std::ifstream file(history_file);
        std::string line;
        while (std::getline(file, line)) {
            if (!line.empty()) history.push_back(line);
        }
    }

    void saveHistory() {
        std::ofstream file(history_file);
        for (const auto& cmd : history) file << cmd << std::endl;
    }

    void addToHistory(const std::string& command) {
        history.push_back(command);
    }

    bool isBuiltin(const std::string& cmd) {
        return cmd == "\\q" || cmd.rfind("echo ", 0) == 0 || 
               cmd.rfind("\\e ", 0) == 0 || cmd.rfind("\\l ", 0) == 0;
    }
    void executeBuiltin(const std::string& cmd) {
        if (cmd == "\\q") {
            std::cout << "Goodbye!" << std::endl;
            exit(0);
        }
        else if (cmd.rfind("echo ", 0) == 0) {
            std::cout << cmd.substr(5) << std::endl;
        }
        else if (cmd.rfind("\\e ", 0) == 0) {
            char* val = getenv(cmd.substr(3).c_str());
            if (val) {
                std::string s(val);
                if (s.find(':') != std::string::npos) {
                    std::istringstream iss(s);
                    std::string item;
                    while (std::getline(iss, item, ':')) std::cout << item << std::endl;
                } else std::cout << s << std::endl;
            }
        }
        else if (cmd.rfind("\\l ", 0) == 0) {
            system(("lsblk " + cmd.substr(3)).c_str());
        }
    }

public:
    Shell() {
        const char* home = getenv("HOME");
        if (!home) home = getpwuid(getuid())->pw_dir;
        history_file = std::string(home) + "/.kubsh_history";
        loadHistory();
        
        std::string users_dir = std::string(home) + "/users";
        mkdir(users_dir.c_str(), 0755);
        
        signal(SIGHUP, [](int) { std::cout << "Configuration reloaded" << std::endl; });
    }

    ~Shell() { saveHistory(); }
    void run() {
        std::string input;
        while (true) {
            std::cout << "kubsh> " << std::flush;
            if (!std::getline(std::cin, input)) break;
            if (input.empty()) continue;
            addToHistory(input);
            if (isBuiltin(input)) executeBuiltin(input);
            else system(input.c_str());
        }
    }
};

int main() {
    Shell shell;
    shell.run();
    return 0;
}
