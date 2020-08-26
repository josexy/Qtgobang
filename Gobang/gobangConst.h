#ifndef GOBANGCONST_H
#define GOBANGCONST_H

enum  {CHESS_POINT=0xaa,CHESS_MSG=0xbb};

struct packet_data{
    bool restart;

    short type;
    short color;

    short x;
    short y;

    short len;
    char data[30];
};

// 主窗口大小
#define WINDOW_WIDTH 1400
#define WINDOW_HEIGHT 1000
// 棋盘区域大小
#define PANEL_SIZE WINDOW_HEIGHT
// 棋盘布局
#define LINE_NUMBER 15
#define LINE_SPACING 60
#define LINE_LEN (LINE_NUMBER-1)*LINE_SPACING
// 相对左上角偏移
#define OFFSET LINE_SPACING
// 棋子大小
#define CHESS_SIZE (LINE_SPACING-5)
// 四星和天元大小
#define STAR_SIZE 10
// 棋盘单元格大小
#define CELL_SIZE LINE_SPACING

// 空位置
#define EMPTY 0
// 黑子
#define BLACK 1
// 白子
#define WHITE 2

// 水平
#define HORIZONTAL 0x0a
// 垂直
#define VERTICAL 0x0b
// 左斜
#define LEFT_OBLIQUE 0x0c
// 右斜
#define RIGHT_OBLIQUE 0x0d

#endif // GOBANGCONST_H
