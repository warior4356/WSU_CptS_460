#define NPIPE 4
#define PSIZE  8

typedef struct pipe{
	char buf[PSIZE]; // circular data buffer
	int head, tail; // circular buf index
	int data, room; // number of data & room in pipe
	int status; // FREE or BUSY
}PIPE;

PIPE pipe[NPIPE]; // global PIPE objects

int pipe_init()
{
  PIPE *p;
  int i;
  for (i=0; i<NPIPE; i++)
  {
    p = &pipe[i];
    p->head = 0;
    p->tail = 0;
    p->data = 0;
    p->room = 8;
    p->status = FREE;
  }
}

PIPE* create_pipe()
{
  PIPE *p;
  int i;
  for (i=0; i<NPIPE; i++)
  {
    p = &pipe[i];
    if (p->status == FREE)
    {
      p->status = BUSY;
      return p;
    }
  }
  printf("No free pipes!\n");
}

int read_pipe(PIPE *p, char *buf, int n)
{ 
	color = CYAN;
	int r = 0;
	if (n<=0)
		return 0;
	//validate PIPE pointer p; // p->status must not be FREE
	//printf("read n = %d read p->data = %d\n", n, p->data);
	while(n){
	  color = CYAN;
		while(p->data){
			*buf++ = p->buf[p->tail++]; // read a byte to buf
			p->tail %= PSIZE;
			p->data--; p->room++; r++; n--;
			if (n==0)
			break;
		}
		//printf("read r = %d\n", r);
		kwakeup(&p->room); // wakeup writers
		if (r) // if has read some data
			return r;
		// pipe has no data
		ksleep(&p->data); // sleep for data
	}
}

/*---------- Algorithm of write_pipe -----------*/
int write_pipe(PIPE *p, char *buf, int n)
{ 
  color = GREEN;
  int r = 0;
	if (n<=0)
	  return 0;
	//validate PIPE pointer p; // p->status must not be FREE
	//printf("write n = %d write p->room = %d\n", n, p->room);
	while(n)
	{
	  color = GREEN;
	  while(p->room)
	  {
	    p->buf[p->head++] = *buf++; // write a byte to pipe;
	    p->head %= PSIZE;
	    p->data++; p->room--; r++; n--;
	    if (n==0)
	    break;
    }
    //printf("write r = %d\n", r);
    kwakeup(&p->data); // wakeup readers, if any.
    if (n==0)
      return r; // finished writing n bytes
    // still has data to write but pipe has no room
    ksleep(&p->room); // sleep for room
  }
}
