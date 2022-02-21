
#ifndef __defines_h__
#define __defines_h__

#define MAP_WIDTH 99

#define TRIG_SCALE 0x10000
#define TRIG_PRECISION_BITS 8
#define TRIG_PRECISION (1 << TRIG_PRECISION_BITS)

#define ROTSPEED 10
#define MOVESPEED 1

// vector struct
typedef struct {
	float x;
	float y;
} vectors_t;

#endif
