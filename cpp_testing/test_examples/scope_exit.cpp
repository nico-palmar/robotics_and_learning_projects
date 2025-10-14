#include <iostream>
#include <scope>  // C++23 feature

void myFunction() {
    std::scope_exit onExit([] {
        std::cout << "Scope exited! Running cleanup routine.\n";
    });

    std::cout << "Inside myFunction\n";
}

int main() {
    myFunction();
    return 0;
}
