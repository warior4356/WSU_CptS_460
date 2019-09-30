/*******************************************************
*                  @t.c file                          *
*******************************************************/

typedef unsigned char  u8;
typedef unsigned short u16;
typedef unsigned long  u32;

#define TRK 18
#define CYL 36
#define BLK 1024

#include "ext2.h"
typedef struct ext2_group_desc  GD;
typedef struct ext2_inode       INODE;
typedef struct ext2_dir_entry_2 DIR;


char *cp, buf1[BLK], buf2[BLK], temp[64], file[64];
GD    *gp;
INODE *ip;
DIR *dp;
u16  i, iblk;
u32 *iblk2;

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

//u16 NSEC = 2;

u16 getblk(u16 blk, char *buf)
{
    readfd( (2*blk)/CYL, ( (2*blk)%CYL)/TRK, ((2*blk)%CYL)%TRK, buf);
}

INODE *search(char *name, INODE *ip, u16 iblk) // finds INODE with given name
{	
	for (i = 0; i < 12; i++)
	{
		if (ip->i_block[i] == 0) break;
		getblk((u16)ip->i_block[i], buf1);
		dp = (DIR *)buf1;
		cp = buf1;
		while (cp < buf1 + BLK)
		{
			strncpy(temp, dp->name, dp->name_len);
			temp[dp->name_len] = 0;
			if (!strcmp(name, temp))
			{
				i = dp->inode - 1;
				getblk((u16)((i / 8) + iblk), buf1);
				return (INODE *)buf1 + (i % 8);
			}
			cp += dp->rec_len;
			dp = (DIR*)cp;
		}
	}
}

main()
{ 	
	getblk((u16)2, buf1);
	gp = (GD *)buf1;
	iblk = (u16)gp->bg_inode_table;
	getblk(iblk, buf1); // read first inode
	ip = (INODE *)buf1 + 1; // point ip at root
	
	prints("Boot: "); // ask what the user wants
	gets(file); // get answer
	
	// search for boot, then in boot
	ip = search("boot", ip, iblk);
	if (ip == -1) error();
	ip = search(file, ip, iblk);
	if (ip == -1) error();

	// get indirect before moving ES	
	getblk((u16)ip->i_block[12], buf2);
	iblk2 = (u32*)buf2;

	setes(0x1000); // set segment 0x1000 up...
	
	// get direct blocks
	for (i = 0; i < 12; i++){
		getblk((u16)ip->i_block[i], 0);
		inces();
	}

	// get indirect blocks
	while (*iblk2 != 0)
	{	
		getblk((u16)*iblk2++, 0);
		inces();
	}
	prints("\n\rgo?"); getc();
}  
