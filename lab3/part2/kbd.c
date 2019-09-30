#include "keymap2"

#define KCNTL 0x00
#define KSTAT 0x04
#define KDATA 0x08
#define KCLK  0x0C
#define KISTA 0x10

#define CAPSLOCK         0x58
#define LSHIFT           0x12
#define RSHIFT           0x59
#define CONTROL          0x14

typedef volatile struct kbd{
  char *base;
  char buf[128];
  int head, tail, data, room;
}KBD;

volatile KBD kbd;

// state variables
int shift;		// left and right shift key state 
int control;		// control key state 
int capslock;		// caps lock key state 
int release;            // key release

int kbd_init()
{
  KBD *kp = &kbd;
  kp->base = (char *)0x10006000;
  *(kp->base + KCNTL) = 0x10; // bit4=Enable bit0=INT on
  *(kp->base + KCLK)  = 8;
  kp->head = kp->tail = 0;
  kp->data = 0; kp->room = 128;

  shift   = 0;
  control = 0;
  capslock = 0;
  release = 0;
}

void kbd_handler()
{
  unsigned char code, c;
  int i;
  KBD *kp = &kbd;

  code = *(kp->base + KDATA);

  if (code & 0x80) // key release: ONLY catch release of shift, control
    {
    release = 1;
    return;
    }
	if(release) 
	{
		if (code == LSHIFT || code == RSHIFT)
			{
			shift = 0;    // released the shift key
			//printf("unshift\n");
			}
		if (code == CONTROL)
			{
			//printf("uncontrol\n");
			control = 0;
			}
		release = 0;
		return;
	}

  // from here on, must be key press 
  if (code == LSHIFT || code == RSHIFT){
    shift = 1;
    //printf("shift\n");
    return;
  }
  if (code == CONTROL){
    control = 1;
    //printf("control\n");
    return;
  }

  if (code == CAPSLOCK){
    capslock = !capslock;
    return;
  }

  c = (shift ? utab[code] : ltab[code]);

	if (capslock){
		if (c >= 'A' && c <= 'Z')
			c += 'a' - 'A';
		else if (c >= 'a' && c <= 'z')
			c -= 'a' - 'A';
	}

	if (control && (c=='c'||c=='C')){// Control-C on PC are 2 keys
	//Control-C Key; send SIGINT(2) signal to processes on console;
		c = '\n'; // force a line, let procs handle SIG#2 when exit Kmode
	}

	if (control && (c=='d'|| c=='D')){ // Control-D, these are 2 keys
		printf("Control-D: set code=4 to let process handle EOF\n");
		c = 4; // Control-D
	}

  // Store char in buf[] for task to get 
	if (c != '\r')
	printf("%c", c);
	//printf("kbd interrupt: c=%x %c\n", c, c);

  if (kp->data == 128)  // buf FULL, ignore current key
    return;

  kp->buf[kp->head++] = c;
  kp->head %= 128;

  kp->data++;
}

int kgetc()
{
  char c;
  KBD *kp = &kbd;

  while(kp->data == 0);

  lock();
    c = kp->buf[kp->tail++];
    kp->tail %= 128;
    kp->data--; kp->room++;
  unlock();
  return c;
}

int kgets(char s[ ]) // get a string from KBD
{
	char c;
	while((c=kgetc()) != '\r'){
	*s++ = c;
	}
	*s = 0;
	return strlen(s);
}
