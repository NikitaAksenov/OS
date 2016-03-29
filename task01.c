#include <errno.h>
#include <sys/wait.h>
#include <dirent.h>
#include <stdio.h>
#include <fcntl.h>
#include <string.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>

/*Running programm: ./OS_prog "m" "dir1" "dir2", where "m" - amount of processes
							"dir1" - directory to copy from
							"dir2" - destination directory */

int main(int argc, char *argv[])
{
	pid_t p;
	int m = atoi(argv[1]);
	
	if (argc != 4)
		error("Not enough arguments");
	
	for (int i = 0; i<m; i=i+1)	//cycle for creating child processes
	{
		p = fork();
		if(p == 0)
		{
			CopyDirectory(argv[2], argv[3]);
			exit(0);
		}
	}
	for (int i = 0; i < m; i=i+1)
		wait(0);
	return 0;
}

void CopyDirectory(char *directory1, char *directory2)
{
	char dir1Name[256], dir2Name[256], tmp1[256], tmp2[256];
	int count = 0;
	strcpy(dir1Name, directory1);
	strcpy(dir2Name, directory2);
	strcat(dir1Name, "/");
	strcat(dir2Name, "/");

	DIR *dir1;
	DIR *dir2;
	struct dirent *entry;
	
	dir1 = opendir(directory1);
	mkdir(directory2, 0777);
	dir2 = opendir(directory2);

	while((entry = readdir(dir1)) != NULL)
	{
		if((strcmp(entry->d_name, ".")!=0)&&(strcmp(entry->d_name, "..")!=0))	//avoid loop
		{
			strcpy(tmp1, dir1Name);
			strcpy(tmp2, dir2Name);
			strcat(tmp1, entry->d_name);
			strcat(tmp2, entry->d_name);
			
			if(access(tmp2, 0) == -1)
			{
				if(entry->d_type == DT_DIR)
				{
					CopyDirectory(tmp1, tmp2);
					count = count + 1;
				}
				else
					CopyFile(tmp1, tmp2);
			}
		}
	}
	printf("PID: %d; files copied: %d\n", getpid(), count);
}

void CopyFile(char *file1, char *file2)
{
	int f1, f2, n;
	char buf[BUFSIZ];

	if ((f1 = open(file1, O_RDONLY, 0)) == -1)
		error("Cannot oper file %s", file1);
	if ((f2 = creat(file2, 0666)) == -1)
		error("Cannot create file %s", file2);
	while ((n = read(f1, buf, BUFSIZ)) > 0)
	{
		if (write(f2, buf, n) != n)
			error("Error during writing in file %s", file2);
	}
}
