#include <errno.h>
#include <netinet/in.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>
#include <malloc.h>

#define PORT 8088
#define SA struct sockaddr

typedef struct 
{
	int fd_players[2];


}great_data;

void* serve_client(void* thread_data){
    int socket_client, count = 0;
    char buff[256];
    bzero(buff, 256);

    
    
    great_data *data = (great_data *)thread_data;
    socket_client = data->fd_players[0];

    while(read(socket_client, buff, sizeof(buff)) > 0){
        for(int i = 0; i < 4; i++){
            if(buff[i] == 0){
                count++;
            }
        }
        if(count != 4){
            printf("From client: %s\n", buff);
        
            send(socket_client, buff, sizeof(buff), 0);
            printf("To client: %s\n", buff);
            bzero(buff, sizeof(buff));
        }
        count = 0;

    }
    
}

int main(){
    int lsocket;    /* Дескриптор прослушиваемого сокета. */
    int csocket;    /* Дескриптор присоединенного сокета. */
    struct sockaddr_in servaddr;
    int *arg;
    pthread_t thread;
    great_data threadData;
    threadData.fd_players[0] = -1;
    threadData.fd_players[1] = -1;

    lsocket = socket(AF_INET, SOCK_STREAM, 0);
    if(lsocket == -1){
        printf("socket creation failed...\n");
        exit(0);
    }

    memset(&servaddr, 0, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(PORT);
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);

    if ((bind(lsocket, (SA*)&servaddr, sizeof(servaddr))) != 0) {
        printf("socket bind failed...\n");
        exit(0);
    }

    else{
        printf("Socket successfully binded..\n");
    }

    if ((listen(lsocket, 5)) != 0) {
        printf("Listen failed...\n");
        exit(0);
    }

    else{
        printf("Server listening..\n");
    }

    for(;;){
        csocket = accept(lsocket, NULL, 0);

        for(int i = 0; i < 2; i++){
            if(threadData.fd_players[i] == -1){
                threadData.fd_players[i] = csocket;
            }
        }
        pthread_create(&thread, NULL, serve_client, &threadData);
    }
        

    return 0;
}