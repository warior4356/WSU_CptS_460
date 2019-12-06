#include "ucode.c"

int main(int argc, char *argv[ ])
{
    char c;
    if(argc == 1)
    {
        print2f("|************Commands************|\n");
        print2f("logout more cat grep l2u cp ls\n");
        print2f("man [command name] for details\n");
    }
    else if(argc == 2)
    {
        print2f("Under Construction\n");
    }
}
