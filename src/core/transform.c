#include "transform.h"
#include "gamesman.h"
static bool rect;
static bool hex;
static int row;
static int col;
static int boardsize;
//Hex helper
static int* hex60Rot;
static int* tempSym;
static int* hex0Ref;
static int convert(int t_row, int t_col);
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
    if (hex) {
        int i;
        hex60Rot = malloc(sizeof(int)*boardsize);
        hex0Ref = malloc(sizeof(int)*boardsize);
        tempSym = malloc(sizeof(int)*boardsize);
        for (int j = 0; j < i_row; j++) {
            for (int k = 0; k < i_col; k++) {
                if (j <= i_row/2) {
                    if (i_col-k-1 > i_row/2) {
                        i = (i_col-k-1) - (i_row/2);
                    } else {
                        i = 0;
                    }
                    hex60Rot[convert(j,k)] = convert(i_col-k-1,j-i);
                } else {
                    if (i_row-k-1 > i_row/2) {
                        i = (i_row-k-1) - (i_row/2);
                    } else {
                        i = 0;
                    }
                    hex60Rot[convert(j,k)] = convert(i_row-k-1,j-i);
                }
                hex0Ref[convert(j,k)] = convert(i_row-j-1,k);
            }
            if (j < i_row/2) {
                i_col++;
            }
            if (j >= i_row/2 && j < i_row-1) {
                i_col--;
            }
        }
    }
}
void free_transform(void) {
    if (hex) {
        free(hex0Ref);
        free(tempSym);
        free(hex60Rot);
    }
}
static int convert(int t_row, int t_col) {
    int i;
    int val = 0;
    int n_col = col;

    for (i = 0; i < row; i++) {
        if (i == t_row) {
            break;
        }
        val += n_col;
        if (i < row/2) {
            n_col++;
        } else if (i >= row/2 && i < row-1) {
            n_col--;
        }
    }
    val += t_col;
    return val;
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
static void rect_ref_0(int* dst, int* src){
    for (int i = 0; i < row; i++) {
        for (int j = 0; j < col; j++) {
            dst[i*col+j] = 
        }
    }
}
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