; void unpack1bpp(void *dest, void *src, size_t len);
; note: len refers to src length, which is 1/8 the length of dest
public	_unpack1bpp
_unpack1bpp:
	ld	iy,0
	add	iy,sp
	ld	hl,(iy+6)
	ld	bc,(iy+9)
	ld	iy,(iy+3)
.outer:
	ld	e,(hl)
	ld	d,8
.inner:
	xor	a,a
	rlc	e
	adc	a,a
	ld	(iy),a
	inc	iy
	dec	d
	jr	nz,.inner
	cpi
	ret	po
	jr	.outer

; void pack2bpp(void *dest, void *src, size_t len);
; note: len refers to dest length, which is 1/8 the length of src
public	_pack1bpp
_pack1bpp:
	ld	iy,0
	add	iy,sp
	ld	hl,(iy+3)
	ld	bc,(iy+9)
	ld	iy,(iy+6)
.outer:
	ld d,8
	xor a,a
.inner:
	ld e,(iy)
	rrc e
	adc a,a
	inc iy
	dec	d
	jr	nz,.inner
	ld	(hl),a
	cpi
	ret	po
	jr	.outer
