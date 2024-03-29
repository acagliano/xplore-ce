
#ifndef __defines_h__
#define __defines_h__

#define MAP_WIDTH 99

#define TRIG_SCALE 0x10000
#define TRIG_PRECISION_BITS 8
#define TRIG_PRECISION (1 << TRIG_PRECISION_BITS)

#define ROTSPEED 0.5f
#define MOVESPEED 0.8f

// vector struct
typedef struct {
	int x;
	int y;
} vectors_t;

#endif
