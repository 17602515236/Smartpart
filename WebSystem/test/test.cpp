#include<sys/types.h>
#include<sys/socket.h>
#include<arpa/inet.h>
#include<netinet/in.h>
#include<string.h>
#include<unistd.h>
#include<iostream>
using namespace std;

#define PORT 8888

int main(int argc,char *argv[])
{
	int sockfd = 0;
	char str[] = "Hello World\n";
	/*create socket fd*/
	sockfd = socket(AF_INET,SOCK_DGRAM,0);
	if(-1 == sockfd)
	{
		cout<<"socket error"<<endl;
		return -1;
	}


	struct sockaddr_in addr;
	addr.sin_family = AF_INET;
	addr.sin_port = htons(PORT);
	addr.sin_addr.s_addr = inet_addr("127.0.0.1");

	while(1)
	{
		sendto(sockfd,str,strlen(str),0,(sockaddr*)&addr,sizeof(addr));
		sleep(1);
		cout<<"Transmit ok"<<endl;
	}
	return 0;
}
