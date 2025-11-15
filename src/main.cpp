#include <iostream>
#include <string>

int main() {
    std::string input;
    if (std::getline(std::cin, input)) {
        std::cout << input << std::endl;
    }
    return 0;
}
