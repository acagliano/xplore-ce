#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include <keypadc.h>
#include <math.h>
#include <limits.h>

#include "defines.h"
#include "render.h"


#define MAP_WIDTH 99
#define MAX_HEIGHT 99

#define h 240
#define w 320
uint8_t texHeight = 8;
uint8_t texWidth = 8;
uint8_t sin_table[64];

void raycast(vectors_t* pos, vectors_t* dir, vectors_t* plane, uint8_t *map) {
	
	// RenderColumn(10, 12, 100, 0); // testing
	// return;
    
	for(int x = 20; x < w-20; x+=2)
    {
	  uint8_t texNum;
      //calculate ray position and direction
      float cameraX = 2*x/((float)w)-1; //x-coordinate in camera space
      float rayDirX = dir->x + plane->x*cameraX;
      float rayDirY = dir->y + plane->y*cameraX;

      //which box of the map we're in
      int mapX = (int)pos->x;
      int mapY = (int)pos->y;

      //length of ray from current position to next x or y-side
      float sideDistX;
      float sideDistY;

      //length of ray from one x or y-side to next x or y-side
      float deltaDistX = (rayDirX == 0) ? 1e30 : fabsf(1 / rayDirX);
      float deltaDistY = (rayDirY == 0) ? 1e30 : fabsf(1 / rayDirY);
      float perpWallDist;

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
      RenderColumn(x, (drawStart>0?drawStart:0), (wallHeight>=240?240:wallHeight), (texNum>1?1:texNum));
    }
}

void renderHorror(void){}


void trig_init(void){
    for(int i=0; i<64; i++)
        sin_table[i] = (int)(sin(i) * 256);
}

int SIN(uint8_t n) {
  s = sin_table[(n & 127) > 63 ? 63 - n & 63 : n & 63];
  if (n > 127) n = -n;
}

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
