

public _RenderColumn

LCD_WIDTH := 320
LCD_HEIGHT := 240
LCD_VRAM := $D40000
LCD_BUFFER := $D52C00

_RenderColumn:
	ld iy,0
	add iy,sp
	ld d,LCD_HEIGHT
	ld e,(iy+3) ; int X; (low byte)
	ld h,d
	ld l,(iy+4) ; int X; (mid byte)
	mlt hl
	mlt de
	add hl,hl
	add hl,hl
	add hl,hl
	add hl,hl
	add hl,hl
	add hl,hl
	add hl,hl
	add hl,hl ; (((X>>8) & 255) * 240) << 8
	add hl,de ; ((((X>>8) & 255) * 240) << 8) + ((X & 255) * 240)
	ld de,LCD_BUFFER
	add hl,de ; &vram[X*240];
assert ~LCD_BUFFER and $FF
	ld d,e
	ld e,(iy+6) ; uint8_t Y;
	add hl,de ; &vram[X*240 + Y];

	push hl ; save draw pointer

	ld e,(iy+12) ; uint8_t tex;
	ld hl,_Sprites
	add hl,de
	add hl,de
	add hl,de
	ld a,(iy+3) ; int x; (low byte)
	rlca ; x >> 2
	rlca
	and a,3 ; (x >> 2) & 3
	add a,a ; ((x >> 2) & 3) << 5
	add a,a
	add a,a
	add a,a
	add a,a
	ld e,a
	ld hl,(hl) ; hl = &sprite
	inc hl
	inc hl
	add hl,de ; hl = &sprite row

	pop de ; restore draw pointer

	ld a,(iy+9) ; uint8_t h;
	ld bc,32
.draw_loop:
	or a,a
	ret z ; return if h == 0
	cp a,c
	jr nc,.h_over_32
	ld c,a ; only copy remaining bytes
.h_over_32:
	ldir
	ret c ; return if h < 32
	ld c,32
	sub a,c
	jr .draw_loop

