#include <iostream>
#include <poll.h>
#include <unistd.h>
#include <string.h>

int main()
{
    char    name[5];
    int counter = 0;

    struct pollfd   mypoll;

    memset(&mypoll, 0, sizeof(mypoll));
    mypoll.fd = 0;
    mypoll.events = POLLIN;

    std::cout << "Type in your name: " << std::endl;

    while (1)
    {
        //std::cout << "counter0: " << counter << std::endl;
        poll(&mypoll, 1, 100);
        if (mypoll.revents & POLLIN)
        {
            read(0, name, 7);
            std::cout << "Hello " << name << std::endl;
            //break;
        }
        else
            counter++;
    }
    std::cout << "counter1: " << counter << std::endl;
    return 0;

    return 0;
}