
extern _textures

public _RenderColumn

LCD_WIDTH := 320
LCD_HEIGHT := 240
LCD_VRAM := $D40000
LCD_BUFFER := $D52C00

_RenderColumn:
	ld iy,0
	add iy,sp
	ld h,LCD_WIDTH/2
	ld l,(iy+6) ; uint8_t Y;
	mlt hl
	add hl,hl
	ld de,(iy+3) ; int X;
	add hl,de
	ld de,LCD_BUFFER
	add hl,de ; &vram[Y*320 + X];
	mlt de

	push hl ; save draw pointer

	ld e,(iy+12) ; uint8_t tex;
	ld hl,_textures
	add hl,de
	add hl,de
	add hl,de
	ld a,(iy+3) ; int x; (low byte)
	and a,3 ; x & 3
	add a,a ; (x & 3) << 5
	add a,a
	add a,a
	add a,a
	add a,a
	ld e,a
	ld hl,(hl) ; hl = &sprite data
	add hl,de ; hl = &sprite data row

	pop de ; restore draw pointer

	ld a,(iy+9) ; uint8_t h;
	or a,a
	ret z ; return if h == 0
	ld bc,LCD_WIDTH
.draw_loop:
	ldi
	ex hl,de
	add hl,bc
	inc bc
	ex hl,de
	dec a
	jr nz,.draw_loop
	ret
