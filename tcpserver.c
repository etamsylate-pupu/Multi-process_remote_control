/**********************************************************\
*                                                         *\
*  program name:tcpserver.c                               *\
*  Author:etamsylate                                      *\
*  Date: 29-09-2020                                       *\
*                                                         *\
***********************************************************/


#include<stdlib.h>
#include<stdio.h>
#include<sys/socket.h>
#include<netdb.h>
#include<string.h>
#include<unistd.h>
#include<netinet/in.h>
#include<arpa/inet.h>

#define PORT 8900

int execute(char* command,char* buf);
void sig_child(int signo)
{
    pid_t pid;
    while((pid = waitpid(-1,NULL,WNOHANG)) > 0);
 
}

int main()
{
	struct sockaddr_in server; 
	struct sockaddr_in client; //声明两个套接字为sockaddr_in结构体变量，分别表示客户端和服务端
	int len;  
	int port;
	int listend;
	int connectd;
	int sendnum;
	int recvnum;
	pid_t pid;
	char cmd[2048];
	char send_buf[2048];
	char recv_buf[2048];


	port= PORT;
	memset(cmd,0,2048);
	memset(send_buf,0,2048);
	memset(recv_buf,0,2048);
	
      
	signal(SIGCHLD,sig_child);  //处理僵尸进程
    if (-1==(listend=socket(AF_INET,SOCK_STREAM,0)))
    {
		perror("create listen socket error\n");
		exit(1);
    }


     memset(&server,0,sizeof(struct sockaddr_in));
	 //初始化服务器端的套接字，并使用htons和htonl将端口和地址转化为网络字节序
     server.sin_family = AF_INET; //地址族
     server.sin_addr.s_addr = htonl(INADDR_ANY); //宏INADDR_ANY，代表0.0.0.0，表明所有地址
     server.sin_port = htons(port);
	 

     if (-1==bind(listend,(struct sockaddr *)&server,sizeof(struct sockaddr)))
     {
		perror("bind error\n");
		close(listend);
		exit(1);

     }
    //设置服务器上的socket为监听状态
    if (-1==listen(listend,5))
    {
		perror("listen error\n");
		exit(1);
    }
	len = sizeof(struct sockaddr_in);
    while (1)
    {
    
		memset(recv_buf,0,2048);
		memset(send_buf,0,2048);
        if (-1==(connectd=accept(listend,(struct sockaddr*)&client,&len))){  	
			perror("create connect socket error\n");
			continue;
    	}

   	#ifdef DEBUG
		printf("the connect id is %d",connect);
		printf("the client ip addr is %s",inet_ntoa(client.sin_addr));
   	#endif
		if((pid=fork())==0){ //创建子进程，若为子进程
			close(listend);
			while(1){
				if (0>=(recvnum = recv(connectd,recv_buf,sizeof(recv_buf),0)))
				{
					perror("recv error\n");
					close(connectd);
					break;
				}
				recv_buf[recvnum]='\0';
				if(0==strcmp(recv_buf,"quit")){
					perror("quitting remote controling\n");
					close(connectd);
					return 0;
				}
		 
				printf("the message is :%s\n",recv_buf);
				strcpy(cmd,"/bin/");
				strcat(cmd,recv_buf);
				execute(cmd,send_buf);

				if ('\0'==*send_buf)
				{
					memset(cmd,0,sizeof(cmd));
					strcpy(cmd,"/sbin/");
					strcat(cmd,recv_buf);
					execute(cmd,send_buf);
			
					if ('\0'==*send_buf)
					{	
						memset(cmd,0,sizeof(cmd));
						strcpy(cmd,"/usr/bin/");
						strcat(cmd,recv_buf);
						execute(cmd,send_buf);
					}
			
					if ('\0'==*send_buf)
					{	
						memset(cmd,0,sizeof(cmd));
						strcpy(cmd,"/usr/sbin/");
						strcat(cmd,recv_buf);
						execute(cmd,send_buf);
					}
				}
				if ('\0'==*send_buf)
					sprintf(send_buf,"command is not vaild,check it please\n");

				printf("the server message is:%s\n",send_buf);
				if(0>(sendnum=send(connectd,send_buf,sizeof(send_buf),0))){
					perror("send error \n");
					continue;
				}
		
			}
		}
		else if(pid>0){
			close(connectd);
			//signal(SIGCHLD,sig_child);
			continue;
		}
   }
    close(listend);
    return 0;

}
//执行命令
int execute(char* command,char* buf)
{
	FILE * 	fp;
	int count;

	if (NULL==(fp = popen(command,"r")))
	{
		perror("creating pipe error\n");
		exit(1);

	}
	
	count = 0 ;

	while(((buf[count] = fgetc(fp))!=EOF)&&count<2047)
	count++;
	buf[count]='\0';

	pclose(fp);
	return count;
	

}




