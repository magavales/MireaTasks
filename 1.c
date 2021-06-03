#include <pthread.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/select.h>
#include <string.h>
#include <unistd.h>
#include <stdio.h>
#include <ncurses.h>
#include <math.h>
#include <stdlib.h>
#ifndef PORT
#define PORT 9999
#define MAIN_COLOR 1
#define FRIEND_COLOR 2
#define ENEMY_COLOR 3
#define HEIGHT 40
#define WIDTH 100 
#endif
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
WINDOW *game_win;

int count_game = 0;
int begin = 0;

typedef struct{
    int x;
    int y;
    int key;
    int cannon;
    int memory_canon[3];
    int x_ammunition[3];
    int y_ammunition[3];
    int condition_ammunition[3];
    int start;
    int end;
    char* ip;
    int condition_player;
}possition;

typedef struct{
    possition friend;
    possition enemy;
}great_data;


int work_flag = 1;

great_data tank_friend(int ch, great_data* data){
    if(data->friend.key == KEY_UP){
        data->friend.y--;
        if(abs(data->friend.x - data->enemy.x) < 3 && (data->enemy.y - data->friend.y) == 3){
            data->friend.y++;
        }
        if(data->friend.y == 1){
            data->friend.y++;
        }
    }
    if(data->friend.key == KEY_DOWN){
        data->friend.y++;
        if(abs(data->friend.x - data->enemy.x) < 3 && (data->enemy.y - data->friend.y) == 3){
            data->friend.y--;
        }
        if(data->friend.y == HEIGHT ){
            data->friend.y--;
        }
    }
    if (data->friend.key == KEY_RIGHT)
    {   
        data->friend.x++;     
        if(abs(data->friend.y - data->enemy.y) < 3 && (data->enemy.x - data->friend.x) == 3){
            data->friend.x--;
        }

        if(data->friend.x == WIDTH - 1){
            data->friend.x--;
        }
    }
    if(data->friend.key == KEY_LEFT){
        if(abs(data->friend.y - data->enemy.y) < 3 && (data->enemy.x - data->friend.x) == 3){
            
        }
        else{
            data->friend.x--;
        }
        if(data->friend.x == 1){
            data->friend.x++;
        }
    }
    
    
    return *data;
}

great_data tank_enemy(int ch, great_data* data){
    if(data->enemy.key == KEY_UP){
        data->enemy.y--;
        if(abs(data->friend.x - data->enemy.x) < 3 && (data->friend.y - data->enemy.y) == 3){
            data->enemy.y++;
        }
        if(data->enemy.y == 1){
            data->enemy.y++;
        }
    }
    if(data->enemy.key == KEY_DOWN){
        data->enemy.y++;
        if(abs(data->friend.x - data->enemy.x) < 3 && (data->friend.y - data->friend.y) == 3){
            data->enemy.y--;
        }
        if(data->enemy.y == HEIGHT){
            data->enemy.y--;
        }
    }
    if (data->enemy.key == KEY_RIGHT)
    {
        
        if(abs(data->friend.y - data->enemy.y) < 3 && (data->enemy.x - data->friend.x) == 3){
            
        }
        else{
            data->enemy.x++;
        }
        if(data->enemy.x == WIDTH - 1){
            data->enemy.x--;
        }
    }
    if(data->enemy.key == KEY_LEFT){
        data->enemy.x--;     
        if(abs(data->enemy.y - data->friend.y) < 3 && (data->friend.x - data->enemy.x) == 3){
            data->enemy.x++;
        }

        if(data->enemy.x == 1){
            data->enemy.x++;
        }
    }
    
    return *data;
}



void *udp_server(void *thread_data)
{
    great_data *data = (great_data *)thread_data;
    int ch;

    int sock, addr_len, count, ret;
    char buffer[1024];
    fd_set readfd;
    struct sockaddr_in addr;

    sock = socket(AF_INET, SOCK_DGRAM, 0);
    if (0 > sock)
    {
        perror("socket");
        return NULL;
    }

    // Нужно для работы моей VPN сети
    #ifdef VPN_ENABLED
    char *devname = "tap0";
    ret = setsockopt(sock, SOL_SOCKET, SO_BINDTODEVICE, devname,
    strlen(devname));
    if (ret == -1)
    {
        perror("setsockopt");
    return 0;
    }
    #endif

    addr_len = sizeof(struct sockaddr_in);
    memset((void *)&addr, 0, addr_len);
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = htons(INADDR_ANY);
    addr.sin_port = htons(PORT);

    if (bind(sock, (struct sockaddr *)&addr, addr_len) < 0)
    {
        perror("bind");
        close(sock);
        return NULL;
    }

    while (work_flag)
    {
        FD_ZERO(&readfd);
        FD_SET(sock, &readfd);

        ret = select(sock + 1, &readfd, NULL, NULL, 0);
        if (ret > 0)
        {
            if (FD_ISSET(sock, &readfd))
            {
                recvfrom(sock, &ch, sizeof(ch), 0, (struct sockaddr *)&addr, &addr_len);
                if(!strcmp(data->friend.ip, inet_ntoa(addr.sin_addr))){
                    if(ch == KEY_LEFT || ch == KEY_RIGHT || ch == KEY_UP || ch == KEY_DOWN){
                        data->friend.key = ch;
                        tank_friend(ch, data);
                    }
                    if(ch == 'Q' || ch == 'W' || ch == 'E' || ch == 'D' || ch == 'C' || ch == 'X' || ch == 'Z' || ch == 'A' || ch == 'q' || ch == 'w' || ch == 'e' || ch == 'd' || ch == 'c' || ch == 'x' || ch == 'z' || ch == 'a'){
                        data->friend.cannon = ch;
                        
                    }
                    if(ch == ' '){
                        for(int i = 0; i < 3; i++){
                            if(data->friend.condition_ammunition[i] != 1){
                                data->friend.condition_ammunition[i] = 1;
                                data->friend.memory_canon[i] = data->friend.cannon;
                                if(data->friend.cannon == 'Q' || data->friend.cannon == 'q'){
                                    data->friend.x_ammunition[i] = data->friend.x - 1;
                                    data->friend.y_ammunition[i] = data->friend.y - 1;
                                }
                                if(data->friend.cannon == 'W' || data->friend.cannon == 'w'){
                                    data->friend.y_ammunition[i] = data->friend.y - 1;
                                    data->friend.x_ammunition[i] = data->friend.x;
                                }
                                if(data->friend.cannon == 'E' || data->friend.cannon == 'e'){
                                    data->friend.x_ammunition[i] = data->friend.x + 1;
                                    data->friend.y_ammunition[i] = data->friend.y - 1;
                                }
                                if(data->friend.cannon == 'D' || data->friend.cannon == 'd'){
                                    data->friend.x_ammunition[i] = data->friend.x + 1;
                                    data->friend.y_ammunition[i] = data->friend.y;
                                }
                                if(data->friend.cannon == 'C' || data->friend.cannon == 'c'){
                                    data->friend.x_ammunition[i] = data->friend.x + 1;
                                    data->friend.y_ammunition[i] = data->friend.y + 1;
                                }
                                if(data->friend.cannon == 'X' || data->friend.cannon == 'x'){
                                    data->friend.y_ammunition[i] = data->friend.y + 1;
                                    data->friend.x_ammunition[i] = data->friend.x;
                                }
                                if(data->friend.cannon == 'Z' || data->friend.cannon == 'z'){
                                    data->friend.x_ammunition[i] = data->friend.x - 1;
                                    data->friend.y_ammunition[i] = data->friend.y + 1;
                                }
                                if(data->friend.cannon == 'A' || data->friend.cannon == 'a'){
                                    data->friend.x_ammunition[i] = data->friend.x - 1;
                                    data->friend.y_ammunition[i] = data->friend.y;
                                }
                                break;
                            }
                        }
                    }
                }
                if(!strcmp(data->enemy.ip, inet_ntoa(addr.sin_addr))){
                    if(ch == KEY_LEFT || ch == KEY_RIGHT || ch == KEY_UP || ch == KEY_DOWN){
                        data->enemy.key = ch;
                        tank_enemy(ch, data);
                    }
                    if(ch == 'Q' || ch == 'W' || ch == 'E' || ch == 'D' || ch == 'C' || ch == 'X' || ch == 'Z' || ch == 'A' || ch == 'q' || ch == 'w' || ch == 'e' || ch == 'd' || ch == 'c' || ch == 'x' || ch == 'z' || ch == 'a'){
                        data->enemy.cannon = ch;
                    }
                    if(ch == ' '){
                        for(int i = 0; i < 3; i++){
                            if(data->enemy.condition_ammunition[i] != 1){
                                data->enemy.condition_ammunition[i] = 1;
                                data->enemy.memory_canon[i] = data->enemy.cannon;
                                if(data->enemy.cannon == 'Q' || data->enemy.cannon == 'q'){
                                    data->enemy.x_ammunition[i] = data->enemy.x - 1;
                                    data->enemy.y_ammunition[i] = data->enemy.y - 1;
                                }
                                if(data->enemy.cannon == 'W' || data->enemy.cannon == 'w'){
                                    data->enemy.y_ammunition[i] = data->enemy.y - 1;
                                    data->enemy.x_ammunition[i] = data->enemy.x;
                                }
                                if(data->enemy.cannon == 'E' || data->enemy.cannon == 'e'){
                                    data->enemy.x_ammunition[i] = data->enemy.x + 1;
                                    data->enemy.y_ammunition[i] = data->enemy.y - 1;
                                }
                                if(data->enemy.cannon == 'D' || data->enemy.cannon == 'd'){
                                    data->enemy.x_ammunition[i] = data->enemy.x + 1;
                                    data->enemy.y_ammunition[i] = data->enemy.y;
                                }
                                if(data->enemy.cannon == 'C' || data->enemy.cannon == 'c'){
                                    data->enemy.x_ammunition[i] = data->enemy.x + 1;
                                    data->enemy.y_ammunition[i] = data->enemy.y + 1;
                                }
                                if(data->enemy.cannon == 'X' || data->enemy.cannon == 'x'){
                                    data->enemy.y_ammunition[i] = data->enemy.y + 1;
                                    data->enemy.x_ammunition[i] = data->enemy.x;
                                }
                                if(data->enemy.cannon == 'Z' || data->enemy.cannon == 'z'){
                                    data->enemy.x_ammunition[i] = data->enemy.x - 1;
                                    data->enemy.y_ammunition[i] = data->enemy.y + 1;
                                }
                                if(data->enemy.cannon == 'A' || data->enemy.cannon == 'a'){
                                    data->enemy.x_ammunition[i] = data->enemy.x - 1;
                                    data->enemy.y_ammunition[i] = data->enemy.y;
                                }
                                break;
                            }
                            
                        }
                    }
                    
                }
                
            }
        }
    }
    close(sock);
    return (void *)NULL;
}

void* strike(void* thread_data){
    great_data *data = (great_data *)thread_data;
    while(count_game != 1){
        usleep(50000);


        for(int i = 0; i < 3; i++){
            if (data->friend.condition_ammunition[i] == 1)
            {

                if (data->friend.memory_canon[i] == 'Q' || data->friend.memory_canon[i] == 'q')
                {
                    data->friend.y_ammunition[i]--;
                    data->friend.x_ammunition[i]--;

                    if(data->friend.y_ammunition[i] == -1 || data->friend.x_ammunition[i] == -1){
                        data->friend.condition_ammunition[i] = 0;
                    }

                    if((data->friend.y_ammunition[i] == data->enemy.y || data->friend.y_ammunition[i] == data->enemy.y - 1 || data->friend.y_ammunition[i] == data->enemy.y + 1) && (data->friend.x_ammunition[i] == data->enemy.x || data->friend.x_ammunition[i] == data->enemy.x - 1 || data->friend.x_ammunition[i] == data->enemy.x + 1)){
                        data->enemy.condition_player = 1;
                        count_game = 1;
                        data->friend.condition_ammunition[i] = 0;
                    }
                }
                if (data->friend.memory_canon[i] == 'W' || data->friend.memory_canon[i] == 'w')
                {
                    data->friend.y_ammunition[i]--;

                    if(data->friend.y_ammunition[i] == -1){
                        data->friend.condition_ammunition[i] = 0;
                    }

                    if((data->friend.y_ammunition[i] == data->enemy.y || data->friend.y_ammunition[i] == data->enemy.y - 1 || data->friend.y_ammunition[i] == data->enemy.y + 1) && (data->friend.x_ammunition[i] == data->enemy.x || data->friend.x_ammunition[i] == data->enemy.x - 1 || data->friend.x_ammunition[i] == data->enemy.x + 1)){
                        data->enemy.condition_player = 1;
                        count_game = 1;
                        data->friend.condition_ammunition[i] = 0;
                    }
                }
                if (data->friend.memory_canon[i] == 'E' || data->friend.memory_canon[i] == 'e')
                {
                    data->friend.y_ammunition[i]--;
                    data->friend.x_ammunition[i]++;

                    if(data->friend.y_ammunition[i] == -1 || data->friend.x_ammunition[i] == WIDTH){
                        data->friend.condition_ammunition[i] = 0;
                    }

                    if((data->friend.y_ammunition[i] == data->enemy.y || data->friend.y_ammunition[i] == data->enemy.y - 1 || data->friend.y_ammunition[i] == data->enemy.y + 1) && (data->friend.x_ammunition[i] == data->enemy.x || data->friend.x_ammunition[i] == data->enemy.x - 1 || data->friend.x_ammunition[i] == data->enemy.x + 1)){
                        data->enemy.condition_player = 1;
                        count_game = 1;
                        data->friend.condition_ammunition[i] = 0;
                    }
                }
                if (data->friend.memory_canon[i] == 'D' || data->friend.memory_canon[i] == 'd')
                {
                    data->friend.x_ammunition[i]++;

                    if(data->friend.x_ammunition[i] == WIDTH){
                        data->friend.condition_ammunition[i] = 0;
                    }

                    if((data->friend.y_ammunition[i] == data->enemy.y || data->friend.y_ammunition[i] == data->enemy.y - 1 || data->friend.y_ammunition[i] == data->enemy.y + 1) && (data->friend.x_ammunition[i] == data->enemy.x || data->friend.x_ammunition[i] == data->enemy.x - 1 || data->friend.x_ammunition[i] == data->enemy.x + 1)){
                        data->enemy.condition_player = 1;
                        count_game = 1;
                        data->friend.condition_ammunition[i] = 0;
                    }
                }
                if (data->friend.memory_canon[i] == 'C' || data->friend.memory_canon[i] == 'c')
                {
                    data->friend.y_ammunition[i]++;
                    data->friend.x_ammunition[i]++;

                    if(data->friend.y_ammunition[i] == HEIGHT || data->friend.x_ammunition[i] == WIDTH){
                        data->friend.condition_ammunition[i] = 0;
                    }

                    if((data->friend.y_ammunition[i] == data->enemy.y || data->friend.y_ammunition[i] == data->enemy.y - 1 || data->friend.y_ammunition[i] == data->enemy.y + 1) && (data->friend.x_ammunition[i] == data->enemy.x || data->friend.x_ammunition[i] == data->enemy.x - 1 || data->friend.x_ammunition[i] == data->enemy.x + 1)){
                        data->enemy.condition_player = 1;
                        count_game = 1;
                        data->friend.condition_ammunition[i] = 0;
                    }
                }
                if (data->friend.memory_canon[i] == 'X' || data->friend.memory_canon[i] == 'x')
                {
                    data->friend.y_ammunition[i]++;

                    if(data->friend.y_ammunition[i] == HEIGHT){
                        data->friend.condition_ammunition[i] = 0;
                    }

                    if((data->friend.y_ammunition[i] == data->enemy.y || data->friend.y_ammunition[i] == data->enemy.y - 1 || data->friend.y_ammunition[i] == data->enemy.y + 1) && (data->friend.x_ammunition[i] == data->enemy.x || data->friend.x_ammunition[i] == data->enemy.x - 1 || data->friend.x_ammunition[i] == data->enemy.x + 1)){
                        data->enemy.condition_player = 1;
                        count_game = 1;
                        data->friend.condition_ammunition[i] = 0;
                    }
                }
                if (data->friend.memory_canon[i] == 'Z' || data->friend.memory_canon[i] == 'z')
                {
                    data->friend.y_ammunition[i]++;
                    data->friend.x_ammunition[i]--;

                    if(data->friend.y_ammunition[i] == HEIGHT || data->friend.x_ammunition[i] == -1){
                        data->friend.condition_ammunition[i] = 0;
                    }

                    if((data->friend.y_ammunition[i] == data->enemy.y || data->friend.y_ammunition[i] == data->enemy.y - 1 || data->friend.y_ammunition[i] == data->enemy.y + 1) && (data->friend.x_ammunition[i] == data->enemy.x || data->friend.x_ammunition[i] == data->enemy.x - 1 || data->friend.x_ammunition[i] == data->enemy.x + 1)){
                        data->enemy.condition_player = 1;
                        count_game = 1;
                        data->friend.condition_ammunition[i] = 0;
                    }
                }
                if (data->friend.memory_canon[i] == 'A' || data->friend.memory_canon[i] == 'a')
                {
                    data->friend.x_ammunition[i]--;

                    if(data->friend.x_ammunition[i] == -1){
                        data->friend.condition_ammunition[i] = 0;
                    }

                    if((data->friend.y_ammunition[i] == data->enemy.y || data->friend.y_ammunition[i] == data->enemy.y - 1 || data->friend.y_ammunition[i] == data->enemy.y + 1) && (data->friend.x_ammunition[i] == data->enemy.x || data->friend.x_ammunition[i] == data->enemy.x - 1 || data->friend.x_ammunition[i] == data->enemy.x + 1)){
                        data->enemy.condition_player = 1;
                        count_game = 1;
                        data->friend.condition_ammunition[i] = 0;
                    }
                }
            }
        }

        for(int i = 0; i < 3; i++){
            if (data->enemy.condition_ammunition[i] == 1)
            {

                if (data->enemy.memory_canon[i] == 'Q' || data->enemy.memory_canon[i] == 'q')
                {
                    data->enemy.y_ammunition[i]--;
                    data->enemy.x_ammunition[i]--;

                    if(data->enemy.y_ammunition[i] == -1 || data->enemy.x_ammunition[i] == -1){
                        data->enemy.condition_ammunition[i] = 0;
                    }
                    if((data->enemy.y_ammunition[i] == data->friend.y || data->enemy.y_ammunition[i] == data->friend.y - 1 || data->enemy.y_ammunition[i] == data->friend.y + 1) && (data->enemy.x_ammunition[i] == data->friend.x || data->enemy.x_ammunition[i] == data->friend.x - 1 || data->enemy.x_ammunition[i] == data->friend.x + 1)){
                        data->friend.condition_player = 1;
                        count_game = 1;
                        data->enemy.condition_ammunition[i] = 0;
                    }

                }
                if (data->enemy.memory_canon[i] == 'W' || data->enemy.memory_canon[i] == 'w')
                {
                    data->enemy.y_ammunition[i]--;
                    if(data->enemy.y_ammunition[i] == -1){
                        data->enemy.condition_ammunition[i] = 0;
                    }
                    if((data->enemy.y_ammunition[i] == data->friend.y || data->enemy.y_ammunition[i] == data->friend.y - 1 || data->enemy.y_ammunition[i] == data->friend.y + 1) && (data->enemy.x_ammunition[i] == data->friend.x || data->enemy.x_ammunition[i] == data->friend.x - 1 || data->enemy.x_ammunition[i] == data->friend.x + 1)){
                        data->friend.condition_player = 1;
                        count_game = 1;
                        data->enemy.condition_ammunition[i] = 0;
                    }
                }
                if (data->enemy.memory_canon[i] == 'E' || data->enemy.memory_canon[i] == 'e')
                {
                    data->enemy.y_ammunition[i]--;
                    data->enemy.x_ammunition[i]++;

                    if(data->enemy.y_ammunition[i] == -1 || data->enemy.x_ammunition[i] == WIDTH){
                        data->enemy.condition_ammunition[i] = 0;
                    }

                    if((data->enemy.y_ammunition[i] == data->friend.y || data->enemy.y_ammunition[i] == data->friend.y - 1 || data->enemy.y_ammunition[i] == data->friend.y + 1) && (data->enemy.x_ammunition[i] == data->friend.x || data->enemy.x_ammunition[i] == data->friend.x - 1 || data->enemy.x_ammunition[i] == data->friend.x + 1)){
                        data->friend.condition_player = 1;
                        count_game = 1;
                        data->enemy.condition_ammunition[i] = 0;
                    }
                }
                if (data->enemy.memory_canon[i] == 'D' || data->enemy.memory_canon[i] == 'd')
                {
                    data->enemy.x_ammunition[i]++;

                    if(data->enemy.x_ammunition[i] == WIDTH){
                        data->enemy.condition_ammunition[i] = 0;
                    }

                    if((data->enemy.y_ammunition[i] == data->friend.y || data->enemy.y_ammunition[i] == data->friend.y - 1 || data->enemy.y_ammunition[i] == data->friend.y + 1) && (data->enemy.x_ammunition[i] == data->friend.x || data->enemy.x_ammunition[i] == data->friend.x - 1 || data->enemy.x_ammunition[i] == data->friend.x + 1)){
                        data->friend.condition_player = 1;
                        count_game = 1;
                        data->enemy.condition_ammunition[i] = 0;
                    }
                }
                if (data->enemy.memory_canon[i] == 'C' || data->enemy.memory_canon[i] == 'c')
                {
                    data->enemy.y_ammunition[i]++;
                    data->enemy.x_ammunition[i]++;

                    if(data->enemy.y_ammunition[i] == HEIGHT || data->enemy.x_ammunition[i] == WIDTH){
                        data->enemy.condition_ammunition[i] = 0;
                    }

                    if((data->enemy.y_ammunition[i] == data->friend.y || data->enemy.y_ammunition[i] == data->friend.y - 1 || data->enemy.y_ammunition[i] == data->friend.y + 1) && (data->enemy.x_ammunition[i] == data->friend.x || data->enemy.x_ammunition[i] == data->friend.x - 1 || data->enemy.x_ammunition[i] == data->friend.x + 1)){
                        data->friend.condition_player = 1;
                        count_game = 1;
                        data->enemy.condition_ammunition[i] = 0;
                    }
                }
                if (data->enemy.memory_canon[i] == 'X' || data->enemy.memory_canon[i] == 'x')
                {
                    data->enemy.y_ammunition[i]++;

                    if(data->enemy.y_ammunition[i] == HEIGHT){
                        data->enemy.condition_ammunition[i] = 0;
                    }

                    if((data->enemy.y_ammunition[i] == data->friend.y || data->enemy.y_ammunition[i] == data->friend.y - 1 || data->enemy.y_ammunition[i] == data->friend.y + 1) && (data->enemy.x_ammunition[i] == data->friend.x || data->enemy.x_ammunition[i] == data->friend.x - 1 || data->enemy.x_ammunition[i] == data->friend.x + 1)){
                        data->friend.condition_player = 1;
                        count_game = 1;
                        data->enemy.condition_ammunition[i] = 0;
                    }
                }
                if (data->enemy.memory_canon[i] == 'Z' || data->enemy.memory_canon[i] == 'z')
                {
                    data->enemy.y_ammunition[i]++;
                    data->enemy.x_ammunition[i]--;

                    if(data->enemy.y_ammunition[i] == HEIGHT || data->enemy.x_ammunition[i] == -1){
                        data->enemy.condition_ammunition[i] = 0;
                    }

                    if((data->enemy.y_ammunition[i] == data->friend.y || data->enemy.y_ammunition[i] == data->friend.y - 1 || data->enemy.y_ammunition[i] == data->friend.y + 1) && (data->enemy.x_ammunition[i] == data->friend.x || data->enemy.x_ammunition[i] == data->friend.x - 1 || data->enemy.x_ammunition[i] == data->friend.x + 1)){
                        data->friend.condition_player = 1;
                        count_game = 1;
                        data->enemy.condition_ammunition[i] = 0;
                    }
                }
                if (data->enemy.memory_canon[i] == 'A' || data->enemy.memory_canon[i] == 'a')
                {
                    data->enemy.x_ammunition[i]--;

                    if(data->enemy.x_ammunition[i] == -1){
                        data->enemy.condition_ammunition[i] = 0;
                    }

                    if((data->enemy.y_ammunition[i] == data->friend.y || data->enemy.y_ammunition[i] == data->friend.y - 1 || data->enemy.y_ammunition[i] == data->friend.y + 1) && (data->enemy.x_ammunition[i] == data->friend.x || data->enemy.x_ammunition[i] == data->friend.x - 1 || data->enemy.x_ammunition[i] == data->friend.x + 1)){
                        data->friend.condition_player = 1;
                        count_game = 1;
                        data->enemy.condition_ammunition[i] = 0;
                    }
                }
            }
        }
    }

}

void* render(void *thread_data){
    char* red_win = "Red WIN!";
    char* blue_win = "Blue WIN!";
    char* exit_win = "Press SOME keys for exit!";
    great_data *data = (great_data *)thread_data;


    while(count_game != 1){
        usleep(1000000 / 120);
        pthread_mutex_lock(&mutex);
        werase(game_win);
        box(game_win, '|', '-');
        if(begin == 0){
            wattron(game_win, COLOR_PAIR(FRIEND_COLOR));
            mvwaddch(game_win, data->friend.y - 1, data->friend.x - 1, '*');
            mvwaddch(game_win, data->friend.y - 1, data->friend.x, '*');
            mvwaddch(game_win, data->friend.y - 1, data->friend.x + 1, '*');
            mvwaddch(game_win, data->friend.y, data->friend.x - 1, '*');
            mvwaddch(game_win, data->friend.y, data->friend.x, '*');
            mvwaddch(game_win, data->friend.y, data->friend.x + 1, '-');
            mvwaddch(game_win, data->friend.y + 1, data->friend.x - 1, '*');
            mvwaddch(game_win, data->friend.y + 1, data->friend.x, '*');
            mvwaddch(game_win, data->friend.y + 1, data->friend.x + 1, '*');
            wattroff(game_win, COLOR_PAIR(FRIEND_COLOR));

            wattron(game_win, COLOR_PAIR(ENEMY_COLOR));
            mvwaddch(game_win, data->enemy.y - 1, data->enemy.x - 1, '*');
            mvwaddch(game_win, data->enemy.y - 1, data->enemy.x, '*');
            mvwaddch(game_win, data->enemy.y - 1, data->enemy.x + 1, '*');
            mvwaddch(game_win, data->enemy.y, data->enemy.x - 1, '-');
            mvwaddch(game_win, data->enemy.y, data->enemy.x, '*');
            mvwaddch(game_win, data->enemy.y, data->enemy.x + 1, '*');
            mvwaddch(game_win, data->enemy.y + 1, data->enemy.x - 1, '*');
            mvwaddch(game_win, data->enemy.y + 1, data->enemy.x, '*');
            mvwaddch(game_win, data->enemy.y + 1, data->enemy.x + 1, '*');
            wattroff(game_win, COLOR_PAIR(ENEMY_COLOR));
        }
        if(data->friend.key == KEY_UP){
            begin = 1;

            if(data->friend.condition_player == 0){
                wattron(game_win, COLOR_PAIR(FRIEND_COLOR));
            }
            if(data->friend.condition_player == 1){
                wattron(game_win, COLOR_PAIR(MAIN_COLOR));
            }
            
            mvwaddch(game_win, data->friend.y - 1, data->friend.x - 1, '*');
            mvwaddch(game_win, data->friend.y - 1, data->friend.x, ' ');
            mvwaddch(game_win, data->friend.y - 1, data->friend.x + 1, '*');
            mvwaddch(game_win, data->friend.y, data->friend.x - 1, '*');
            mvwaddch(game_win, data->friend.y, data->friend.x, '*');
            mvwaddch(game_win, data->friend.y, data->friend.x + 1, '*');
            mvwaddch(game_win, data->friend.y + 1, data->friend.x - 1, '*');
            mvwaddch(game_win, data->friend.y + 1, data->friend.x, '*');
            mvwaddch(game_win, data->friend.y + 1, data->friend.x + 1, '*');
            if(data->friend.cannon == 'Q' || data->friend.cannon == 'q'){
                mvwaddch(game_win, data->friend.y - 1, data->friend.x - 1, '\\');
            }
            if(data->friend.cannon == 'W' || data->friend.cannon == 'w'){
                mvwaddch(game_win, data->friend.y - 1, data->friend.x, '|');
            }
            if(data->friend.cannon == 'E' || data->friend.cannon == 'e'){
                mvwaddch(game_win, data->friend.y - 1, data->friend.x + 1, '/');
            }
            if(data->friend.cannon == 'D' || data->friend.cannon == 'd'){
                mvwaddch(game_win, data->friend.y, data->friend.x + 1, '-');
            }
            if(data->friend.cannon == 'C' || data->friend.cannon == 'c'){
                mvwaddch(game_win, data->friend.y + 1, data->friend.x + 1, '\\');
            }
            if(data->friend.cannon == 'X' || data->friend.cannon == 'x'){
                mvwaddch(game_win, data->friend.y + 1, data->friend.x, '|');
            }
            if(data->friend.cannon == 'Z' || data->friend.cannon == 'z'){
                mvwaddch(game_win, data->friend.y + 1, data->friend.x - 1, '/');
            }
            if(data->friend.cannon == 'A' || data->friend.cannon == 'a'){
                mvwaddch(game_win, data->friend.y, data->friend.x - 1, '-');
            }
            wattroff(game_win, COLOR_PAIR(FRIEND_COLOR));
            wattroff(game_win, COLOR_PAIR(MAIN_COLOR));
        }
        if(data->enemy.key == KEY_UP){
            begin = 1;

            if(data->enemy.condition_player == 0){
                wattron(game_win, COLOR_PAIR(ENEMY_COLOR));
            }
            if(data->enemy.condition_player == 1){
                wattron(game_win, COLOR_PAIR(MAIN_COLOR));
            }

            mvwaddch(game_win, data->enemy.y - 1, data->enemy.x - 1, '*');
            mvwaddch(game_win, data->enemy.y - 1, data->enemy.x, ' ');
            mvwaddch(game_win, data->enemy.y - 1, data->enemy.x + 1, '*');
            mvwaddch(game_win, data->enemy.y, data->enemy.x - 1, '*');
            mvwaddch(game_win, data->enemy.y, data->enemy.x, '*');
            mvwaddch(game_win, data->enemy.y, data->enemy.x + 1, '*');
            mvwaddch(game_win, data->enemy.y + 1, data->enemy.x - 1, '*');
            mvwaddch(game_win, data->enemy.y + 1, data->enemy.x, '*');
            mvwaddch(game_win, data->enemy.y + 1, data->enemy.x + 1, '*');
            if(data->enemy.cannon == 'Q' || data->enemy.cannon == 'q'){
                mvwaddch(game_win, data->enemy.y - 1, data->enemy.x - 1, '\\');
            }
            if(data->enemy.cannon == 'W' || data->enemy.cannon == 'w'){
                mvwaddch(game_win, data->enemy.y - 1, data->enemy.x, '|');
            }
            if(data->enemy.cannon == 'E' || data->enemy.cannon == 'e'){
                mvwaddch(game_win, data->enemy.y - 1, data->enemy.x + 1, '/');
            }
            if(data->enemy.cannon == 'D' || data->enemy.cannon == 'd'){
                mvwaddch(game_win, data->enemy.y, data->enemy.x + 1, '-');
            }
            if(data->enemy.cannon == 'C' || data->enemy.cannon == 'c'){
                mvwaddch(game_win, data->enemy.y + 1, data->enemy.x + 1, '\\');
            }
            if(data->enemy.cannon == 'X' || data->enemy.cannon == 'x'){
                mvwaddch(game_win, data->enemy.y + 1, data->enemy.x, '|');
            }
            if(data->enemy.cannon == 'Z' || data->enemy.cannon == 'z'){
                mvwaddch(game_win, data->enemy.y + 1, data->enemy.x - 1, '/');
            }
            if(data->enemy.cannon == 'A' || data->enemy.cannon == 'a'){
                mvwaddch(game_win, data->enemy.y, data->enemy.x - 1, '-');
            }
            wattroff(game_win, COLOR_PAIR(ENEMY_COLOR));
            wattroff(game_win, COLOR_PAIR(MAIN_COLOR));
        }
        if(data->friend.key == KEY_DOWN){
            begin = 1;

            if(data->friend.condition_player == 0){
                wattron(game_win, COLOR_PAIR(FRIEND_COLOR));
            }
            if(data->friend.condition_player == 1){
                wattron(game_win, COLOR_PAIR(MAIN_COLOR));
            }

            mvwaddch(game_win, data->friend.y - 1, data->friend.x - 1, '*');
            mvwaddch(game_win, data->friend.y - 1, data->friend.x, '*');
            mvwaddch(game_win, data->friend.y - 1, data->friend.x + 1, '*');
            mvwaddch(game_win, data->friend.y, data->friend.x - 1, '*');
            mvwaddch(game_win, data->friend.y, data->friend.x, '*');
            mvwaddch(game_win, data->friend.y, data->friend.x + 1, '*');
            mvwaddch(game_win, data->friend.y + 1, data->friend.x - 1, '*');
            mvwaddch(game_win, data->friend.y + 1, data->friend.x, ' ');
            mvwaddch(game_win, data->friend.y + 1, data->friend.x + 1, '*');
            if(data->friend.cannon == 'Q' || data->friend.cannon == 'q'){
                mvwaddch(game_win, data->friend.y - 1, data->friend.x - 1, '\\');
            }
            if(data->friend.cannon == 'W' || data->friend.cannon == 'w'){
                mvwaddch(game_win, data->friend.y - 1, data->friend.x, '|');
            }
            if(data->friend.cannon == 'E' || data->friend.cannon == 'e'){
                mvwaddch(game_win, data->friend.y - 1, data->friend.x + 1, '/');
            }
            if(data->friend.cannon == 'D' || data->friend.cannon == 'd'){
                mvwaddch(game_win, data->friend.y, data->friend.x + 1, '-');
            }
            if(data->friend.cannon == 'C' || data->friend.cannon == 'c'){
                mvwaddch(game_win, data->friend.y + 1, data->friend.x + 1, '\\');
            }
            if(data->friend.cannon == 'X' || data->friend.cannon == 'x'){
                mvwaddch(game_win, data->friend.y + 1, data->friend.x, '|');
            }
            if(data->friend.cannon == 'Z' || data->friend.cannon == 'z'){
                mvwaddch(game_win, data->friend.y + 1, data->friend.x - 1, '/');
            }
            if(data->friend.cannon == 'A' || data->friend.cannon == 'a'){
                mvwaddch(game_win, data->friend.y, data->friend.x - 1, '-');
            }
            wattroff(game_win, COLOR_PAIR(FRIEND_COLOR));
            wattroff(game_win, COLOR_PAIR(MAIN_COLOR));
        }
        if (data->friend.key == KEY_RIGHT)
        {
            begin = 1;

            if(data->friend.condition_player == 0){
                wattron(game_win, COLOR_PAIR(FRIEND_COLOR));
            }
            if(data->friend.condition_player == 1){
                wattron(game_win, COLOR_PAIR(MAIN_COLOR));
            }

            mvwaddch(game_win, data->friend.y - 1, data->friend.x - 1, '*');
            mvwaddch(game_win, data->friend.y - 1, data->friend.x, '*');
            mvwaddch(game_win, data->friend.y - 1, data->friend.x + 1, '*');
            mvwaddch(game_win, data->friend.y, data->friend.x - 1, '*');
            mvwaddch(game_win, data->friend.y, data->friend.x, '*');
            mvwaddch(game_win, data->friend.y, data->friend.x + 1, ' ');
            mvwaddch(game_win, data->friend.y + 1, data->friend.x - 1, '*');
            mvwaddch(game_win, data->friend.y + 1, data->friend.x, '*');
            mvwaddch(game_win, data->friend.y + 1, data->friend.x + 1, '*');
            if(data->friend.cannon == 'Q' || data->friend.cannon == 'q'){
                mvwaddch(game_win, data->friend.y - 1, data->friend.x - 1, '\\');
            }
            if(data->friend.cannon == 'W' || data->friend.cannon == 'w'){
                mvwaddch(game_win, data->friend.y - 1, data->friend.x, '|');
            }
            if(data->friend.cannon == 'E' || data->friend.cannon == 'e'){
                mvwaddch(game_win, data->friend.y - 1, data->friend.x + 1, '/');
            }
            if(data->friend.cannon == 'D' || data->friend.cannon == 'd'){
                mvwaddch(game_win, data->friend.y, data->friend.x + 1, '-');
            }
            if(data->friend.cannon == 'C' || data->friend.cannon == 'c'){
                mvwaddch(game_win, data->friend.y + 1, data->friend.x + 1, '\\');
            }
            if(data->friend.cannon == 'X' || data->friend.cannon == 'x'){
                mvwaddch(game_win, data->friend.y + 1, data->friend.x, '|');
            }
            if(data->friend.cannon == 'Z' || data->friend.cannon == 'z'){
                mvwaddch(game_win, data->friend.y + 1, data->friend.x - 1, '/');
            }
            if(data->friend.cannon == 'A' || data->friend.cannon == 'a'){
                mvwaddch(game_win, data->friend.y, data->friend.x - 1, '-');
            }
            wattroff(game_win, COLOR_PAIR(FRIEND_COLOR));
            wattroff(game_win, COLOR_PAIR(MAIN_COLOR));
        }
        if(data->friend.key == KEY_LEFT){
            begin = 1;
            if(data->friend.condition_player == 0){
                wattron(game_win, COLOR_PAIR(FRIEND_COLOR));
            }
            if(data->friend.condition_player == 1){
                wattron(game_win, COLOR_PAIR(MAIN_COLOR));
            }
            mvwaddch(game_win, data->friend.y - 1, data->friend.x - 1, '*');
            mvwaddch(game_win, data->friend.y - 1, data->friend.x, '*');
            mvwaddch(game_win, data->friend.y - 1, data->friend.x + 1, '*');
            mvwaddch(game_win, data->friend.y, data->friend.x - 1, ' ');
            mvwaddch(game_win, data->friend.y, data->friend.x, '*');
            mvwaddch(game_win, data->friend.y, data->friend.x + 1, '*');
            mvwaddch(game_win, data->friend.y + 1, data->friend.x - 1, '*');
            mvwaddch(game_win, data->friend.y + 1, data->friend.x, '*');
            mvwaddch(game_win, data->friend.y + 1, data->friend.x + 1, '*');
            if(data->friend.cannon == 'Q' || data->friend.cannon == 'q'){
                mvwaddch(game_win, data->friend.y - 1, data->friend.x - 1, '\\');
            }
            if(data->friend.cannon == 'W' || data->friend.cannon == 'w'){
                mvwaddch(game_win, data->friend.y - 1, data->friend.x, '|');
            }
            if(data->friend.cannon == 'E' || data->friend.cannon == 'e'){
                mvwaddch(game_win, data->friend.y - 1, data->friend.x + 1, '/');
            }
            if(data->friend.cannon == 'D' || data->friend.cannon == 'd'){
                mvwaddch(game_win, data->friend.y, data->friend.x + 1, '-');
            }
            if(data->friend.cannon == 'C' || data->friend.cannon == 'c'){
                mvwaddch(game_win, data->friend.y + 1, data->friend.x + 1, '\\');
            }
            if(data->friend.cannon == 'X' || data->friend.cannon == 'x'){
                mvwaddch(game_win, data->friend.y + 1, data->friend.x, '|');
            }
            if(data->friend.cannon == 'Z' || data->friend.cannon == 'z'){
                mvwaddch(game_win, data->friend.y + 1, data->friend.x - 1, '/');
            }
            if(data->friend.cannon == 'A' || data->friend.cannon == 'a'){
                mvwaddch(game_win, data->friend.y, data->friend.x - 1, '-');
            }
            wattroff(game_win, COLOR_PAIR(FRIEND_COLOR));
            wattroff(game_win, COLOR_PAIR(MAIN_COLOR));
        }
        
        if(data->enemy.key == KEY_DOWN){
            begin = 1;

            if(data->enemy.condition_player == 0){
                wattron(game_win, COLOR_PAIR(ENEMY_COLOR));
            }
            if(data->enemy.condition_player == 1){
                wattron(game_win, COLOR_PAIR(MAIN_COLOR));
            }

            mvwaddch(game_win, data->enemy.y - 1, data->enemy.x - 1, '*');
            mvwaddch(game_win, data->enemy.y - 1, data->enemy.x, '*');
            mvwaddch(game_win, data->enemy.y - 1, data->enemy.x + 1, '*');
            mvwaddch(game_win, data->enemy.y, data->enemy.x - 1, '*');
            mvwaddch(game_win, data->enemy.y, data->enemy.x, '*');
            mvwaddch(game_win, data->enemy.y, data->enemy.x + 1, '*');
            mvwaddch(game_win, data->enemy.y + 1, data->enemy.x - 1, '*');
            mvwaddch(game_win, data->enemy.y + 1, data->enemy.x, ' ');
            mvwaddch(game_win, data->enemy.y + 1, data->enemy.x + 1, '*');
            if(data->enemy.cannon == 'Q' || data->enemy.cannon == 'q'){
                mvwaddch(game_win, data->enemy.y - 1, data->enemy.x - 1, '\\');
            }
            if(data->enemy.cannon == 'W' || data->enemy.cannon == 'w'){
                mvwaddch(game_win, data->enemy.y - 1, data->enemy.x, '|');
            }
            if(data->enemy.cannon == 'E' || data->enemy.cannon == 'e'){
                mvwaddch(game_win, data->enemy.y - 1, data->enemy.x + 1, '/');
            }
            if(data->enemy.cannon == 'D' || data->enemy.cannon == 'd'){
                mvwaddch(game_win, data->enemy.y, data->enemy.x + 1, '-');
            }
            if(data->enemy.cannon == 'C' || data->enemy.cannon == 'c'){
                mvwaddch(game_win, data->enemy.y + 1, data->enemy.x + 1, '\\');
            }
            if(data->enemy.cannon == 'X' || data->enemy.cannon == 'x'){
                mvwaddch(game_win, data->enemy.y + 1, data->enemy.x, '|');
            }
            if(data->enemy.cannon == 'Z' || data->enemy.cannon == 'z'){
                mvwaddch(game_win, data->enemy.y + 1, data->enemy.x - 1, '/');
            }
            if(data->enemy.cannon == 'A' || data->enemy.cannon == 'a'){
                mvwaddch(game_win, data->enemy.y, data->enemy.x - 1, '-');
            }
            wattroff(game_win, COLOR_PAIR(ENEMY_COLOR));
            wattroff(game_win, COLOR_PAIR(MAIN_COLOR));
        }
        if (data->enemy.key == KEY_RIGHT)
        {
            begin = 1;

            if(data->enemy.condition_player == 0){
                wattron(game_win, COLOR_PAIR(ENEMY_COLOR));
            }
            if(data->enemy.condition_player == 1){
                wattron(game_win, COLOR_PAIR(MAIN_COLOR));
            }

            mvwaddch(game_win, data->enemy.y - 1, data->enemy.x - 1, '*');
            mvwaddch(game_win, data->enemy.y - 1, data->enemy.x, '*');
            mvwaddch(game_win, data->enemy.y - 1, data->enemy.x + 1, '*');
            mvwaddch(game_win, data->enemy.y, data->enemy.x - 1, '*');
            mvwaddch(game_win, data->enemy.y, data->enemy.x, '*');
            mvwaddch(game_win, data->enemy.y, data->enemy.x + 1, ' ');
            mvwaddch(game_win, data->enemy.y + 1, data->enemy.x - 1, '*');
            mvwaddch(game_win, data->enemy.y + 1, data->enemy.x, '*');
            mvwaddch(game_win, data->enemy.y + 1, data->enemy.x + 1, '*');
            if(data->enemy.cannon == 'Q' || data->enemy.cannon == 'q'){
                mvwaddch(game_win, data->enemy.y - 1, data->enemy.x - 1, '\\');
            }
            if(data->enemy.cannon == 'W' || data->enemy.cannon == 'w'){
                mvwaddch(game_win, data->enemy.y - 1, data->enemy.x, '|');
            }
            if(data->enemy.cannon == 'E' || data->enemy.cannon == 'e'){
                mvwaddch(game_win, data->enemy.y - 1, data->enemy.x + 1, '/');
            }
            if(data->enemy.cannon == 'D' || data->enemy.cannon == 'd'){
                mvwaddch(game_win, data->enemy.y, data->enemy.x + 1, '-');
            }
            if(data->enemy.cannon == 'C' || data->enemy.cannon == 'c'){
                mvwaddch(game_win, data->enemy.y + 1, data->enemy.x + 1, '\\');
            }
            if(data->enemy.cannon == 'X' || data->enemy.cannon == 'x'){
                mvwaddch(game_win, data->enemy.y + 1, data->enemy.x, '|');
            }
            if(data->enemy.cannon == 'Z' || data->enemy.cannon == 'z'){
                mvwaddch(game_win, data->enemy.y + 1, data->enemy.x - 1, '/');
            }
            if(data->enemy.cannon == 'A' || data->enemy.cannon == 'a'){
                mvwaddch(game_win, data->enemy.y, data->enemy.x - 1, '-');
            }
            wattroff(game_win, COLOR_PAIR(ENEMY_COLOR));
            wattroff(game_win, COLOR_PAIR(MAIN_COLOR));
        }
        if(data->enemy.key == KEY_LEFT){
            begin = 1;

            if(data->enemy.condition_player == 0){
                wattron(game_win, COLOR_PAIR(ENEMY_COLOR));
            }
            if(data->enemy.condition_player == 1){
                wattron(game_win, COLOR_PAIR(MAIN_COLOR));
            }

            mvwaddch(game_win, data->enemy.y - 1, data->enemy.x - 1, '*');
            mvwaddch(game_win, data->enemy.y - 1, data->enemy.x, '*');
            mvwaddch(game_win, data->enemy.y - 1, data->enemy.x + 1, '*');
            mvwaddch(game_win, data->enemy.y, data->enemy.x - 1, ' ');
            mvwaddch(game_win, data->enemy.y, data->enemy.x, '*');
            mvwaddch(game_win, data->enemy.y, data->enemy.x + 1, '*');
            mvwaddch(game_win, data->enemy.y + 1, data->enemy.x - 1, '*');
            mvwaddch(game_win, data->enemy.y + 1, data->enemy.x, '*');
            mvwaddch(game_win, data->enemy.y + 1, data->enemy.x + 1, '*');
            if(data->enemy.cannon == 'Q' || data->enemy.cannon == 'q'){
                mvwaddch(game_win, data->enemy.y - 1, data->enemy.x - 1, '\\');
            }
            if(data->enemy.cannon == 'W' || data->enemy.cannon == 'w'){
                mvwaddch(game_win, data->enemy.y - 1, data->enemy.x, '|');
            }
            if(data->enemy.cannon == 'E' || data->enemy.cannon == 'e'){
                mvwaddch(game_win, data->enemy.y - 1, data->enemy.x + 1, '/');
            }
            if(data->enemy.cannon == 'D' || data->enemy.cannon == 'd'){
                mvwaddch(game_win, data->enemy.y, data->enemy.x + 1, '-');
            }
            if(data->enemy.cannon == 'C' || data->enemy.cannon == 'c'){
                mvwaddch(game_win, data->enemy.y + 1, data->enemy.x + 1, '\\');
            }
            if(data->enemy.cannon == 'X' || data->enemy.cannon == 'x'){
                mvwaddch(game_win, data->enemy.y + 1, data->enemy.x, '|');
            }
            if(data->enemy.cannon == 'Z' || data->enemy.cannon == 'z'){
                mvwaddch(game_win, data->enemy.y + 1, data->enemy.x - 1, '/');
            }
            if(data->enemy.cannon == 'A' || data->enemy.cannon == 'a'){
                mvwaddch(game_win, data->enemy.y, data->enemy.x - 1, '-');
            }
            wattroff(game_win, COLOR_PAIR(ENEMY_COLOR));
            wattroff(game_win, COLOR_PAIR(MAIN_COLOR));
        }
        for(int i = 0; i < 3; i++){
            if(data->friend.condition_ammunition[i] == 1){
                wattron(game_win, COLOR_PAIR(FRIEND_COLOR));
                mvwaddch(game_win, data->friend.y_ammunition[i], data->friend.x_ammunition[i], '*');
                wattroff(game_win, COLOR_PAIR(FRIEND_COLOR));
            }
            if(data->enemy.condition_ammunition[i] == 1){
                wattron(game_win, COLOR_PAIR(ENEMY_COLOR));
                mvwaddch(game_win, data->enemy.y_ammunition[i], data->enemy.x_ammunition[i], '*');
                wattroff(game_win, COLOR_PAIR(ENEMY_COLOR));
            }
            
        }
        
        if(count_game == 1 && data->friend.condition_player == 1){
            wattron(game_win, COLOR_PAIR(ENEMY_COLOR));
            mvwaddstr(game_win, HEIGHT / 2, WIDTH / 2 - 6, blue_win);
            wattroff(game_win, COLOR_PAIR(ENEMY_COLOR));
            mvwaddstr(game_win, HEIGHT - 2, WIDTH / 2 - 13, exit_win);
        }
        
        if(count_game == 1 && data->enemy.condition_player == 1){
            wattron(game_win, COLOR_PAIR(FRIEND_COLOR));
            mvwaddstr(game_win, HEIGHT / 2, WIDTH / 2 - 5, red_win);
            wattroff(game_win, COLOR_PAIR(FRIEND_COLOR));
            mvwaddstr(game_win, HEIGHT - 2, WIDTH / 2 - 13, exit_win);
        }
        
        

        wrefresh(game_win);

        if(data->friend.condition_player == 1 || data->enemy.condition_player == 1){
            usleep(16000);
        }

        data->friend.condition_player = 0;
        data->enemy.condition_player = 0;

        pthread_mutex_unlock(&mutex);
    }
}

int main(int argc, char *argv[])
{
    int height, widht;
    if (argc != 3)
    {
        printf("Missing player's IP addresses.\n");
        return 0;
    }
    if ((LINES > HEIGHT || COLS > WIDTH) && 1)
    {
        endwin();
        printf("Size of screen is too small. Min size: %dx%d\n", WIDTH, HEIGHT);
        return 0;
    }

    pthread_mutex_init(&mutex, NULL);
    pthread_mutex_lock(&mutex);
    great_data threadData;
    threadData.friend.ip = argv[1];
    threadData.enemy.ip = argv[2];
    threadData.friend.condition_player = 0;
    threadData.enemy.condition_player = 0;
    threadData.friend.cannon = 'd';
    threadData.enemy.cannon = 'd';
    threadData.friend.key = 'd';
    threadData.enemy.key = 'd';
    threadData.friend.x = 25;
    threadData.friend.y = 20;
    threadData.enemy.x = 75;
    threadData.enemy.y = 20;
    for(int i = 0; i < 3; i++){
        threadData.friend.condition_ammunition[i] = 0;
        threadData.enemy.condition_ammunition[i] = 0;
    }
    threadData.friend.end = 0;
    threadData.friend.start = 0;
    threadData.enemy.end = 0;
    threadData.enemy.start = 0;

    int ch;
    int time;

    pthread_t threads_render;
	pthread_t threads_tank;
    pthread_t threads_strike;


	initscr();
	curs_set(0);
	noecho();
	keypad(stdscr,TRUE);

    game_win = newwin(HEIGHT + 2, WIDTH + 2, (LINES - HEIGHT) / 2 - 1, (COLS - WIDTH) / 2 - 1);
    box(game_win, 0, 0);
    wrefresh(game_win);

    keypad(game_win, true); // Корректный вывод системных кпопок
    touchwin(game_win);

    start_color();
    init_pair(MAIN_COLOR, COLOR_WHITE, COLOR_BLACK);
    init_pair(FRIEND_COLOR, COLOR_RED, COLOR_BLACK);
    init_pair(ENEMY_COLOR, COLOR_BLUE, COLOR_BLACK);

    int sock, yes = 1;
    pthread_t pid;
    struct sockaddr_in addr;
    int addr_len;
    int count;
    int ret;
    fd_set readfd;
    char buffer[1024];
    int i;

    

    

    sock = socket(AF_INET, SOCK_DGRAM, 0);
    if (sock < 0)
    {
        perror("sock");
        return -1;
    }

    // Нужно для работы моей VPN сети
    #ifdef VPN_ENABLED
    char *devname = "tap0";
    ret = setsockopt(sock, SOL_SOCKET, SO_BINDTODEVICE, devname,
    strlen(devname));
    if (ret == -1)
    {
        perror("setsockopt");
    return 0;
    }
    #endif

    ret = setsockopt(sock, SOL_SOCKET, SO_BROADCAST, (char *)&yes, sizeof(yes));
    if (ret == -1)
    {
        perror("setsockopt");
        return 0;
    }

    addr_len = sizeof(struct sockaddr_in);
    memset((void *)&addr, 0, addr_len);
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = htonl(INADDR_BROADCAST);
    addr.sin_port = htons(PORT);

    
    
    pthread_mutex_unlock(&mutex);
    pthread_create(&pid, NULL, udp_server, &threadData);
    pthread_create(&threads_render, NULL, render, &threadData);
    pthread_create(&threads_strike, NULL, strike, &threadData);

    

    while (work_flag)
    {
        
        ch = getch();
       
        sendto(sock, &ch, sizeof(ch), 0, (struct sockaddr *)&addr, addr_len);
        if(count_game == 1){
            work_flag = 0;
            break;
        }
    }
    pthread_join(pid, NULL);
    pthread_join(threads_render, NULL);
    pthread_join(threads_strike, NULL);
    close(sock);
    wgetch(game_win);
    delwin(game_win);
    endwin();
    pthread_mutex_destroy(&mutex);
    return 0;
}