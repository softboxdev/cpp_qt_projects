#include <iostream>
#include <vector>
using namespace std;

vector<int> stack;

void push(int value) {
    stack.push_back(value);
    cout << "Added value : " << value <<endl;
}

void pop() {
    if(stack.empty()) {
        cout << "Stack is empty!" << endl;
        return;
    }
    cout << "Deleted value " << stack.back() << endl;
    stack.pop_back();
}

int top() {
    if (stack.empty()) {
        cout << "Stack is empty!" << endl;
        return -1;
    }
    return stack.back();
}

void display() {
    if(stack.empty()) {
        return;
    }
    for (int i = 0; i < stack.size(); i++) {
        cout << stack[i] << " ";
    }
    cout << "Top pointer = " << stack.back() << endl;
}

int main(){}
