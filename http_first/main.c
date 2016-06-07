#include"http.h"
int main(int argc,char* argv[])
{
	if(argc!=3)
	{
		usage(argv[0]);
		return 1;
	}
	char* ip=argv[1];
	int port=atoi(argv[2]);
	int listen_sock=startup(ip,port);
	struct sockaddr_in client;
	socklen_t len=sizeof(client);
	fflush(stdout);
	while(1)
	{
	//	printf("kkkkkkkkkkk");
	//	fflush(stdout);
		int new_sock=accept(listen_sock,(struct sockaddr*)&client,&len);
		if(new_sock<0)
		{
			printf("no client");
			fflush(stdout);
			continue;
		}
		pthread_t id;
	//	printf("con");
		pthread_create(&id,NULL,handle_client,(void*)new_sock);
		pthread_detach(id);//set detach return val:0/errno		
	}
	return 0;
}
