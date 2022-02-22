/*
 *--------------------------------------
 * Program Name: XPLORE
 * Author: Anthony Cagliano
 * License: GPL v 3.0
 * Description: An Exploration Game
 *--------------------------------------
*/

#include <tice.h>
#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include <keypadc.h>
#include <compression.h>
#include <math.h>
#include <limits.h>

#include "defines.h"
#include "raycast.h"
#include "setup.h"
#include "unpack.h"
#include "gfx/level.h"
#include "gfx/textures.h"

uint8_t *map;
const size_t map_size = 99*99;
uint8_t mapbuffer[map_size];

int sin_table[TRIG_PRECISION / 4];
int sec_table[TRIG_PRECISION / 4];

uint8_t* textures = {
    wall_data,
    wall_glitched_data
};

void init_trig(void) {
    for(uint24_t i = 0; i < TRIG_PRECISION / 4; i++) {
        sin_table[i] = sin(i * M_PI / (TRIG_PRECISION / 2.0)) * TRIG_SCALE;
        sec_table[i] = 1.0 / cos(i * M_PI / (TRIG_PRECISION / 2.0)) * TRIG_SCALE;
    }
}

void rotateRight(vectors_t* dir, vectors_t* plane) {
    float dirX = dir->x, dirY = dir->y;
    float planeX = plane->x, planeY = plane->y;
    float oldDirX = dirX;
    dir->x = dirX * cos(-ROTSPEED) - dirY * sin(-ROTSPEED);
    dir->y = oldDirX * sin(-ROTSPEED) + dirY * cos(-ROTSPEED);
    float oldPlaneX = planeX;
    plane->x = planeX * cos(-ROTSPEED) - planeY * sin(-ROTSPEED);
    plane->y = oldPlaneX * sin(-ROTSPEED) + planeY * cos(-ROTSPEED);
}

void rotateLeft(vectors_t* dir, vectors_t* plane) {
    float dirX = dir->x, dirY = dir->y;
    float planeX = plane->x, planeY = plane->y;
    float oldDirX = dirX;
    dir->x = dirX * cos(ROTSPEED) - dirY * sin(ROTSPEED);
    dir->y = oldDirX * sin(ROTSPEED) + dirY * cos(ROTSPEED);
    float oldPlaneX = planeX;
    plane->x = planeX * cos(ROTSPEED) - planeY * sin(ROTSPEED);
    plane->y = oldPlaneX * sin(ROTSPEED) + planeY * cos(ROTSPEED);
}

void walkForward(vectors_t* player, vectors_t* dir, uint8_t *map) {
    float posX = player->x, posY = player->y;
    float dirX = dir->x, dirY = dir->y;
    if(map[(int)(posX + dirX * MOVESPEED + posY * MAP_WIDTH)] == false) player->x += dirX * MOVESPEED;
    if(map[(int)(posX + (posY + dirY * MOVESPEED) * MAP_WIDTH)] == false) player->y += dirY * MOVESPEED;
}

int main(void) {
    bool gameplay=true;
    kb_key_t key = 0;
    vectors_t pos = {};                     // this has to be set to a random point in maze that's valid
    vectors_t dir = {-1.0, 0.0};             // initial standing direction
    vectors_t plane = {0.0, 0.66};           // camera plane (whatever tf this does)
    size_t panic=0, ticks=0;

	map = &mapbuffer;

	BeginColumnMode();

    srandom(rtc_Time());
    unpack1bpp(map, maze_data, sizeof maze_data);
    while(gameplay) {
        //render(map, map_size, player_x, player_y, player_angle);
        
        // key detect
        kb_Scan();
        if(kb_Data[7] & kb_Up) walkForward(&pos, &dir, map);
        if(kb_Data[7] & kb_Left) rotateLeft(&dir, &plane);
        if(kb_Data[7] & kb_Right) rotateRight(&dir, &plane);
		if (kb_Data[6] & kb_Clear) gameplay = false;
        
        if(++ticks == 0) panic += (panic<3);
        if(panic)
            if(randInt(0, 5) < panic)
                renderHorror();
    }

	EndColumnMode();
    return 0;
}
