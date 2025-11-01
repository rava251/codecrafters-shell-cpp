#include "shell.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <unistd.h>
#include <pwd.h>
#include <cstdlib>

Shell::Shell() : should_exit(false) {
    const char* home = getenv("HOME");
    if (!home) {
        home = getpwuid(getuid())->pw_dir;
    }
    history_file = std::string(home) + "/.kubsh_history";
    loadHistory();
}

int Shell::run() {
    std::string line;
    
    std::cout << "Welcome to kubsh! Type '\\q' to exit." << std::endl;
    
    while (!should_exit) {
        printPrompt();
        
        if (!std::getline(std::cin, line)) {
            std::cout << std::endl << "Goodbye!" << std::endl;
            break;
        }
        
        processLine(line);
    }
    
    saveHistory();
    return 0;
}

void Shell::processLine(const std::string& line) {
    if (line.empty()) return;
    
    addToHistory(line);
    
    if (line == "\\q") {
        should_exit = true;
        std::cout << "Goodbye!" << std::endl;
        return;
    }
    
    executeCommand(line);
}

void Shell::executeCommand(const std::string& command) {
    if (command.rfind("echo ", 0) == 0) {
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
            std::cout << "Variable '" << var_name << "' not found" << std::endl;
        }
    }
    else if (command.rfind("\\l ", 0) == 0) {
        std::string device = command.substr(3);
        std::string cmd = "lsblk " + device + " 2>/dev/null || echo 'Device not found'";
        system(cmd.c_str());
    }
    else {
        system(command.c_str());
    }
}

void Shell::addToHistory(const std::string& command) {
    history.push_back(command);
    if (history.size() > 1000) {
        history.erase(history.begin());
    }
}

void Shell::saveHistory() {
    std::ofstream file(history_file);
    if (file.is_open()) {
        for (const auto& cmd : history) {
            file << cmd << std::endl;
        }
    }
}

void Shell::loadHistory() {
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

void Shell::printPrompt() {
    std::cout << "kubsh> " << std::flush;
}
