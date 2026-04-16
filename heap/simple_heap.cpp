#include <iostream>
#include <vector>
#include <algorithm>

using namespace std;
int main() {
    vector<int> nums = {5,2,8,1,9};
    make_heap(nums.begin(), nums.end());
    cout << "HEAP: ";
    for (int n: nums) cout << n << " " << endl;
    cout << "\n Maximum element" << nums.front() << endl;
    // add position (element)
    nums.push_back(21);
    push_heap(nums.begin(), nums.end());
    cout << "\nNew maximum" << nums.front() << endl;
    cout << "HEAP: " << endl;
    for (int n: nums) cout << n << " ";
    //delete element
    pop_heap(nums.begin(), nums.end());
    nums.pop_back();
    cout << "HEAP: " << endl;
    for (int n: nums) cout << n << " ";
    
}
