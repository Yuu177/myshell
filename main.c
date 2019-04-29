//
//  main.c
//  MyShell
//
//  Created by u on 2019/4/19.
//  Copyright © 2019年 u. All rights reserved.
//

#include <stdio.h>
#include <malloc.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <dirent.h>//dirent ，为了获取某个文件夹的内容，所使用的结构体；
#include <sys/stat.h>//mkdir需要
#include <sys/utsname.h>
#include <fcntl.h>//open
#include <sys/types.h>
#include <sys/wait.h>
//#include <sys/sched.h> //test

#define MAX_LINE 100
#define MAX_PATH_NAME 100
#define MAX_DIR_NAME 100
#define MAX_JOBS 100

char* cmdArry[MAX_LINE];
int cmdCnt;//命令数从0开始
int pipe_default[2];//test pipe
int befor;//test pipe
int after;//test pipe
char original_path[MAX_PATH_NAME];//源目录
int jobs[MAX_JOBS];//test
int jobsCnt;//test
char* cmdName[MAX_LINE];//test
int tpid;//test

void welcome(void);
void get_command(void);
int deal_command(void);
void is_pipe(void);//test
void pwd(void);
void echo(void);
void ls(void);//ls or ls 参数
void cd(void);
void makedir(void);
void delete(void);
void cat(void);
void wc(void);
void login_info(void);
void clear(void);
void help(void);
void cat_redirection(void);
void echo_redirection(void);
void wc_redirection(void);//test
void echo_pipe();//test
int is_bg(void);//后台程序
void tail(void);//tail命令用途是依照要求将指定的文件的最后部分输出到标准设备
void tail_f(void);//tail -f用来实现后台程序
void quit(void);
//void ps(void);//test

void welcome()
{
	if(!getcwd(original_path, MAX_PATH_NAME))//getcwd在linux下可用
    {
        printf("get path_name error\n");
        exit(-1);
    }
	struct utsname uts;
	if(uname(&uts))
	{
		perror("uname");
		exit(1);	
	}
    printf("Welcome to %s\n",uts.version);
    printf("\n");
    printf("*************************************************************\n");
    printf("**Welcome to Tanpan Y's shell                              **\n");
    printf("**Please input \"help\" to show what commands can you use  **\n");
    printf("*************************************************************\n");
    printf(" _   _      _ _        __        __         _     _ \n");
    printf("| | | | ___| | | ___   \\ \\      / /__  _ __| | __| |\n");
    printf("| |_| |/ _ \\ | |/ _ \\   \\ \\ /\\ / / _ \\| '__| |/ _` |\n");
    printf("|  _  |  __/ | | (_) |   \\ V  V / (_) | |  | | (_| |\n");
    printf("|_| |_|\\___|_|_|\\___/     \\_/\\_/ \\___/|_|  |_|\\__,_|\n");
    printf("\n");
}

void login_info()
{
	struct utsname uts;
	char* login_name;
	char* host_name;
	login_name = getlogin();
	if(uname(&uts))
	{
		perror("uname");
		exit(1);	
	}
	host_name = uts.nodename;
	char path_name[MAX_PATH_NAME];
	if(!getcwd(path_name, MAX_PATH_NAME))//getcwd在linux下可用
    {
        printf("get path_name error\n");
        exit(-1);
    }
	int i = 0;
	char Catalog[MAX_PATH_NAME/5];//除以5只是为了分配较小的空间，Catalog为最后一个路径
	memset(Catalog, 0, MAX_PATH_NAME/5);
	i = strlen(path_name);//定位到数组的最后一个字符
	int j = 0;
	if(strcmp(path_name, original_path) != 0)
	{
		while(path_name[i] != '/')
		{
			--i;
		}
		while(path_name[i] != '\0')
		{
			Catalog[j] = path_name[i];
			++j;
			++i;
		}
	}
	//如果源目录是当前目录的子串，则用～代源目录替
	if(strstr(getcwd(path_name, MAX_PATH_NAME), original_path) != NULL)
	{
		printf("\033[1;3;31m%s@%s\033[m:\033[34m~%s\033[m$ ", login_name
		, host_name, Catalog);
	} 
	else
	{
		printf("\033[1;3;31m%s@%s\033[m:\033[34m%s\033[m$ ", login_name
		, host_name, path_name);
	}	
	return;
}

void get_command()
{
    int cnt = 0;
    char str[MAX_LINE];
    char* temp;
	memset(cmdArry, 0, sizeof(cmdArry));
    fgets(str, MAX_LINE, stdin);//fgets保存回车，gets不保存，scanf遇到空格停止
    if(str[strlen(str)-1] == '\n')
    {
        str[strlen(str)-1] = '\0';
    }
    temp = strtok(str, " ");
    while(temp != NULL)
    {
        cmdArry[cnt] = (char*)malloc(sizeof(*temp));
        strcpy(cmdArry[cnt],temp);
        ++cnt;
        temp = strtok(NULL, " ");
    }
    cmdCnt = cnt;
    return;
}

int deal_command()
{
    if(cmdArry[0] == NULL)
    {
        return 0;
    }
    else if(strcmp(cmdArry[0], "pwd") == 0)
    {
        pwd();
        return 1;
    }
    else if(strcmp(cmdArry[0], "echo") == 0)
    {
		for(int i = 1; i < cmdCnt; i++)
		{
			if(strcmp(cmdArry[i], ">") == 0 || strcmp(cmdArry[i], ">>") == 0)
			{
				echo_redirection();
				return 1;
			}
			if(strcmp(cmdArry[i], "|") == 0)//test
			{
				echo_pipe();
				return 1;
			}
		}
		echo();
		return 1;
	}
	else if(strcmp(cmdArry[0], "ls") == 0)
    {
		ls();
		return 1;
	}
	else if(strcmp(cmdArry[0], "cd") == 0)
    {
		cd();
		return 1;
	}
	else if(strcmp(cmdArry[0], "mkdir") == 0)
    {
		makedir();
		return 1;
	}
	else if(strcmp(cmdArry[0], "rm") == 0)
    {
		delete();
		return 1;
	}
	else if(strcmp(cmdArry[0], "cat") == 0)
    {
		for(int i = 1; i < cmdCnt; i++)
		{
			if(strcmp(cmdArry[i], ">") == 0 || strcmp(cmdArry[i], ">>") == 0)
			{
				cat_redirection();
				return 1;
			}
		}
		cat();
		return 1;
	}
	else if(strcmp(cmdArry[0], "wc") == 0)
    {
		wc();
		return 1;
	}
	else if(strcmp(cmdArry[0], "clear") == 0)
    {
		clear();
		return 1;
	}
	else if(strcmp(cmdArry[0], "help") == 0)
    {
		help();
		return 1;
	}
	else if(strcmp(cmdArry[0], "tail") == 0)
    {
		if(strcmp(cmdArry[1], "-f") == 0)
		{
			tail_f();
			return 1;
		}
		else
		{
			tail();
			return 1;
		}		
	}
	else if(strcmp(cmdArry[0], "exit") == 0)
	{
		quit();
		return 1;
	}
	else if(strcmp(cmdArry[0], "ps") == 0)
	{
		//ps();
		return 1;
	}
	else
	{
		printf("No has this command!\n");
		return 0;
	}
	return 0;
}

void is_pipe()//test
{
	for(int i = 1; i < cmdCnt; i++)
	{
		if(cmdArry[i] != NULL && strcmp(cmdArry[i], "|") == 0)
		{
			cmdArry[i] = NULL;
			after = i+1;
			befor = i -1;
			return;
		}
	}
	return ;
}

void pwd()
{
    char path_name[MAX_PATH_NAME];
    if(getcwd(path_name, MAX_PATH_NAME))//getcwd在linux下可用
    {
        printf("%s\n",path_name);
    }
    else
    {
        printf("error!\n");
    }
}

void echo()
{
	for(int i = 1 ; i < cmdCnt; i++)
	{
		if(strcmp(cmdArry[i], ">") == 0 || strcmp(cmdArry[i], ">>") == 0)
		{
			break;
		}
		if(strcmp(cmdArry[i], "|") == 0)//test
		{
			break;
		}
		printf("%s ",cmdArry[i]);
	}
	printf("\n");
	return;
}

//~ struct dirent
//~ {
//~ long d_ino; /* inode number 索引节点号 
//~ off_t d_off; /* offset to this dirent 在目录文件中的偏移 
//~ unsigned short d_reclen; /* length of this d_name 文件名长 
//~ unsigned char d_type; /* the type of d_name 文件类型 
//~ char d_name [NAME_MAX+1]; /* file name (null-terminated) 文件名，最长256字符
//~ }
void ls()
{
	char path_name[MAX_PATH_NAME];
	char dir_name[MAX_DIR_NAME];
    if(!getcwd(path_name, MAX_PATH_NAME))//getcwd在linux下可用
    {
        printf("getcwd error!\n");
    }
    DIR* dir;
    struct dirent* pDIR;
    struct stat dir_stat;
    if(cmdArry[1] != NULL)//带参数
    {
		if((dir = opendir(cmdArry[1])) == NULL)
		{
			printf("No such file or directory!\n");
			return;
		}
	}
	else
	{
		dir = opendir(path_name);
	}		 
    int i = 0;//用来标记每一行能输出的个数
    int j = 0;//决定是否换行
    int n = 5;//n表示一行输出多少个
    while((pDIR = readdir(dir)) != NULL)
    {
		strcpy(dir_name, pDIR->d_name);
		if(dir_name[0] == '.')
		{
			continue;
		}
		else
		{
			stat(dir_name, &dir_stat);
			if(S_ISREG(dir_stat.st_mode))//是否为文件
			{
				if(access(pDIR->d_name, X_OK) != -1)//判断文件是否可执行
				{
					printf("\033[1;32m%s\033[m\t",pDIR->d_name);
				}
				else
					printf("%s\t",pDIR->d_name);
			}
			else //if(S_ISDIR(dir_stat.st_mode))//S_ISDIR(st_mode)是否为目录
			{
				printf("\033[1;34m%s\033[m\t",pDIR->d_name);
			}	
			++i;
			++j;
			if(i == n)//每行输出5个
			{
				printf("\n");
				i = 0;//用来标记每一行能输出的个数
			}
		}		
	}
	if(j % n != 0)//如果有余数，说明没有换行
		printf("\n");
	j = 0;
    return;
}

void cd()
{
	char path_name[MAX_PATH_NAME];
	char tmp[1] = "/";
	if(cmdArry[1] == NULL)
	{
		chdir(original_path);
		return;
	}
	if(chdir(cmdArry[1]) != -1)//先直接打开文件目录
	{
		return;
	}
	if(!getcwd(path_name, MAX_PATH_NAME))//getcwd在linux下可用
    {
        printf("cd error\n");
        exit(-1);
    } 
    if(strcmp(cmdArry[1], "..") == 0)
	{
		int i = 0;
		while(path_name[i] != '\0')
		{
			++i;
		}
		while(path_name[i] != '/')
		{
			path_name[i] = '\0';
			--i;
		}
	}
	else if(strcmp(cmdArry[1], ".") == 0)
	{
		printf("\n");
	}
	else if(cmdArry[1] != NULL)
	{
		strncat(path_name, tmp, sizeof(tmp));
		strncat(path_name, cmdArry[1], 1000);
	} 
    if(chdir(path_name) == -1)
    {
		printf("cd no dir\n");
	}
    return;
}

//~ access to作为有权使用什么，即可理解access（）函数想表达有做某事的权限。
//~ 函数参数有两个，第一个为文件，那么对应的第二个参数就不难推想出为文件有那些权限和是否存在。
void makedir()
{
	if(access(cmdArry[1], 0) != 0)
	{
		if(mkdir(cmdArry[1], 0777) == -1)
		{
			printf("mkdir error\n");
			return;
		}
	}
	else
	{
		printf("the dir had exist!\n");
	}
	return;
}

//~ struct stat结构体简介
//~ 在使用这个结构体和方法时，需要引入：
//~ <sys/types.h>
//~ <sys/stat.h>
//~ struct stat这个结构体是用来描述一个linux系统文件系统中的文件属性的结构。
//~ 可以有两种方法来获取一个文件的属性：
//~ 通过路径：
//~ int stat(const char *path, struct stat *struct_stat);
//~ int lstat(const char *path,struct stat *struct_stat);
//~ 两个函数的第一个参数都是文件的路径，第二个参数是struct stat的指针。返回值为0，表示成功执行。
//~ Linux下系统api int rmdir(const char *pathname);可以删除目录。但使用rmdir函数时，
//~ 目录必须为空，否则调用失败，函数返回-1，执行成功时，函数返回0。
//~ 若要删除一个目录，必须将给目录下的文件全部删除，系统调用int remove(const char * pathname)可以删除一个普通文件；
void delete()
{
	struct stat dir_stat;
	if(access(cmdArry[1], 0) != 0)
	{
		printf("No such file or directory!\n");
		return;
	}
	stat(cmdArry[1], &dir_stat);
	if(S_ISREG(dir_stat.st_mode))
	{
		if(remove(cmdArry[1]) != 0)
		{
			printf("delete file false!\n");
			return;
		}
	}
	else
	{
		if(rmdir(cmdArry[1]) != 0)
		{
			printf("delete dir false!\n");
			return;
		}
	}
	return;
}

void cat()
{
	if(access(cmdArry[1], 0) != 0)
	{
		printf("No such file or directory!\n");
		return;
	}
	char buf[1024];
	FILE* fp;
	int len;
	if((fp = fopen(cmdArry[1], "r")) == NULL)
	{
		printf("open false!\n");
		return;
	}
	while(fgets(buf, 1024, fp) != NULL)
	{
		len = strlen(buf);
		buf[len - 1] = '\0';//去掉换行符
		printf("%s\n",buf);
	}
	fclose(fp);
	return;
}

void wc()
{
	if(access(cmdArry[1], 0) != 0)
	{
		printf("No such file or directory!\n");
		return;
	}
	char buf[1024];
	FILE* fp;
	int len;
	int length = 0;//总长度
	int RowNumber = 0;//行数
	int sizeLen = 0;//字节长度
	if((fp = fopen(cmdArry[1], "r")) == NULL)
	{
		printf("open false!\n");
		return;
	}
	while(fgets(buf, 1024, fp) != NULL)
	{
		len = strlen(buf);
		buf[len - 1] = '\0';//去掉换行符
		--len;
		length += len;
		++RowNumber;
		sizeLen += sizeof(buf);
	}
	printf("%d\t%d\t%d %s",sizeLen, length, RowNumber, cmdArry[1]);
	printf("\n");
	fclose(fp);
	return;
}

void clear()
{
	printf("\033[2J");
	printf("\033[H");
	return;
}

void help()//用户手册还不完整
{
	printf("welcome to the manual of myshell, hope it's useful for you\n");
	printf("the following are the BUILT-IN commands supported by myshell\n");
	printf("\n");
	printf("NAMES      FORMATS                         DESCRIPTIONS\n");
	printf("&:         [job_spec] &                    execute commands in background\n");
	printf("cd:        cd [dir]						   go to a specified directory\n");
	printf("echo:      echo [arg ...]                  print strings after echo,redirection is supported\n");
	printf("exit:      exit                            quit the shell directly\n");
	printf("jobs:      jobs                            check the processes running in the system\n");
	printf("pwd:       pwd                             print the current working directory\n");
	printf("time:      time                            show the current time in an elegant format\n");
	printf("clear:     clear                           clear the screen\n");
	printf("ls:        ls [dir]                        list the file names in the target directory\n");
	printf("help:      help             			   show the manual of help/get help info of a sepcified command\n");
	printf("quit:      exit                            quit the shell \n");
	fflush(stdout);
}

void cat_redirection()
{
	pid_t pid;
	char dest_filename[MAX_PATH_NAME];
	char source_filename[MAX_PATH_NAME];
	for(int i = 1; i < cmdCnt; i++)
	{
		if(strcmp(cmdArry[i], ">") == 0 || strcmp(cmdArry[i], ">>") == 0)
		{
			if(cmdArry[i+1] == NULL)
			{
				printf("> or >> no file\n");
			}
			else
			{
				strcpy(dest_filename, cmdArry[i+1]);
				strcpy(source_filename, cmdArry[i-1]);
			}
			if(strcmp(cmdArry[i], ">") == 0)
			{
				int output = open(dest_filename, O_WRONLY | O_TRUNC | O_CREAT, 0666);
				//O_WRONLY | O_TRUNC | O_CREAT，写文件，从头开始写，没有就创建
				if(output < 0)
				{
					printf("open or no this file false!\n");
					return;
				}
				while((pid = fork()) < 0);
				if(pid == 0)
				{
					if(dup2(output, 1) < 0)
					{
						printf("err in dup2\n");
						return;
					}
					cmdArry[1] = source_filename;
					cat();
					exit(0);
				}
				else 
				{
					waitpid(pid, NULL , 0);
				}
			}
			if(strcmp(cmdArry[i], ">>") == 0)
			{
				int output = open(dest_filename, O_WRONLY | O_APPEND | O_CREAT, 0666);
				//O_WRONLY | O_APPEND | O_CREAT，写文件，追加，没有就创建
				if(output < 0)
				{
					printf("open or no this file false!\n");
					return;
				}
				while((pid = fork()) < 0);
				if(pid == 0)
				{
					if(dup2(output, 1) < 0)
					{
						printf("err in dup2\n");
						return;
					}
					cmdArry[1] = source_filename;
					cat();
					exit(0);
				}
				else 
				{
					waitpid(pid, NULL , 0);
				}
			}
		}
	}
	return;
}

void wc_redirection()
{
}

void echo_redirection()
{
	pid_t pid;
	char dest_filename[MAX_PATH_NAME];
	for(int i = 1; i < cmdCnt; i++)
	{
		if(strcmp(cmdArry[i], ">") == 0 || strcmp(cmdArry[i], ">>") == 0)
		{
			if(cmdArry[i+1] == NULL)
			{
				printf("> or >> no file\n");
			}
			else
			{
				strcpy(dest_filename, cmdArry[i+1]);
			}
			if(strcmp(cmdArry[i], ">") == 0)
			{
				int output = open(dest_filename, O_WRONLY | O_TRUNC | O_CREAT, 0666);
				//O_WRONLY | O_TRUNC | O_CREAT，写文件，从头开始写，没有就创建
				if(output < 0)
				{
					printf("open or no this file false!\n");
					return;
				}
				while((pid = fork()) < 0);
				if(pid == 0)
				{
					if(dup2(output, 1) < 0)
					{
						printf("err in dup2\n");
						return;
					}
					echo();
					exit(0);
				}
				else 
				{
					waitpid(pid, NULL , 0);
				}
			}
			if(strcmp(cmdArry[i], ">>") == 0)
			{
				int output = open(dest_filename, O_WRONLY | O_APPEND | O_CREAT, 0666);
				//O_WRONLY | O_APPEND | O_CREAT，写文件，追加，没有就创建
				if(output < 0)
				{
					printf("open or no this file false!\n");
					return;
				}
				while((pid = fork()) < 0);
				if(pid == 0)
				{
					if(dup2(output, 1) < 0)
					{
						printf("err in dup2\n");
						return;
					}
					echo();
					exit(0);
				}
				else 
				{
					waitpid(pid, NULL , 0);
				}
			}
		}
	}
	return;
}

void echo_pipe(int pos)//test
{
	//char msg[1024];
	pid_t pid;
	char command[MAX_LINE];
	//char* test = "hello world!";
	for(int i = 1; i < cmdCnt; i++)
	{
		if(strcmp(cmdArry[i], "|") == 0 )
		{
			if(cmdArry[i+1] == NULL)
			{
				printf("no pipe\n");
			}
			else
			{
				strcpy(command, cmdArry[i+1]);
			}
			while((pid = fork()) < 0);
			if(pid == 0)
			{
				dup2(pipe_default[1], 1);
				echo();
				close(pipe_default[0]);
				close(pipe_default[1]);
				//printf("asfasf\n");
				//write(pipe_default[1], test, strlen(test)+1);
			}
			else
			{
				if(strcmp(command, "echo"))
				{
					//close(pipe_default[1]);
					dup2(pipe_default[0], 0);
					close(pipe_default[0]);
					close(pipe_default[1]);
					//ssize_t s = read(pipe_default[0], msg, sizeof(msg));
				}
				waitpid(pid, NULL , 0);
			}
		}
	}
	return;
}

int is_bg(void)
{
	int i = 0;
	if(cmdCnt == 0)
		return 0;
	while(i < cmdCnt && cmdArry[i] != NULL)
		++i; 
	i = i - 1; 
	if(strcmp(cmdArry[i], "&") == 0)
	{
		cmdArry[i] = NULL;
		cmdCnt--;
		return 1;
	}
	else
		return 0;
}

void tail()
{
	char buf[1024];
	FILE* fp;
	if((fp = fopen(cmdArry[1], "r")) == NULL)
	{
		printf("No such file or directory!\n");
		return;
	}
	fseek(fp, -1024, SEEK_END);//定位到文件文件结尾前1024个字节
	while(!feof(fp))//feof文件结束：返回非0值，文件未结束，返回0值
	{
		memset(buf, 0, sizeof(buf));
		fgets(buf, 1024, fp);
		printf("%s",buf);
	}
	fclose(fp);
	return;
}

void tail_f()//tail -f
{
	char buf[1024];
	FILE* fp;
	if((fp = fopen(cmdArry[2], "r")) == NULL)
	{
		printf("No such file or directory!\n");
		return;
	}
	fseek(fp, -1024, SEEK_END);//定位到文件文件结尾前1024个字节
	while(1)//1,后台tail -f 一直读取
	{
		memset(buf, 0, sizeof(buf));
		fgets(buf, 1024, fp);
		printf("%s",buf);
	}
	fclose(fp);
	return;
}

void quit()
{
	exit(-1);
}

//~ void ps()//test
//~ {
	//~ read(fd[0], s, 30);
	//~ //read(fd[0], jobsCnt, sizeof(jobsCnt));
	//~ printf("ssssssssss:%d\n",s[0]);
	//printf("tpid : %d\n",tpid);
	//printf("jobscnt:%d\n",jobsCnt);
	//~ for(int i = 0; i < jobsCnt; i++)
	//~ {
		//~ char dir[MAX_PATH_NAME];
		//~ int pid;
		//~ char pname[MAX_LINE];
		//~ char state;
		//~ int ppid;
		//~ sprintf(dir,"/proc/%d/stat",jobs[i]);
		//~ //FILE*fp=fopen(dir,"r");
		//~ FILE* fp;
		//~ if((fp = fopen(dir, "r")) == NULL)
		//~ {
			//~ printf("NULLL!!!!!!!\n");
			//~ return;
		//~ } 
		//~ while(4==fscanf(fp,"%d %s %c %d\n",&(pid),pname,&(state),&(ppid)))
		//~ {
			//~ break;							          
		//~ }
		//~ //printf("name :%s\n",cmdName[i]);
		//~ strcpy(pname, cmdName[i]);
		//~ printf("%d | %s | %c | %d\n", pid, pname, state, ppid);	
		//~ fclose(fp);
	//~ }
	//~ return;
//~ }

void run_bg()//后台进程这里写的不是很好，也许哪一天会修改,bug，后台进程开启时会偶尔影响主进程；
{
	pid_t pid;
	while((pid = fork()) < 0);
	if(pid == 0)
	{
		if(deal_command())
		{
			;
		}
	}
	else
	{
		signal(SIGCHLD,SIG_IGN);				
	}
	return;
}

int main(void)
{
	int bg = 0;
    welcome();
    while(1)
    {	
		if(bg != 1)
		{
			login_info();
		}
		get_command();
		bg = is_bg();
		if(bg == 1)//bg = 1表示该程序为后台程序
		{
			run_bg();//&有时候没有换行输出，why？
		}
		else
		{
			if(deal_command())
			{
				;
			}
		}	
	}  
    return 0;
}
