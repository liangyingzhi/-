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
Difficulty game_difficulty = EASY; // ��ǰ��Ϸ�Ѷ�

#define WIDTH 800
#define HEIGHT 600
#define MAX_LEVEL 3 // �޸ģ�Ĭ��3x3
int current_level = 3; //��ǰ�Ѷ�
int ROW = 3, COL = 3;
int block_w_size;    //ƴͼ�ߴ�
int block_h_size;
int start_x = (WIDTH - 480) / 2;
int start_y = (HEIGHT - 480) / 2;
int puzzle[MAX_LEVEL][MAX_LEVEL];
IMAGE img;
IMAGE whiteimg;
int move_count = 0;          // �ƶ�������
clock_t game_start_time;     // ��Ϸ��ʼʱ��
const int time_limit_easy = 3 * 60;   // 3���ӣ�180�룩

// ͼƬԭʼ�ߴ�
int img_width = 480;
int img_height = 480;

// �����ֽ���
typedef enum {
    NORTHERN_PAPER_CUT,  // ������ֽ
    SOUTHERN_PAPER_CUT,  // �Ϸ���ֽ
    MODERN_PAPER_CUT     // �ִ���ֽ
} PaperCutCategory;

// ��ǰѡ��ļ�ֽ���
PaperCutCategory current_category = NORTHERN_PAPER_CUT;

// ��ͬ���ļ�ֽͼƬ·��
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

// ף���ﶨ��
const char* north_congrats[] = {
    "�ӱ���ֽ�ļ��̼���Ҫ���µ�׼ȷ���˵�����",
    "ε�ؼ�ֽ�ֳƴ�������ȫ��Ψһһ��������Ϊ��������Ϊ���ĵ�ʼ�ֽ",
    "���Ǽ�ֽ�Դ��Լ�ֽΪ������ȫ������һ��",
    "һ�Ѽ��������ã��ܼ������ܼ������������",
    "������ֽ����Ϊ���������ϣ��Եþ���ϸ��",
    "���������Ӷ����飬���ֽ���й��ӻ�������������������",
    "��������ֽ�����������Ļ��ں���Ũ����������Ϣ",
};
const int north_congrats_count = sizeof(north_congrats) / sizeof(char*);

const char* south_congrats[] = {
    "Բ�����·���ש��ȱ���������롪�����ݼ�ֽ",
    "���ݼ�ֽ������ϣ��γ�һ�ַ����صļ�ֽ�����������ݻ���",
    "ƽ�������ߣ������һƬ�������ݼ�ֽ",
    "�����ֽ��ϸ�����������Ͻ�������ϸ����ص�",
    "��������ƽ�������ȵ��Կ̻�ɽ�ݼ���϶࣬��ʵ���ӵ���ζ��ΪŨ��",
    "���ϵ������Ժ������续��������ϸ�������ķ��",
    "�������һ���Ի������ɵ���Ʒ���뻭",
    "���ֹ�ͼ�Գ�ƽ�⣬����������Ȼ������������������ȡ�ᣬ������ʵ��������"
};
const int south_congrats_count = sizeof(south_congrats) / sizeof(char*);

const char* modern_congrats[] = {
    "�й���ֽ�ķ��ࡪ����ɫ��ֽ����ɫ��ֽ�������ֽ",
    "�й���ֽ�ļ��ɡ����۵��������̡��̿�",
    "���̡�����ͼ������Ĳ��ּ���������ͼ��ԭ�еĵ�����",
    "���̡�����ͼ��������̵���ʣ�µ���ͼ������Ĳ���"
};
const int modern_congrats_count = sizeof(modern_congrats) / sizeof(char*);

// ��ȡ���ף����ĺ���
const char* get_random_congrats() {
    switch (current_category) {
    case NORTHERN_PAPER_CUT:
        return north_congrats[rand() % north_congrats_count];
    case SOUTHERN_PAPER_CUT:
        return south_congrats[rand() % south_congrats_count];
    case MODERN_PAPER_CUT:
        return modern_congrats[rand() % modern_congrats_count];
    default:
        return "��ϲͨ�أ�";
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
    srand((unsigned)time(NULL)); // ��ǰ��ʼ�����������

    ROW = COL = current_level; // �̶�3x3

    // ѡ��ͼƬ�߼�����
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

            if (num == ROW * COL - 1) { // �հ׿�
                putimage(x, y, &whiteimg);
            }
            else {
                int src_x = (num % COL) * block_w_size;
                int src_y = (num / COL) * block_h_size;
                putimage(x, y, block_w_size, block_h_size, &img, src_x, src_y);
            }
        }
    }
    // ����������
    for (int i = 0; i < ROW; i++) { // ����
        line(start_x, start_y + i * block_h_size,
            start_x + img_width, start_y + i * block_h_size);
    }
    for (int j = 0; j < COL; j++) { // ����
        line(start_x + j * block_w_size, start_y,
            start_x + j * block_w_size, start_y + img_height);
    }

    settextcolor(WHITE);
    settextstyle(20, 0, "����");
    setbkmode(TRANSPARENT);
    char diff_text[50];
    sprintf(diff_text, "��ǰ�Ѷȣ�3x3"); // �̶���ʾ3x3
    outtextxy(10, HEIGHT - 100, diff_text);

    // ��ʾ�ƶ�����
    char moves_str[50];
    sprintf(moves_str, "�ƶ�����: %d", move_count);
    outtextxy(10, HEIGHT - 80, moves_str);

    // ���㲢��ʾʣ��ʱ��
    int elapsed = (clock() - game_start_time) / CLOCKS_PER_SEC;
    int remaining = time_limit_easy - elapsed;
    if (remaining < 0) remaining = 0;

    char time_str[50];
    sprintf(time_str, "ʣ��ʱ��: %02d:%02d",
        remaining / 60, remaining % 60);
    outtextxy(10, HEIGHT - 60, time_str);

    // ��ʾ��ֽ���
    const char* category_names[] = { "������ֽ", "�Ϸ���ֽ", "�ִ���ֽ" };
    char category_str[50];
    sprintf(category_str, "��ֽ���: %s", category_names[current_category]);
    outtextxy(10, HEIGHT - 40, category_str);

    // ��ʾͼƬ�ߴ�
    char size_str[50];
    sprintf(size_str, "ͼƬ�ߴ�: %dx%d", img_width, img_height);
    outtextxy(WIDTH - 200, HEIGHT - 40, size_str);

    // Ԥ������
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

int menu_selection = 0;   //��ǰѡ����

typedef struct {
    int x, y;
    int width, height;
    const char* text;
}Button;

Button menu_items[]{
    {0,0,200,50,"��ʼ��Ϸ"},
    {0,0,200,50,"�ؿ�ѡ��"},
    {0,0,200,50,"��Ϸ˵��"}
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
    settextstyle(70, 0, "����");
    setbkmode(TRANSPARENT);
    outtextxy((WIDTH - textwidth("����赺�")) / 2 + 100, (180 - textheight("����赺�")) / 2 + 50, "����赺�");

    settextstyle(50, 0, "΢���ź�");
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
    settextstyle(30, 0, "����");
    setbkmode(TRANSPARENT);
    const char* lines[]{
    "����һ����ֽ�����ƴͼ��Ϸ��",
    "�㽫������ƶ��׿飬ʹƴͼ˳�����У�",
    "�׿����λ�����½ǡ�",
    "��Esc�������˵�"
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
    const char* category_options[] = { "������ֽ", "�Ϸ���ֽ", "�ִ���ֽ" };
    int selected = 0;
    bool exit_flag = false;

    while (!exit_flag) {
        cleardevice();
        BeginBatchDraw();

        IMAGE bg;
        loadimage(&bg, "images/bg.jpg", WIDTH, HEIGHT);
        putimage(0, 0, &bg);

        settextcolor(BLACK);
        settextstyle(50, 0, "΢���ź�");
        outtextxy(350, 100, "ѡ���ֽ���");
        settextstyle(40, 0, "΢���ź�");
        outtextxy(350, 200 + 3 * 80, "��Esc����");

        settextstyle(40, 0, "΢���ź�");
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
    srand((unsigned)time(NULL)); // ��ʼ�����������

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
                    case 0: /* ��ʼ��Ϸ */ break;
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
                case 0:  // ��ʼ��Ϸ
                    init_puzzle();
                    move_count = 0;
                    game_start_time = clock();

                    BeginBatchDraw();
                    while (true) {
                        int elapsed = (clock() - game_start_time) / CLOCKS_PER_SEC;
                        if (elapsed >= time_limit_easy) {
                            MessageBox(GetHWnd(), "ʱ�䵽����Ϸʧ�ܣ�", "��ʾ", MB_OK);
                            break;
                        }

                        paint_line();

                        if (isOver()) {
                            FlushBatchDraw();

                            // ��ȡ���ף����
                            const char* congrats_text = get_random_congrats();

                            // ������ʱ
                            int elapsed = (clock() - game_start_time) / CLOCKS_PER_SEC;

                            // ��������ַ���
                            char result[200];
                            sprintf(result, "%s\n\n��ʱ: %d��\n�ƶ�����: %d",
                                congrats_text, elapsed, move_count);

                            // ��ʾʤ����Ϣ��
                            MessageBox(GetHWnd(), result, "��Ϸʤ��", MB_OK);

                            // �˳���Ϸѭ�����ص����˵�
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