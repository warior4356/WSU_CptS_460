#define TLOAD 0x0
#define TVALUE 0x1
#define TCNTL 0x2
#define TINTCLR 0x3
#define TRIS 0x4
#define TMIS 0x5
#define TBGLOAD 0x6
typedef volatile struct timer{
u32 *base; // timer's base address; as u32 pointer
int tick, hh, mm, ss; // per timer data area
char clock[16];
}TIMER;

typedef struct tq{
  struct tq *next; // next element pointer
  int time; // requested time
  PROC *proc; // pointer to PROC
}TQE;
TQE *tq, tqe[NPROC]; // tq = timer queue pointer

volatile TIMER timer[4]; //4 timers; 2 per unit; at 0x00 and 0x20
void timer_init()
{
int i; TIMER *tp;
TQE *t;
printf("timer_init()\n");
i = 0;
tp = &timer[i];
if (i==0) tp->base = (u32 *)0x101E2000;
*(tp->base+TLOAD) = 0x0; // reset
*(tp->base+TVALUE)= 0xFFFFFFFF;
*(tp->base+TRIS) = 0x0;
*(tp->base+TMIS) = 0x0;
*(tp->base+TLOAD) = 0x100;
// CntlReg=011-0010=|En|Pe|IntE|-|scal=01|32bit|0=wrap|=0x66
*(tp->base+TCNTL) = 0x66;
*(tp->base+TBGLOAD) = 0x1C00 / 16; // timer counter value
tp->tick = tp->hh = tp->mm = tp->ss = 0; // initialize wall clock
strcpy((char *)tp->clock, "00:00:00");
tq->next = 0;
for (i=0; i<NPROC; i++){
  t = &tqe[i];
  t->next = 0;
  t->time = 0;
  t->proc = 0;
}
}
void timer_handler(int n) {
int i;
TIMER *t = &timer[n];
t->tick++; // Assume 120 ticks per second
if (t->tick==120){
t->tick = 0; t->ss++;
tick_tqe();
if (t->ss == 60){
t->ss = 0; t->mm++;
if (t->mm == 60){
t->mm = 0; t->hh++; // no 24 hour roll around
}
}
t->clock[7]='0'+(t->ss%10); t->clock[6]='0'+(t->ss/10);
t->clock[4]='0'+(t->mm%10); t->clock[3]='0'+(t->mm/10);
t->clock[1]='0'+(t->hh%10); t->clock[0]='0'+(t->hh/10);
}
color = n; // display in different color
for (i=0; i<8; i++){
//row = n;
//col = 70+i;
erasechar();
kpchar(t->clock[i], n, 70+i); // to line n of LCD
}
timer_clearInterrupt(n); // clear timer interrupt
}
void timer_start(int n) // timer_start(0), 1, etc.
{
TIMER *tp = &timer[n];
kprintf("timer_start %d base=%x\n", n, tp->base);
*(tp->base+TCNTL) |= 0x80; // set enable bit 7
}
int timer_clearInterrupt(int n) // timer_start(0), 1, etc.
{
TIMER *tp = &timer[n];
*(tp->base+TINTCLR) = 0xFFFFFFFF;
}
void timer_stop(int n) // stop a timer
{
TIMER *tp = &timer[n];
*(tp->base+TCNTL) &= 0x7F; // clear enable bit 7
}


int emptyfunction(){}

int tick_tqe()
{
  TQE *top = tq->next;
  top->time--;
  if (top && top->time <= 0)
  {
    printf("\nkwakeup %d\n\n", top->proc->pid);
    kwakeup(proc);
    top->proc = 0;
    tq->next = top->next;
  }
  print_tqe();
}

int print_tqe()
{
  // print child list of running PROC
  TQE *t = tq->next;
  printf("timerQueue= ");
  while(t)
  {
    printf("[ %d %d ]->", t->proc->pid, t->time);
    t = t->next;
  }
  printf("NULL\n");
}

int itimer(int t)
{
  //(1). Fill in Tformation, e.g. proc pointer, action.QE[pid] in
  int i = 0;
  TQE *newtqe, *tqueue = tq;
  
  for (i=0; i<NPROC; i++){
    newtqe = &tqe[i];
    if(newtqe->proc == 0) break;  
  }
  if(newtqe->proc != 0)
  {
    printf("No free TQEs!\n");
    return -1;
  }
  newtqe->time = t;
  newtqe->proc = kfork((int)emptyfunction, 1);
  lock(); // mask out interrupts
  //(3). traverse timer queue to compute the position to insert TQE;
  while(tqueue->next != 0 && tqueue->next->time < newtqe->time)
  {
    newtqe->time = newtqe->time - tqueue->next->time;
    tqueue = tqueue->next;
  }
  //(4). insert the TQE and update the time of next TQE;
  newtqe->next = tqueue->next;
  tqueue->next =  newtqe;
  if(newtqe->next != 0)
  {
    newtqe->next->time = newtqe->next->time - newtqe->time;
  }  

  unlock(); // unmask interrupts
}
