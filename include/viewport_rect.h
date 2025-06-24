/*
Utility script for calculating RenderTexture source and destination

For implementation "*.c" file part use:
#define VIEWPORT_RECT
*/


#ifndef VIEWPORT_RECT_H
#define VIEWPORT_RECT_H

#include "raylib.h"

void KeepAspectCentered(int scr_w, int scr_h, int view_w, int view_h, Rectangle* source_rect, Rectangle* dest_rect);
void KeepHeightCentered(int scr_w, int scr_h, int view_w, int view_h, Rectangle* source_rect, Rectangle* dest_rect);
void KeepWidthCentered(int scr_w, int scr_h, int view_w, int view_h, Rectangle* source_rect, Rectangle* dest_rect);

#endif

#ifdef VIEWPORT_RECT_IMPLEMENTATION
#undef VIEWPORT_RECT_IMPLEMENTATION

void KeepAspectCentered(int scr_w, int scr_h, int view_w, int view_h, Rectangle* source_rect, Rectangle* dest_rect){
    source_rect->x = 0.f;
    source_rect->y = view_h;
    source_rect->width = view_w;
    source_rect->height = -view_h;

    const int ratio_x = (scr_w/view_w);
    const int ratio_y = (scr_h/view_h);
    const float resize_ratio = (float)(ratio_x < ratio_y ? ratio_x : ratio_y);

    dest_rect->x = (scr_w - view_w * resize_ratio) * 0.5;
    dest_rect->y = (scr_h - view_h * resize_ratio) * 0.5;
    dest_rect->width = view_w * resize_ratio;
    dest_rect->height = view_h * resize_ratio;
}

void KeepHeightCentered(int scr_w, int scr_h, int view_w, int view_h, Rectangle* source_rect, Rectangle* dest_rect){
    const float resize_ratio = (float)(scr_h/view_h);
    source_rect->x = 0.f;
    source_rect->y = 0.f;
    source_rect->width = (float)(int)(scr_w / resize_ratio);
    source_rect->height = -view_h;

    dest_rect->x = (scr_w - source_rect->width * resize_ratio) * 0.5;
    dest_rect->y = (scr_h - view_h * resize_ratio) * 0.5;
    dest_rect->width = source_rect->width * resize_ratio;
    dest_rect->height = view_h * resize_ratio;
}

void KeepWidthCentered(int scr_w, int scr_h, int view_w, int view_h, Rectangle* source_rect, Rectangle* dest_rect){
    const float resize_ratio = (float)(scr_w/view_w);
    source_rect->x = 0.f;
    source_rect->y = 0.f;
    source_rect->width = view_w;
    source_rect->height = -(float)(int)(scr_h / resize_ratio);

    dest_rect->x = (scr_w - view_w * resize_ratio) * 0.5;
    dest_rect->y = (scr_h - -source_rect->height * resize_ratio) * 0.5;
    dest_rect->width = view_w * resize_ratio;
    dest_rect->height = -source_rect->height * resize_ratio;
}

#endif