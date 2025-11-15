#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <unistd.h>
#include <pwd.h>

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
            
            if (input == "\\q") {
                std::cout << "Goodbye!" << std::endl;
                break;
            }
            
            if (!input.empty()) {
                addToHistory(input);
                std::cout << input << std::endl;
            }
        }
    }
};

int main() {
    Shell shell;
    shell.run();
    return 0;
}
