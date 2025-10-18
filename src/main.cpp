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
#include <dirent.h>

class Shell {
private:
    std::vector<std::string> history;
    std::string history_file;
    std::string users_dir;

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

    void initializeUserVFS() {
        const char* home = getenv("HOME");
        if (!home) {
            home = getpwuid(getuid())->pw_dir;
        }
        users_dir = std::string(home) + "/users";
        
        // Create users directory
        mkdir(users_dir.c_str(), 0755);
        
        std::cout << "User VFS directory created: " << users_dir << std::endl;
        std::cout << "Manual commands for VFS:" << std::endl;
        std::cout << "  Create user: sudo adduser username" << std::endl;
        std::cout << "  Delete user: sudo userdel username" << std::endl;
    }

    bool isBuiltinCommand(const std::string& command) {
        return command == "\\q" || 
               command.rfind("echo ", 0) == 0 ||
               command.rfind("\\e ", 0) == 0 ||
               command.rfind("\\l ", 0) == 0 ||
               command.rfind("adduser ", 0) == 0 ||
               command.rfind("userdel ", 0) == 0;
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
        else if (command.rfind("\\l ", 0) == 0) {
            std::string device = command.substr(3);
            std::string cmd = "lsblk " + device + " 2>/dev/null || echo 'Device not found or no permission'";
            system(cmd.c_str());
        }
        else if (command.rfind("adduser ", 0) == 0) {
            std::string username = command.substr(8);
            std::string cmd = "sudo adduser --disabled-password --gecos '' " + username + " 2>/dev/null && mkdir -p " + users_dir + "/" + username + " && echo 'User created and VFS directory added' || echo 'Failed to create user (need sudo)'";
            system(cmd.c_str());
        }
        else if (command.rfind("userdel ", 0) == 0) {
            std::string username = command.substr(8);
            std::string cmd = "sudo userdel " + username + " 2>/dev/null && rm -rf " + users_dir + "/" + username + " && echo 'User deleted and VFS directory removed' || echo 'Failed to delete user (need sudo)'";
            system(cmd.c_str());
        }
    }

    void executeCommand(const std::string& command) {
        if (isBuiltinCommand(command)) {
            executeBuiltinCommand(command);
        } else {
            int result = system(command.c_str());
            if (result != 0) {
                std::cout << "Command not found or failed: " << command << std::endl;
            }
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
        
        // Initialize User VFS
        initializeUserVFS();
        
        // Set up SIGHUP handler
        signal(SIGHUP, [](int sig) {
            std::cout << "Configuration reloaded" << std::endl;
        });
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
