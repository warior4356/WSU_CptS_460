int color;

#include "type.h"
#include "string.c"
// #include "queue.c"  // use provided queue.obj  during linking
// #include "kbd.c"    // use provided kbd.obj    during linking
#include "vid.c"
#include "exceptions.c"
#include "kernel.c"
#include "wait.c"
#include "uart.c"
#include "pipe.c" // pipe implementation


PIPE *kpipe;

void copy_vectors(void) {
    extern u32 vectors_start;
    extern u32 vectors_end;
    u32 *vectors_src = &vectors_start;
    u32 *vectors_dst = (u32 *)0;

    while(vectors_src < &vectors_end)
       *vectors_dst++ = *vectors_src++;
}
int kprintf(char *fmt, ...);

void IRQ_handler()
{
    int vicstatus, sicstatus;
    int ustatus, kstatus;

    // read VIC status register to find out which interrupt
    vicstatus = VIC_STATUS; // VIC_STATUS=0x10140000=status reg
    sicstatus = SIC_STATUS;  
    if (vicstatus & 0x80000000){
       if (sicstatus & 0x08){
          kbd_handler();
       }
    }
}

int body();

int pipe_writer() // pipe writer task code
{
  color=GREEN;
  struct uart *up = &uart[0];
  char line[128];
  int n;
  while(1){
    uprintf(up, "Enter a line for task1 to get : ");
    printf("task%d waits for line from UART0\n", running->pid);
    tswitch(); // you little bastard took me 3 hours to find
    ugets(up, line);
    uprints(up, "\r\n");
    printf("task%d writes line=[%s] to pipe\n", running->pid, line);
    n = write_pipe(kpipe, line, strlen(line));
    color=GREEN;
    printf("task%d wrote %d bytes to pipe\n", running->pid, n);
  }
}

int pipe_reader() // pipe reader task code
{
  color=CYAN;
  char line[128];
  int i, n;
  while(1){
    printf("task%d reading from pipe\n", running->pid);
    n = read_pipe(kpipe, line, 20);
    color=CYAN;
    printf("task%d read n=%d bytes from pipe : [", running->pid, n);
    for (i=0; i<n; i++)
    kputc(line[i]);
    printf("]\n");
  }
}

int main()
{
  fbuf_init();
  color = WHITE;
  kprintf("Welcome to Wanix in ARM\n");
  uart_init();
  kbd_init();
  /* enable timer0,1, uart0,1 SIC interrupts */
  VIC_INTENABLE |= (1<<4);  // timer0,1 at bit4 
  VIC_INTENABLE |= (1<<5);  // timer2,3 at bit5 
  VIC_INTENABLE |= (1<<31); // SIC to VIC's IRQ31

  /* enable KBD IRQ */
  SIC_ENSET = 1<<3;  // KBD int=3 on SIC
  SIC_PICENSET = 1<<3;  // KBD int=3 on SIC

  pipe_init(); // initialize PIPEs
  kpipe = create_pipe(); // create global kpipe
  init();
  kprintf("P0 kfork tasks\n");
  kfork((int)pipe_writer, 1); // pipe writer process
  kfork((int)pipe_reader, 1); // pipe reader process
  while(1){
    if (readyQueue)
    tswitch();
  }
}
