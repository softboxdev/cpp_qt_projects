#include <iostream>
#include <algorithm>
#include <vector>
using namespace std;

class BinaryHeap {
private:
    vector<int> heap;
    // additional functions
    int parent(int i) { return ( i- 1)/2;} //  v kuche element c indexom i 
    int leftChild(int i) {return 2 * i + 1;}
    int rightChild(int i) {return 2 * i + 2;}
public:
    void push(int value) {
       heap.push_back(value);
}
    void pop() {
       // method udalenya max element
       if (heap.empty()) return;
       heap[0] = heap.back(); 
       heap.pop_back();
       if (!heap.empty()) 
          //

}
}



int main() 
{   
    cout << "Binary heap exemple!" << endl;
    



    return 0;
}
