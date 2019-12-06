#include "ucode.c" // user commands and syscall interface
char cmdline[256];

char *passwdTokens[7];
char *newcmd[64];
char username[64];
int current_arg;

void run_commands();

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

void getUsername()
{
    int uid;
    uid = getuid();
    
    int passwd_file = open("/etc/passwd", O_RDONLY);
        
    char passwdLine[256];
    
    while(getfline(passwdLine, passwd_file))
    {  
        getPasswdTokens(passwdLine);
        //printf("passwdTokens[2] = %d\n", atoi(passwdTokens[2]));
        if(atoi(passwdTokens[2]) == uid)
        {
			strcpy(username, passwdTokens[0]);
			close(passwd_file);
			return;
        }
    }
}

void execCmd()
{
	//print2f("inside execCmd\n");
	
	//printf("cmd = %s\n", cmd);
	//exec(cmd);
	
    int j = 0;
    char combined[256] = "";
    char filename[256] = "";
   
    //I/O redirections!!!!!!!!!
	//printf("newcmd[j] = \"%s\"\n", newcmd[j]);
	while(newcmd[j] != 0)
	{
		if(strcmp(newcmd[j], ">") == 0)
		{
			strcpy(filename, newcmd[j+1]);
			//printf("filename = \"%s\"\n", filename);
			close(1);
			open(filename, O_WRONLY|O_CREAT);
			newcmd[j] = 0;
		}
		else if(strcmp(newcmd[j], ">>") == 0)
		{
			strcpy(filename, newcmd[j+1]);
			close(1);
			open(filename, O_WRONLY|O_CREAT|O_APPEND);
			newcmd[j] = 0;
		}
		else if(strcmp(newcmd[j], "<") == 0)
		{
			strcpy(filename, newcmd[j+1]);
			close(0);
			open(filename, O_RDONLY);
			newcmd[j] = 0;
		}
		j++;
	}
	j = 0;
	while(newcmd[j] != 0)
	{
		//printf("cmds[j] = \"%s\"\n", newcmd[j]);
		strcat(combined, newcmd[j]);
		if(newcmd[j+1] != 0)
		{
			strcat(combined, " ");
		}
		//printf("newcmd = \"%s\"\n", combined);
		j++;
	}
    //write(2, combined, (int)strlen(combined));
    exec(combined);
}

void execPipe()
{
	//print2f("inside pipe\n");
	int pipefd[2], pid, status;
	//char pcmdline[256];
	
	/**pcmdline = cmdline;
    while(*cmdline != '|' && *cmdline != '\0')
	{
		++cmdline;
	}
    *cmdline = '\0';
    cmdline++;*/
	
	pipe(pipefd);
	pid = fork();
	
	if (pid) // parent sh waits for child to die
	{
		//pid = wait(&status);
		close(pipefd[1]);
		//close(0);
		dup2(pipefd[0], 0);
		//print2f("parrent\n");
		run_commands();
	}	
	else // child exec cmdline
	{
		close(pipefd[0]);
        //close(1);
        dup2(pipefd[1], 1);
        //close(pd[1]);
		execCmd();
	}
}

void run_commands()
{
	//printf("current arg = %d, argc = %d\n", current_arg, argc);
	int i = 0;
	for(; current_arg < argc; current_arg++)
	{
		if(strcmp(argv[current_arg],"|") == 0)
		{
			newcmd[i] = 0;
			current_arg++;
			execPipe(newcmd);
			return;
		}
		newcmd[i] = argv[current_arg];
		//strcpy(newcmd[i], argv[current_arg]);
		//strcat(newcmd, argv[current_arg]);
		//if(current_arg + 1 < argc)
		//	strcat(newcmd, " ");
		//printf("current arg = %d, argc = %d\n", current_arg, argc);
		//print2f(newcmd[i]);
		//print2f(argv[current_arg]);
		//printf("newcmd[i] = %s\n", newcmd[i]);
		i++;
	}
	newcmd[i] = 0;
	execCmd(newcmd);
	return;
}

main(int argc, char *argv[ ])
{
	//print2f("inside shell\n");
	print2f("Check man for commands\n\r");
	int pid, status;
	char cwd[64];
	getUsername();
	current_arg = 0;
	while(1){
		// display executable commands in /bin directory
		// print local info on each line
		getcwd(cwd);
		printf("%s@wanix:%s$ ", username, cwd);
		//prompt for a command line cmdline = "cmd a1 a2 …. an"
		gets(cmdline);
		if(strlen(cmdline) == 0)
		    continue;
		token(cmdline);
		
		if (!strcmp(cmdline,"logout") || !strcmp(cmdline,"exit"))
			exit(0);
		
		if(argc > 0)
		{
			// fork a child process to execute the cmd line
			pid = fork();
			if (pid) // parent sh waits for child to die
				pid = wait(&status);
				
			else // child exec cmdline
			{
				run_commands();
			}	
				//exec(cmdline); // exec("cmd a1 a2 … an");
		}
	}
}

