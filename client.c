#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/epoll.h>
#include <arpa/inet.h>
#include <errno.h>
#define MAX 80
#define PORT 8088
#define SA struct sockaddr

void func(int sockfd)

{

    char buff[MAX];

    int n, count = 0;

    for (;;) {

        /*bzero(buff, sizeof(buff));
        if(count == 0){
            buff[0] = ' ';
            write(sockfd, buff, sizeof(buff));
        }*/
       // else{
            printf("Enter the string : ");

            n = 0;

            while ((buff[n++] = getchar()) != '\n')

                ;

            write(sockfd, buff, sizeof(buff));

            bzero(buff, sizeof(buff));

            read(sockfd, buff, sizeof(buff));

            printf("From Server : %s", buff);

            if ((strncmp(buff, "exit", 4)) == 0) {

                printf("Client Exit...\n");

                break;

            }
        //}
        count++;
    }

}

  

int main()

{
    

    int sockfd, connfd;

    struct sockaddr_in servaddr, cli;

  

    // создание и изменение сокета

    sockfd = socket(AF_INET, SOCK_STREAM, 0);

    if (sockfd == -1) {

        printf("socket creation failed...\n");

        exit(0);

    }

    else

        printf("Socket successfully created..\n");

    bzero(&servaddr, sizeof(servaddr));

  

    // назначаем IP, PORT

    servaddr.sin_family = AF_INET;

    servaddr.sin_addr.s_addr = inet_addr("127.0.0.1");

    servaddr.sin_port = htons(PORT);

  

    // подключаем сокет клиента к сокету сервера

    if (connect(sockfd, (SA*)&servaddr, sizeof(servaddr)) != 0) {

        printf("connection with the server failed...\n");

        exit(0);

    }

    else

        printf("connected to the server..\n");

  

    // функция для чата

    func(sockfd);

  

    // закрываем сокет

    close(sockfd);

}
