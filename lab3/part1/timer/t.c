/********************************************************************
Copyright 2010-2017 K.C. Wang, <kwang@eecs.wsu.edu>
This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.
********************************************************************/

#include "defines.h"
#include "string.c"

char *tab = "0123456789ABCDEF";
int color;

#include "vid.c"
#include "interrupts.c"
#include "kbd.c"
#include "timer.c"

void copy_vectors(void) {
    extern u32 vectors_start;
    extern u32 vectors_end;
    u32 *vectors_src = &vectors_start;
    u32 *vectors_dst = (u32 *)0;

    while(vectors_src < &vectors_end)
       *vectors_dst++ = *vectors_src++;
}
int kprintf(char *fmt, ...);
void timer_handler();
TIMER *tp[4]; // 4 TIMER structure pointers

void IRQ_handler() // IRQ interrupt handler in C
{
	// read VIC status registers to determine interrupt source
	int vicstatus = VIC_STATUS;
	// VIC status BITs: timer0,1=4, uart0=13, uart1=14
	if (vicstatus & (1<<4)){ // bit4=1:timer0,1
		if (*(tp[0]->base+TVALUE)==0) // timer 0
		timer_handler(0);
		if (*(tp[1]->base+TVALUE)==0) // timer 1
		timer_handler(1);
		}
		if (vicstatus & (1<<5)){ // bit5=1:timer2,3
		if (*(tp[2]->base+TVALUE)==0) // timer 2
		timer_handler(2);
		if (*(tp[3]->base+TVALUE)==0) // timer 3
		timer_handler(3);
	}
}

int main()
{
	int i;
	color = RED; // int color in vid.c file
	fbuf_init(); // initialize LCD driver
	printf("main starts\n");
	/* enable VIC for timer interrupts */
	VIC_INTENABLE = 0;
	VIC_INTENABLE |= (1<<4); // timer0,1 at VIC.bit4
	VIC_INTENABLE |= (1<<5); // timer2,3 at VIC.bit5
	timer_init();
	for (i=0; i<4; i++){ // start all 4 timers
		tp[i] = &timer[i];
		timer_start(i);
	}
	printf("Enter while(1) loop, handle timer interrupts\n");
}
