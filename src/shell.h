#ifndef SHELL_H
#define SHELL_H

#include <string>
#include <vector>

class Shell {
private:
    std::vector<std::string> history;
    std::string history_file;
    bool should_exit;

public:
    Shell();
    int run();
    void executeCommand(const std::string& command);
    void addToHistory(const std::string& command);
    void saveHistory();
    void loadHistory();
    
private:
    void processLine(const std::string& line);
    void printPrompt();
};

#endif
