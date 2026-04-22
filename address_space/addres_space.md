#include <iostream>

void hello() { std::cout << "Hello!\n"; }
void world() { std::cout << "World!\n"; }

int main() {
    std::cout << "Адрес hello: " << (void*)hello << "\n";
    std::cout << "Адрес world: " << (void*)world << "\n";
    
    void (*func_ptr)() = hello;  // Указатель на функцию
    func_ptr();  // Вызываем hello через указатель
    
    func_ptr = world;
    func_ptr();  // Вызываем world через указатель
    
    return 0;
}



