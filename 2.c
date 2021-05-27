#include <pthread.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/select.h>
#include <string.h>
#include <unistd.h>
#include <stdio.h>
#include <ncurses.h>
#ifndef PORT
#define PORT 9999
#endif

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

int count_game = 0;

typedef struct{
    int x;
    int y;
    int key;
    int cannon;
    int memory_canon;
    int x_ammunition;
    int y_ammunition;
    int condition_ammunition;
    int start;
    int end;
    char* ip_1;
    char* ip_2;
    int condition_player_1;
    int condition_player_2;
}possition;

typedef struct{
    possition friend;
    possition enemy;
}great_data;


int work_flag = 1;

great_data tank(int ch, great_data* data){
    if(data->friend.key == KEY_UP){
        data->friend.y--;
    }
    if(data->friend.key == KEY_DOWN){
        data->friend.y++;
    }
    if (data->friend.key == KEY_RIGHT)
    {
        data->friend.x++;
    }
    if(data->friend.key == KEY_LEFT){
        data->friend.x--;
    }
    if(data->enemy.key == KEY_UP){
        data->enemy.y--;
    }
    if(data->enemy.key == KEY_DOWN){
        data->enemy.y++;
    }
    if (data->enemy.key == KEY_RIGHT)
    {
        data->enemy.x++;
    }
    if(data->enemy.key == KEY_LEFT){
        data->enemy.x--;
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
                if(!strcmp(data->friend.ip_1, inet_ntoa(addr.sin_addr))){
                    if(ch == KEY_LEFT || ch == KEY_RIGHT || ch == KEY_UP || ch == KEY_DOWN){
                        data->friend.key = ch;
                    }
                    if(ch == 'Q' || ch == 'W' || ch == 'E' || ch == 'D' || ch == 'C' || ch == 'X' || ch == 'Z' || ch == 'A' || ch == 'q' || ch == 'w' || ch == 'e' || ch == 'd' || ch == 'c' || ch == 'x' || ch == 'z' || ch == 'a'){
                        data->friend.cannon = ch;
                    }
                    tank(ch, data);
                }
                if(!strcmp(data->enemy.ip_2, inet_ntoa(addr.sin_addr))){
                    if(ch == KEY_LEFT || ch == KEY_RIGHT || ch == KEY_UP || ch == KEY_DOWN){
                        data->enemy.key = ch;
                    }
                    if(ch == 'Q' || ch == 'W' || ch == 'E' || ch == 'D' || ch == 'C' || ch == 'X' || ch == 'Z' || ch == 'A' || ch == 'q' || ch == 'w' || ch == 'e' || ch == 'd' || ch == 'c' || ch == 'x' || ch == 'z' || ch == 'a'){
                        data->enemy.cannon = ch;
                    }
                    tank(ch, data);
                }
                
                /*fprintf(stderr, "\rClient with IP %s send %d bytes: %s\n", inet_ntoa(addr.sin_addr),
                        count, buffer);*/
            }
        }
    }
    close(sock);
    return (void *)NULL;
}

void* render(void *thread_data){
    great_data *data = (great_data *)thread_data;

    while(count_game != 1){
        usleep(16000);
        pthread_mutex_lock(&mutex);
        clear();
        if(data->friend.key == KEY_UP){
            mvaddch(data->friend.y - 1, data->friend.x - 1, '*');
            mvaddch(data->friend.y - 1, data->friend.x, ' ');
            mvaddch(data->friend.y - 1, data->friend.x + 1, '*');
            mvaddch(data->friend.y, data->friend.x - 1, '*');
            mvaddch(data->friend.y, data->friend.x, '*');
            mvaddch(data->friend.y, data->friend.x + 1, '*');
            mvaddch(data->friend.y + 1, data->friend.x - 1, '*');
            mvaddch(data->friend.y + 1, data->friend.x, '*');
            mvaddch(data->friend.y + 1, data->friend.x + 1, '*');
            if(data->friend.cannon == 'Q' || data->friend.cannon == 'q'){
                mvaddch(data->friend.y - 1, data->friend.x - 1, '\\');
            }
            if(data->friend.cannon == 'W' || data->friend.cannon == 'w'){
                mvaddch(data->friend.y - 1, data->friend.x, '|');
            }
            if(data->friend.cannon == 'E' || data->friend.cannon == 'e'){
                mvaddch(data->friend.y - 1, data->friend.x + 1, '/');
            }
            if(data->friend.cannon == 'D' || data->friend.cannon == 'd'){
                mvaddch(data->friend.y, data->friend.x + 1, '-');
            }
            if(data->friend.cannon == 'C' || data->friend.cannon == 'c'){
                mvaddch(data->friend.y + 1, data->friend.x + 1, '\\');
            }
            if(data->friend.cannon == 'X' || data->friend.cannon == 'x'){
                mvaddch(data->friend.y + 1, data->friend.x, '|');
            }
            if(data->friend.cannon == 'Z' || data->friend.cannon == 'z'){
                mvaddch(data->friend.y + 1, data->friend.x - 1, '/');
            }
            if(data->friend.cannon == 'A' || data->friend.cannon == 'a'){
                mvaddch(data->friend.y, data->friend.x - 1, '-');
            }
        }
        if(data->friend.key == KEY_DOWN){
            mvaddch(data->friend.y - 1, data->friend.x - 1, '*');
            mvaddch(data->friend.y - 1, data->friend.x, '*');
            mvaddch(data->friend.y - 1, data->friend.x + 1, '*');
            mvaddch(data->friend.y, data->friend.x - 1, '*');
            mvaddch(data->friend.y, data->friend.x, '*');
            mvaddch(data->friend.y, data->friend.x + 1, '*');
            mvaddch(data->friend.y + 1, data->friend.x - 1, '*');
            mvaddch(data->friend.y + 1, data->friend.x, ' ');
            mvaddch(data->friend.y + 1, data->friend.x + 1, '*');
            if(data->friend.cannon == 'Q' || data->friend.cannon == 'q'){
                mvaddch(data->friend.y - 1, data->friend.x - 1, '\\');
            }
            if(data->friend.cannon == 'W' || data->friend.cannon == 'w'){
                mvaddch(data->friend.y - 1, data->friend.x, '|');
            }
            if(data->friend.cannon == 'E' || data->friend.cannon == 'e'){
                mvaddch(data->friend.y - 1, data->friend.x + 1, '/');
            }
            if(data->friend.cannon == 'D' || data->friend.cannon == 'd'){
                mvaddch(data->friend.y, data->friend.x + 1, '-');
            }
            if(data->friend.cannon == 'C' || data->friend.cannon == 'c'){
                mvaddch(data->friend.y + 1, data->friend.x + 1, '\\');
            }
            if(data->friend.cannon == 'X' || data->friend.cannon == 'x'){
                mvaddch(data->friend.y + 1, data->friend.x, '|');
            }
            if(data->friend.cannon == 'Z' || data->friend.cannon == 'z'){
                mvaddch(data->friend.y + 1, data->friend.x - 1, '/');
            }
            if(data->friend.cannon == 'A' || data->friend.cannon == 'a'){
                mvaddch(data->friend.y, data->friend.x - 1, '-');
            }
        }
        if (data->friend.key == KEY_RIGHT)
        {
            mvaddch(data->friend.y - 1, data->friend.x - 1, '*');
            mvaddch(data->friend.y - 1, data->friend.x, '*');
            mvaddch(data->friend.y - 1, data->friend.x + 1, '*');
            mvaddch(data->friend.y, data->friend.x - 1, '*');
            mvaddch(data->friend.y, data->friend.x, '*');
            mvaddch(data->friend.y, data->friend.x + 1, ' ');
            mvaddch(data->friend.y + 1, data->friend.x - 1, '*');
            mvaddch(data->friend.y + 1, data->friend.x, '*');
            mvaddch(data->friend.y + 1, data->friend.x + 1, '*');
            if(data->friend.cannon == 'Q' || data->friend.cannon == 'q'){
                mvaddch(data->friend.y - 1, data->friend.x - 1, '\\');
            }
            if(data->friend.cannon == 'W' || data->friend.cannon == 'w'){
                mvaddch(data->friend.y - 1, data->friend.x, '|');
            }
            if(data->friend.cannon == 'E' || data->friend.cannon == 'e'){
                mvaddch(data->friend.y - 1, data->friend.x + 1, '/');
            }
            if(data->friend.cannon == 'D' || data->friend.cannon == 'd'){
                mvaddch(data->friend.y, data->friend.x + 1, '-');
            }
            if(data->friend.cannon == 'C' || data->friend.cannon == 'c'){
                mvaddch(data->friend.y + 1, data->friend.x + 1, '\\');
            }
            if(data->friend.cannon == 'X' || data->friend.cannon == 'x'){
                mvaddch(data->friend.y + 1, data->friend.x, '|');
            }
            if(data->friend.cannon == 'Z' || data->friend.cannon == 'z'){
                mvaddch(data->friend.y + 1, data->friend.x - 1, '/');
            }
            if(data->friend.cannon == 'A' || data->friend.cannon == 'a'){
                mvaddch(data->friend.y, data->friend.x - 1, '-');
            }
        }
        if(data->friend.key == KEY_LEFT){
            mvaddch(data->friend.y - 1, data->friend.x - 1, '*');
            mvaddch(data->friend.y - 1, data->friend.x, '*');
            mvaddch(data->friend.y - 1, data->friend.x + 1, '*');
            mvaddch(data->friend.y, data->friend.x - 1, ' ');
            mvaddch(data->friend.y, data->friend.x, '*');
            mvaddch(data->friend.y, data->friend.x + 1, '*');
            mvaddch(data->friend.y + 1, data->friend.x - 1, '*');
            mvaddch(data->friend.y + 1, data->friend.x, '*');
            mvaddch(data->friend.y + 1, data->friend.x + 1, '*');
            if(data->friend.cannon == 'Q' || data->friend.cannon == 'q'){
                mvaddch(data->friend.y - 1, data->friend.x - 1, '\\');
            }
            if(data->friend.cannon == 'W' || data->friend.cannon == 'w'){
                mvaddch(data->friend.y - 1, data->friend.x, '|');
            }
            if(data->friend.cannon == 'E' || data->friend.cannon == 'e'){
                mvaddch(data->friend.y - 1, data->friend.x + 1, '/');
            }
            if(data->friend.cannon == 'D' || data->friend.cannon == 'd'){
                mvaddch(data->friend.y, data->friend.x + 1, '-');
            }
            if(data->friend.cannon == 'C' || data->friend.cannon == 'c'){
                mvaddch(data->friend.y + 1, data->friend.x + 1, '\\');
            }
            if(data->friend.cannon == 'X' || data->friend.cannon == 'x'){
                mvaddch(data->friend.y + 1, data->friend.x, '|');
            }
            if(data->friend.cannon == 'Z' || data->friend.cannon == 'z'){
                mvaddch(data->friend.y + 1, data->friend.x - 1, '/');
            }
            if(data->friend.cannon == 'A' || data->friend.cannon == 'a'){
                mvaddch(data->friend.y, data->friend.x - 1, '-');
            }
        }
        if(data->enemy.key == KEY_UP){
            mvaddch(data->enemy.y - 1, data->enemy.x - 1, '*');
            mvaddch(data->enemy.y - 1, data->enemy.x, ' ');
            mvaddch(data->enemy.y - 1, data->friend.x + 1, '*');
            mvaddch(data->enemy.y, data->enemy.x - 1, '*');
            mvaddch(data->enemy.y, data->enemy.x, '*');
            mvaddch(data->enemy.y, data->enemy.x + 1, '*');
            mvaddch(data->enemy.y + 1, data->enemy.x - 1, '*');
            mvaddch(data->enemy.y + 1, data->enemy.x, '*');
            mvaddch(data->enemy.y + 1, data->enemy.x + 1, '*');
            if(data->enemy.cannon == 'Q' || data->enemy.cannon == 'q'){
                mvaddch(data->enemy.y - 1, data->enemy.x - 1, '\\');
            }
            if(data->enemy.cannon == 'W' || data->enemy.cannon == 'w'){
                mvaddch(data->enemy.y - 1, data->enemy.x, '|');
            }
            if(data->enemy.cannon == 'E' || data->enemy.cannon == 'e'){
                mvaddch(data->enemy.y - 1, data->enemy.x + 1, '/');
            }
            if(data->enemy.cannon == 'D' || data->enemy.cannon == 'd'){
                mvaddch(data->enemy.y, data->enemy.x + 1, '-');
            }
            if(data->enemy.cannon == 'C' || data->enemy.cannon == 'c'){
                mvaddch(data->enemy.y + 1, data->enemy.x + 1, '\\');
            }
            if(data->enemy.cannon == 'X' || data->enemy.cannon == 'x'){
                mvaddch(data->enemy.y + 1, data->enemy.x, '|');
            }
            if(data->enemy.cannon == 'Z' || data->enemy.cannon == 'z'){
                mvaddch(data->enemy.y + 1, data->enemy.x - 1, '/');
            }
            if(data->enemy.cannon == 'A' || data->enemy.cannon == 'a'){
                mvaddch(data->enemy.y, data->enemy.x - 1, '-');
            }
        }
        if(data->enemy.key == KEY_DOWN){
            mvaddch(data->enemy.y - 1, data->enemy.x - 1, '*');
            mvaddch(data->enemy.y - 1, data->enemy.x, '*');
            mvaddch(data->enemy.y - 1, data->friend.x + 1, '*');
            mvaddch(data->enemy.y, data->enemy.x - 1, '*');
            mvaddch(data->enemy.y, data->enemy.x, '*');
            mvaddch(data->enemy.y, data->enemy.x + 1, '*');
            mvaddch(data->enemy.y + 1, data->enemy.x - 1, '*');
            mvaddch(data->enemy.y + 1, data->enemy.x, ' ');
            mvaddch(data->enemy.y + 1, data->enemy.x + 1, '*');
            if(data->enemy.cannon == 'Q' || data->enemy.cannon == 'q'){
                mvaddch(data->enemy.y - 1, data->enemy.x - 1, '\\');
            }
            if(data->enemy.cannon == 'W' || data->enemy.cannon == 'w'){
                mvaddch(data->enemy.y - 1, data->enemy.x, '|');
            }
            if(data->enemy.cannon == 'E' || data->enemy.cannon == 'e'){
                mvaddch(data->enemy.y - 1, data->enemy.x + 1, '/');
            }
            if(data->enemy.cannon == 'D' || data->enemy.cannon == 'd'){
                mvaddch(data->enemy.y, data->enemy.x + 1, '-');
            }
            if(data->enemy.cannon == 'C' || data->enemy.cannon == 'c'){
                mvaddch(data->enemy.y + 1, data->enemy.x + 1, '\\');
            }
            if(data->enemy.cannon == 'X' || data->enemy.cannon == 'x'){
                mvaddch(data->enemy.y + 1, data->enemy.x, '|');
            }
            if(data->enemy.cannon == 'Z' || data->enemy.cannon == 'z'){
                mvaddch(data->enemy.y + 1, data->enemy.x - 1, '/');
            }
            if(data->enemy.cannon == 'A' || data->enemy.cannon == 'a'){
                mvaddch(data->enemy.y, data->enemy.x - 1, '-');
            }
        }
        if (data->enemy.key == KEY_RIGHT)
        {
            mvaddch(data->enemy.y - 1, data->enemy.x - 1, '*');
            mvaddch(data->enemy.y - 1, data->enemy.x, '*');
            mvaddch(data->enemy.y - 1, data->friend.x + 1, '*');
            mvaddch(data->enemy.y, data->enemy.x - 1, '*');
            mvaddch(data->enemy.y, data->enemy.x, '*');
            mvaddch(data->enemy.y, data->enemy.x + 1, ' ');
            mvaddch(data->enemy.y + 1, data->enemy.x - 1, '*');
            mvaddch(data->enemy.y + 1, data->enemy.x, '*');
            mvaddch(data->enemy.y + 1, data->enemy.x + 1, '*');
            if(data->enemy.cannon == 'Q' || data->enemy.cannon == 'q'){
                mvaddch(data->enemy.y - 1, data->enemy.x - 1, '\\');
            }
            if(data->enemy.cannon == 'W' || data->enemy.cannon == 'w'){
                mvaddch(data->enemy.y - 1, data->enemy.x, '|');
            }
            if(data->enemy.cannon == 'E' || data->enemy.cannon == 'e'){
                mvaddch(data->enemy.y - 1, data->enemy.x + 1, '/');
            }
            if(data->enemy.cannon == 'D' || data->enemy.cannon == 'd'){
                mvaddch(data->enemy.y, data->enemy.x + 1, '-');
            }
            if(data->enemy.cannon == 'C' || data->enemy.cannon == 'c'){
                mvaddch(data->enemy.y + 1, data->enemy.x + 1, '\\');
            }
            if(data->enemy.cannon == 'X' || data->enemy.cannon == 'x'){
                mvaddch(data->enemy.y + 1, data->enemy.x, '|');
            }
            if(data->enemy.cannon == 'Z' || data->enemy.cannon == 'z'){
                mvaddch(data->enemy.y + 1, data->enemy.x - 1, '/');
            }
            if(data->enemy.cannon == 'A' || data->enemy.cannon == 'a'){
                mvaddch(data->enemy.y, data->enemy.x - 1, '-');
            }
        }
        if(data->enemy.key == KEY_LEFT){
            mvaddch(data->enemy.y - 1, data->enemy.x - 1, '*');
            mvaddch(data->enemy.y - 1, data->enemy.x, '*');
            mvaddch(data->enemy.y - 1, data->friend.x + 1, '*');
            mvaddch(data->enemy.y, data->enemy.x - 1, ' ');
            mvaddch(data->enemy.y, data->enemy.x, '*');
            mvaddch(data->enemy.y, data->enemy.x + 1, '*');
            mvaddch(data->enemy.y + 1, data->enemy.x - 1, '*');
            mvaddch(data->enemy.y + 1, data->enemy.x, '*');
            mvaddch(data->enemy.y + 1, data->enemy.x + 1, '*');
            if(data->enemy.cannon == 'Q' || data->enemy.cannon == 'q'){
                mvaddch(data->enemy.y - 1, data->enemy.x - 1, '\\');
            }
            if(data->enemy.cannon == 'W' || data->enemy.cannon == 'w'){
                mvaddch(data->enemy.y - 1, data->enemy.x, '|');
            }
            if(data->enemy.cannon == 'E' || data->enemy.cannon == 'e'){
                mvaddch(data->enemy.y - 1, data->enemy.x + 1, '/');
            }
            if(data->enemy.cannon == 'D' || data->enemy.cannon == 'd'){
                mvaddch(data->enemy.y, data->enemy.x + 1, '-');
            }
            if(data->enemy.cannon == 'C' || data->enemy.cannon == 'c'){
                mvaddch(data->enemy.y + 1, data->enemy.x + 1, '\\');
            }
            if(data->enemy.cannon == 'X' || data->enemy.cannon == 'x'){
                mvaddch(data->enemy.y + 1, data->enemy.x, '|');
            }
            if(data->enemy.cannon == 'Z' || data->enemy.cannon == 'z'){
                mvaddch(data->enemy.y + 1, data->enemy.x - 1, '/');
            }
            if(data->enemy.cannon == 'A' || data->enemy.cannon == 'a'){
                mvaddch(data->enemy.y, data->enemy.x - 1, '-');
            }
        }

        mvaddch(data->friend.y_ammunition, data->friend.x_ammunition, '*');
        mvaddch(data->enemy.y_ammunition, data->enemy.x_ammunition, '*');

        refresh();


        pthread_mutex_unlock(&mutex);
    }
}

int main(int argc, char *argv[])
{
    pthread_mutex_init(&mutex, NULL);

    great_data threadData;
    threadData.friend.ip_1 = argv[0];
    threadData.enemy.ip_2 = argv[1];
    threadData.friend.condition_player_1 = 0;
    threadData.enemy.condition_player_2 = 0;
    threadData.friend.x = 1;
    threadData.friend.y = 10;
    threadData.enemy.x = 40;
    threadData.enemy.y = 10;
    threadData.friend.condition_ammunition = 0;
    threadData.enemy.condition_ammunition = 0;
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

    int sock, yes = 1;
    pthread_t pid;
    struct sockaddr_in addr;
    int addr_len;
    int count;
    int ret;
    fd_set readfd;
    char buffer[1024];
    int i;

    

    pthread_create(&pid, NULL, udp_server, &threadData);
    pthread_create(&threads_render, NULL, render, &threadData);

    sock = socket(AF_INET, SOCK_DGRAM, 0);
    if (sock < 0)
    {
        perror("sock");
        return -1;
    }

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

    
    mvaddch(threadData.friend.y - 1, threadData.friend.x - 1, '*');
    mvaddch(threadData.friend.y - 1, threadData.friend.x, '*');
    mvaddch(threadData.friend.y - 1, threadData.friend.x + 1, '*');
    mvaddch(threadData.friend.y, threadData.friend.x - 1, '*');
    mvaddch(threadData.friend.y, threadData.friend.x, '*');
    mvaddch(threadData.friend.y, threadData.friend.x + 1, '-');
    mvaddch(threadData.friend.y + 1, threadData.friend.x - 1, '*');
    mvaddch(threadData.friend.y + 1, threadData.friend.x, '*');
    mvaddch(threadData.friend.y + 1, threadData.friend.x + 1, '*');
    mvaddch(threadData.enemy.y - 1, threadData.enemy.x - 1, '*');
    mvaddch(threadData.enemy.y - 1, threadData.enemy.x, '*');
    mvaddch(threadData.enemy.y - 1, threadData.enemy.x + 1, '*');
    mvaddch(threadData.enemy.y, threadData.enemy.x - 1, '-');
    mvaddch(threadData.enemy.y, threadData.enemy.x, '*');
    mvaddch(threadData.enemy.y, threadData.enemy.x + 1, '*');
    mvaddch(threadData.enemy.y + 1, threadData.enemy.x - 1, '*');
    mvaddch(threadData.enemy.y + 1, threadData.enemy.x, '*');
    mvaddch(threadData.enemy.y + 1, threadData.enemy.x + 1, '*');
    refresh();

    while (work_flag)
    {
        ch = getch();
        sendto(sock, &ch, sizeof(ch), 0, (struct sockaddr *)&addr, addr_len);
    }
    pthread_join(pid, NULL);
    close(sock);

    return 0;
}
