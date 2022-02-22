

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

mpLcdCtrl    = $E30018
lcdBpp8      = $27
lcdBpp16     = $2C


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
	jr _EndColumnMode.clearvram

_EndColumnMode:
	spi	$2a, 0,0, 1,$3f
	spi	$2b, 0,0, 0,$ef
	spi	$36, $08
	ld	a,lcdBpp16
	ld	(mpLcdCtrl),a
.clearvram:
	jp _boot_ClearVRAM


