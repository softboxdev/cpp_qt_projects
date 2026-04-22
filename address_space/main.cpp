#include <iostream>
#include <cstdint>

int main() {
    int arr[5] = {10, 20, 30, 40, 50};
    int* ptr = arr;  // ptr указывает на arr[0]
    
    std::cout << "Базовый адрес: " << ptr << "\n";
    std::cout << "sizeof(int): " << sizeof(int) << "\n\n";
    
    for(int i = 0; i < 5; i++) {
        std::cout << "arr[" << i << "] = " << arr[i] << "\n";
        std::cout << "&arr[" << i << "] = " << &arr[i] 
                  << " (смещение: " << (uintptr_t)(&arr[i]) - (uintptr_t)arr 
                  << " байт)\n";
        std::cout << "ptr + " << i << " = " << ptr + i << "\n";
        std::cout << "*(ptr + " << i << ") = " << *(ptr + i) << "\n\n";
    }
    
    return 0;
}
