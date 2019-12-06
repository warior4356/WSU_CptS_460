#include "ucode.c"

int isRedirection()
{
    STAT fs, fs2;
    // Stat stdout
    fstat(0, &fs);
    fstat(1, &fs2);


    if (fs.st_dev == fs2.st_dev && fs.st_dev > 0)
        return 0;
    return 1;
}

int isFile(int fd)
{
    STAT s;
    // Stat stdout
    fstat(fd, &s);

    if(((s.st_mode & 0xF000) == 0x8000) ||
        ((s.st_mode & 0xF000) == 0x4000) ||
        ((s.st_mode & 0xF000) == 0xA000))
        return 1;
    return 0;
}

/* four cases:
    file in file out
    stndin file out
    stdin stdout
    file in stdout
*/

int main(int argc, char *argv[ ])
{
    char c, buffer[256], r = '\r', n = '\n', tty[64], X = 'X', *cp;
    int fd, i = 0, ln, patternLen = strlen(argv[1]);
    

    // read from stdin
    if (argc == 2)
    {
        fd = 0;
    }
    // read from file
    else
    {
        fd = open(argv[2], O_RDONLY);
    }
    
    int outFile = isFile(1);
    int redirected = isRedirection();
    
    if (fd < 0)
    {
        print2f("file not found.\n");
        return -1;
    }
    
    // read the input
    if(fd) // prints file contents
    {
        ln = read(fd, &c, 1);

        while (ln)
        {
            buffer[i++] = c;
            if(c == '\n')
            {
                if(outFile == 0)
                    buffer[i++] = '\r';
                buffer[i] = '\0';
                cp = buffer;
                for(int j = 0; j < i; j++)
                {
                    if(strncmp(cp, argv[1], patternLen) == 0)
                    {
                        write(1, buffer, (int)strlen(buffer));
                        break;
                    }
                    cp++;
                }
                i = 0;
            }
            ln = read(fd, &c, 1);
        }
    }
    else
    {
        fd = 0;
        while(read(fd, &c, 1) > 0)
        {
            if(redirected)
            {
                if (c == '\n')
                {
                    buffer[i++] = '\n';
                    if(outFile == 0)
                        buffer[i++] = '\r';
                    buffer[i] = '\0';
                    cp = buffer;
                    for(int j = 0; j < i; j++)
                    {
                        if(strncmp(cp, argv[1], patternLen) == 0)
                        {
                            write(1, buffer, (int)strlen(buffer));
                            break;
                        }
                        cp++;
                    }
                    i = 0;

                }
                else if (c == '\r' && outFile == 0) //doesnt pass on '\r' values
                    continue;
                else                // write all other characters
                {
                    buffer[i++] = c;
                    //write(1, &c, 1);
                    //write(1, &X, 1);
                }
            }
            else // print the line on \r
            {
                if(c == '\r')
                {
                    buffer[i++] = '\n';
                    if(outFile)
                    {
                        write(2, &n, 1);
                        write(2, &r, 1);
                    }
                    else
                    {
                        write(2, &n, 1);
                        write(2, &r, 1);
                        buffer[i++] = '\r';    
                    }
                    buffer[i] = '\0';
                    cp = buffer;
                    for(int j = 0; j < i; j++)
                    {
                        if(strncmp(cp, argv[1], patternLen) == 0)
                        {
                            print2f("stndinmatch!\n");
                            write(1, buffer, (int)strlen(buffer));
                            break;
                        }
                        cp++;
                    }
                    i = 0;
                }
                else // echo back what the user types
                {
                    buffer[i++] = c;
                    if (outFile == 1)
                    {
                        write(2, &c, 1);
                        if(c == '\n')
                        {
                            write(2, &r, 1);
                        }
                    }
                    else
                    {
                        write(1, &c, 1);   
                        if(c == '\n')
                        {
                            write(1, &r, 1);
                        }
                    }
                }
            }
        }
    }    
    close(fd);
}
