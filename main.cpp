#include <iostream>
#include "allocator.hpp"

using namespace std;


int main() {
    myAlloc myAlloc(2, 100);
    int *temp = (int*)myAlloc.m_alloc(50);
    *temp = 555;
    myAlloc.m_alloc(50);
    int *newA = (int*)myAlloc.m_realoc(temp, 50);
    cout << *temp << endl;
    cout << newA << endl;
    return 0;
}
