#include "conway.h"
#include <stdlib.h>
#include <errno.h>
// #include<assert.h>
#include <stdint.h>
#include <stdio.h>
#include <time.h>
#include <string.h>
void print_game(const Conway *c)
{
    for (int i = 1; i < c->m - 1; i++)
    {
        for (int j = 1; j < c->n - 1; j++)
        {
            if (c->_grids[i][j])
                printf("|*");
            else
                printf("| ");
        }
        printf("|\n");
    }
}
// 构造新格点，分配内存
// malloc()
Conway new_conway(const uint16_t rows, const uint16_t columns)
{
    Conway *c = malloc(2 * sizeof(uint16_t));
    c->_grids = (GridState **)malloc((rows + 2) * sizeof(GridState *));
    for (int i = 0; i < rows + 2; i++)
    {
        c->_grids[i] = (GridState *)malloc((columns + 2) * sizeof(GridState));
    }
    c->m = rows + 2;
    c->n = columns + 2; // 生成dummy columns and rows
    for (int i = 0; i < c->m; i++)
        memset(c->_grids[i], 0, c->n * sizeof(uint16_t));
    return *c;
}

// 删除格点，回收分配给格点二维数组的内存
// Conway游戏本身的结构体 c 不用删除，只删除其格点
// 使用free()
void delete_grids(Conway *c)
{
    for (int i = 0; i < c->m; i++)
    {
        free(c->_grids[i]);
    }
    free(c->_grids);
}

// 随机地初始化格点
void init_random(Conway *c)
{
    int randomNUM;
    for (int i = 1; i < (*c).m - 1; i++)
    {
        for (int j = 1; j < (*c).n - 1; j++)
        {

            randomNUM = rand() % 2; // 得到零或一
            // printf("%d",randomNUM);
            c->_grids[i][j] = randomNUM;
        }
    }
}

// 将系统演化到下一世代
void next_generation(Conway *c)
{
    // assert(c);
    // printf("%d\n",c->m);
    GridState **grids_tmp = (GridState **)malloc(c->m * sizeof(GridState *));
    if (grids_tmp == NULL)
    {
        puts("malloc failed!");
        return;
    }
    for (int i = 0; i < c->m; i++)
    {
        grids_tmp[i] = (GridState *)malloc(c->n * sizeof(GridState));
        if (grids_tmp[i] == NULL)
        {
            puts("malloc failed!");
            return;
        }
    }
    for (int i = 0; i < c->m; i++)
    {
        for (int j = 0; j < c->n; j++)
        {
            grids_tmp[i][j] = c->_grids[i][j]; // 创建副本，防止下面牵一发而动全身，就是说边改的时候便影响了其他各点进化的情况，我们行啊要的是按照上一代的静态图像来决定下一代的情况
        }
    }
    for (int i = 1; i < c->m - 1; i++)
    {
        for (int j = 1; j < c->n - 1; j++)
        {
            int cnt = 0;
            cnt = grids_tmp[i - 1][j - 1] + grids_tmp[i][j - 1] + grids_tmp[i - 1][j + 1] + grids_tmp[i][j + 1] + grids_tmp[i - 1][j] + grids_tmp[i + 1][j - 1] + grids_tmp[i + 1][j] + grids_tmp[i + 1][j + 1];
            // if(grids[i-1][j])cnt++;
            // if(grids[i+1][j])cnt++;
            // if(grids[i][j-1])cnt++;
            // if(grids[i][j+1])cnt++;//事实上，直接将他们相加更简单，不过这样也没错就是了。
            if (grids_tmp[i][j] == 1 && (cnt < 2 || cnt > 3))
                c->_grids[i][j] = 0;
            if (grids_tmp[i][j] == 0 && cnt == 3)
                c->_grids[i][j] = 1;
        }
    }
    for (int i = 0; i < c->m; i++)
        free(grids_tmp[i]);
    free(grids_tmp);
}

// 获取格点的当前状态
// 注意下标边界检查
// 0 <= x < m,
// 0 <= y < n,
// 虽然看上去这样一个封装没有必要，毕竟可以 c->_grids[x][y]来访问
// 但是封装后会安全一点
// 越界或者遇到空指针返回GridState::None ?
// if (get_current_state(c, x, y) == GridState::None) {
//     // balabalabala
// }
GridState get_state(const Conway *c, const uint16_t x, const uint16_t y)
{
    if (x < 1 || x > (*c).m - 2 || y < 1 || y > (*c).n - 2 || c == NULL)
    {
        return -1;
    }
    return c->_grids[x][y];
}

void set_state(Conway *c, const uint16_t x, const uint16_t y, GridState s)
{
    if (x < 1 || x > (*c).m - 2 || y < 1 || y > (*c).n - 2 || c == NULL)
    {
        puts("Index out of range!Or nullptr passed!");
        return;
    }
    c->_grids[x][y] = s;
}

// 获取格点下一个世代的状态
// 注意下标边界检查
// 0 <= x < m,
// 0 <= y < n,
GridState get_next_state(const Conway *c, const uint16_t x, const uint16_t y)
{
    if (x < 1 || x > (*c).m - 2 || y < 1 || y > (*c).n - 2 || c == NULL)
    {
        puts("Index out of range!Or nullptr passed!");
        return STATE_NONE;
    }
    int cnt = 0;
    cnt = c->_grids[x - 1][y - 1] + c->_grids[x][y - 1] + c->_grids[x - 1][y + 1] + c->_grids[x][y + 1] + c->_grids[x - 1][y] + c->_grids[x + 1][y - 1] + c->_grids[x + 1][y] + c->_grids[x + 1][y + 1];
    if (c->_grids[x][y] == 1 && (cnt < 2 || cnt > 3))
        return STATE_DEAD;
    if (c->_grids[x][y] == 0 && cnt == 3)
        return STATE_ALIVE;
}

// 展示格点，一般来说是printf打印吧
// 不过长和宽设置的都是uint16_t类型，稍微大一点的格点就不好打印了
void show_conway(const Conway *c) // 决定用图片展示，不过这里就先打印吧
{
    for (int i = 1; i < c->m - 1; i++)
    {
        for (int j = 1; j < c->n - 1; j++)
        {
            if (c->_grids[i][j])
                printf("1,");
            else
                printf("0,");
        }
        printf("\n");
    }
}

// 保存格点到文件（可能得考虑一下数据保存到文件的格式）
// 成功则返回0，失败返回非0值
int save_conway(const Conway *c, const char *path) // D:\vscode_c\projects\life_game\output.txt
{

    FILE *f_w = fopen(path, "w");
    if (f_w == NULL)
    {
        perror("fopen");
        return 1;
    }
    int weishu1 = 0, weishu2 = 0;
    uint16_t _m = c->m - 2, _n = c->n - 2;
    do
        weishu1++;
    while (_m /= 10);
    do
        weishu2++;
    while (_n /= 10);
    char str[1];
    str[0] = '\n';
    char st[1];
    st[0] = ',';
    char *row = malloc(sizeof(char) * (weishu1 + 1));
    sprintf(row, "%d", c->m - 2);
    char *col = malloc(sizeof(char) * (weishu2 + 1));
    sprintf(col, "%d", c->n - 2);
    fwrite(row, sizeof(char), weishu1, f_w);
    fwrite(st, sizeof(char), 1, f_w);
    fwrite(col, sizeof(char), weishu2, f_w);
    fwrite(str, sizeof(char), 1, f_w);
    for (int i = 1; i < c->m - 1; i++)
    {
        for (int j = 1; j < c->n - 1; j++)
        {
            char tmp[2];
            sprintf(tmp, "%d", *(c->_grids[i] + j));
            fwrite(tmp, sizeof(char), 1, f_w);
            fwrite(st, sizeof(char), 1, f_w);
        }

        fwrite(str, sizeof(char), 1, f_w);
    }
    fclose(f_w);
    f_w = NULL;
    // puts("all fine!");
    return 0;
}

// 从文件读取格点
// 失败则Conway._grids = NULL
// 涉及malloc()
Conway new_conway_from_file(const char *path) // D:\vscode_c\projects\life_game\input.txt
{
    Conway c;
    FILE *f_r = fopen(path, "r");
    if (f_r == NULL)
    {
        perror("fopen");
        exit(1);
    }
    char str[1];
    int cnt = 0;
    int j = 0, i = 0;
    while (fread(str, sizeof(char), 1, f_r))
    {
        if (str[0] == '\n')
        {
            break;
        }
        else if (str[0] == ',')
        {
            cnt++;
            continue;
        }
        else
        {
            if (cnt == 0)
            {
                i = (str[0] - '0') + i * 10;
            }
            if (cnt == 1)
            {
                j = (str[0] - '0') + j * 10;
            }
        }
    }
    c.m = i + 2;
    c.n = j + 2;
    //printf("%d %d", c.m, c.n);
    c._grids = malloc(c.m * sizeof(GridState *));
    for (int i = 0; i < c.m; i++)
        c._grids[i] = malloc(c.n * sizeof(GridState));
    i = 1, j = 1;
    while (fread(str, sizeof(char), 1, f_r))
    {
        //puts("OK AT LEAST FOR NOW!!");
        if (str[0] == '\n')
        {
            i++;
            j=1;
            continue;
        }
        if (str[0] == ',')
            continue;
        c._grids[i][j] = str[0] - '0';
        j++;
    }
    ;
    return c;
}
