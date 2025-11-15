#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <sstream>
#include <unistd.h>
#include <pwd.h>
#include <cstdlib>

class Shell {
private:
    std::vector<std::string> history;
    std::string history_file;

    void loadHistory() {
        std::ifstream file(history_file);
        if (file.is_open()) {
            std::string line;
            while (std::getline(file, line)) {
                if (!line.empty()) {
                    history.push_back(line);
                }
            }
        }
    }

    void saveHistory() {
        std::ofstream file(history_file);
        if (file.is_open()) {
            for (const auto& cmd : history) {
                file << cmd << std::endl;
            }
        }
    }

    void addToHistory(const std::string& command) {
        history.push_back(command);
        if (history.size() > 1000) {
            history.erase(history.begin());
        }
    }

    bool isBuiltinCommand(const std::string& command) {
        return command == "\\q" || 
               command.rfind("echo ", 0) == 0 ||
               command.rfind("\\e ", 0) == 0;
    }

    void executeBuiltinCommand(const std::string& command) {
        if (command == "\\q") {
            std::cout << "Goodbye!" << std::endl;
            exit(0);
        }
        else if (command.rfind("echo ", 0) == 0) {
            std::string text = command.substr(5);
            std::cout << text << std::endl;
        }
        else if (command.rfind("\\e ", 0) == 0) {
            std::string var_name = command.substr(3);
            char* value = getenv(var_name.c_str());
            if (value) {
                std::string env_value(value);
                if (env_value.find(':') != std::string::npos) {
                    // Split by colon and print each on new line
                    std::istringstream iss(env_value);
                    std::string item;
                    while (std::getline(iss, item, ':')) {
                        std::cout << item << std::endl;
                    }
                } else {
                    std::cout << env_value << std::endl;
                }
            } else {
                std::cout << "Environment variable '" << var_name << "' not found" << std::endl;
            }
        }
    }

    void executeCommand(const std::string& command) {
        if (isBuiltinCommand(command)) {
            executeBuiltinCommand(command);
        } else {
            // External command - execute via system
            system(command.c_str());
        }
    }

public:
    Shell() {
        const char* home = getenv("HOME");
        if (!home) {
            home = getpwuid(getuid())->pw_dir;
        }
        history_file = std::string(home) + "/.kubsh_history";
        loadHistory();
    }

    ~Shell() {
        saveHistory();
    }

    void run() {
        std::string input;
        
        while (true) {
            std::cout << "kubsh> " << std::flush;
            
            if (!std::getline(std::cin, input)) {
                std::cout << std::endl << "Goodbye!" << std::endl;
                break;
            }
            
            if (input.empty()) {
                continue;
            }
            
            addToHistory(input);
            executeCommand(input);
        }
    }
};

int main() {
    Shell shell;
    shell.run();
    return 0;
}
