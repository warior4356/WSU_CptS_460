#define NPROC 9
PROC proc[NPROC], *running, *freeList, *readyQueue, *sleepList;
int procsize = sizeof(PROC);

int body();

int init()
{
  int i, j; 
  PROC *p;
  kprintf("kernel_init()\n");
  for (i=0; i<NPROC; i++){
    p = &proc[i];
    p->pid = i;
    p->status = READY;
    p->next = p + 1;
  }
  proc[NPROC-1].next = 0; // circular proc list
  freeList = &proc[0];
  readyQueue = 0;
  sleepList = 0;
  
  printList("freeList", freeList);
  
  // creat P0 as initial running process
  p = (PROC *)dequeue(&freeList);
  p->priority = 0;
  p->ppid = 0;
  p->parent = p;
  p->child = 0;
  p->sibling = 0;
  running = p;

  kprintf("running = %d\n", running->pid);
}

int enter_child(PROC *child, PROC *parent)
{
  PROC *temp;
  if(child){
    child->parent = parent;
    if(running->child)
    {
      temp = parent->child;
      while(temp->sibling)
      {
        temp = temp->sibling;
      }
      temp->sibling = child;
    }
    else{
      parent->child = child;
    }
  }
}

char *status[ ] = {"FREE", "READY", "SLEEP", "BLOCK", "ZOMABIE"};  

int sendChild()
{
	PROC *p = &proc[1];
  enter_child(running->child, p);
}

int printChild()
{
  // print child list of running PROC
  PROC *p = running;
  printf("childList= ");
  if(p->child)
  {
    printf("[ %d %d ]->", p->child->pid, p->child->status);
    p = p->child;
  }
  while(p)
  {
    if(p->sibling == 0)
    {
      printf("NULL\n");
    }
    else
    {
      printf("[ %d %d ]->", p->sibling->pid, p->sibling->status);
    }
    p = p->sibling;
  }

  
}

int delete_child(PROC *p)
{
  // delete child p from running's childList
}
  
PROC *kfork(int func, int priority)
{
  int i;
  PROC *p = (PROC *)dequeue(&freeList);
  enter_child(p, running);
  if (!p)
  {
     printf("no more PROC, kfork failed\n");
     return(0);
  }  

  p->status = READY;
  p->priority = priority;
  p->ppid = running->pid;
  
  // set kstack to resume to body
  // stack = r0,r1,r2,r3,r4,r5,r6,r7,r8,r9,r10,r11,r12,r14
  //         1  2  3  4  5  6  7  8  9  10 11  12  13  14
  for (i=1; i<15; i++)
      p->kstack[SSIZE-i] = 0;
  p->kstack[SSIZE-1] = (int)func;  // in dec reg=address ORDER !!!
  p->ksp = &(p->kstack[SSIZE-14]);
  enqueue(&readyQueue, p);
  printf("proc %d kforked a child %d\n", running->pid, p->pid);
  printList("readyQueue", readyQueue);
  return p;
}

int scheduler()
{
  kprintf("proc %d in scheduler ", running->pid);
  if (running->status == READY)
    enqueue(&readyQueue, running);
  running = (PROC *)dequeue(&readyQueue);
  kprintf("next running = %d\n", running->pid);
}  

int menu()
{
  printf("------- menu --------\n");
  printf("switch fork exit wait\n");
  printf("---------------------\n");
}  
    
int do_exit()
{
  char s[16];
  int exitValue;
  if (running->pid == 1){
    printf("P1 never dies\n");
    return -1;
  }
  printf("enter an exit value : ");
  kgets(s);
  exitValue = atoi(s);
  kexit(exitValue);
}
int do_wait()
{
  int pid, status;
  printf("proc %d wait for a ZOMBIE child\n", running->pid);
  pid = kwait(&status);
  printf("proc %d wait for a ZOMBIE child %d ststus=%d\n",
	 running->pid, pid, status);
}

int body()
{
  char command[64];
  int pid, status;
  kprintf("proc %d resume to body()\n", running->pid);

  while(1){

    pid = running->pid;
    if (pid==0) color=BLUE;
    if (pid==1) color=GREEN;
    if (pid==2) color=CYAN;
    if (pid==3) color=YELLOW;
    if (pid==4) color=WHITE;
    if (pid==5) color=GREEN;
    if (pid==6) color=CYAN;   
    if (pid==7) color=YELLOW;
    if (pid==8) color=WHITE;

    kprintf("proc %d running : ", running->pid);
    printChild();

    printList("freeList  ", freeList);
    printList("readyQueue", readyQueue);
    
    menu();
    printf("enter command : ");
    kgets(command);

    if (strcmp(command, "switch")==0)
       tswitch();
    if (strcmp(command, "fork")==0)
       kfork((int)body, 1);
    if (strcmp(command, "exit")==0)
       do_exit();
    if (strcmp(command, "wait")==0)
      do_wait();
  }
}
