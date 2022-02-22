

public _BeginColumnMode
public _EndColumnMode


pSpiRange    = 0D000h
mpSpiRange   = 0F80000h
spiValid     = 8
pSpiValid    = pSpiRange + spiValid
mpSpiValid   = mpSpiRange + spiValid
spiStatus    = 12
pSpiStatus   = pSpiRange + spiStatus
mpSpiStatus  = mpSpiRange + spiStatus
spiData      = 24
pSpiData     = pSpiRange + spiData
mpSpiData    = mpSpiRange + spiData

lcdTiming0   = $00
lcdCtrl      = $18
mpLcdUpbase  = $E30010
mpLcdCtrl    = $E30018
mpLcdPalette = $E30200
lcdBpp8      = $27
lcdBpp16     = $2C

vRam         = $D40000

_boot_ClearVRAM := $000374

macro spi cmd, params&
	ld	a, cmd
	call	spiCmd
	match any, params
		iterate param, any
			ld	a, param
			call	_spiParam
		end iterate
	end match
end macro

_spiParam:
	scf
	virtual
		jr	nc, $
		load .jr_nc : byte from $$
	end virtual
	db	.jr_nc
spiCmd:
	or	a, a
	ld	hl, mpSpiData or spiValid shl 8
	ld	b, 3
.loop:	rla
	rla
	rla
	ld	(hl), a
	djnz	.loop
	ld	l, h
	ld	(hl), 1
.wait:	ld	l, spiStatus + 1
.wait1:	ld	a, (hl)
	and	a, $f0
	jr	nz, .wait1
	dec	l
.wait2:	bit	2, (hl)
	jr	nz, .wait2
	ld	l, h
	ld	(hl), a
	ret

_BeginColumnMode:
	spi	$36, $2c		; Do mysterious spi stuff to get it in column-major mode
	spi	$2a, 0,0, 0,$ef
	spi	$2b, 0,0, 1,$3f
     ld      hl,mpLcdCtrl+1
     set     4,(hl)
     set     5,(hl)
	dec hl
	ld	(hl),lcdBpp8
	call _boot_ClearVRAM
	ld	de,mpLcdPalette	; address of mmio palette
	ld	b,e			; b = 0
.loop:
	ld	a,b
	rrca
	xor	a,b
	and	a,224
	xor	a,b
	ld	(de),a
	inc	de
	ld	a,b
	rla
	rla
	rla
	ld	a,b
	rra
	ld	(de),a
	inc	de
	inc	b
	jr	nz,.loop		; loop for 256 times to fill palette
	ret

_EndColumnMode:
	spi	$2a, 0,0, 1,$3f
	spi	$2b, 0,0, 0,$ef
	spi	$36, $08
	ld	a,lcdBpp16
	ld	(mpLcdCtrl),a
	call _boot_ClearVRAM
	ld	bc,vRam
	ld	(mpLcdUpbase),bc			; set the current draw to the screen
	ld	l,lcdCtrl
	ld	(hl),de			; set lots of control parameters
	ld	l,lcdTiming0+1
	ld	de,_LcdTiming
assert vRam and $FF = 0
	ld	b,8+1			; +1 because c = 0, so first ldi will
					; decrement b
.ExchangeTimingLoop:			; exchange stored and active timing
	ld	a,(de)
	ldi
	dec	hl
	ld	(hl),a
	inc	hl
	djnz	.ExchangeTimingLoop
	ret

_LcdTiming:
;	db	14 shl 2		; PPL shl 2
	db	7			; HSW
	db	87			; HFP
	db	63			; HBP
	dw	(0 shl 10)+319		; (VSW shl 10)+LPP
	db	179			; VFP
	db	0			; VBP
	db	(0 shl 6)+(0 shl 5)+0	; (ACB shl 6)+(CLKSEL shl 5)+PCD_LO

