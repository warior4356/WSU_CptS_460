#include "ucode.c"

char *tty;
char username[64], password[64];

char *passwdTokens[7];

void getPasswdTokens(char * passwdLine)
{
    for(int i = 0; i < 7; i++)
    {
        passwdTokens[i] = passwdLine;
        while(*passwdLine != ':' && *passwdLine != '\0')
	{
            ++passwdLine;
	}
        *passwdLine = '\0';
        passwdLine++;
        //prints(passwdTokens[i]);
    }
}

unsigned int hashing(unsigned char *str) // djb2 hashing function
{
    unsigned int hash = 5381;
    int c;

    while (c = *str++)
        hash = ((hash << 5) + hash) + c; /* hash * 33 + c */

    return hash;
}

main(int argc, char *argv[])   // invoked by exec("login /dev/ttyxx")
{ 
    tty =  argv[1];

    // close 0, 1, 2 in case INIT opened them before
    close(0); close(1); close(2);

    // open tty (passed in by INIT) as stdin, stdout, stderr
    int stdin  = open(tty, O_RDONLY);
    int stdout = open(tty, O_WRONLY);
    int stderr = open(tty, O_WRONLY);

    settty(tty);   // store tty string in PROC.tty[] for putc()

    // NOW we can use printf, which calls putc() to our tty
    printf("KCLOGIN : open %s as stdin, stdout, stderr\n", tty);

    signal(2,1);  // syscall(51, 2, 1); ignore Control-C interrupts so that 
                // Control-C KILLs other procs on this tty but not the main sh

    while(1)
    {       
        // 1. show login:           to stdout
        printf("login: ");
        // 2. read user name        from stdin
        gets(username);
        // 3. show passwd:
        printf("passwd: ");
        // 4. read user passwd
	    close(1);
        gets(password);
        stdout = open(tty, O_WRONLY);
        // 5. verify user name and passwd from /etc/passwd file
        int passwd_file = open("/etc/passwd", O_RDONLY);
        
        char passwdLine[256];
        
        unsigned int hash = hashing(password);
        //printf("hash = %u\n", hash);
        
        while(getfline(passwdLine, passwd_file))
        {  
            getPasswdTokens(passwdLine);
            
            /*6. if (user account valid){
                setuid to user uid.
                chdir to user HOME directory.
                exec to the program in users's account
            }*/
	        //printf(passwdTokens[0]);
	    
            if(strcmp(passwdTokens[0], username) == 0 && atoi(passwdTokens[1]) == hash)
            {
                chuid(atoi(passwdTokens[3]), atoi(passwdTokens[2]));
                chdir(passwdTokens[5]);
		        close(passwd_file);
		        //printf(passwdTokens[5]);
                exec("sh");
            }
        }

        printf("login failed, try again\n");
   }
}
