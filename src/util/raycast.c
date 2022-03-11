#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include <keypadc.h>
#include <math.h>
#include <limits.h>

#include "../defines.h"
#include "render.h"


#define MAP_WIDTH 99
#define MAX_HEIGHT 99

#define VIEWPORT_H 240
#define VIEWPORT_W 320
uint8_t texHeight = 8;
uint8_t texWidth = 8;

//credit: https://theshoemaker.de/2016/02/ray-casting-in-2d-grids/
//though I've rewritten a lot of it
//returns 0 if hits across x axis, non-zero if y axis
uint8_t raycast(vectors_t* pos, vectors_t* dir, vectors_t* plane, uint8_t *map) {
    uint startX = pos->x * 256;
    uint startY = pos->y * 256;
    for(int x = 20; x < w-20; x+=1)
        int dirX = fast_sec(angle);
        int dirY = fast_csc(angle);

        int8_t dirSignX = dirX >= 0 ? 1 : -1;
        int8_t dirSignY = dirY >= 0 ? 1 : -1;
        int8_t row_offset = dirY >= 0 ? LEVEL_SIZE_X : -LEVEL_SIZE_X;

        int8_t tileX = COORD_TO_X_TILE(startX);
        int8_t tileY = COORD_TO_Y_TILE(startY);
        const tile_t *tile_ptr = &tiles[tileY][tileX];
        int t = 0;

        int dtX = (TILE_TO_X_COORD(tileX + (dirX >= 0)) - startX) * dirX;
        int dtY = (TILE_TO_Y_COORD(tileY + (dirY >= 0)) - startY) * dirY;

        int dtXr = TILE_SIZE * dirSignX * dirX;
        int dtYr = TILE_SIZE * dirSignY * dirY;

        if(dirX == INT_MAX || dirX == INT_MIN) {
            dtXr = INT_MAX;
            dtX = INT_MAX;
        }

        if(dirY == INT_MAX || dirY == INT_MIN) {
            dtYr = INT_MAX;
            dtY = INT_MAX;
        }

        while(true) {
            tile_t tile = *tile_ptr;

            if(TILE_HEIGHT(tile)) {
                break;
            }

        //not entirely sure how this works
            if(dtX < dtY) {
                t += dtX;
                tile_ptr += dirSignX;
                dtY -= dtX;
                dtX = dtXr;
            } else {
                t += dtY;
                tile_ptr += row_offset;
                dtX -= dtY;
                dtY = dtYr;
            }
        }

    }
    // compute necessities for texture render
    int lineDist = (t * t / dirX / dirX) + (t * t / dirY / dirY);
    int drawHeight = isqrt(VIEWPORT_H * VIEWPORT_H / lineDist);
    uint8_t drawColumnY = (VIEWPORT_H>>1) - (drawHeight>>1);
    RenderColumn(
        x,                                      // x
        (drawColumnY>0?drawColumnY:0),          // y
        (drawHeight>=240?240:drawHeight),       // h
        (texNum>1?1:texNum));                   // texture num
 }
    

/*
void raycast(vectors_t* pos, vectors_t* dir, vectors_t* plane, uint8_t *map) {
	
	// RenderColumn(10, 12, 100, 0); // testing
	// return;
    
	for(int x = 20; x < w-20; x+=2)
    {
	  uint8_t texNum;
      //calculate ray position and direction
      int cameraX = 2*x/(w)-1; //x-coordinate in camera space
      int rayDirX = dir->x + plane->x*cameraX;
      int rayDirY = dir->y + plane->y*cameraX;

      //which box of the map we're in
      int mapX = (int)pos->x;
      int mapY = (int)pos->y;

      //length of ray from current position to next x or y-side
      int sideDistX;
      int sideDistY;

      //length of ray from one x or y-side to next x or y-side
      int deltaDistX = (rayDirX == 0) ? 1e30 : fabsf(1 / rayDirX);
      int deltaDistY = (rayDirY == 0) ? 1e30 : fabsf(1 / rayDirY);
      int perpWallDist;

      //what direction to step in x or y-direction (either +1 or -1)
      int stepX;
      int stepY;

      int hit = 0; //was there a wall hit?
      int side; //was a NS or a EW wall hit?

      //calculate step and initial sideDist
      if (rayDirX < 0)
      {
        stepX = -1;
        sideDistX = (pos->x - mapX) * deltaDistX;
      }
      else
      {
        stepX = 1;
        sideDistX = (mapX + 1.0 - pos->x) * deltaDistX;
      }
      if (rayDirY < 0)
      {
        stepY = -1;
        sideDistY = (pos->y - mapY) * deltaDistY;
      }
      else
      {
        stepY = 1;
        sideDistY = (mapY + 1.0 - pos->y) * deltaDistY;
      }
      //perform DDA
      while (hit == 0)
      {
        //jump to next map square, either in x-direction, or in y-direction
        if (sideDistX < sideDistY)
        {
          sideDistX += deltaDistX;
          mapX += stepX;
          side = 0;
        }
        else
        {
          sideDistY += deltaDistY;
          mapY += stepY;
          side = 1;
        }
        //Check if ray has hit a wall
		if (mapX < 0 || mapX >= MAP_WIDTH || mapY < 0 || mapY >= MAX_HEIGHT) break;
        if ((texNum = map[mapX + mapY * MAP_WIDTH]) > 0) hit = 1;
      }

      //Calculate distance of perpendicular ray (Euclidean distance would give fisheye effect!)
      if(side == 0) perpWallDist = (sideDistX - deltaDistX);
      else          perpWallDist = (sideDistY - deltaDistY);

      //Calculate height of line to draw on screen
      int lineHeight = (int)(h / perpWallDist);

      //calculate lowest and highest pixel to fill in current stripe
      int drawStart = -lineHeight / 2 + h / 2;
      if(drawStart < 0) drawStart = 0;
      int drawEnd = lineHeight / 2 + h / 2;
      if(drawEnd >= h) drawEnd = h - 1;
      

      //calculate value of wallX
      // float wallX; //where exactly the wall was hit
      // if (side == 0) wallX = pos->y + perpWallDist * rayDirY;
      // else           wallX = pos->x + perpWallDist * rayDirX;
      // wallX -= floor((wallX));

      //x coordinate on the texture
      // int texX = (int)(wallX * (float)texWidth);
      // if(side == 0 && rayDirX > 0) texX = texWidth - texX - 1;
      // if(side == 1 && rayDirY < 0) texX = texWidth - texX - 1;
      
    // How much to increase the texture coordinate per screen pixel
      // float step = 1.0 * texHeight / lineHeight;
      // Starting texture coordinate
      // float texPos = (drawStart - h / 2 + lineHeight / 2) * step;
      
	  int wallHeight = ((drawEnd-drawStart)>0?drawEnd-drawStart:1);
      RenderColumn(x/256, (drawStart>0?drawStart:0), (wallHeight>=240?240:wallHeight), (texNum>1?1:texNum));
    }
}
*/
void renderHorror(void){}


#define COS(n)  SIN((n)+64)

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
    float dirX = dir->x, dirY = dir->y;
	float posX = player->x, posY = player->y;
    if(map[(int)(posX + dir->x * MOVESPEED + posY * MAP_WIDTH)]) {
		player->x = posX + dir->x * (MOVESPEED / 1.1f);
	} else {
		player->x += dirX * MOVESPEED;
	}
    if(map[(int)(posX + (posY + dir->y * MOVESPEED) * MAP_WIDTH)]) {
		player->y = posY + dir->y * (MOVESPEED / 1.1f);
	} else {
		player->y += dirY * MOVESPEED;
	}
}

void walkBackward(vectors_t* player, vectors_t* dir, uint8_t *map) {
    float dirX = dir->x, dirY = dir->y;
	float posX = player->x, posY = player->y;
    if(map[(int)(posX - dir->x * MOVESPEED + posY * MAP_WIDTH)]) {
		player->x = posX - dir->x * (MOVESPEED / 1.1f);
	} else {
		player->x -= dirX * MOVESPEED;
	}
    if(map[(int)(posX + (posY - dir->y * MOVESPEED) * MAP_WIDTH)]) {
		player->y = posY - dir->y * (MOVESPEED / 1.1f);
	} else {
		player->y -= dirY * MOVESPEED;
	}
}
