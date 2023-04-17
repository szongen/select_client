#include <stdio.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <string.h>
#include <unistd.h>
#include <sys/select.h>
#include <fcntl.h>
#include <errno.h>

#define BUFFER_SIZE 1024

int main(int argc, const char *argv[])
{
    struct sockaddr_in server_addr;
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(8000);
    server_addr.sin_addr.s_addr = inet_addr("192.168.0.103");
    bzero(&(server_addr.sin_zero), 8);

    int server_sock_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_sock_fd == -1)
    {
        perror("socket error");
        return 1;
    }
    char recv_msg[BUFFER_SIZE];
    char input_msg[BUFFER_SIZE];
    int itime = 0;
    if (0 > fcntl(server_sock_fd, F_SETFL, fcntl(server_sock_fd, F_GETFL, 0) | O_NONBLOCK))
       {
           printf("fcntl failed/n");
           return 1;
       }
    printf("connect beginning\n");
    fcntl(server_sock_fd, F_SETFL, fcntl(server_sock_fd, F_GETFL, 0 ) | O_NONBLOCK);
    if (connect(server_sock_fd, (struct sockaddr *)&server_addr, sizeof(struct sockaddr_in)) == 0 || errno == EINPROGRESS)
    {
        if (errno != 0) {
            printf("%s(%d)\n", strerror(errno), errno);
        }
        fd_set client_fd_set;
        fd_set rfds, wfds, efds;
        struct timeval tv;
        printf("connect is success\n");
        while (1)
        {
            itime ++;
            int r = -1;
            tv.tv_sec = 0;
            tv.tv_usec = 500 *1000;
            FD_ZERO(&rfds);
            FD_ZERO(&wfds);
            FD_ZERO(&efds);
            // FD_SET(server_sock_fd, &wfds);
            FD_SET(server_sock_fd, &rfds);
            FD_SET(server_sock_fd, &efds);
            if(itime >10)
            {
                FD_SET(server_sock_fd, &wfds);
                itime = 0;
            }
            r = select(server_sock_fd + 1, &rfds, &wfds, &efds, &tv);
            if(r < 0 || (int)FD_ISSET(server_sock_fd, &efds))
            {
                printf("select failed: %d %d %s", r, (int)FD_ISSET(server_sock_fd, &efds), strerror(errno));
            }
            else{
                printf("r:%d\n",r);
                if (FD_ISSET(server_sock_fd, &wfds))
                {
                    // printf("11111\n");
                    bzero(input_msg, BUFFER_SIZE);
                    // fgets(input_msg, BUFFER_SIZE, stdin);
                    sprintf(input_msg, "%s", "11111");
                    if (send(server_sock_fd, input_msg, strlen(input_msg), 0) == -1)
                    {
                        perror("发送消息出错!\n");
                    }
                }
                else if (FD_ISSET(server_sock_fd, &rfds))
                {
                    bzero(recv_msg, BUFFER_SIZE);
                    long byte_num = recv(server_sock_fd, recv_msg, BUFFER_SIZE, 0);
                    if (byte_num > 0)
                    {
                        if (byte_num > BUFFER_SIZE)
                        {
                            byte_num = BUFFER_SIZE;
                        }
                        recv_msg[byte_num] = '\0';
                        printf("服务器:%s\n", recv_msg);
                    }
                    // else if (byte_num < 0)
                    // {
                    //     printf("接受消息出错!\n");
                    // }
                    // else
                    // {
                    //     printf("服务器端退出!\n");
                    //     exit(0);
                    // }
                }
            }

        }
    }
    return 0;
}
