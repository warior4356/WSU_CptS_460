#include "ucode.c"

int main(int argc, char *argv[ ])
{
    char command[256];
    strcpy(command, "cat ");
    strcat(command, argv[1]);
    close(1);
    open(argv[2], O_WRONLY|O_CREAT);
    exec(command); 
}
