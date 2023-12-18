#include <stdio.h>
#include <pthread.h>//posix threads
#include "conway.h"
#include <unistd.h>//posix中的,including read(),write(),sleep()etc.

void clear_screen()
{
    // Use ANSI escape codes to clear the screen
    printf("\e[1;1H\e[2J");
    printf("\e[?25l");
}

void print_help()
{
    printf("-------------------------------------------------------------------------\n");
    printf("i<row>  <col>   intialize an empty grid with <row> rows and <col> columns\n");
    printf("r               randomly set alive/dead states for all grids\n");
    printf("n               evolve into next generation\n");
    printf("b               pause evolution\n");
    printf("c               autoamtically evolve, until receiving 'b' cammand\n");
    printf("s <path>        save grid states to file <path>\n");
    printf("l <path>        load grid states from file <path>\n");
    printf("q               quit\n");
    printf("h               help\n");
    printf("-------------------------------------------------------------------------\n");
}

// void print_game(const Conway *c)
// {
//     for(int i=1;i<c->m-1;i++){
//         for(int j=1;j<c->n-1;j++)
//         {
//             if(c->_grids[i][j])printf("1,");
//             else printf("0,");
//         }
//         printf("\n");
//     }
// }

void *listen_break(void *flag)
{
    char c = 0;
    while (c != 'b')
    {
        scanf(" %c", &c);
        scanf("%*[^\n]"); // 清除未读内容
        scanf("%*c");//清除换行符
    }

    *(int *)flag = 1;
    return NULL;
}

void automatic_evolve(Conway *c)//这里用到了线程
{
    int flag = 0;
    pthread_t listener;
    pthread_create(&listener, NULL, listen_break, &flag);
    while (flag != 1)
    {
        next_generation(c);
        print_game(c);
        printf("automatically evolving...\n");
        sleep(1); // 每秒演化一次
    }

    pthread_join(listener, NULL);
    print_game(c);
}

int main()
{
    Conway c = new_conway(0, 0);
    //print_game(&c);
    print_help();
    char running = 1;
    while (running)
    {
        char cmd;
        char path[40];
        scanf(" %c", &cmd);
        switch (cmd)
        {
        case 'i':
        {
            unsigned int rows,columns;
            printf("rows>");
            scanf("%u",&rows);
            printf("columns>");
            scanf("%u",&columns);
            c=new_conway(rows,columns);
            break;
        }
        case 'c':
            automatic_evolve(&c);
            break;
        case 'r':{
            init_random(&c);
            break;
        }
        case 'n':{
            next_generation(&c);
            break;
        }
        case 'b':{
            printf("Paused. Press any key to resume...\n");
            getchar(); // Wait for user to press any key
            break;
        }
        case 's':{
            printf("path>");
            scanf("%s",path);
            if(save_conway(&c,path)!=0){
                printf("Save Failed!\n");
            };
            break;
        }
        case 'q':exit(0);
        case 'h':{
            print_help();
            break;
        }
        case 'l':{
            printf("path>");
            scanf("%s",path);
            c=new_conway_from_file(path);
            break;
        }
        // others
        default:
            break;
        }
    }

    return 0;
}