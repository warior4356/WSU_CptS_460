#include "ucode.c"

#define screen 26

int main(int argc, char *argv[ ])
{
    int lines = screen;
    char tty[64];
    int file = 0, backupStdin, cmdfd;
    int n, mult = 2;
    char c;    
    char fileLine[256];
    if(argc == 2)
    {
        file = open(argv[1], O_RDONLY);
        mult = 1;
    }
    
    backupStdin = file;
    gettty(tty);
    cmdfd = open(tty, O_RDONLY);
    
    while(1)
    {    

        for(int i = 0; i < lines * mult; i++)
        {
            if(!getfline(fileLine, file))
                return;
            prints(fileLine);    
        }
        lines = 0;
        
        read(cmdfd, &c, 1);
        if(c == '\r')
        {
            lines = 1;
        }
        if(c == ' ')
        {
            lines = screen;
        }
        if(c == 4)
        {
            return;
        }
    }
}
