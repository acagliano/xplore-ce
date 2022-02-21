
void raycast(vectors_t* pos, vectors_t* dir, vectors_t* plane, uint8_t map[99][99]){
    for(int x = 0; x < w; x++)
    {
      //calculate ray position and direction
      float cameraX = 2*x/float(w)-1; //x-coordinate in camera space
      float rayDirX = dir->x + plane->y*cameraX;
      float rayDirY = dir->y + plane->y*cameraX;

      //which box of the map we're in
      int mapX = int(pos->x);
      int mapY = int(pos->y);

      //length of ray from current position to next x or y-side
      float sideDistX;
      float sideDistY;

      //length of ray from one x or y-side to next x or y-side
      float deltaDistX = sqrt(1 + (rayDirY * rayDirY) / (rayDirX * rayDirX));
      float deltaDistY = sqrt(1 + (rayDirX * rayDirX) / (rayDirY * rayDirY));
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
        sideDistX = (pos->x - map->x) * deltaDistX;
      }
      else
      {
        stepX = 1;
        sideDistX = (mapX + 1.0 - pos->x) * deltaDistX;
      }
      if (rayDirY < 0)
      {
        stepY = -1;
        sideDistY = (pos->y - map->y) * deltaDistY;
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
        if (worldMap[mapX][mapY] > 0) hit = 1;
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
      
      //texturing calculations
      int texNum = (worldMap[mapX][mapY]>>1) - 1; //1 subtracted from it so that texture 0 can be used!

      //calculate value of wallX
      float wallX; //where exactly the wall was hit
      if (side == 0) wallX = posY + perpWallDist * rayDirY;
      else           wallX = posX + perpWallDist * rayDirX;
      wallX -= floor((wallX));

      //x coordinate on the texture
      int texX = int(wallX * double(texWidth));
      if(side == 0 && rayDirX > 0) texX = texWidth - texX - 1;
      if(side == 1 && rayDirY < 0) texX = texWidth - texX - 1;
      
    // How much to increase the texture coordinate per screen pixel
      float step = 1.0 * texHeight / lineHeight;
      // Starting texture coordinate
      float texPos = (drawStart - h / 2 + lineHeight / 2) * step;
      for(int y = drawStart; y<drawEnd; y++)
      {
        // Cast the texture coordinate to integer, and mask with (texHeight - 1) in case of overflow
        int texY = (int)texPos & (texHeight - 1);
        texPos += step;
        Uint32 color = texture[texNum][texHeight * texY + texX];
        //make color darker for y-sides: R, G and B byte each divided through two with a "shift" and an "and"
        if(side == 1) color = (color >> 1) & 8355711;
        buffer[y][x] = color;
      }
    }
}
