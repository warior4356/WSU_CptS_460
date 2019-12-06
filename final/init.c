#include "ucode.c"
int console;
int serial1;
int serial2; 
int parent() // P1's code
{
	int pid, status;
	while(1)
	{
		printf("INIT : wait for ZOMBIE child\n");
		pid = wait(&status);
		if (pid==console)
		{ // if console login process died
			printf("INIT: forks a new console login\n");
			console = fork(); // fork another one
			if (console)
				continue;
			else
				exec("login /dev/tty0"); // new console login process
		}
		else if (pid==serial1)
		{ // if console login process died
			printf("INIT: forks a new console login\n");
			serial1 = fork(); // fork another one
			if (serial1)
				continue;
			else
				exec("login /dev/ttyS0"); // new console login process
		}
		else if (pid==serial2)
		{ // if console login process died
			printf("INIT: forks a new console login\n");
			serial2 = fork(); // fork another one
			if (serial2)
				continue;
			else
				exec("login /dev/ttyS1"); // new console login process
		}
		printf("INIT: I just buried an orphan child proc %d\n", pid);
	}
}
main()
{
	int in, out; // file descriptors for terminal I/O
	in = open("/dev/tty0", O_RDONLY); // file descriptor 0
	out = open("/dev/tty0", O_WRONLY); // for display to console
	printf("INIT : fork a login proc on console\n");
	console = fork();
	if (console) // parent
	{
		serial1 = fork();
		if (serial1) // parent
		{
			serial2 = fork();
			if (serial2) // parent
				parent();
			else
				exec("login /dev/ttyS1");
		}
		else
			exec("login /dev/ttyS0");
	}
	else // child: exec to login on tty0
		exec("login /dev/tty0");
}
