int ksleep(int event)
{
  int sr = int_off();
  printf("proc %d going to sleep on event=%x\n", running->pid, event);
  running->event = event;
  running->status = SLEEP;
  enqueue(&sleepList, running);
  proc[running->ppid].status = READY;
  tswitch();
  int_on(sr);
}

int kwakeup(int event)
{
  PROC *p, *temp=0;
  int sr = int_off();
  while((p = dequeue(&sleepList))!=0)
  {
    if (p->event==event)
    {
      p->status = READY;
      enqueue(&readyQueue, p);
      printf("proc %d waking up proc %d", running->pid, p->pid);
    }
    else
    {
      enqueue(&temp, p);
    }
  }
  sleepList = temp;
  //reschedule(); // if wake up any higher priority procs
  int_on(sr);
}

      
int kexit(int exitCode)
{
  int i;
  if (running->pid == proc[1].pid)
  {
    printf("P1 never dies!\n");
    return;
  }
  for (i = 2; i < NPROC; i++)
  {
    // check if child of running proc
    if (proc[i].ppid == running->pid)
    {
      proc[i].ppid = proc[1].pid; // give it to p1
    }
  }

  running->exitCode = exitCode;
  running->status = ZOMBIE;

  kwakeup((int)running->ppid);
  tswitch();
}

int kwait(int *status)
{
  int i;
  PROC *p;
  for (i = 0; i < NPROC; i++) // check for children
  {
    if (proc[i].ppid == running->pid && proc[i].status != FREE && proc[i].status != ZOMBIE)
      break;
  }
  if (i == NPROC)
	{
	  // if no children, stop
		printf("wait error: no child\n");
		return -1;
	}

	while (1)
	{
	  // find a zombie child!
		for (i = 0; i < NPROC; i++)
		{
      // if a zombie child exists, stop waiting
			if (proc[i].status == ZOMBIE && proc[i].ppid == running->pid)
			{
				*status = proc[i].exitCode;
				proc[i].status = FREE;
				proc[i].priority = 0;
				enqueue(&freeList, &proc[i]);
				return proc[i].pid;
			}
		}
		// sleep!
		ksleep((int)running->pid);
	}
}


