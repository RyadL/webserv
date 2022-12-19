#include <fstream>
#include <iostream>
#include <string>
 
using namespace std;

void    pass_addr(char **buffer)
{
    
    printf("OKAY0\n");
    printf("BUFFER2: %p\n", buffer);
   
}

int main()
{
    char buffer[5];

    buffer[0] = 'T';
    buffer[1] = 'E';
    buffer[2] = 'S';
    buffer[3] = 'T';
    buffer[4] = 0;

    printf("BUFFER0: %p\n", &buffer);

    printf("BUFFER1: %s\n", buffer);
    
    pass_addr((char**)buffer);
    return 0;
}