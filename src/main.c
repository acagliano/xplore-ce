/*
 *--------------------------------------
 * Program Name: XPLORE
 * Author: Anthony Cagliano
 * License: GPL v 3.0
 * Description: An Exploration Game
 *--------------------------------------
*/

#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include <graphx.h>
#include <keypadc.h>
#include <compression.h>
#include <math.h>
#include <limits.h>

#include "unpack.h"
#include "gfx/level.h"
#include "gfx/textures.h"

uint8_t maze_decompressed[9804];
uint8_t *map[99][99] = maze_decompressed;
size_t map_size = 99*99;

#define TRIG_SCALE 0x10000
#define TRIG_PRECISION_BITS 8
#define TRIG_PRECISION (1 << TRIG_PRECISION_BITS)
int sin_table[TRIG_PRECISION / 4];
int sec_table[TRIG_PRECISION / 4];

uint8_t* textures = {
    wall_data,
    wall_glitched_data
}

#define ROTSPEED 10
#define MOVESPEED 1

struct vectors_t {float x; float y}  // vector defines

void init_trig(void) {
    for(uint24_t i = 0; i < TRIG_PRECISION / 4; i++) {
        sin_table[i] = sin(i * M_PI / (TRIG_PRECISION / 2.0)) * TRIG_SCALE;
        sec_table[i] = 1.0 / cos(i * M_PI / (TRIG_PRECISION / 2.0)) * TRIG_SCALE;
    }
}

void rotateRight(vectors_t* dir, vectors_t* plane){
    float dirX = dir->x, dirY = dir->y;
    float planeX = plane->x, planeY = plane->y;
    float oldDirX = dirX;
    dir->x = dirX * cos(-ROTSPEED) - dirY * sin(-ROTSPEED);
    dir->y = oldDirX * sin(-ROTSPEED) + dirY * cos(-ROTSPEED);
    float oldPlaneX = planeX;
    plane->x = planeX * cos(-ROTSPEED) - planeY * sin(-ROTSPEED);
    plane->y = oldPlaneX * sin(-ROTSPEED) + planeY * cos(-ROTSPEED);
}

void rotateLeft(vectors_t* dir, vectors_t* plane){
    float dirX = dir->x, dirY = dir->y;
    float planeX = plane->x, planeY = plane->y;
    float oldDirX = dirX;
    dir->x = dirX * cos(ROTSPEED) - dirY * sin(ROTSPEED);
    dir->y = oldDirX * sin(ROTSPEED) + dirY * cos(ROTSPEED);
    float oldPlaneX = planeX;
    plane->x = planeX * cos(ROTSPEED) - planeY * sin(ROTSPEED);
    plane->y = oldPlaneX * sin(ROTSPEED) + planeY * cos(ROTSPEED);
}

void walkForward(vectors_t* player, vectors_t* dir, uint8_t map[99][99]){
    float posX = player->x, posY = player->y;
    float dirX = dir->x, dirY = dir->y;
    if(map[int(posX + dirX * MOVESPEED)][int(posY)] == false) player->x += dirX * moveSpeed;
    if(map[int(posX)][int(posY + dirY * MOVESPEED)] == false) player->y += dirY * moveSpeed;
}

int main(void)
{
    bool gameplay=true;
    kb_key_t key = 0;
    vectors_t pos = {};                     // this has to be set to a random point in maze that's valid
    vectors_t dir = {-1.0, 0.0}             // initial standing direction
    vectors_t plane = {0.0, 0.66}           // camera plane (whatever tf this does)
    size_t panic=0, ticks=0;
    srandom(rtc_Time());
    unpack1bpp(maze_decompressed, maze_data, sizeof maze_data);
    while(gameplay){
        //render(map, map_size, player_x, player_y, player_angle);
        
        // key detect
        kb_ScanGroup(7);
        if(kb_Data[7] & kb_Up) walkForward(&pos, &dir, map);
        if(kb_Data[7] & kb_Left) rotateLeft(&dir, &plane, map);
        if(kb_Data[7] & kb_Right) rotateRight(&dir, &plane, map);
        
        if(++ticks == 0) panic += (panic<3);
        if(panic)
            if(randInt(0, 5) < panic)
                renderHorror();
    }
    return 0;
}
