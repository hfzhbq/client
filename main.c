/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/*
 * File:   main.c
 * Author: root
 *
 * Created on July 3, 2019, 9:33 PM
 */

#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <time.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>



#define RECV_LINE 2048
#define SEND_LINE 16384

static char recvbuff[RECV_LINE];
static char sendbuff[SEND_LINE];

size_t my_write(int sockfd, const void *buf, size_t size)
{
    int nwrite = 0;
    int nleft = 0;
    const char *ptr;

    ptr = buf;
    nleft = size;
    while (nleft > 0) {

        nwrite = send(sockfd, ptr, nleft, MSG_DONTWAIT);
        if (nwrite <= 0) {

            if (errno == EAGAIN) {
                continue;
            }
            perror("inj send error");
        }
 //       if (nwrite != size) {
 //           printf("nwrite : %d\n", nwrite);
//        }
        nleft -= nwrite;
        ptr += nwrite;
    }
    return size;
}

void io_loop(int sockfd)
{
    int total = 0;
    int nwrite = 0;
    while(1) {
        nwrite = my_write(sockfd, sendbuff, sizeof(sendbuff));
        total += nwrite;
        printf("total : %d bytes\n", total);
    }
}


void str_cli(FILE* fp, int sockfd)
{
    char sendline[SEND_LINE],recvline[RECV_LINE];
    int nread, nwrite;
    while (fgets(sendline, RECV_LINE, fp) != NULL) {
        nwrite = send(sockfd, sendline, strlen(sendline), NULL);
        printf("nwrite:%d\n", nwrite);
        if ((nread = recv(sockfd, recvline, RECV_LINE, NULL)) == 0)
            printf("no echo\n");
        fputs(recvline, stdout);
    }

}
/*
 *
 */
int main(int argc, char** argv) {

    int sockfd = 0;
    int ret = 0;
    char recvline[RECV_LINE+1];
    struct sockaddr_in servaddr;

    if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        printf("socket error");
        return 1;
    }

    int s = -1;
    int snd_size = 0;
    socklen_t optlen;
    optlen = sizeof(snd_size);
    getsockopt(s, SOL_SOCKET, SO_SNDBUF,&snd_size, &optlen);

    bzero(&servaddr, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(9090);

    if (inet_pton(AF_INET, argv[1], &servaddr.sin_addr) <= 0) {
        printf("inet_pton error");
        return 1;
    }

    if (connect(sockfd, (struct sockaddr *)&servaddr, sizeof(servaddr)) < 0) {
        perror("connect perror");
        printf("connect error: %d: %s\n", errno, strerror(errno));
        return 1;
    }

    memset(sendbuff, 'm', sizeof(sendbuff));
    io_loop(sockfd);

    //str_cli(stdin, sockfd);

/*
    while ((ret = read(sockfd, recvline, 50)) > 0) {
        recvline[ret] = 0;
        if (fputs(recvline, stdout) == EOF) {
            printf("fputs error");
            return 1;
        }
    }
*/

    if (ret < 0) {
        printf("read error");
        return 1;
    }
    return (EXIT_SUCCESS);
}

