#ifndef GMCORE_TRANSFORM_H
#define GMCORE_TRANSFORM_H
/* Initializes the transform module with
inputed boardsize rows and columns to reduce what we 
have to pass into each call.*/ 
void init_transform(bool rect, bool hex, int row, int col, int boardsize);
void reflection(int* dst, int* src, int angle);
void rotation(int* dst, int* src, int angle);
#endif