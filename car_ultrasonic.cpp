#include <iostream>
#include <chrono>
using namespace std;

int main()
{
    auto x=1;
    auto c='a';
    std::cout<<x<<c;
    auto begin = std::chrono::high_resolution_clock::now();

    for(int i = 0; i < 35000; ++i);
        //std::cout<<"cacat";

    auto end = std::chrono::high_resolution_clock::now();

    std::cout << std::chrono::duration_cast<std::chrono::seconds>(end-begin).count() << "ns" << std::endl;
    return 0;
}
