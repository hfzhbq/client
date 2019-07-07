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

#define MAXLINE 1024
/*
 *
 */
int main(int argc, char** argv) {

    int sockfd = 0;
    int ret = 0;
    char recvline[MAXLINE+1];
    struct sockaddr_in servaddr;

    if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        printf("socket error");
        return 1;
    }
    bzero(&servaddr, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(9090);

    if (inet_pton(AF_INET, argv[1], &servaddr.sin_addr) <= 0) {
        printf("inet_pton error");
        return 1;
    }

    if (connect(sockfd, (struct sockaddr *)&servaddr, sizeof(servaddr)) < 0) {
        printf("connect error: %d: %s\n", errno, strerror(errno));
        return 1;
    }
    while ((ret = read(sockfd, recvline, 50)) > 0) {
        recvline[ret] = 0;
        if (fputs(recvline, stdout) == EOF) {
            printf("fputs error");
            return 1;
        }
    }

    if (ret < 0) {
        printf("read error");
        return 1;
    }
    return (EXIT_SUCCESS);
}

