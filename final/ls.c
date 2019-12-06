#include "ucode.c"

#define S_IFMT      0b000001111000000000000
#define S_IFREG     0b000001000000000000000
#define S_IFDIR     0b000000100000000000000
#define S_IFLNK     0b000001010000000000000

#define S_IRWXU     0b000000000000111000000   //owner has read, write, and execute permission
#define S_IRUSR     0b000000000000100000000   //owner has read permission
#define S_IWUSR     0b000000000000010000000   //owner has write permission
#define S_IXUSR     0b000000000000001000000   //owner has execute permission

#define S_IRWXG     0b000000000000000111000   //group has read, write, and execute permission
#define S_IRGRP     0b000000000000000100000   //group has read permission
#define S_IWGRP     0b000000000000000010000   //group has write permission
#define S_IXGRP     0b000000000000000001000   //group has execute permission

#define S_IRWXO     0b000000000000000000111   //others (not in group) have read, write, and execute permission
#define S_IROTH     0b000000000000000000100   //others have read permission
#define S_IWOTH     0b000000000000000000010   //others have write permission
#define S_IXOTH     0b000000000000000000001   //others have execute permission

char dirPrefix[256];

void ls_file(char* filename)
{
		STAT fstat, *s = &fstat;
		char filepath[256];
		strcpy(filepath, dirPrefix);
		strcat(filepath, "/");
		strcat(filepath, filename);
		//printf("filepath = %s\n", filepath);
		int r = stat(filepath, s);
		if(r != 0)
		{
			print2f("file not found!\n");
			return;
		}
		if((s->st_mode & S_IFMT) == S_IFDIR)
		    mputc('d');
		if((s->st_mode & S_IFMT) == S_IFREG)
		    mputc('-');		
		if((s->st_mode & S_IFMT) == S_IFLNK)
		    mputc('l');

		if((s->st_mode & S_IRUSR) == S_IRUSR)
		    mputc('r');
		else
		    mputc('-');
		if((s->st_mode & S_IWUSR) == S_IWUSR)
		    mputc('w');
		else
		    mputc('-');
		if((s->st_mode & S_IXUSR) == S_IXUSR)
		    mputc('x');
		else
		    mputc('-');

		if((s->st_mode & S_IRGRP) == S_IRGRP)
		    mputc('r');
		else
		    mputc('-');
		if((s->st_mode & S_IWGRP) == S_IWGRP)
		    mputc('w');
		else
		    mputc('-');
		if((s->st_mode & S_IXGRP) == S_IXGRP)
		    mputc('x');
		else
		    mputc('-');

		if((s->st_mode & S_IROTH) == S_IROTH)
		    mputc('r');
		else
		    mputc('-');
		if((s->st_mode & S_IWOTH) == S_IWOTH)
		    mputc('w');
		else
		    mputc('-');
		if((s->st_mode & S_IXOTH) == S_IXOTH)
		    mputc('x');
		else
		    mputc('-');
		
		
		printf("\t%d\t%d\t%d\t%d\t%s\n\r", s->st_nlink, s->st_uid, s->st_gid, s->st_size, filename);
}

void ls_dir(char* dirname)
{
	char buffer[1024];
	char filename[256];
	strcpy(dirPrefix, dirname);
	int filePtr = open(dirname, O_RDONLY);
	read(filePtr, buffer, 1024);
	char * charPtr = buffer;
	DIR * dirPtr = (DIR*) buffer;
	while(charPtr < 1024 + buffer)
	{
		strcpy(filename, dirPtr->name);
		filename[dirPtr->name_len] = '\0';
		//printf("filename = %s\n", filename);
		ls_file(filename);
		charPtr += dirPtr->rec_len;
		dirPtr = (DIR*)charPtr;	
	}
}

main(int argc, char *argv[])   // invoked by exec("ls")
{
    char cwd[64];
    getcwd(cwd);
	//printf("cwd = %s\n", cwd);
	

    if(argc < 2) // ls CWD
    {
        ls_dir(cwd);
    }
    
    else // ls var
    {
		//printf("argv[1] = %s\n", argv[1]);
		STAT fstat, *s = &fstat;
		stat(argv[1], s);
		if((s->st_mode & S_IFMT) == S_IFDIR)
			ls_dir(argv[1]);
		else
			ls_file(argv[1]);
    }

}
