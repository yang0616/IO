#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <assert.h>
#include <sys/select.h>
#include <sys/time.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <poll.h>

#define MAXFD 10

void client_init(struct pollfd client[])
{
    int i = 0;
    for(; i < MAXFD; i++)
    {
        client[i].fd = -1;
        client[i].events = 0;
    }
}


void client_add(struct pollfd client[], int fd)
{
    int i = 0;
    for( ; i < MAXFD; i++)
    {
        if(client[i].fd  == -1)
        {
            client[i].fd = fd;
            client[i].events = POLLIN;
            return ;
        }
    }
}

void client_del(struct pollfd client[], int fd)
{
   int i = 0;
   for(; i < MAXFD; i++)
    {
        if(client[i].fd  == fd)
        {
            client[i].fd = -1;
            client[i].events = 0;
            return ;
        }
    }
}

int main()
{
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    assert( sockfd != -1);
    
    struct sockaddr_in saddr, caddr;
    memset(&saddr, 0, sizeof(saddr));
    saddr.sin_family = AF_INET;
    saddr.sin_port = htons(6000);
    saddr.sin_addr.s_addr = inet_addr("127.0.0.1");

    int res = bind(sockfd, (struct sockaddr *)&saddr, sizeof(saddr));
    assert(res != -1);

    listen(sockfd, 5);

    struct pollfd client[MAXFD];
    client_init(client);
    client_add(client, sockfd);

    while( 1 )
    {       
        int n_ready = poll(client, MAXFD, 3000);
        if(n_ready == -1)
        {
            printf("poll error\n");
            continue;
        }
        if(n_ready  == 0)
        {
            printf("time out!\n");
        }
        
        int i = 0;
        for( ; i < MAXFD; i++)
        {
            if(client[i].fd == -1)
            {
                continue;
            }
            if( client[i].revents & POLLIN)
            {
                if(client[i].fd == sockfd)
                {
                    int len = sizeof(caddr);
                    int c = accept(sockfd, (struct sockaddr*)&caddr, &len);
                    if( c < 0)
                    {
                        continue;
                    }
                    printf("accept c = %d\n",c);
                    client_add(client, c);
                }
                else
                {
                    char buff[128] = {0};
                    int num = recv(client[i].fd, buff, 127, 0);
                    if(num <= 0)
                    {
                        close(client[i].fd);
                        client_del(client, client[i].fd);
                        printf("one client over!\n");
                    }
                    else
                    {
                        printf("recv(%d) = %s\n",client[i].fd,buff);
                        send(client[i].fd,"ok", 2, 0);
                    }

                }
            }
        }

     }        
}

/*  if(client[i].fd == sockfd)
 *  {
 *      int len = sizeof(caddr);
 *      int c = accept(client[i].fd, (struct sockaddr *)&caddr, &len);
 *      if( c < 0)
 *      {
 *          continue;
 *      }
 *      client_add(client, c);
 *      printf("accept c = %d\n",c);
 *  }
 *  else
 *  {
 *      char buff[128] = {0};
 *      int num =  recv(client[i].fd, buff, 127, 0);
 *      if( num <= 0)
 *      {
 *          close(client[i].fd);
 *          client_del(client, client[i].fd);
 *          printf("one over!\n");
 *      }
 *      else
 *      {
 *          printf("c = %d buff = %s\n",client[i].fd, buff);
 *          send(client[i].fd, "ok", 2, 0);
 *      }
 *  }
 * 
 * */    
    
    
    
    
    
    
    
    
    
    
    
        
