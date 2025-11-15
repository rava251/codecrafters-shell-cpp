#include <iostream>
#include <string>
#include <vector>
#include <fstream>
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
        return command == "\\q" || command.rfind("echo ", 0) == 0;
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
