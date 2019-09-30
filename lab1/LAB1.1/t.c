/*********  t.c file *********************/

int prints(char *s)
{
	// print one char at a time until null	
	while (*s) putc(*s++);
}

int gets(char *s)
{
	// get one char at a time till new line
	while ((*s = getc()) != '\r') putc(*s++);
	// add a null char
	*s = 0;
}

char ans[64];

main()
{
  while(1){
    prints("What's your name? ");
    gets(ans);  prints("\n\r");

    if (ans[0]==0){
      prints("return to assembly and hang\n\r");
      return;
    }
    prints("Welcome "); prints(ans); prints("\n\r");
  }
}
  
