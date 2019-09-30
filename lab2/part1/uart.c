// uart.c file

#define DR   0x00
#define FR   0x18

#define RXFE 0x10
#define TXFF 0x20

typedef struct uart{
  char *base;
  int n;
}UART;

UART uart[4];

int uart_init() // UART initialization function
{
	int i; UART *up;
	for (i=0; i<4; i++)
	{ // uart0 to uart2 are adjacent
		up = &uart[i];
		up->base = (char *)(0x101F1000 + i*0x1000);
		up->n = i;
	}
	uart[3].base = (char *)(0x10009000); // uart3 at 0x10009000
}
int ugetc(UART *up) // input a char from UART pointed by up
{
	while (*(up->base+FR) & RXFE); // loop if UFR is REFE
	return *(up->base+DR); // return a char in UDR
}
int uputc(UART *up, char c) // output a char to UART pointed by up
{
	while (*(up->base+FR) & TXFF); // loop if UFR is TXFF
	*(up->base+DR) = c; // write char to data register
}
int ugets(UART *up, char *s) // input a string of chars
{
	while ((*s = ugetc(up)) != '\r') 
	{
		uputc(up, *s);
		s++;
	}
	*s = 0;
}
int uprints(UART *up, char *s) // output a string of chars
{
	while (*s)
		uputc(up, *s++);
}


char *ctable = "0123456789ABCDEF";

int urpu(UART *up, u32 x, int BASE){
	char c;
	if (x){
		c = ctable[x % BASE];
		urpu(up, x / BASE, BASE);
		uputc(up, c);
	}
}

int uprintu(UART *up, u32 x){
	(x==0)? uputc(up, '0') : urpu(up, x, 10);
	uputc(up, ' ');
}


int uprintd(UART *up, int x){
	if(x < 0){
		uputc(up, '-');
		x = x * -1;
	}
	(x==0)? uputc(up, '0') : urpu(up, x, 10);
       	uputc(up, ' ');
}

int  uprintx(UART *up, u32 x){
	uputc(up, '0');
	uputc(up, 'x');
	(x==0)? uputc(up, '0') : urpu(up, x, 16);
       	uputc(up, ' ');
}

int ufprintf(UART *up, char *fmt, ...)
{
	char cp;
	int *ip = (int *)(&fmt);
	int i = 0;
	cp = fmt[i];
	while(cp != 0){
		if(cp == '%'){
			i++;
			ip++;
			cp = fmt[i];
			switch(cp){	
				case 'c':
					uputc(up, *ip);
					break;
				case 's':
					uprints(up, *ip);
					break;
				case 'u':
					uprintu(up, *ip);
					break;
				case 'd':
					uprintd(up, *ip);
					break;
				case 'x':
					uprintx(up, *ip);
					break;
			}
		}
		else{
			uputc(up, cp);
		}
		i++;
		cp = fmt[i];
	}
}
