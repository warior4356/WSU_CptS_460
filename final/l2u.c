#include "ucode.c"

int main(int argc, char *argv[ ])
{
    char c;
    if(argc == 3)
    {
        close(0);
        open(argv[1], O_RDONLY);
        close(1);
        open(argv[2], O_WRONLY|O_CREAT);
    }
    
    while(read(0, &c, 1) > 0)
    {
        if (('a' <= c) && (c <= 'z'))
        {
            c = c - ('a' - 'A');
        }
        write(1, &c, 1);
    }
}
