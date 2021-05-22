#include <stdio.h>
#include <ncurses.h>
#include <pthread.h>
#include <malloc.h>
#include <unistd.h>
#include <time.h>

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
}possition;

void* tank(void *thread_data){
    possition *data = (possition *)thread_data;

    while(count_game != 1){
		sleep(1);
		pthread_mutex_lock(&mutex);
		if(data->key == KEY_UP){
            data->y--;
        }
        if(data->key == KEY_DOWN){
            data->y++;
        }
        if (data->key == KEY_RIGHT)
        {
            data->x++;
        }
        if(data->key == KEY_LEFT){
            data->x--;
        }
        
		pthread_mutex_unlock(&mutex);
	}
}
void* strike(void *thread_data){
    possition *data = (possition *)thread_data;
    while(count_game != 1){
		usleep(20000);
		pthread_mutex_lock(&mutex);
        
		if(data->condition_ammunition == 1){
            
            if(data->memory_canon == 'Q' || data->memory_canon == 'q'){
                data->y_ammunition--; 
                data->x_ammunition--;
            }
            if(data->memory_canon == 'W' || data->memory_canon == 'w'){
                data->y_ammunition--;
            }
            if(data->memory_canon == 'E' || data->memory_canon == 'e'){
                data->y_ammunition--; 
                data->x_ammunition++;
            }
            if(data->memory_canon == 'D' || data->memory_canon == 'd'){
                data->x_ammunition++;
            }
            if(data->memory_canon == 'C' || data->memory_canon == 'c'){
                data->y_ammunition++; 
                data->x_ammunition++;
            }
            if(data->memory_canon == 'X' || data->memory_canon == 'x'){
                data->y_ammunition++;
            }
            if(data->memory_canon == 'Z' || data->memory_canon == 'z'){
                data->y_ammunition++; 
                data->x_ammunition--;
            }
            if(data->memory_canon == 'A' || data->memory_canon == 'a'){
                data->x_ammunition--;
            }
            
        }
        data->end = clock();
        
        
		pthread_mutex_unlock(&mutex);
    }
}

void* render(void *thread_data){
    possition *data = (possition *)thread_data;

    while(count_game != 1){
        usleep(50000);
        pthread_mutex_lock(&mutex);
        clear();
        if(data->key == KEY_UP){
            mvaddch(data->y - 1, data->x - 1, '*');
            mvaddch(data->y - 1, data->x, ' ');
            mvaddch(data->y - 1, data->x + 1, '*');
            mvaddch(data->y, data->x - 1, '*');
            mvaddch(data->y, data->x, '*');
            mvaddch(data->y, data->x + 1, '*');
            mvaddch(data->y + 1, data->x - 1, '*');
            mvaddch(data->y + 1, data->x, '*');
            mvaddch(data->y + 1, data->x + 1, '*');
            if(data->cannon == 'Q' || data->cannon == 'q'){
                mvaddch(data->y - 1, data->x - 1, '\\');
            }
            if(data->cannon == 'W' || data->cannon == 'w'){
                mvaddch(data->y - 1, data->x, '|');
            }
            if(data->cannon == 'E' || data->cannon == 'e'){
                mvaddch(data->y - 1, data->x + 1, '/');
            }
            if(data->cannon == 'D' || data->cannon == 'd'){
                mvaddch(data->y, data->x + 1, '-');
            }
            if(data->cannon == 'C' || data->cannon == 'c'){
                mvaddch(data->y + 1, data->x + 1, '\\');
            }
            if(data->cannon == 'X' || data->cannon == 'x'){
                mvaddch(data->y + 1, data->x, '|');
            }
            if(data->cannon == 'Z' || data->cannon == 'z'){
                mvaddch(data->y + 1, data->x - 1, '/');
            }
            if(data->cannon == 'A' || data->cannon == 'a'){
                mvaddch(data->y, data->x - 1, '-');
            }
        }
        if(data->key == KEY_DOWN){
            mvaddch(data->y - 1, data->x - 1, '*');
            mvaddch(data->y - 1, data->x, '*');
            mvaddch(data->y - 1, data->x + 1, '*');
            mvaddch(data->y, data->x - 1, '*');
            mvaddch(data->y, data->x, '*');
            mvaddch(data->y, data->x + 1, '*');
            mvaddch(data->y + 1, data->x - 1, '*');
            mvaddch(data->y + 1, data->x, ' ');
            mvaddch(data->y + 1, data->x + 1, '*');
            if(data->cannon == 'Q' || data->cannon == 'q'){
                mvaddch(data->y - 1, data->x - 1, '\\');
            }
            if(data->cannon == 'W' || data->cannon == 'w'){
                mvaddch(data->y - 1, data->x, '|');
            }
            if(data->cannon == 'E' || data->cannon == 'e'){
                mvaddch(data->y - 1, data->x + 1, '/');
            }
            if(data->cannon == 'D' || data->cannon == 'd'){
                mvaddch(data->y, data->x + 1, '-');
            }
            if(data->cannon == 'C' || data->cannon == 'c'){
                mvaddch(data->y + 1, data->x + 1, '\\');
            }
            if(data->cannon == 'X' || data->cannon == 'x'){
                mvaddch(data->y + 1, data->x, '|');
            }
            if(data->cannon == 'Z' || data->cannon == 'z'){
                mvaddch(data->y + 1, data->x - 1, '/');
            }
            if(data->cannon == 'A' || data->cannon == 'a'){
                mvaddch(data->y, data->x - 1, '-');
            }
        }
        if (data->key == KEY_RIGHT)
        {
            mvaddch(data->y - 1, data->x - 1, '*');
            mvaddch(data->y - 1, data->x, '*');
            mvaddch(data->y - 1, data->x + 1, '*');
            mvaddch(data->y, data->x - 1, '*');
            mvaddch(data->y, data->x, '*');
            mvaddch(data->y, data->x + 1, ' ');
            mvaddch(data->y + 1, data->x - 1, '*');
            mvaddch(data->y + 1, data->x, '*');
            mvaddch(data->y + 1, data->x + 1, '*');
            if(data->cannon == 'Q' || data->cannon == 'q'){
                mvaddch(data->y - 1, data->x - 1, '\\');
            }
            if(data->cannon == 'W' || data->cannon == 'w'){
                mvaddch(data->y - 1, data->x, '|');
            }
            if(data->cannon == 'E' || data->cannon == 'e'){
                mvaddch(data->y - 1, data->x + 1, '/');
            }
            if(data->cannon == 'D' || data->cannon == 'd'){
                mvaddch(data->y, data->x + 1, '-');
            }
            if(data->cannon == 'C' || data->cannon == 'c'){
                mvaddch(data->y + 1, data->x + 1, '\\');
            }
            if(data->cannon == 'X' || data->cannon == 'x'){
                mvaddch(data->y + 1, data->x, '|');
            }
            if(data->cannon == 'Z' || data->cannon == 'z'){
                mvaddch(data->y + 1, data->x - 1, '/');
            }
            if(data->cannon == 'A' || data->cannon == 'a'){
                mvaddch(data->y, data->x - 1, '-');
            }
        }
        if(data->key == KEY_LEFT){
            mvaddch(data->y - 1, data->x - 1, '*');
            mvaddch(data->y - 1, data->x, '*');
            mvaddch(data->y - 1, data->x + 1, '*');
            mvaddch(data->y, data->x - 1, ' ');
            mvaddch(data->y, data->x, '*');
            mvaddch(data->y, data->x + 1, '*');
            mvaddch(data->y + 1, data->x - 1, '*');
            mvaddch(data->y + 1, data->x, '*');
            mvaddch(data->y + 1, data->x + 1, '*');
            if(data->cannon == 'Q' || data->cannon == 'q'){
                mvaddch(data->y - 1, data->x - 1, '\\');
            }
            if(data->cannon == 'W' || data->cannon == 'w'){
                mvaddch(data->y - 1, data->x, '|');
            }
            if(data->cannon == 'E' || data->cannon == 'e'){
                mvaddch(data->y - 1, data->x + 1, '/');
            }
            if(data->cannon == 'D' || data->cannon == 'd'){
                mvaddch(data->y, data->x + 1, '-');
            }
            if(data->cannon == 'C' || data->cannon == 'c'){
                mvaddch(data->y + 1, data->x + 1, '\\');
            }
            if(data->cannon == 'X' || data->cannon == 'x'){
                mvaddch(data->y + 1, data->x, '|');
            }
            if(data->cannon == 'Z' || data->cannon == 'z'){
                mvaddch(data->y + 1, data->x - 1, '/');
            }
            if(data->cannon == 'A' || data->cannon == 'a'){
                mvaddch(data->y, data->x - 1, '-');
            }
        }

        mvaddch(data->y_ammunition, data->x_ammunition, '*');
        

        refresh();


        pthread_mutex_unlock(&mutex);
    }
}

int main(){
    pthread_mutex_init(&mutex, NULL);
	int ch;
    int time;


	initscr();
	curs_set(0);
	noecho();
	keypad(stdscr,TRUE);

    pthread_t threads_render;
	pthread_t threads_tank;
    pthread_t threads_strike;
	possition threadData;

    threadData.x = 1;
    threadData.y = 10;
    threadData.condition_ammunition = 0;
    threadData.end = 0;
    threadData.start = 0;


    pthread_create(&threads_render, NULL, render, &threadData);
	pthread_create(&threads_tank, NULL, tank, &threadData);
    pthread_create(&threads_strike, NULL, strike, &threadData);

    while (1)
    {
        ch = getch();
        if(ch == 'q'){
			pthread_mutex_destroy(&mutex);
			
			refresh();
			endwin();
			return 0;
		}
        if(ch == ' '){
            time = (threadData.end - threadData.start);
            if(time > 5000){
                threadData.condition_ammunition = 0;
            }
            threadData.start = clock();
            if(threadData.condition_ammunition != 1){
                threadData.condition_ammunition = 1;
                threadData.memory_canon = threadData.cannon;
                if(threadData.cannon == 'Q' || threadData.cannon == 'q'){
                    threadData.x_ammunition = threadData.x - 1;
                    threadData.y_ammunition = threadData.y - 1;
                }
                if(threadData.cannon == 'W' || threadData.cannon == 'w'){
                    threadData.y_ammunition = threadData.y - 1;
                    threadData.x_ammunition = threadData.x;
                }
                if(threadData.cannon == 'E' || threadData.cannon == 'e'){
                    threadData.x_ammunition = threadData.x + 1;
                    threadData.y_ammunition = threadData.y - 1;
                }
                if(threadData.cannon == 'D' || threadData.cannon == 'd'){
                    threadData.x_ammunition = threadData.x + 1;
                    threadData.y_ammunition = threadData.y;
                }
                if(threadData.cannon == 'C' || threadData.cannon == 'c'){
                    threadData.x_ammunition = threadData.x + 1;
                    threadData.y_ammunition = threadData.y + 1;
                }
                if(threadData.cannon == 'X' || threadData.cannon == 'x'){
                    threadData.y_ammunition = threadData.y + 1;
                    threadData.x_ammunition = threadData.x;
                }
                if(threadData.cannon == 'Z' || threadData.cannon == 'z'){
                    threadData.x_ammunition = threadData.x - 1;
                    threadData.y_ammunition = threadData.y + 1;
                }
                if(threadData.cannon == 'A' || threadData.cannon == 'a'){
                    threadData.x_ammunition = threadData.x - 1;
                    threadData.y_ammunition = threadData.y;
                }
            }
        }
        if(ch == KEY_LEFT || ch == KEY_RIGHT || ch == KEY_UP || ch == KEY_DOWN){
            threadData.key = ch;
        }
        if(ch == 'Q' || ch == 'W' || ch == 'E' || ch == 'D' || ch == 'C' || ch == 'X' || ch == 'Z' || ch == 'A' || ch == 'q' || ch == 'w' || ch == 'e' || ch == 'd' || ch == 'c' || ch == 'x' || ch == 'z' || ch == 'a'){
            threadData.cannon = ch;
        }
        
    }

    pthread_join(threads_render, NULL);
	pthread_join(threads_tank, NULL);
    pthread_join(threads_strike, NULL);

    return 0;
}