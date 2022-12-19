#include <stdio.h>

/*
void    pass_addr(char **buffer)
{
    
    printf("OKAY0\n");
    printf("BUFFER2: %s\n", (char*)buffer);
    //printf("BUFFER3: %s\n", (char*)buffer);
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
    
    pass_addr((char**)&buffer);
    return 0;
}
*/

void    pass_addr(char *buffer)
{
    
    printf("OKAY0\n");
    printf("BUFFER2: %p\n", &buffer);
    printf("BUFFER3: %s\n", buffer);
    //printf("BUFFER3: %s\n", (char*)buffer);
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
    
    pass_addr(buffer);
    return 0;
}