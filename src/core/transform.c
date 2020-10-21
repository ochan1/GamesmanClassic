#include "transform.h"
#include "gamesman.h"
static bool rect;
static bool hex;
static int row;
static int col;
static int boardsize;
// Reflection logic
static void rect_ref_0(int* dst, int* src);
static void rect_ref_45(int* dst, int* src);
static void rect_ref_90(int* dst, int* src);
static void rect_ref_135(int* dst, int* src);
static void hex_ref_0(int* dst, int* src);
static void hex_ref_30(int* dst, int* src, int n);
// Rotation logic
static void rect_rot_90(int* dst, int* src);
static void rect_rot_180(int* dst, int* src);
static void rect_rot_270(int* dst, int* src);
static void hex_rot_n(int* dst, int* src, int n);
void init_transform(bool i_rect, bool i_hex, int i_row, int i_col,int i_boardsize) {
    rect = i_rect;
    hex = i_hex;
    row = i_row;
    col = i_col;
    boardsize = i_boardsize;
}
void reflection(int* dst, int* src, int angle) {
    int angle_local = angle > 150 ? angle-180 : angle;
    if (rect) {
        switch (angle_local) {
            case 0:
                rect_ref_0(dst,src);
                break;
            case 45:
                rect_ref_45(dst,src);
                break;
            case 90:
                rect_ref_90(dst,src);
                break;
            case 135:
                rect_ref_135(dst,src);
                break;
        }
    } else if (hex) {
        if (angle_local == 0) {
            hex_ref_0(dst,src);
        } else {
            hex_ref_30(dst,src,angle_local/30);
        }
    }
}
void rotation(int* dst, int* src, int angle) {
    if (rect) {
        switch (angle) {
            case 90:
                rect_rot_90(dst,src);
                break;
            case 180:
                rect_rot_180(dst,src);
                break;
            case 270:
                rect_rot_270(dst,src);
                break;
        }

    } else if (hex) {
        hex_rot_n(dst,src,angle/60);
    }
}