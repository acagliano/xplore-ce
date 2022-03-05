
#ifndef __raycast_h__
#define __raycast_h__

void raycast(vectors_t* pos, vectors_t* dir, vectors_t* plane, uint8_t *map);

void walkBackward(vectors_t* player, vectors_t* dir, uint8_t *map);
void walkForward(vectors_t* player, vectors_t* dir, uint8_t *map);
void rotateLeft(vectors_t* dir, vectors_t* plane);
void rotateRight(vectors_t* dir, vectors_t* plane);

#endif
