#define _CRT_SECURE_NO_WARNINGS
#include<stdio.h>
#include<easyx.h>
#include<time.h>
#include<conio.h>
#include <stdlib.h>
#include <windows.h>
#include <mmsystem.h>
#pragma comment(lib, "winmm.lib")

typedef enum { EASY = 3, NORMAL = 4 } Difficulty;
Difficulty game_difficulty = EASY; // 当前游戏难度

#define WIDTH 800
#define HEIGHT 600
#define MAX_LEVEL 3 // 修改：默认3x3
int current_level = 3; //当前难度
int ROW = 3, COL = 3;
int block_w_size;    //拼图尺寸
int block_h_size;
int start_x = (WIDTH - 480) / 2;
int start_y = (HEIGHT - 480) / 2;
int puzzle[MAX_LEVEL][MAX_LEVEL];
IMAGE img;
IMAGE whiteimg;
int move_count = 0;          // 移动计数器
clock_t game_start_time;     // 游戏开始时间
const int time_limit_easy = 3 * 60;   // 3分钟（180秒）

// 图片原始尺寸
int img_width = 480;
int img_height = 480;

// 定义剪纸类别
typedef enum {
    NORTHERN_PAPER_CUT,  // 北方剪纸
    SOUTHERN_PAPER_CUT,  // 南方剪纸
    MODERN_PAPER_CUT     // 现代剪纸
} PaperCutCategory;

// 当前选择的剪纸类别
PaperCutCategory current_category = NORTHERN_PAPER_CUT;

// 不同类别的剪纸图片路径
const char* north_images[] = {
    "./images/north0.png",
    "./images/north1.png",
    "./images/north2.png",
    "./images/north3.jpg",
    "./images/north4.png",
    "./images/north5.jpg",
    "./images/north6.jpg",
    "./images/north7.jpg",
    "./images/north8.png",
    "./images/north9.png",
    "./images/north10.jpg",
    "./images/north11.jpg"
    "./images/north12.png"
};
const int north_count = sizeof(north_images) / sizeof(char*);

const char* south_images[] = {
    "./images/south0.png",
    "./images/south1.png",
    "./images/south2.png",
    "./images/south3.png",
    "./images/south4.png",
    "./images/south5.png",
    "./images/south6.png",
    "./images/south7.png",
    "./images/south8.png",
    "./images/south9.png",
    "./images/south10.png",
    "./images/south11.png",
    "./images/south12.jpg",
    "./images/south13.jpg",
    "./images/south14.jpg"
};
const int south_count = sizeof(south_images) / sizeof(char*);

const char* modern_images[] = {
    "./images/modern0.png",
    "./images/modern1.png",
    "./images/modern2.jpg",
    "./images/modern3.png",
};
const int modern_count = sizeof(modern_images) / sizeof(char*);

// 祝贺语定义
const char* north_congrats[] = {
    "河北剪纸的剪刻技法要求下到准确，运刀流畅",
    "蔚县剪纸又称窗花，是全国唯一一种以阴刻为主、阳刻为辅的点彩剪纸",
    "晋城剪纸以磁性剪纸为代表，在全国独树一帜",
    "一把剪刀多有用，能剪龙，能剪凤，剪的老鼠会打洞",
    "胶东剪纸以线为主，线面结合，显得精巧细致",
    "滨州人民淳朴而热情，其剪纸具有古朴浑厚、热烈明快的鲜明特征",
    "陕西民间剪纸充满了深厚的文化内涵和浓郁的乡土气息",
};
const int north_congrats_count = sizeof(north_congrats) / sizeof(char*);

const char* south_congrats[] = {
    "圆如秋月方如砖，缺如锯齿线入须——扬州剪纸",
    "扬州剪纸与刺绣结合，形成一种风格独特的剪纸艺术——扬州花样",
    "平看条条线，提起成一片——扬州剪纸",
    "乐清剪纸纤细优美、布局严谨，刀法细腻的特点",
    "闽北的南平、华安等地以刻画山禽家畜较多，厚实淳朴的意味较为浓厚",
    "闽南地区常以海洋动物如画，倾向于细腻生动的风格",
    "莆田、仙游一带以华丽纤巧的礼品花入画",
    "漳浦构图对称平衡，线条连接自然，阳剪线线相连，大胆取舍，形象朴实，生动大方"
};
const int south_congrats_count = sizeof(south_congrats) / sizeof(char*);

const char* modern_congrats[] = {
    "中国剪纸的分类——单色剪纸、彩色剪纸、立体剪纸",
    "中国剪纸的技巧——折叠、阴阳刻、刺刻",
    "阳刻——把图案以外的部分剪掉，保留图案原有的点线面",
    "阴刻——把图案自身剪刻掉，剩下的是图案以外的部分"
};
const int modern_congrats_count = sizeof(modern_congrats) / sizeof(char*);

// 获取随机祝贺语的函数
const char* get_random_congrats() {
    switch (current_category) {
    case NORTHERN_PAPER_CUT:
        return north_congrats[rand() % north_congrats_count];
    case SOUTHERN_PAPER_CUT:
        return south_congrats[rand() % south_congrats_count];
    case MODERN_PAPER_CUT:
        return modern_congrats[rand() % modern_congrats_count];
    default:
        return "恭喜通关！";
    }
}

bool isPeek = false;

void show() {
    for (size_t i = 0; i < ROW; i++) {
        for (size_t j = 0; j < COL; j++) {
            printf("%d", puzzle[i][j]);
        }
        printf("\n");
    }
}

void init_puzzle() {
    srand((unsigned)time(NULL)); // 提前初始化随机数种子

    ROW = COL = current_level; // 固定3x3

    // 选择图片逻辑不变
    const char** category_images = nullptr;
    int category_count = 0;

    switch (current_category) {
    case NORTHERN_PAPER_CUT:
        category_images = north_images;
        category_count = north_count;
        break;
    case SOUTHERN_PAPER_CUT:
        category_images = south_images;
        category_count = south_count;
        break;
    case MODERN_PAPER_CUT:
        category_images = modern_images;
        category_count = modern_count;
        break;
    }

    int image_index = rand() % category_count;
    loadimage(&img, category_images[image_index]);
    img_width = img.getwidth();
    img_height = img.getheight();

    block_w_size = img_width / current_level;
    block_h_size = img_height / current_level;
    start_x = (WIDTH - img_width) / 2;
    start_y = (HEIGHT - img_height) / 2;

    loadimage(&whiteimg, "./images/white.jpg", block_w_size, block_h_size);

    int total = ROW * COL;
    for (int i = 0; i < ROW; i++) {
        for (int j = 0; j < COL; j++) {
            puzzle[i][j] = i * COL + j;
        }
    }

    int blank_r = ROW - 1, blank_c = COL - 1;
    for (int i = 0; i < ROW * COL * 10; i++) {
        int r1 = rand() % ROW;
        int c1 = rand() % COL;
        int r2 = rand() % ROW;
        int c2 = rand() % COL;

        int temp = puzzle[r1][c1];
        puzzle[r1][c1] = puzzle[r2][c2];
        puzzle[r2][c2] = temp;

        if (puzzle[r1][c1] == total - 1) {
            blank_r = r1;
            blank_c = c1;
        }
        if (puzzle[r2][c2] == total - 1) {
            blank_r = r2;
            blank_c = c2;
        }
    }
}

void paint_line() {
    setlinecolor(WHITE);
    cleardevice();

    for (int i = 0; i < ROW; i++) {
        for (int j = 0; j < COL; j++) {
            int num = puzzle[i][j];
            int x = start_x + j * block_w_size;
            int y = start_y + i * block_h_size;

            if (num == ROW * COL - 1) { // 空白块
                putimage(x, y, &whiteimg);
            }
            else {
                int src_x = (num % COL) * block_w_size;
                int src_y = (num / COL) * block_h_size;
                putimage(x, y, block_w_size, block_h_size, &img, src_x, src_y);
            }
        }
    }
    // 绘制网格线
    for (int i = 0; i < ROW; i++) { // 横线
        line(start_x, start_y + i * block_h_size,
            start_x + img_width, start_y + i * block_h_size);
    }
    for (int j = 0; j < COL; j++) { // 竖线
        line(start_x + j * block_w_size, start_y,
            start_x + j * block_w_size, start_y + img_height);
    }

    settextcolor(WHITE);
    settextstyle(20, 0, "宋体");
    setbkmode(TRANSPARENT);
    char diff_text[50];
    sprintf(diff_text, "当前难度：3x3"); // 固定显示3x3
    outtextxy(10, HEIGHT - 100, diff_text);

    // 显示移动次数
    char moves_str[50];
    sprintf(moves_str, "移动次数: %d", move_count);
    outtextxy(10, HEIGHT - 80, moves_str);

    // 计算并显示剩余时间
    int elapsed = (clock() - game_start_time) / CLOCKS_PER_SEC;
    int remaining = time_limit_easy - elapsed;
    if (remaining < 0) remaining = 0;

    char time_str[50];
    sprintf(time_str, "剩余时间: %02d:%02d",
        remaining / 60, remaining % 60);
    outtextxy(10, HEIGHT - 60, time_str);

    // 显示剪纸类别
    const char* category_names[] = { "北方剪纸", "南方剪纸", "现代剪纸" };
    char category_str[50];
    sprintf(category_str, "剪纸类别: %s", category_names[current_category]);
    outtextxy(10, HEIGHT - 40, category_str);

    // 显示图片尺寸
    char size_str[50];
    sprintf(size_str, "图片尺寸: %dx%d", img_width, img_height);
    outtextxy(WIDTH - 200, HEIGHT - 40, size_str);

    // 预览处理
    if (isPeek) {
        putimage(start_x, start_y, &img);
    }
}

void findWhiteIndex(int* r, int* c) {
    for (int i = 0; i < ROW; i++) {
        for (int j = 0; j < COL; j++) {
            if (puzzle[i][j] == ROW * COL - 1) {
                *r = i;
                *c = j;
                return;
            }
        }
    }
}


void play() {
    ExMessage msg;
    if (peekmessage(&msg)) {
        int wr, wc;
        findWhiteIndex(&wr, &wc);

        if (msg.message == WM_LBUTTONDOWN) {
            if (msg.x < start_x || msg.x > start_x + img_width ||
                msg.y < start_y || msg.y > start_y + img_height) {
                return;
            }
            int c = (msg.x - start_x) / block_w_size;
            int r = (msg.y - start_y) / block_h_size;

            if ((r == wr - 1 && c == wc) ||
                (r == wr + 1 && c == wc) ||
                (r == wr && c == wc - 1) ||
                (r == wr && c == wc + 1)) {
                int temp = puzzle[r][c];
                puzzle[r][c] = puzzle[wr][wc];
                puzzle[wr][wc] = temp;

                move_count++;
            }
        }
        else if (msg.message == WM_RBUTTONDOWN) {
            isPeek = true;
        }
        else if (msg.message == WM_RBUTTONUP) {
            isPeek = false;
        }
        else if (msg.message == WM_MBUTTONDOWN) {
            for (size_t i = 0; i < ROW; i++) {
                for (size_t j = 0; j < COL; j++) {
                    puzzle[i][j] = i * ROW + j;
                }
            }
        }
    }
}

bool isOver() {
    int c = 0;
    for (int i = 0; i < ROW; ++i) {
        for (int j = 0; j < COL; ++j) {
            if (puzzle[i][j] != c)return 0;
            c++;
        }
    }
    return 1;
}

ExMessage msg = { 0 };

int menu_selection = 0;   //当前选中项

typedef struct {
    int x, y;
    int width, height;
    const char* text;
}Button;

Button menu_items[]{
    {0,0,200,50,"开始游戏"},
    {0,0,200,50,"关卡选择"},
    {0,0,200,50,"游戏说明"}
};

void init_menu() {
    const int total = sizeof(menu_items) / sizeof(Button);
    const int startY = (HEIGHT - 60 * total) / 2 + 50;

    for (int i = 0; i < total; i++) {
        menu_items[i].x = (WIDTH - menu_items[i].width) / 2;
        menu_items[i].y = startY + 60 * i;
    }
}

void draw_menu() {
    setbkcolor(BLACK);
    cleardevice();

    IMAGE bg;
    loadimage(&bg, "images/bg.jpg", WIDTH, HEIGHT);
    putimage(0, 0, &bg);

    settextcolor(BLACK);
    settextstyle(70, 0, "楷体");
    setbkmode(TRANSPARENT);
    outtextxy((WIDTH - textwidth("珠联璧合")) / 2 + 100, (180 - textheight("珠联璧合")) / 2 + 50, "珠联璧合");

    settextstyle(50, 0, "微软雅黑");
    for (int i = 0; i < 3; i++) {
        setbkmode(TRANSPARENT);
        settextcolor(i == menu_selection ? RED : BLACK);
        outtextxy(menu_items[i].x + 100, menu_items[i].y, menu_items[i].text);
    }
}

void show_instructions() {
    cleardevice();

    IMAGE bg;
    loadimage(&bg, "images/bg.jpg", WIDTH, HEIGHT);
    putimage(0, 0, &bg);

    settextcolor(BLACK);
    settextstyle(30, 0, "宋体");
    setbkmode(TRANSPARENT);
    const char* lines[]{
    "这是一个剪纸主题的拼图游戏，",
    "你将用鼠标移动白块，使拼图顺序排列，",
    "白块必须位于右下角。",
    "按Esc返回主菜单"
    };
    for (int i = 0; i < sizeof(lines) / sizeof(lines[1]); i++) {
        outtextxy(200, 100 + 50 * i, lines[i]);
    }

    while (true) {
        while (peekmessage(&msg, EX_KEY)) {
            if (msg.message == WM_KEYDOWN) {
                if (msg.vkcode == VK_ESCAPE)return;
            }
        }
    }
}

void select_category() {
    ExMessage msg;
    const char* category_options[] = { "北方剪纸", "南方剪纸", "现代剪纸" };
    int selected = 0;
    bool exit_flag = false;

    while (!exit_flag) {
        cleardevice();
        BeginBatchDraw();

        IMAGE bg;
        loadimage(&bg, "images/bg.jpg", WIDTH, HEIGHT);
        putimage(0, 0, &bg);

        settextcolor(BLACK);
        settextstyle(50, 0, "微软雅黑");
        outtextxy(350, 100, "选择剪纸类别");
        settextstyle(40, 0, "微软雅黑");
        outtextxy(350, 200 + 3 * 80, "按Esc返回");

        settextstyle(40, 0, "微软雅黑");
        for (int i = 0; i < 3; i++) {
            if (i == selected) {
                settextcolor(RED);
            }
            else {
                settextcolor(BLACK);
            }
            outtextxy(350, 200 + i * 80, category_options[i]);
        }

        if (peekmessage(&msg, EX_KEY | EX_MOUSE)) {
            if (msg.message == WM_MOUSEMOVE) {
                for (int i = 0; i < 3; i++) {
                    if (msg.y > 200 + i * 80 && msg.y < 240 + i * 80 &&
                        msg.x > 300 && msg.x < 300 + textwidth(category_options[i])) {
                        selected = i;
                    }
                }
            }

            if (msg.message == WM_LBUTTONDOWN) {
                current_category = (PaperCutCategory)selected;
                exit_flag = true;
            }

            if (msg.message == WM_KEYDOWN) {
                if (msg.vkcode == VK_ESCAPE) exit_flag = true;
                else if (msg.vkcode == VK_UP) {
                    selected = (selected - 1 + 3) % 3;
                }
                else if (msg.vkcode == VK_DOWN) {
                    selected = (selected + 1) % 3;
                }
                else if (msg.vkcode == VK_RETURN) {
                    current_category = (PaperCutCategory)selected;
                    exit_flag = true;
                }
            }
        }
        FlushBatchDraw();
    }
}

void main_menu() {
    init_menu();
    srand((unsigned)time(NULL)); // 初始化随机数种子

    while (true) {
        const int total = sizeof(menu_items) / sizeof(Button);
        const int startY = (HEIGHT - 60 * total) / 2 + 50;

        for (int i = 0; i < total; i++) {
            menu_items[i].x = (WIDTH - menu_items[i].width) / 2;
            menu_items[i].y = startY + 60 * i;
        }

        while (peekmessage(&msg, EX_KEY | EX_MOUSE)) {
            if (msg.message == WM_KEYDOWN) {
                if (msg.vkcode == VK_ESCAPE) exit(1);
                else if (msg.vkcode == VK_UP) {
                    menu_selection = (menu_selection - 1 + total) % total;
                }
                else if (msg.vkcode == VK_DOWN) {
                    menu_selection = (menu_selection + 1) % total;
                }
                else if (msg.vkcode == VK_RETURN) {
                    switch (menu_selection) {
                    case 0: /* 开始游戏 */ break;
                    case 1:
                        select_category();
                        break;
                    case 2:
                        show_instructions();
                        break;
                    }
                }
            }

            if (msg.message == WM_MOUSEMOVE) {
                for (int i = 0; i < total; i++) {
                    if (msg.x > menu_items[i].x && msg.x < menu_items[i].x + menu_items[i].width &&
                        msg.y > menu_items[i].y && msg.y < menu_items[i].y + menu_items[i].height) {
                        menu_selection = i;
                    }
                }
            }

            if (msg.message == WM_LBUTTONDOWN) {
                switch (menu_selection) {
                case 0:  // 开始游戏
                    init_puzzle();
                    move_count = 0;
                    game_start_time = clock();

                    BeginBatchDraw();
                    while (true) {
                        int elapsed = (clock() - game_start_time) / CLOCKS_PER_SEC;
                        if (elapsed >= time_limit_easy) {
                            MessageBox(GetHWnd(), "时间到！游戏失败！", "提示", MB_OK);
                            break;
                        }

                        paint_line();

                        if (isOver()) {
                            FlushBatchDraw();

                            // 获取随机祝贺语
                            const char* congrats_text = get_random_congrats();

                            // 计算用时
                            int elapsed = (clock() - game_start_time) / CLOCKS_PER_SEC;

                            // 创建结果字符串
                            char result[200];
                            sprintf(result, "%s\n\n用时: %d秒\n移动次数: %d",
                                congrats_text, elapsed, move_count);

                            // 显示胜利消息框
                            MessageBox(GetHWnd(), result, "游戏胜利", MB_OK);

                            // 退出游戏循环，回到主菜单
                            break;
                        }

                        play();
                        FlushBatchDraw();
                    }
                    EndBatchDraw();
                    break;
                case 1:
                    select_category();
                    break;
                case 2:
                    show_instructions();
                    break;
                }
            }
        }

        BeginBatchDraw();
        draw_menu();
        FlushBatchDraw();
        EndBatchDraw();
        Sleep(50);
    }
};

int main() {
    initgraph(WIDTH, HEIGHT, EX_SHOWCONSOLE);
    main_menu();
    closegraph();
    return 0;
}