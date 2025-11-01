#include <iostream>
#include "shell.h"
#include <signal.h>
#include <sys/stat.h>
#include <unistd.h>
#include <pwd.h>

Shell* g_shell = nullptr;

void sighup_handler(int sig) {
    if (g_shell) {
        std::cout << "Configuration reloaded" << std::endl;
    }
}

int main() {
    Shell shell;
    g_shell = &shell;
    
    signal(SIGHUP, sighup_handler);
    
    const char* home_dir = getenv("HOME");
    if (!home_dir) {
        home_dir = getpwuid(getuid())->pw_dir;
    }
    
    std::string users_dir = std::string(home_dir) + "/users";
    mkdir(users_dir.c_str(), 0755);
    std::cout << "Created: " << users_dir << std::endl;
    
    return shell.run();
}
