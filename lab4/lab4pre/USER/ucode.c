int umenu()
{
  uprintf("-----------------------------------------------\n");
  uprintf("getpid getppid ps chname switch kfork exit wait\n");
  uprintf("-----------------------------------------------\n");
}

int getpid()
{
  int pid;
  pid = syscall(0,0,0,0);
  return pid;
}    

int getppid()
{ 
  return syscall(1,0,0,0);
}

int ugetpid()
{
  int pid = getpid();
  uprintf("pid = %d\n", pid);
}

int ugetppid()
{
  int ppid = getppid();
  uprintf("ppid = %d\n", ppid);
}

int ups()
{
  return syscall(2,0,0,0);
}

int uchname()
{
  char s[32];
  uprintf("input a name string : ");
  ugetline(s);
  printf("\n");
  return syscall(3,s,0,0);
}

int uswitch()
{
  return syscall(4,0,0,0);
}

int ugetc()
{
  return syscall(90,0,0,0);
}

int uputc(char c)
{
  return syscall(91,c,0,0);
}

int getPA()
{
  return syscall(92,0,0,0);
}

int ufork()
{
  char s[32];
  int child, pid;
  uprintf("syscall to kernal fork, enter a filename: ");
  ugetline(s);
  printf("\n");
  pid = getpid();
  printf("Proc %d entering kernal to fork a child\n", pid); 
  child = syscall(5,s,0,0);
  printf("Proc %d forked a child pid = %d\n", pid, child); 
}

int getint()
{
  char s[16];
  return atoi(ugetline(s));
}

int uexit()
{
  int exitCode;
  printf("enter an exit code: ");
  exitCode = geti();
  printf("\nexitvalue = %d\n", exitCode);
  printf("syscall to enter kernel to die with exit code = %d\n", exitCode);
  syscall(6,exitCode,0,0);
}

int uwait()
{
  int childPid, exitCode;
  printf("sycall proc %d to enter Kernel to wait for a child to die\n", getpid());
  childPid = syscall(7, &exitCode, 0);
  printf("proc %d back from wait syscall, dead child->pid = %d", getpid(), childPid);
  if (childPid>=0)
    printf(" exit code = %d", exitCode);
  printf("\n"); 
}
