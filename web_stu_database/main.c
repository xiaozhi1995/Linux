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
	int epo_fd=epoll_create(256);
	if(epo_fd<0)//success:not 0 fd/error:-1
	{
		printLog(strerror(errno),__FUNCTION__,__LINE__);
		return -1;
	}
	bf_p fd_bf=(bf_p)malloc(sizeof(bf_t));
	memset(fd_bf->_buf,'\0',sizeof(fd_bf->_buf));
	fd_bf->_fd=listen_sock;
	struct epoll_event ev;
	ev.events=EPOLLIN;
	ev.data.fd=listen_sock;
	if(epoll_ctl(epo_fd,EPOLL_CTL_ADD,listen_sock,&ev)<0)//success:0 fail:-1
	{
		printLog(strerror(errno),__FUNCTION__,__LINE__);
		return -1;
	}
	struct epoll_event evfds[_SIZE_];//_SIZE_ 1024
	int _timeout=5000;
	int ret=-1;
	
	int i=0;
	while(1)
	{
		switch((ret=epoll_wait(epo_fd,evfds,_SIZE_,_timeout)))
		{
			case -1://error
				printLog(strerror(errno),__FUNCTION__,__LINE__);
				break;
			case 0://time out
				printf("time out...\n");
				break;
			default://normal
				{
					for(i=0;i<ret;++i)
					{
						if(evfds[i].data.fd==listen_sock&&evfds[i].events&EPOLLIN)
						{
							int new_sock=accept(listen_sock,(struct sockaddr*)&client,&len);
							if(new_sock<0)
							{
								printLog(strerror(errno),__FUNCTION__,__LINE__);
								continue;
							}
							bf_p _bf=(bf_p)malloc(sizeof( bf_t));
							memset(_bf->_buf,'\0',sizeof(_bf->_buf));
							_bf->_fd=new_sock;

							ev.events=EPOLLIN;
							ev.data.ptr=_bf;
							epoll_ctl(epo_fd,EPOLL_CTL_ADD,new_sock,&ev);
						}
						else if(((bf_p)(evfds[i].data.ptr))->_fd>0&&evfds[i].events&EPOLLIN)
						{
							accept_request(evfds[i].data.ptr);
							ev.events=EPOLLOUT;
							ev.data.ptr=evfds[i].data.ptr;
							epoll_ctl(epo_fd,EPOLL_CTL_MOD,((bf_p)(evfds[i].data.ptr))->_fd,&ev);
						}
						else if(((bf_p)(evfds[i].data.ptr))->_fd>0&&evfds[i].events&EPOLLOUT)
						{
							bf_p _bf=(bf_p)evfds[i].data.ptr;
							if(_bf->_err)
							{
								send(_bf->_fd,_bf->_buf,strlen(_bf->_buf),0);
							}
							else if(_bf->_cgi)//cgi=1
							{
								send(_bf->_fd,_bf->_buf,strlen(_bf->_buf),0);
							}
							else
							{
								char* path=_bf->_path;
								int fd=open(path,O_RDONLY);
								if(fd<0)
								{
									printLog(strerror(errno),__FUNCTION__,__LINE__);
									exit(1);
								}
								send(_bf->_fd,_bf->_buf,strlen(_bf->_buf),0);
								if(sendfile(_bf->_fd,fd,NULL,_bf->_st_size)<0)
								{
									printf("error");
								}
								close(fd);
							}
		
							epoll_ctl(epo_fd,EPOLL_CTL_DEL,_bf->_fd,NULL);
							close(_bf->_fd);
							free(_bf);
						}
					}

					break;
				}
		}
	}
	return 0;
}
