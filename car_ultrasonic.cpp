#include <iostream>
#include <ctime>
#include <unistd.h>
using namespace std;

int main()
{
    clock_t cl;
    cl = clock();
    sleep(5);
    cl = clock() - cl;
    cout <<(float)cl/10 << endl;
    return 0;
}
