#include <iostream>
#include <string>

int main() {
    std::string input;
    
    while (true) {
        std::cout << "kubsh> " << std::flush;
        
        if (!std::getline(std::cin, input)) {
            // Ctrl+D pressed - exit
            std::cout << std::endl << "Goodbye!" << std::endl;
            break;
        }
        
        if (!input.empty()) {
            std::cout << input << std::endl;
        }
    }
    
    return 0;
}
