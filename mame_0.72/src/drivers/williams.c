/***************************************************************************

	Williams 6809 system

    driver by Michael Soderstrom, Marc LaFontaine, Aaron Giles

	Games supported:
		* Defender
		* Mayday
		* Colony 7
		* Stargate
		* Robotron
		* Joust
		* Bubbles
		* Splat!
		* Sinistar
		* PlayBall!
		* Blaster
		* Mystic Marathon
		* Turkey Shoot
		* Inferno
		* Joust 2
		* Lotto Fun

****************************************************************************

	Blitter (Stargate and Defender do not have blitter)
	---------------------------------------------------

	CA00 start_blitter    Each bits has a function
	      1000 0000 Do not process half the byte 4-7
	      0100 0000 Do not process half the byte 0-3
	      0010 0000 Shift the shape one pixel right (to display a shape on an odd pixel)
	      0001 0000 Remap, if shape != 0 then pixel = mask
	      0000 1000 Source  1 = take source 0 = take Mask only
	      0000 0100 ?
	      0000 0010 Transparent
	      0000 0001
	CA01 blitter_mask     Not really a mask, more a remap color, see Blitter
	CA02 blitter_source   hi
	CA03 blitter_source   lo
	CA04 blitter_dest     hi
	CA05 blitter_dest     lo
	CA06 blitter_w_h      H  Do a XOR with 4 to have the real value (Except Splat)
	CA07 blitter_w_h      W  Do a XOR with 4 to have the real value (Except Splat)

	CB00 6 bits of the video counters bits 2-7

	CBFF watchdog

	CC00-CFFF 1K X 4 CMOS ram battery backed up (8 bits on Sinistar)

****************************************************************************

	Blaster Bubbles Joust Robotron Sinistar Splat Stargate
	------------------------------------------------------

	0000-8FFF ROM   (for Blaster, 0000-3FFF is a bank of 12 ROMs)
	0000-97FF Video  RAM Bank switched with ROM (96FF for Blaster)
	9800-BFFF RAM
		0xBB00 Blaster only, Color 0 for each line (256 entry)
		0xBC00 Blaster only, Color 0 flags, latch color only if bit 0 = 1 (256 entry)
		    Do something else with the bit 1, I do not know what
	C000-CFFF I/O
	D000-FFFF ROM

	c000-C00F color_registers  (16 bytes of BBGGGRRR)

	c804 widget_pia_dataa (widget = I/O board)
	c805 widget_pia_ctrla
	c806 widget_pia_datab
	c807 widget_pia_ctrlb (CB2 select between player 1 and player 2
			       controls if Table or Joust)
	      bits 5-3 = 110 = player 2
	      bits 5-3 = 111 = player 1

	c80c rom_pia_dataa
	c80d rom_pia_ctrla
	c80e rom_pia_datab
	      bit 0 \
	      bit 1 |
	      bit 2 |-6 bits to sound board
	      bit 3 |
	      bit 4 |
	      bit 5 /
	      bit 6 \
	      bit 7 /Plus CA2 and CB2 = 4 bits to drive the LED 7 segment
	c80f rom_pia_ctrlb

	C900 rom_enable_scr_ctrl  Switch between video ram and rom at 0000-97FF

	C940 Blaster only: Select bank in the color Prom for color remap
	C980 Blaster only: Select which ROM is at 0000-3FFF
	C9C0 Blaster only: bit 0 = enable the color 0 changing each lines
			   bit 1 = erase back each frame

****************************************************************************

	Robotron
	--------
	c804 widget_pia_dataa (widget = I/O board)
	  bit 0  Move Up
	  bit 1  Move Down
	  bit 2  Move Left
	  bit 3  Move Right
	  bit 4  1 Player
	  bit 5  2 Players
	  bit 6  Fire Up
	  bit 7  Fire Down

	c806 widget_pia_datab
	  bit 0  Fire Left
	  bit 1  Fire Right
	  bit 2
	  bit 3
	  bit 4
	  bit 5
	  bit 6
	  bit 7

	c80c rom_pia_dataa
	  bit 0  Auto Up
	  bit 1  Advance
	  bit 2  Right Coin
	  bit 3  High Score Reset
	  bit 4  Left Coin
	  bit 5  Center Coin
	  bit 6  Slam Door Tilt
	  bit 7  Hand Shake from sound board

****************************************************************************

	Joust
	-----
	c804 widget_pia_dataa (widget = I/O board)
	  bit 0  Move Left   player 1/2
	  bit 1  Move Right  player 1/2
	  bit 2  Flap        player 1/2
	  bit 3
	  bit 4  2 Player
	  bit 5  1 Players
	  bit 6
	  bit 7

	c806 widget_pia_datab
	  bit 0
	  bit 1
	  bit 2
	  bit 3
	  bit 4
	  bit 5
	  bit 6
	  bit 7

	c80c rom_pia_dataa
	  bit 0  Auto Up
	  bit 1  Advance
	  bit 2  Right Coin
	  bit 3  High Score Reset
	  bit 4  Left Coin
	  bit 5  Center Coin
	  bit 6  Slam Door Tilt
	  bit 7  Hand Shake from sound board

****************************************************************************

	Stargate
	--------
	c804 widget_pia_dataa (widget = I/O board)
	  bit 0  Fire
	  bit 1  Thrust
	  bit 2  Smart Bomb
	  bit 3  HyperSpace
	  bit 4  2 Players
	  bit 5  1 Player
	  bit 6  Reverse
	  bit 7  Down

	c806 widget_pia_datab
	  bit 0  Up
	  bit 1  Inviso
	  bit 2
	  bit 3
	  bit 4
	  bit 5
	  bit 6
	  bit 7  0 = Upright  1 = Table

	c80c rom_pia_dataa
	  bit 0  Auto Up
	  bit 1  Advance
	  bit 2  Right Coin        (High Score Reset in schematics)
	  bit 3  High Score Reset  (Left Coin in schematics)
	  bit 4  Left Coin         (Center Coin in schematics)
	  bit 5  Center Coin       (Right Coin in schematics)
	  bit 6  Slam Door Tilt
	  bit 7  Hand Shake from sound board

****************************************************************************

	Splat
	-----
	c804 widget_pia_dataa (widget = I/O board)
	  bit 0  Walk Up
	  bit 1  Walk Down
	  bit 2  Walk Left
	  bit 3  Walk Right
	  bit 4  1 Player
	  bit 5  2 Players
	  bit 6  Throw Up
	  bit 7  Throw Down

	c806 widget_pia_datab
	  bit 0  Throw Left
	  bit 1  Throw Right
	  bit 2
	  bit 3
	  bit 4
	  bit 5
	  bit 6
	  bit 7

	c80c rom_pia_dataa
	  bit 0  Auto Up
	  bit 1  Advance
	  bit 2  Right Coin
	  bit 3  High Score Reset
	  bit 4  Left Coin
	  bit 5  Center Coin
	  bit 6  Slam Door Tilt
	  bit 7  Hand Shake from sound board

****************************************************************************

	Blaster
	-------
	c804 widget_pia_dataa (widget = I/O board)
	  bit 0  up/down switch a
	  bit 1  up/down switch b
	  bit 2  up/down switch c
	  bit 3  up/down direction
	  bit 4  left/right switch a
	  bit 5  left/right switch b
	  bit 6  left/right switch c
	  bit 7  left/right direction

	c806 widget_pia_datab
	  bit 0  Thrust (Panel)
	  bit 1  Blast
	  bit 2  Thrust (Joystick)
	  bit 3
	  bit 4  1 Player
	  bit 5  2 Player
	  bit 6
	  bit 7

	c80c rom_pia_dataa
	  bit 0  Auto Up
	  bit 1  Advance
	  bit 2  Right Coin
	  bit 3  High Score Reset
	  bit 4  Left Coin
	  bit 5  Center Coin
	  bit 6  Slam Door Tilt
	  bit 7  Hand Shake from sound board

****************************************************************************

	Sinistar
	--------
	c804 widget_pia_dataa (widget = I/O board)
	  bit 0  up/down switch a
	  bit 1  up/down switch b
	  bit 2  up/down switch c
	  bit 3  up/down direction
	  bit 4  left/right switch a
	  bit 5  left/right switch b
	  bit 6  left/right switch c
	  bit 7  left/right direction

	c806 widget_pia_datab
	  bit 0  Fire
	  bit 1  Bomb
	  bit 2
	  bit 3
	  bit 4  1 Player
	  bit 5  2 Player
	  bit 6
	  bit 7

	c80c rom_pia_dataa
	  bit 0  Auto Up
	  bit 1  Advance
	  bit 2  Right Coin
	  bit 3  High Score Reset
	  bit 4  Left Coin
	  bit 5  Center Coin
	  bit 6  Slam Door Tilt
	  bit 7  Hand Shake from sound board

****************************************************************************

	Bubbles
	-------
	c804 widget_pia_dataa (widget = I/O board)
	  bit 0  Up
	  bit 1  Down
	  bit 2  Left
	  bit 3  Right
	  bit 4  2 Players
	  bit 5  1 Player
	  bit 6
	  bit 7

	c806 widget_pia_datab
	  bit 0
	  bit 1
	  bit 2
	  bit 3
	  bit 4
	  bit 5
	  bit 6
	  bit 7

	c80c rom_pia_dataa
	  bit 0  Auto Up
	  bit 1  Advance
	  bit 2  Right Coin        (High Score Reset in schematics)
	  bit 3  High Score Reset  (Left Coin in schematics)
	  bit 4  Left Coin         (Center Coin in schematics)
	  bit 5  Center Coin       (Right Coin in schematics)
	  bit 6  Slam Door Tilt
	  bit 7  Hand Shake from sound board

****************************************************************************

	Defender
	--------
	0000-9800 Video RAM
	C000-CFFF ROM (4 banks) + I/O
	d000-ffff ROM

	c000-c00f color_registers  (16 bytes of BBGGGRRR)

	C3FC      WatchDog

	C400-C4FF CMOS ram battery backed up

	C800      6 bits of the video counters bits 2-7

	cc00 pia1_dataa (widget = I/O board)
	  bit 0  Auto Up
	  bit 1  Advance
	  bit 2  Right Coin
	  bit 3  High Score Reset
	  bit 4  Left Coin
	  bit 5  Center Coin
	  bit 6
	  bit 7
	cc01 pia1_ctrla

	cc02 pia1_datab
	  bit 0 \
	  bit 1 |
	  bit 2 |-6 bits to sound board
	  bit 3 |
	  bit 4 |
	  bit 5 /
	  bit 6 \
	  bit 7 /Plus CA2 and CB2 = 4 bits to drive the LED 7 segment
	cc03 pia1_ctrlb (CB2 select between player 1 and player 2 controls if Table)

	cc04 pia2_dataa
	  bit 0  Fire
	  bit 1  Thrust
	  bit 2  Smart Bomb
	  bit 3  HyperSpace
	  bit 4  2 Players
	  bit 5  1 Player
	  bit 6  Reverse
	  bit 7  Down
	cc05 pia2_ctrla

	cc06 pia2_datab
	  bit 0  Up
	  bit 1
	  bit 2
	  bit 3
	  bit 4
	  bit 5
	  bit 6
	  bit 7
	cc07 pia2_ctrlb
	  Control the IRQ

	d000 Select bank (c000-cfff)
	  0 = I/O
	  1 = BANK 1
	  2 = BANK 2
	  3 = BANK 3
	  7 = BANK 4

****************************************************************************

	Mystic Marathon (1983)
	Turkey Shoot (1984)
	Inferno (1984)
	Joust2 Survival of the Fittest (1986)

	All have two boards, a large board with lots of RAM and
	three ROMs, and a smaller board with lots of ROMs,
	the CPU, the 6821 PIAs, and the two "Special Chip 2"
	custom BIT/BLT chips.
	Joust 2 has an additional music/speech board that has a
	68B09E CPU, 68B21 PIA, Harris 55564-5 CVSD, and a YM2151.

	Contact Michael Soderstrom (ichael@geocities.com) if you
	have any additional information or corrections.

	Memory Map:

	15 14 13 12  11 10  9  8   7  6  5  4   3  2  1  0
	--------------------------------------------------
	 x  x  x  x   x  x  x  x   x  x  x  x   x  x  x  x	0000-BFFF	48K DRAM

	 0  0  0  x   x  x  x  x   x  x  x  x   x  x  x  x	0000-1FFF	8K ROM
	 0  0  1  x   x  x  x  x   x  x  x  x   x  x  x  x	2000-3FFF	8K ROM
	 0  1  0  x   x  x  x  x   x  x  x  x   x  x  x  x	4000-5FFF	8K ROM
	 0  1  1  x   x  x  x  x   x  x  x  x   x  x  x  x	6000-7FFF	8K ROM

	 1  0  0  0   x  x  x  x   x  x  x  x   x  x  x  x	8000-8FFF	EN_COLOR* (PAGE3 only)

	 0  x  x  x   x  x  x  x   x  x  x  x   x  x  x  x	0000-7FFF	OE_DRAM* (PAGE0 and read only) or:
	 1  0  x  x   x  x  x  x   x  x  x  x   x  x  x  x	9000-BFFF	OE_DRAM* (!EN COLOR and read only)

	 1  1  0  0   x  x  x  x   x  x  x  x   x  x  x  x	C000-CFFF	I/O:
	 1  1  0  0   0  x  x  x   x  x  x  x   x  x  x  x	C000-C7FF	MAP_EN*
	 1  1  0  0   1  0  0  0   0  x  x  x   x  x  x  x	C800-C87F	CS_PAGE
	 1  1  0  0   1  0  0  0   1  x  x  x   x  x  x  x	C880-C87F	CS_INT* (blitter)
	 1  1  0  0   1  0  0  1   0  x  x  x   x  x  x  x	C900-C97F	CS_WDOG* (data = 0x14)
	 1  1  0  0   1  0  0  1   1  x  x  x   x  x  x  x	C980-C9FF	CS_PIA
	 1  1  0  0   1  0  0  1   1  x  x  x   0  0  x  x	C980-C983	PIA IC5
	 1  1  0  0   1  0  0  1   1  x  x  x   0  1  x  x	C984-C987	PIA IC6
	 1  1  0  0   1  0  0  1   1  x  x  x   1  1  x  x	C98C		7 segment LED

	 1  1  0  0   1  0  1  1   0  0  0  x   x  x  x  x	CB00-CB1F	CK_FG
	 1  1  0  0   1  0  1  1   0  0  1  x   x  x  x  x	CB20-CB3F	CK_BG
	 1  1  0  0   1  0  1  1   0  1  0  x   x  x  x  x	CB40-CB5F	CK_SCL
	 1  1  0  0   1  0  1  1   0  1  1  x   x  x  x  x	CB60-CB7F	CK_SCH
	 1  1  0  0   1  0  1  1   1  0  0  x   x  x  x  x	CB80-CB9F	FLIP clk
	 1  1  0  0   1  0  1  1   1  0  1  x   x  x  x  x	CBA0-CBBF	DMA_WRINH clk

	 1  1  0  0   1  0  1  1   1  1  1  0   x  x  x  x	CBE0-CBEF	EN VPOS*

	 1  1  0  0   1  1  0  0   x  x  x  x   x  x  x  x	CC00-CCFF	1Kx4 CMOS RAM MEM_PROT protected
	 1  1  0  0   1  1  x  x   x  x  x  x   x  x  x  x	CD00-CFFF	          not MEM_PROT protected

	 Mystic Marathon/Inferno:
	 1  1  0  1   0  x  x  x   x  x  x  x   x  x  x  x	D000-D7FF	SRAM0*
	 1  1  0  1   1  x  x  x   x  x  x  x   x  x  x  x	D800-DFFF	SRAM1*
	 1  1  1  0   x  x  x  x   x  x  x  x   x  x  x  x	E000-EFFF	EXXX* 4K ROM
	 1  1  1  1   x  x  x  x   x  x  x  x   x  x  x  x	F000-FFFF	FXXX* 4K ROM

	 Turkey Shoot/Joust2:
	 1  1  0  1   x  x  x  x   x  x  x  x   x  x  x  x	D000-DFFF	DXXX* 4K ROM
	 1  1  1  0   x  x  x  x   x  x  x  x   x  x  x  x	E000-EFFF	EXXX* 4K ROM
	 1  1  1  1   x  x  x  x   x  x  x  x   x  x  x  x	F000-FFFF	FXXX* 4K ROM

	6802/6808 Sound

	 0  0  0  x   x  x  x  x   0  x  x  x   x  x  x  x	0000-007F	128 bytes RAM
	 0  0  1  x   x  x  x  x   x  x  x  x   x  x  x  x	2000-3FFF	CS PIA IC4
	 1  1  1  x   x  x  x  x   x  x  x  x   x  x  x  x	E000-FFFF	8K ROM

***************************************************************************/

#include "driver.h"
#include "machine/6821pia.h"
#include "sndhrdw/williams.h"
#include "vidhrdw/generic.h"
#include "williams.h"


/**** configuration macros ****/

#define CONFIGURE_CMOS(a,l) \
	generic_nvram = &memory_region(REGION_CPU1)[a];\
	generic_nvram_size = l

#define CONFIGURE_BLITTER(x,r,c) \
	williams_blitter_xor = x;\
	williams_blitter_remap = r;\
	williams_blitter_clip = c

#define CONFIGURE_TILEMAP(m,p,f,s,b) \
	williams2_tilemap_mask = m;\
	williams2_row_to_palette = p;\
	williams2_M7_flip = (f) ? 0x80 : 0x00;\
	williams2_videoshift = s;\
	williams2_special_bg_color = b

#define CONFIGURE_PIAS(a,b,c) \
	pia_unconfig();\
	pia_config(0, PIA_STANDARD_ORDERING, &a);\
	pia_config(1, PIA_STANDARD_ORDERING, &b);\
	pia_config(2, PIA_STANDARD_ORDERING, &c)



/*************************************
 *
 *	Defender memory handlers
 *
 *************************************/

static MEMORY_READ_START( defender_readmem )
	{ 0x0000, 0x97ff, MRA_BANK1 },
	{ 0x9800, 0xbfff, MRA_RAM },
	{ 0xc000, 0xcfff, MRA_BANK2 },
	{ 0xd000, 0xffff, MRA_ROM },
MEMORY_END


static MEMORY_WRITE_START( defender_writemem )
	{ 0x0000, 0x97ff, williams_videoram_w, &videoram, &videoram_size },
	{ 0x9800, 0xbfff, MWA_RAM },
	{ 0xc000, 0xcfff, MWA_BANK2, &defender_bank_base },
	{ 0xc000, 0xc00f, MWA_RAM, &paletteram },
	{ 0xd000, 0xdfff, defender_bank_select_w },
	{ 0xe000, 0xffff, MWA_ROM },
MEMORY_END



/*************************************
 *
 *	General Williams memory handlers
 *
 *************************************/

static MEMORY_READ_START( williams_readmem )
	{ 0x0000, 0x97ff, MRA_BANK1 },
	{ 0x9800, 0xbfff, MRA_RAM },
	{ 0xc804, 0xc807, pia_0_r },
	{ 0xc80c, 0xc80f, pia_1_r },
	{ 0xcb00, 0xcb00, williams_video_counter_r },
	{ 0xcc00, 0xcfff, MRA_RAM },
	{ 0xd000, 0xffff, MRA_ROM },
MEMORY_END


static MEMORY_WRITE_START( williams_writemem )
	{ 0x0000, 0x97ff, williams_videoram_w, &williams_bank_base, &videoram_size },
	{ 0x9800, 0xbfff, MWA_RAM },
	{ 0xc000, 0xc00f, paletteram_BBGGGRRR_w, &paletteram },
	{ 0xc804, 0xc807, pia_0_w },
	{ 0xc80c, 0xc80f, pia_1_w },
	{ 0xc900, 0xc900, williams_vram_select_w },
	{ 0xca00, 0xca07, williams_blitter_w, &williams_blitterram },
	{ 0xcbff, 0xcbff, watchdog_reset_w },
	{ 0xcc00, 0xcfff, MWA_RAM },
	{ 0xd000, 0xffff, MWA_ROM },
MEMORY_END



/*************************************
 *
 *	Blaster memory handlers
 *
 *************************************/

static MEMORY_READ_START( blaster_readmem )
	{ 0x0000, 0x3fff, MRA_BANK1 },
	{ 0x4000, 0x96ff, MRA_BANK2 },
	{ 0x9700, 0xbfff, MRA_RAM },
	{ 0xc804, 0xc807, pia_0_r },
	{ 0xc80c, 0xc80f, pia_1_r },
	{ 0xcb00, 0xcb00, williams_video_counter_r },
	{ 0xcc00, 0xcfff, MRA_RAM },
	{ 0xd000, 0xffff, MRA_ROM },
MEMORY_END


static MEMORY_WRITE_START( blaster_writemem )
	{ 0x0000, 0x96ff, williams_videoram_w, &williams_bank_base, &videoram_size },
	{ 0x9700, 0xbaff, MWA_RAM },
	{ 0xbb00, 0xbbff, blaster_palette_0_w, &blaster_color_zero_table },
	{ 0xbc00, 0xbcff, MWA_RAM, &blaster_color_zero_flags },
	{ 0xbd00, 0xbfff, MWA_RAM },
	{ 0xc000, 0xc00f, paletteram_BBGGGRRR_w, &paletteram },
	{ 0xc804, 0xc807, pia_0_w },
	{ 0xc80c, 0xc80f, pia_1_w },
	{ 0xc900, 0xc900, blaster_vram_select_w },
	{ 0xc940, 0xc940, blaster_remap_select_w },
	{ 0xc980, 0xc980, blaster_bank_select_w },
	{ 0xc9c0, 0xc9c0, MWA_RAM, &blaster_video_bits },
	{ 0xca00, 0xca07, williams_blitter_w, &williams_blitterram },
	{ 0xcbff, 0xcbff, watchdog_reset_w },
	{ 0xcc00, 0xcfff, MWA_RAM },
	{ 0xd000, 0xffff, MWA_ROM },
MEMORY_END



/*************************************
 *
 *	Later Williams memory handlers
 *
 *************************************/

static MEMORY_READ_START( williams2_readmem )
	{ 0x0000, 0x7fff, MRA_BANK1 },
	{ 0x8000, 0x87ff, MRA_BANK2 },
	{ 0x8800, 0x8fff, MRA_BANK3 },
	{ 0x9000, 0xbfff, MRA_RAM },
	{ 0xc000, 0xc7ff, MRA_RAM },
	{ 0xc980, 0xc983, pia_1_r },
	{ 0xc984, 0xc987, pia_0_r },
	{ 0xcbe0, 0xcbe0, williams_video_counter_r },
	{ 0xcc00, 0xcfff, MRA_RAM },
	{ 0xd000, 0xffff, MRA_ROM },
MEMORY_END


static MEMORY_WRITE_START( williams2_writemem )
	{ 0x0000, 0x8fff, williams2_videoram_w, &videoram, &videoram_size },
	{ 0x9000, 0xbfff, MWA_RAM },
	{ 0xc000, 0xc7ff, MWA_RAM },
	{ 0xc800, 0xc800, williams2_bank_select_w },
	{ 0xc880, 0xc887, williams_blitter_w, &williams_blitterram },
	{ 0xc900, 0xc900, watchdog_reset_w },
	{ 0xc980, 0xc983, pia_1_w },
	{ 0xc984, 0xc987, pia_0_w },
	{ 0xc98c, 0xc98c, williams2_7segment_w },
	{ 0xcb00, 0xcb00, williams2_fg_select_w },
	{ 0xcb20, 0xcb20, williams2_bg_select_w },
	{ 0xcb40, 0xcb40, MWA_RAM, &williams2_xscroll_low },
	{ 0xcb60, 0xcb60, MWA_RAM, &williams2_xscroll_high },
	{ 0xcb80, 0xcb80, MWA_RAM },
	{ 0xcba0, 0xcba0, MWA_RAM, &williams2_blit_inhibit },
	{ 0xcc00, 0xcfff, MWA_RAM },
	{ 0xd000, 0xffff, MWA_ROM },
MEMORY_END



/*************************************
 *
 *	Sound board memory handlers
 *
 *************************************/

static MEMORY_READ_START( sound_readmem )
	{ 0x0000, 0x007f, MRA_RAM },
	{ 0x0400, 0x0403, pia_2_r },
	{ 0x8400, 0x8403, pia_2_r },	/* used by Colony 7, perhaps others? */
	{ 0xb000, 0xffff, MRA_ROM },	/* most games start at $F000, Sinistar starts at $B000 */
MEMORY_END


static MEMORY_WRITE_START( sound_writemem )
	{ 0x0000, 0x007f, MWA_RAM },
	{ 0x0400, 0x0403, pia_2_w },
	{ 0x8400, 0x8403, pia_2_w },	/* used by Colony 7, perhaps others? */
	{ 0xb000, 0xffff, MWA_ROM },	/* most games start at $F000, Sinistar starts at $B000 */
MEMORY_END



/*************************************
 *
 *	Later sound board memory handlers
 *
 *************************************/

static MEMORY_READ_START( williams2_sound_readmem )
	{ 0x0000, 0x00ff, MRA_RAM },
	{ 0x2000, 0x2003, pia_2_r },
	{ 0xe000, 0xffff, MRA_ROM },
MEMORY_END


static MEMORY_WRITE_START( williams2_sound_writemem )
	{ 0x0000, 0x00ff, MWA_RAM },
	{ 0x2000, 0x2003, pia_2_w },
	{ 0xe000, 0xffff, MWA_ROM },
MEMORY_END



/*************************************
 *
 *	Port definitions
 *
 *************************************/

INPUT_PORTS_START( defender )
	PORT_START      /* IN0 */
	PORT_BITX(0x01, IP_ACTIVE_HIGH, IPT_BUTTON1, "Fire", IP_KEY_DEFAULT, IP_JOY_DEFAULT )
	PORT_BITX(0x02, IP_ACTIVE_HIGH, IPT_BUTTON2, "Thrust", IP_KEY_DEFAULT, IP_JOY_DEFAULT )
	PORT_BITX(0x04, IP_ACTIVE_HIGH, IPT_BUTTON3, "Smart Bomb", IP_KEY_DEFAULT, IP_JOY_DEFAULT )
	PORT_BITX(0x08, IP_ACTIVE_HIGH, IPT_BUTTON4, "Hyperspace", IP_KEY_DEFAULT, IP_JOY_DEFAULT )
	PORT_BIT( 0x10, IP_ACTIVE_HIGH, IPT_START2 )
	PORT_BIT( 0x20, IP_ACTIVE_HIGH, IPT_START1 )
	PORT_BITX(0x40, IP_ACTIVE_HIGH, IPT_BUTTON6, "Reverse", IP_KEY_DEFAULT, IP_JOY_DEFAULT )
	PORT_BIT( 0x80, IP_ACTIVE_HIGH, IPT_JOYSTICK_DOWN | IPF_8WAY )

	PORT_START      /* IN1 */
	PORT_BIT( 0x01, IP_ACTIVE_HIGH, IPT_JOYSTICK_UP | IPF_8WAY )
	PORT_BIT( 0xfe, IP_ACTIVE_HIGH, IPT_UNKNOWN )

	PORT_START      /* IN2 */
	PORT_BITX(0x01, IP_ACTIVE_HIGH, 0, "Auto Up", KEYCODE_F1, IP_JOY_NONE )
	PORT_BITX(0x02, IP_ACTIVE_HIGH, 0, "Advance", KEYCODE_F2, IP_JOY_NONE )
	PORT_BIT( 0x04, IP_ACTIVE_HIGH, IPT_SERVICE1 )
	PORT_BITX(0x08, IP_ACTIVE_HIGH, 0, "High Score Reset", KEYCODE_7, IP_JOY_NONE )
	PORT_BIT( 0x10, IP_ACTIVE_HIGH, IPT_COIN1 )
	PORT_BIT( 0x20, IP_ACTIVE_HIGH, IPT_COIN2 )
	PORT_BIT( 0x40, IP_ACTIVE_HIGH, IPT_TILT )

	PORT_START      /* IN3 - fake port for better joystick control */
	/* This fake port is handled via defender_input_port_1 */
	PORT_BIT( 0x02, IP_ACTIVE_HIGH, IPT_JOYSTICK_RIGHT | IPF_8WAY | IPF_CHEAT )
	PORT_BIT( 0x40, IP_ACTIVE_HIGH, IPT_JOYSTICK_LEFT | IPF_8WAY | IPF_CHEAT )
INPUT_PORTS_END


INPUT_PORTS_START( colony7 )
	PORT_START	/* IN0 */
	PORT_BIT( 0x01, IP_ACTIVE_HIGH, IPT_JOYSTICK_DOWN )
	PORT_BIT( 0x02, IP_ACTIVE_HIGH, IPT_JOYSTICK_RIGHT )
	PORT_BIT( 0x04, IP_ACTIVE_HIGH, IPT_JOYSTICK_LEFT )
	PORT_BIT( 0x08, IP_ACTIVE_HIGH, IPT_JOYSTICK_UP )
	PORT_BIT( 0x10, IP_ACTIVE_HIGH, IPT_START2 )
	PORT_BIT( 0x20, IP_ACTIVE_HIGH, IPT_START1 )
	PORT_BIT( 0x40, IP_ACTIVE_HIGH, IPT_BUTTON1 )
	PORT_BIT( 0x80, IP_ACTIVE_HIGH, IPT_BUTTON2 )

	PORT_START	/* IN1 */
	PORT_BIT( 0x01, IP_ACTIVE_HIGH, IPT_BUTTON3 )
	PORT_BIT( 0xfe, IP_ACTIVE_HIGH, IPT_UNKNOWN )

	PORT_START	/* IN2 */
	PORT_BIT( 0x01, IP_ACTIVE_HIGH, IPT_UNKNOWN )
	PORT_BIT( 0x02, IP_ACTIVE_HIGH, IPT_UNKNOWN )
	PORT_BIT( 0x04, IP_ACTIVE_HIGH, IPT_UNKNOWN )
	PORT_BIT( 0x08, IP_ACTIVE_HIGH, IPT_UNKNOWN )
	PORT_BIT( 0x10, IP_ACTIVE_HIGH, IPT_COIN1 )
	PORT_BIT( 0x20, IP_ACTIVE_HIGH, IPT_COIN2 )
	PORT_BIT( 0x40, IP_ACTIVE_HIGH, IPT_UNKNOWN )
	PORT_BIT( 0x80, IP_ACTIVE_HIGH, IPT_UNKNOWN )
INPUT_PORTS_END


INPUT_PORTS_START( stargate )
	PORT_START      /* IN0 */
	PORT_BITX(0x01, IP_ACTIVE_HIGH, IPT_BUTTON1, "Fire", IP_KEY_DEFAULT, IP_JOY_DEFAULT )
	PORT_BITX(0x02, IP_ACTIVE_HIGH, IPT_BUTTON2, "Thrust", IP_KEY_DEFAULT, IP_JOY_DEFAULT )
	PORT_BITX(0x04, IP_ACTIVE_HIGH, IPT_BUTTON3, "Smart Bomb", IP_KEY_DEFAULT, IP_JOY_DEFAULT )
	PORT_BITX(0x08, IP_ACTIVE_HIGH, IPT_BUTTON6, "Hyperspace", IP_KEY_DEFAULT, IP_JOY_DEFAULT )
	PORT_BIT( 0x10, IP_ACTIVE_HIGH, IPT_START2 )
	PORT_BIT( 0x20, IP_ACTIVE_HIGH, IPT_START1 )
	PORT_BITX(0x40, IP_ACTIVE_HIGH, IPT_BUTTON4, "Reverse", IP_KEY_DEFAULT, IP_JOY_DEFAULT )
	PORT_BIT( 0x80, IP_ACTIVE_HIGH, IPT_JOYSTICK_DOWN | IPF_8WAY )

	PORT_START      /* IN1 */
	PORT_BIT( 0x01, IP_ACTIVE_HIGH, IPT_JOYSTICK_UP | IPF_8WAY )
	PORT_BITX(0x02, IP_ACTIVE_HIGH, IPT_BUTTON5, "Inviso", IP_KEY_DEFAULT, IP_JOY_DEFAULT )
	PORT_BIT( 0xfc, IP_ACTIVE_HIGH, IPT_UNKNOWN )

	PORT_START      /* IN2 */
	PORT_BITX(0x01, IP_ACTIVE_HIGH, 0, "Auto Up", KEYCODE_F1, IP_JOY_NONE )
	PORT_BITX(0x02, IP_ACTIVE_HIGH, 0, "Advance", KEYCODE_F2, IP_JOY_NONE )
	PORT_BIT( 0x04, IP_ACTIVE_HIGH, IPT_SERVICE1 )
	PORT_BITX(0x08, IP_ACTIVE_HIGH, 0, "High Score Reset", KEYCODE_7, IP_JOY_NONE )
	PORT_BIT( 0x10, IP_ACTIVE_HIGH, IPT_COIN1 )
	PORT_BIT( 0x20, IP_ACTIVE_HIGH, IPT_COIN2 )
	PORT_BIT( 0x40, IP_ACTIVE_HIGH, IPT_TILT )

	PORT_START      /* IN3 - fake port for better joystick control */
	/* This fake port is handled via stargate_input_port_0 */
	PORT_BIT( 0x02, IP_ACTIVE_HIGH, IPT_JOYSTICK_RIGHT | IPF_8WAY | IPF_CHEAT )
	PORT_BIT( 0x40, IP_ACTIVE_HIGH, IPT_JOYSTICK_LEFT | IPF_8WAY | IPF_CHEAT )
INPUT_PORTS_END


INPUT_PORTS_START( joust )
	PORT_START      /* IN0 */
	PORT_BIT( 0x01, IP_ACTIVE_HIGH, IPT_JOYSTICK_LEFT | IPF_2WAY | IPF_PLAYER2 )
	PORT_BIT( 0x02, IP_ACTIVE_HIGH, IPT_JOYSTICK_RIGHT | IPF_2WAY | IPF_PLAYER2 )
	PORT_BIT( 0x04, IP_ACTIVE_HIGH, IPT_BUTTON1 | IPF_PLAYER2 )
	PORT_BIT( 0x08, IP_ACTIVE_HIGH, IPT_UNUSED )
	PORT_BIT( 0x10, IP_ACTIVE_HIGH, IPT_START2 )
	PORT_BIT( 0x20, IP_ACTIVE_HIGH, IPT_START1 )
	PORT_BIT( 0xc0, IP_ACTIVE_HIGH, IPT_UNUSED )

	PORT_START      /* IN1 */
	PORT_BIT( 0xff, IP_ACTIVE_HIGH, IPT_UNUSED )

	PORT_START      /* IN2 */
	PORT_BITX(0x01, IP_ACTIVE_HIGH, 0, "Auto Up", KEYCODE_F1, IP_JOY_NONE )
	PORT_BITX(0x02, IP_ACTIVE_HIGH, 0, "Advance", KEYCODE_F2, IP_JOY_NONE )
	PORT_BIT( 0x04, IP_ACTIVE_HIGH, IPT_SERVICE1 )
	PORT_BITX(0x08, IP_ACTIVE_HIGH, 0, "High Score Reset", KEYCODE_7, IP_JOY_NONE )
	PORT_BIT( 0x10, IP_ACTIVE_HIGH, IPT_COIN1 )
	PORT_BIT( 0x20, IP_ACTIVE_HIGH, IPT_COIN2 )
	PORT_BIT( 0x40, IP_ACTIVE_HIGH, IPT_TILT )

	PORT_START      /* IN3 (muxed with IN0) */
	PORT_BIT( 0x01, IP_ACTIVE_HIGH, IPT_JOYSTICK_LEFT | IPF_2WAY | IPF_PLAYER1 )
	PORT_BIT( 0x02, IP_ACTIVE_HIGH, IPT_JOYSTICK_RIGHT | IPF_2WAY | IPF_PLAYER1 )
	PORT_BIT( 0x04, IP_ACTIVE_HIGH, IPT_BUTTON1 | IPF_PLAYER1 )
	PORT_BIT( 0x08, IP_ACTIVE_HIGH, IPT_UNUSED )
	PORT_BIT( 0x10, IP_ACTIVE_HIGH, IPT_START2 )
	PORT_BIT( 0x20, IP_ACTIVE_HIGH, IPT_START1 )
	PORT_BIT( 0xc0, IP_ACTIVE_HIGH, IPT_UNUSED )
INPUT_PORTS_END


INPUT_PORTS_START( robotron )
	PORT_START      /* IN0 */
	PORT_BIT( 0x01, IP_ACTIVE_HIGH, IPT_JOYSTICKLEFT_UP )
	PORT_BIT( 0x02, IP_ACTIVE_HIGH, IPT_JOYSTICKLEFT_DOWN )
	PORT_BIT( 0x04, IP_ACTIVE_HIGH, IPT_JOYSTICKLEFT_LEFT )
	PORT_BIT( 0x08, IP_ACTIVE_HIGH, IPT_JOYSTICKLEFT_RIGHT )
	PORT_BIT( 0x10, IP_ACTIVE_HIGH, IPT_START1 )
	PORT_BIT( 0x20, IP_ACTIVE_HIGH, IPT_START2 )
	PORT_BIT( 0x40, IP_ACTIVE_HIGH, IPT_JOYSTICKRIGHT_UP )
	PORT_BIT( 0x80, IP_ACTIVE_HIGH, IPT_JOYSTICKRIGHT_DOWN )

	PORT_START      /* IN1 */
	PORT_BIT( 0x01, IP_ACTIVE_HIGH, IPT_JOYSTICKRIGHT_LEFT )
	PORT_BIT( 0x02, IP_ACTIVE_HIGH, IPT_JOYSTICKRIGHT_RIGHT )
	PORT_BIT( 0xfc, IP_ACTIVE_HIGH, IPT_UNKNOWN )

	PORT_START      /* IN2 */
	PORT_BITX(0x01, IP_ACTIVE_HIGH, 0, "Auto Up", KEYCODE_F1, IP_JOY_NONE )
	PORT_BITX(0x02, IP_ACTIVE_HIGH, 0, "Advance", KEYCODE_F2, IP_JOY_NONE )
	PORT_BIT( 0x04, IP_ACTIVE_HIGH, IPT_SERVICE1 )
	PORT_BITX(0x08, IP_ACTIVE_HIGH, 0, "High Score Reset", KEYCODE_7, IP_JOY_NONE )
	PORT_BIT( 0x10, IP_ACTIVE_HIGH, IPT_COIN1 )
	PORT_BIT( 0x20, IP_ACTIVE_HIGH, IPT_COIN2 )
	PORT_BIT( 0x40, IP_ACTIVE_HIGH, IPT_TILT )
INPUT_PORTS_END


INPUT_PORTS_START( spdball )
	PORT_START      /* IN0 */
	PORT_BIT( 0xff, IP_ACTIVE_HIGH, IPT_UNKNOWN )

	PORT_START      /* IN1 */
	PORT_BIT( 0xff, IP_ACTIVE_HIGH, IPT_UNKNOWN )

	PORT_START      /* IN2 */
	PORT_BITX(0x01, IP_ACTIVE_HIGH, 0, "Auto Up", KEYCODE_F1, IP_JOY_NONE )
	PORT_BITX(0x02, IP_ACTIVE_HIGH, 0, "Advance", KEYCODE_F2, IP_JOY_NONE )
	PORT_BIT( 0x04, IP_ACTIVE_HIGH, IPT_SERVICE1 )
	PORT_BITX(0x08, IP_ACTIVE_HIGH, 0, "High Score Reset", KEYCODE_7, IP_JOY_NONE )
	PORT_BIT( 0x10, IP_ACTIVE_HIGH, IPT_COIN1 )
	PORT_BIT( 0x20, IP_ACTIVE_HIGH, IPT_COIN2 )
	PORT_BIT( 0x40, IP_ACTIVE_HIGH, IPT_TILT )

	PORT_START      /* IN3 */
	PORT_BIT( 0x01, IP_ACTIVE_HIGH, IPT_JOYSTICK_UP   | IPF_PLAYER1 )
	PORT_BIT( 0x02, IP_ACTIVE_HIGH, IPT_JOYSTICK_DOWN | IPF_PLAYER1 )
	PORT_BIT( 0x04, IP_ACTIVE_HIGH, IPT_JOYSTICK_UP   | IPF_PLAYER2 )
	PORT_BIT( 0x08, IP_ACTIVE_HIGH, IPT_JOYSTICK_DOWN | IPF_PLAYER2 )
	PORT_BIT( 0x10, IP_ACTIVE_HIGH, IPT_BUTTON1 | IPF_PLAYER1 )
	PORT_BIT( 0x20, IP_ACTIVE_HIGH, IPT_UNKNOWN )
	PORT_BIT( 0x40, IP_ACTIVE_HIGH, IPT_BUTTON1 | IPF_PLAYER2 )
	PORT_BIT( 0x80, IP_ACTIVE_HIGH, IPT_UNKNOWN )

	PORT_START      /* IN4 */
	PORT_BIT( 0x01, IP_ACTIVE_HIGH, IPT_JOYSTICK_LEFT  | IPF_PLAYER1 )
	PORT_BIT( 0x02, IP_ACTIVE_HIGH, IPT_JOYSTICK_RIGHT | IPF_PLAYER1 )
	PORT_BIT( 0x04, IP_ACTIVE_HIGH, IPT_JOYSTICK_LEFT  | IPF_PLAYER2 )
	PORT_BIT( 0x08, IP_ACTIVE_HIGH, IPT_JOYSTICK_RIGHT | IPF_PLAYER2 )
	PORT_BIT( 0x10, IP_ACTIVE_HIGH, IPT_UNKNOWN )
	PORT_BIT( 0x20, IP_ACTIVE_HIGH, IPT_UNKNOWN )
	PORT_BIT( 0x40, IP_ACTIVE_HIGH, IPT_START1 )
	PORT_BIT( 0x80, IP_ACTIVE_HIGH, IPT_START2 )

	PORT_START		/* analog */
    PORT_ANALOG( 0xff, 0x80, IPT_TRACKBALL_Y | IPF_PLAYER1 | IPF_REVERSE, 25, 32, 0, 255 )

	PORT_START		/* analog */
    PORT_ANALOG( 0xff, 0x80, IPT_TRACKBALL_X | IPF_PLAYER1, 25, 32, 0, 255 )

	PORT_START		/* analog */
    PORT_ANALOG( 0xff, 0x80, IPT_TRACKBALL_Y | IPF_PLAYER2 | IPF_REVERSE, 25, 32, 0, 255 )

	PORT_START		/* analog */
    PORT_ANALOG( 0xff, 0x80, IPT_TRACKBALL_X | IPF_PLAYER2, 25, 32, 0, 255 )
INPUT_PORTS_END


INPUT_PORTS_START( bubbles )
	PORT_START      /* IN0 */
	PORT_BIT( 0x01, IP_ACTIVE_HIGH, IPT_JOYSTICK_UP )
	PORT_BIT( 0x02, IP_ACTIVE_HIGH, IPT_JOYSTICK_DOWN )
	PORT_BIT( 0x04, IP_ACTIVE_HIGH, IPT_JOYSTICK_LEFT )
	PORT_BIT( 0x08, IP_ACTIVE_HIGH, IPT_JOYSTICK_RIGHT )
	PORT_BIT( 0x10, IP_ACTIVE_HIGH, IPT_START2 )
	PORT_BIT( 0x20, IP_ACTIVE_HIGH, IPT_START1 )

	PORT_START      /* IN1 */
	PORT_BIT( 0xff, IP_ACTIVE_HIGH, IPT_UNKNOWN )

	PORT_START      /* IN2 */
	PORT_BITX(0x01, IP_ACTIVE_HIGH, 0, "Auto Up", KEYCODE_F1, IP_JOY_NONE )
	PORT_BITX(0x02, IP_ACTIVE_HIGH, 0, "Advance", KEYCODE_F2, IP_JOY_NONE )
	PORT_BIT( 0x04, IP_ACTIVE_HIGH, IPT_SERVICE1 )
	PORT_BITX(0x08, IP_ACTIVE_HIGH, 0, "High Score Reset", KEYCODE_7, IP_JOY_NONE )
	PORT_BIT( 0x10, IP_ACTIVE_HIGH, IPT_COIN1 )
	PORT_BIT( 0x20, IP_ACTIVE_HIGH, IPT_COIN2 )
	PORT_BIT( 0x40, IP_ACTIVE_HIGH, IPT_TILT )
INPUT_PORTS_END


INPUT_PORTS_START( splat )
	PORT_START      /* IN0 */
	PORT_BIT( 0x01, IP_ACTIVE_HIGH, IPT_JOYSTICKLEFT_UP | IPF_8WAY | IPF_PLAYER2 )
	PORT_BIT( 0x02, IP_ACTIVE_HIGH, IPT_JOYSTICKLEFT_DOWN | IPF_8WAY | IPF_PLAYER2 )
	PORT_BIT( 0x04, IP_ACTIVE_HIGH, IPT_JOYSTICKLEFT_LEFT | IPF_8WAY | IPF_PLAYER2 )
	PORT_BIT( 0x08, IP_ACTIVE_HIGH, IPT_JOYSTICKLEFT_RIGHT | IPF_8WAY | IPF_PLAYER2 )
	PORT_BIT( 0x10, IP_ACTIVE_HIGH, IPT_START1 )
	PORT_BIT( 0x20, IP_ACTIVE_HIGH, IPT_START2 )
	PORT_BIT( 0x40, IP_ACTIVE_HIGH, IPT_JOYSTICKRIGHT_UP | IPF_8WAY | IPF_PLAYER2 )
	PORT_BIT( 0x80, IP_ACTIVE_HIGH, IPT_JOYSTICKRIGHT_DOWN | IPF_8WAY | IPF_PLAYER2 )

	PORT_START      /* IN2 */
	PORT_BIT( 0x01, IP_ACTIVE_HIGH, IPT_JOYSTICKRIGHT_LEFT | IPF_8WAY | IPF_PLAYER2 )
	PORT_BIT( 0x02, IP_ACTIVE_HIGH, IPT_JOYSTICKRIGHT_RIGHT | IPF_8WAY | IPF_PLAYER2 )
	PORT_BIT( 0xfc, IP_ACTIVE_HIGH, IPT_UNKNOWN )

	PORT_START      /* IN4 */
	PORT_BITX(0x01, IP_ACTIVE_HIGH, 0, "Auto Up", KEYCODE_F1, IP_JOY_NONE )
	PORT_BITX(0x02, IP_ACTIVE_HIGH, 0, "Advance", KEYCODE_F2, IP_JOY_NONE )
	PORT_BIT( 0x04, IP_ACTIVE_HIGH, IPT_SERVICE1 )
	PORT_BITX(0x08, IP_ACTIVE_HIGH, 0, "High Score Reset", KEYCODE_7, IP_JOY_NONE )
	PORT_BIT( 0x10, IP_ACTIVE_HIGH, IPT_COIN1 )
	PORT_BIT( 0x20, IP_ACTIVE_HIGH, IPT_COIN2 )
	PORT_BIT( 0x40, IP_ACTIVE_HIGH, IPT_TILT )

	PORT_START      /* IN3 */
	PORT_BIT( 0x01, IP_ACTIVE_HIGH, IPT_JOYSTICKLEFT_UP | IPF_8WAY | IPF_PLAYER1 )
	PORT_BIT( 0x02, IP_ACTIVE_HIGH, IPT_JOYSTICKLEFT_DOWN | IPF_8WAY | IPF_PLAYER1 )
	PORT_BIT( 0x04, IP_ACTIVE_HIGH, IPT_JOYSTICKLEFT_LEFT | IPF_8WAY | IPF_PLAYER1 )
	PORT_BIT( 0x08, IP_ACTIVE_HIGH, IPT_JOYSTICKLEFT_RIGHT | IPF_8WAY | IPF_PLAYER1 )
	PORT_BIT( 0x10, IP_ACTIVE_HIGH, IPT_START1 )
	PORT_BIT( 0x20, IP_ACTIVE_HIGH, IPT_START2 )
	PORT_BIT( 0x40, IP_ACTIVE_HIGH, IPT_JOYSTICKRIGHT_UP | IPF_8WAY | IPF_PLAYER1 )
	PORT_BIT( 0x80, IP_ACTIVE_HIGH, IPT_JOYSTICKRIGHT_DOWN | IPF_8WAY | IPF_PLAYER1 )

	PORT_START      /* IN4 */
	PORT_BIT( 0x01, IP_ACTIVE_HIGH, IPT_JOYSTICKRIGHT_LEFT | IPF_8WAY | IPF_PLAYER1 )
	PORT_BIT( 0x02, IP_ACTIVE_HIGH, IPT_JOYSTICKRIGHT_RIGHT | IPF_8WAY | IPF_PLAYER1 )
	PORT_BIT( 0xfc, IP_ACTIVE_HIGH, IPT_UNKNOWN )
INPUT_PORTS_END


INPUT_PORTS_START( sinistar )
	PORT_START      /* IN0 */
	/* pseudo analog joystick, see below */

	PORT_START      /* IN1 */
	PORT_BIT( 0x01, IP_ACTIVE_HIGH, IPT_BUTTON1 )
	PORT_BIT( 0x02, IP_ACTIVE_HIGH, IPT_BUTTON2 )
	PORT_BIT( 0x10, IP_ACTIVE_HIGH, IPT_START1 )
	PORT_BIT( 0x20, IP_ACTIVE_HIGH, IPT_START2 )

	PORT_START      /* IN2 */
	PORT_BITX(0x01, IP_ACTIVE_HIGH, 0, "Auto Up", KEYCODE_F1, IP_JOY_NONE )
	PORT_BITX(0x02, IP_ACTIVE_HIGH, 0, "Advance", KEYCODE_F2, IP_JOY_NONE )
	PORT_BIT( 0x04, IP_ACTIVE_HIGH, IPT_SERVICE1 )
	PORT_BITX(0x08, IP_ACTIVE_HIGH, 0, "High Score Reset", KEYCODE_7, IP_JOY_NONE )
	PORT_BIT( 0x10, IP_ACTIVE_HIGH, IPT_COIN1 )
	PORT_BIT( 0x20, IP_ACTIVE_HIGH, IPT_COIN2 )
	PORT_BIT( 0x40, IP_ACTIVE_HIGH, IPT_TILT )

	PORT_START	/* fake, converted by sinistar_input_port_0() */
	PORT_ANALOG( 0xff, 0x38, IPT_AD_STICK_X, 100, 10, 0x00, 0x6f )

	PORT_START	/* fake, converted by sinistar_input_port_0() */
	PORT_ANALOG( 0xff, 0x38, IPT_AD_STICK_Y | IPF_REVERSE, 100, 10, 0x00, 0x6f )
INPUT_PORTS_END


INPUT_PORTS_START( playball )
	PORT_START      /* IN0 */
	PORT_BIT( 0x01, IP_ACTIVE_HIGH, IPT_START1 )
	PORT_BIT( 0x02, IP_ACTIVE_HIGH, IPT_JOYSTICK_LEFT )
	PORT_BIT( 0x04, IP_ACTIVE_HIGH, IPT_JOYSTICK_RIGHT )
	PORT_BIT( 0x08, IP_ACTIVE_HIGH, IPT_START2 )

	PORT_START      /* IN1 */
	PORT_BIT( 0xff, IP_ACTIVE_HIGH, IPT_UNKNOWN )

	PORT_START      /* IN2 */
	PORT_BITX(0x01, IP_ACTIVE_HIGH, 0, "Auto Up", KEYCODE_F1, IP_JOY_NONE )
	PORT_BITX(0x02, IP_ACTIVE_HIGH, 0, "Advance", KEYCODE_F2, IP_JOY_NONE )
	PORT_BIT( 0x04, IP_ACTIVE_HIGH, IPT_SERVICE1 )
	PORT_BITX(0x08, IP_ACTIVE_HIGH, 0, "High Score Reset", KEYCODE_7, IP_JOY_NONE )
	PORT_BIT( 0x10, IP_ACTIVE_HIGH, IPT_COIN1 )
	PORT_BIT( 0x20, IP_ACTIVE_HIGH, IPT_COIN2 )
	PORT_BIT( 0x40, IP_ACTIVE_HIGH, IPT_TILT )
INPUT_PORTS_END


INPUT_PORTS_START( lottofun )
	PORT_START		/* IN0 */
	PORT_BIT( 0x01, IP_ACTIVE_HIGH, IPT_JOYSTICK_UP )
	PORT_BIT( 0x02, IP_ACTIVE_HIGH, IPT_JOYSTICK_DOWN )
	PORT_BIT( 0x04, IP_ACTIVE_HIGH, IPT_JOYSTICK_LEFT )
	PORT_BIT( 0x08, IP_ACTIVE_HIGH, IPT_JOYSTICK_RIGHT )
	PORT_BIT( 0x10, IP_ACTIVE_HIGH, IPT_START1 )
	PORT_BIT( 0x80, IP_ACTIVE_HIGH, IPT_UNKNOWN ) /* Used by ticket dispenser */

	PORT_START		/* IN1 */
	PORT_BIT( 0xff, IP_ACTIVE_HIGH, IPT_UNKNOWN )

	PORT_START		/* IN2 */
	PORT_BITX(0x01, IP_ACTIVE_HIGH, IPF_TOGGLE, "Memory Protect", KEYCODE_F1, IP_JOY_NONE )
	PORT_BITX(0x02, IP_ACTIVE_HIGH, 0, "Advance", KEYCODE_F2, IP_JOY_NONE )
	PORT_BIT( 0x04, IP_ACTIVE_HIGH, IPT_COIN2 )
	PORT_BITX(0x08, IP_ACTIVE_HIGH, 0, "High Score Reset", KEYCODE_7, IP_JOY_NONE )
	PORT_BIT( 0x10, IP_ACTIVE_HIGH, IPT_COIN1 )
	PORT_BIT( 0x20, IP_ACTIVE_HIGH, IPT_UNKNOWN ) // COIN1.5? :)
	PORT_BIT( 0x40, IP_ACTIVE_HIGH, IPT_TILT )
	PORT_BIT( 0x80, IP_ACTIVE_HIGH, IPT_UNKNOWN ) // Sound board handshake
INPUT_PORTS_END


INPUT_PORTS_START( blaster )
	PORT_START      /* IN0 */
	/* pseudo analog joystick, see below */

	PORT_START      /* IN1 */
	PORT_BIT( 0x01, IP_ACTIVE_HIGH, IPT_BUTTON2 )
	PORT_BIT( 0x02, IP_ACTIVE_HIGH, IPT_BUTTON1 )
	PORT_BIT( 0x04, IP_ACTIVE_HIGH, IPT_BUTTON3 )
	PORT_BIT( 0x08, IP_ACTIVE_HIGH, IPT_UNKNOWN )
	PORT_BIT( 0x10, IP_ACTIVE_HIGH, IPT_START1 )
	PORT_BIT( 0x20, IP_ACTIVE_HIGH, IPT_START2 )
	PORT_BIT( 0xc0, IP_ACTIVE_HIGH, IPT_UNKNOWN )

	PORT_START      /* IN2 */
	PORT_BITX(0x01, IP_ACTIVE_HIGH, 0, "Auto Up", KEYCODE_F1, IP_JOY_NONE )
	PORT_BITX(0x02, IP_ACTIVE_HIGH, 0, "Advance", KEYCODE_F2, IP_JOY_NONE )
	PORT_BIT( 0x04, IP_ACTIVE_HIGH, IPT_SERVICE1 )
	PORT_BITX(0x08, IP_ACTIVE_HIGH, 0, "High Score Reset", KEYCODE_7, IP_JOY_NONE )
	PORT_BIT( 0x10, IP_ACTIVE_HIGH, IPT_COIN1 )
	PORT_BIT( 0x20, IP_ACTIVE_HIGH, IPT_COIN2 )
	PORT_BIT( 0x40, IP_ACTIVE_HIGH, IPT_TILT )

	PORT_START	/* fake, converted by sinistar_input_port_0() */
	PORT_ANALOG( 0xff, 0x38, IPT_AD_STICK_X, 100, 10, 0x00, 0x6f )

	PORT_START	/* fake, converted by sinistar_input_port_0() */
	PORT_ANALOG( 0xff, 0x38, IPT_AD_STICK_Y | IPF_REVERSE, 100, 10, 0x00, 0x6f )
INPUT_PORTS_END


INPUT_PORTS_START( blastkit )
	PORT_START      /* IN0 */
	/* pseudo analog joystick, see below */

	PORT_START      /* IN1 */
	PORT_BIT( 0x01, IP_ACTIVE_HIGH, IPT_BUTTON2 )
	PORT_BIT( 0xfe, IP_ACTIVE_HIGH, IPT_UNKNOWN )

	PORT_START      /* IN2 */
	PORT_BITX(0x01, IP_ACTIVE_HIGH, 0, "Auto Up", KEYCODE_F1, IP_JOY_NONE )
	PORT_BITX(0x02, IP_ACTIVE_HIGH, 0, "Advance", KEYCODE_F2, IP_JOY_NONE )
	PORT_BIT( 0x04, IP_ACTIVE_HIGH, IPT_SERVICE1 )
	PORT_BITX(0x08, IP_ACTIVE_HIGH, 0, "High Score Reset", KEYCODE_7, IP_JOY_NONE )
	PORT_BIT( 0x10, IP_ACTIVE_HIGH, IPT_COIN1 )
	PORT_BIT( 0x20, IP_ACTIVE_HIGH, IPT_COIN2 )
	PORT_BIT( 0x40, IP_ACTIVE_HIGH, IPT_TILT )

	PORT_START	/* fake, converted by sinistar_input_port_0() */
	PORT_ANALOG( 0xff, 0x38, IPT_AD_STICK_X, 100, 10, 0x00, 0x6f )

	PORT_START	/* fake, converted by sinistar_input_port_0() */
	PORT_ANALOG( 0xff, 0x38, IPT_AD_STICK_Y | IPF_REVERSE, 100, 10, 0x00, 0x6f )

	PORT_START      /* IN3 */
	PORT_BIT( 0x01, IP_ACTIVE_HIGH, IPT_BUTTON1 )
	PORT_BIT( 0x02, IP_ACTIVE_HIGH, IPT_BUTTON3 )
	PORT_BIT( 0x04, IP_ACTIVE_HIGH, IPT_START1 )
	PORT_BIT( 0x08, IP_ACTIVE_HIGH, IPT_START2 )
	PORT_BIT( 0xf0, IP_ACTIVE_HIGH, IPT_UNKNOWN )
INPUT_PORTS_END


INPUT_PORTS_START( mysticm )
	PORT_START	/* IN1 */
	PORT_BIT( 0x01, IP_ACTIVE_HIGH, IPT_JOYSTICK_UP )
	PORT_BIT( 0x02, IP_ACTIVE_HIGH, IPT_JOYSTICK_RIGHT )
	PORT_BIT( 0x04, IP_ACTIVE_HIGH, IPT_JOYSTICK_DOWN )
	PORT_BIT( 0x08, IP_ACTIVE_HIGH, IPT_JOYSTICK_LEFT )
	PORT_BIT( 0x10, IP_ACTIVE_HIGH, IPT_START1 )
	PORT_BIT( 0x20, IP_ACTIVE_HIGH, IPT_START2 )
	PORT_BIT( 0x40, IP_ACTIVE_HIGH, IPT_UNUSED ) /* Key */
	PORT_BIT( 0x80, IP_ACTIVE_HIGH, IPT_BUTTON1 )

	PORT_START
	PORT_BIT( 0xff, IP_ACTIVE_HIGH, IPT_UNUSED )

	PORT_START	/* IN0 */
	PORT_BITX(0x01, IP_ACTIVE_HIGH, 0, "Auto Up", KEYCODE_F1, IP_JOY_NONE )
	PORT_BITX(0x02, IP_ACTIVE_HIGH, 0, "Advance", KEYCODE_F2, IP_JOY_NONE )
	PORT_BITX(0x04, IP_ACTIVE_HIGH, 0, "High Score Reset", KEYCODE_7, IP_JOY_NONE )
	PORT_BIT( 0x08, IP_ACTIVE_HIGH, IPT_COIN1 )
	PORT_BIT( 0x10, IP_ACTIVE_HIGH, IPT_COIN2 )
	PORT_BIT( 0x20, IP_ACTIVE_HIGH, IPT_SERVICE1 )
	PORT_BIT( 0x40, IP_ACTIVE_HIGH, IPT_TILT )
	PORT_BIT( 0x80, IP_ACTIVE_HIGH, IPT_UNUSED )
INPUT_PORTS_END


INPUT_PORTS_START( tshoot )
	PORT_START	/* IN0 (muxed with IN3)*/
	PORT_ANALOG(0x3F, 0x20, IPT_LIGHTGUN_Y, 25, 10, 0, 0x3F)
	PORT_BIT( 0x40, IP_ACTIVE_LOW, IPT_UNKNOWN )
	PORT_BIT( 0x80, IP_ACTIVE_LOW,  IPT_BUTTON1 )

	PORT_START	/* IN1 */
	PORT_BIT( 0x01, IP_ACTIVE_HIGH, IPT_BUTTON2 )
	PORT_BIT( 0x02, IP_ACTIVE_HIGH, IPT_BUTTON3 )
	PORT_BIT( 0x3C, IP_ACTIVE_HIGH, IPT_UNUSED ) /* 0011-1100 output */
	PORT_BIT( 0x40, IP_ACTIVE_HIGH, IPT_START1 )
	PORT_BIT( 0x80, IP_ACTIVE_HIGH, IPT_START2 )

	PORT_START	/* IN2 */
	PORT_BITX(0x01, IP_ACTIVE_HIGH, 0, "Auto Up", KEYCODE_F1, IP_JOY_NONE )
	PORT_BITX(0x02, IP_ACTIVE_HIGH, 0, "Advance", KEYCODE_F2, IP_JOY_NONE )
	PORT_BITX(0x04, IP_ACTIVE_HIGH, 0, "High Score Reset", KEYCODE_7, IP_JOY_NONE )
	PORT_BIT( 0x08, IP_ACTIVE_HIGH, IPT_COIN1 )
	PORT_BIT( 0x10, IP_ACTIVE_HIGH, IPT_COIN2 )
	PORT_BIT( 0x20, IP_ACTIVE_HIGH, IPT_SERVICE1 )
	PORT_BIT( 0x40, IP_ACTIVE_HIGH, IPT_TILT )
	PORT_BIT( 0x80, IP_ACTIVE_HIGH, IPT_UNUSED )

	PORT_START	/* IN3 (muxed with IN0) */
   	PORT_ANALOG(0x3F, 0x20, IPT_LIGHTGUN_X, 25, 10, 0, 0x3F)
	PORT_BIT( 0x40, IP_ACTIVE_LOW, IPT_UNKNOWN )
	PORT_BIT( 0x80, IP_ACTIVE_LOW, IPT_BUTTON1 )
INPUT_PORTS_END


INPUT_PORTS_START( inferno )
	PORT_START	/* IN0 (muxed with IN3) */
	PORT_BIT( 0x01, IP_ACTIVE_LOW, IPF_PLAYER1 | IPT_JOYSTICKLEFT_UP )
	PORT_BIT( 0x02, IP_ACTIVE_LOW, IPF_PLAYER1 | IPT_JOYSTICKLEFT_LEFT )
	PORT_BIT( 0x04, IP_ACTIVE_LOW, IPF_PLAYER1 | IPT_JOYSTICKLEFT_RIGHT )
	PORT_BIT( 0x08, IP_ACTIVE_LOW, IPF_PLAYER1 | IPT_JOYSTICKLEFT_DOWN )
	PORT_BIT( 0x10, IP_ACTIVE_LOW, IPF_PLAYER1 | IPT_JOYSTICKRIGHT_UP )
	PORT_BIT( 0x20, IP_ACTIVE_LOW, IPF_PLAYER1 | IPT_JOYSTICKRIGHT_LEFT )
	PORT_BIT( 0x40, IP_ACTIVE_LOW, IPF_PLAYER1 | IPT_JOYSTICKRIGHT_RIGHT )
	PORT_BIT( 0x80, IP_ACTIVE_LOW, IPF_PLAYER1 | IPT_JOYSTICKRIGHT_DOWN )

	PORT_START /* IN1 */
	PORT_BIT( 0x01, IP_ACTIVE_HIGH, IPF_PLAYER1 | IPT_BUTTON1 )
	PORT_BIT( 0x02, IP_ACTIVE_HIGH, IPF_PLAYER2 | IPT_BUTTON1 )
	PORT_BIT( 0x3C, IP_ACTIVE_HIGH, IPT_UNUSED )
	PORT_BIT( 0x40, IP_ACTIVE_HIGH, IPT_START1 )
	PORT_BIT( 0x80, IP_ACTIVE_HIGH, IPT_START2 )

	PORT_START	/* IN2 */
	PORT_BITX(0x01, IP_ACTIVE_HIGH, 0, "Auto Up", KEYCODE_F1, IP_JOY_NONE )
	PORT_BITX(0x02, IP_ACTIVE_HIGH, 0, "Advance", KEYCODE_F2, IP_JOY_NONE )
	PORT_BITX(0x04, IP_ACTIVE_HIGH, 0, "High Score Reset", KEYCODE_7, IP_JOY_NONE )
	PORT_BIT( 0x08, IP_ACTIVE_HIGH, IPT_COIN1 )
	PORT_BIT( 0x10, IP_ACTIVE_HIGH, IPT_COIN2 )
	PORT_BIT( 0x20, IP_ACTIVE_HIGH, IPT_SERVICE1 )
	PORT_BIT( 0x40, IP_ACTIVE_HIGH, IPT_TILT )
	PORT_BIT( 0x80, IP_ACTIVE_HIGH, IPT_UNUSED )

	PORT_START	/* IN3 (muxed with IN0) */
	PORT_BIT( 0x01, IP_ACTIVE_LOW, IPF_PLAYER2 | IPT_JOYSTICKLEFT_UP )
	PORT_BIT( 0x02, IP_ACTIVE_LOW, IPF_PLAYER2 | IPT_JOYSTICKLEFT_LEFT )
	PORT_BIT( 0x04, IP_ACTIVE_LOW, IPF_PLAYER2 | IPT_JOYSTICKLEFT_RIGHT )
	PORT_BIT( 0x08, IP_ACTIVE_LOW, IPF_PLAYER2 | IPT_JOYSTICKLEFT_DOWN )
	PORT_BIT( 0x10, IP_ACTIVE_LOW, IPF_PLAYER2 | IPT_JOYSTICKRIGHT_UP )
	PORT_BIT( 0x20, IP_ACTIVE_LOW, IPF_PLAYER2 | IPT_JOYSTICKRIGHT_LEFT )
	PORT_BIT( 0x40, IP_ACTIVE_LOW, IPF_PLAYER2 | IPT_JOYSTICKRIGHT_RIGHT )
	PORT_BIT( 0x80, IP_ACTIVE_LOW, IPF_PLAYER2 | IPT_JOYSTICKRIGHT_DOWN )
INPUT_PORTS_END


INPUT_PORTS_START( joust2 )
	PORT_START      /* IN0 */
	PORT_BIT( 0x01, IP_ACTIVE_LOW, IPT_JOYSTICK_LEFT | IPF_2WAY | IPF_PLAYER1 )
	PORT_BIT( 0x02, IP_ACTIVE_LOW, IPT_JOYSTICK_RIGHT | IPF_2WAY | IPF_PLAYER1 )
	PORT_BIT( 0x04, IP_ACTIVE_LOW, IPT_BUTTON1 | IPF_PLAYER1 )
	PORT_BIT( 0x08, IP_ACTIVE_LOW, IPT_UNUSED )
	PORT_BIT( 0x10, IP_ACTIVE_LOW, IPT_BUTTON2 | IPF_PLAYER2 )
	PORT_BIT( 0x20, IP_ACTIVE_LOW, IPT_BUTTON2 | IPF_PLAYER1 )
	PORT_BIT( 0xc0, IP_ACTIVE_LOW, IPT_UNUSED )

	PORT_START      /* IN1 */
	PORT_BIT( 0xFF, IP_ACTIVE_HIGH, IPT_UNUSED )

	PORT_START	/* IN2 */
	PORT_BITX(0x01, IP_ACTIVE_HIGH, 0, "Auto Up", KEYCODE_F1, IP_JOY_NONE )
	PORT_BITX(0x02, IP_ACTIVE_HIGH, 0, "Advance", KEYCODE_F2, IP_JOY_NONE )
	PORT_BITX(0x04, IP_ACTIVE_HIGH, 0, "High Score Reset", KEYCODE_7, IP_JOY_NONE )
	PORT_BIT( 0x08, IP_ACTIVE_HIGH, IPT_COIN1 )
	PORT_BIT( 0x10, IP_ACTIVE_HIGH, IPT_COIN2 )
	PORT_BIT( 0x20, IP_ACTIVE_HIGH, IPT_SERVICE1 )
	PORT_BIT( 0x40, IP_ACTIVE_HIGH, IPT_TILT )
	PORT_BIT( 0x80, IP_ACTIVE_HIGH, IPT_UNUSED )

	PORT_START /* IN3 (muxed with IN0) */
	PORT_BIT( 0x01, IP_ACTIVE_LOW, IPT_JOYSTICK_LEFT | IPF_2WAY | IPF_PLAYER2 )
	PORT_BIT( 0x02, IP_ACTIVE_LOW, IPT_JOYSTICK_RIGHT | IPF_2WAY | IPF_PLAYER2 )
	PORT_BIT( 0x04, IP_ACTIVE_LOW, IPT_BUTTON1 | IPF_PLAYER2 )
	PORT_BIT( 0x08, IP_ACTIVE_LOW, IPT_UNUSED )
	PORT_BIT( 0x10, IP_ACTIVE_LOW, IPT_BUTTON2 | IPF_PLAYER2 )
	PORT_BIT( 0x20, IP_ACTIVE_LOW, IPT_BUTTON2 | IPF_PLAYER1 )
	PORT_BIT( 0xc0, IP_ACTIVE_LOW, IPT_UNUSED )
INPUT_PORTS_END



/*************************************
 *
 *	Graphics definitions
 *
 *************************************/

static struct GfxLayout williams2_layout =
{
	24, 16,
	256,
	4,
	{ 0, 1, 2, 3 },
	{ 0+0*8, 4+0*8, 0+0*8+0x4000*8, 4+0*8+0x4000*8, 0+0*8+0x8000*8, 4+0*8+0x8000*8,
	  0+1*8, 4+1*8, 0+1*8+0x4000*8, 4+1*8+0x4000*8, 0+1*8+0x8000*8, 4+1*8+0x8000*8,
	  0+2*8, 4+2*8, 0+2*8+0x4000*8, 4+2*8+0x4000*8, 0+2*8+0x8000*8, 4+2*8+0x8000*8,
	  0+3*8, 4+3*8, 0+3*8+0x4000*8, 4+3*8+0x4000*8, 0+3*8+0x8000*8, 4+3*8+0x8000*8
	},
	{ 0*8, 4*8, 8*8, 12*8, 16*8, 20*8, 24*8, 28*8, 32*8, 36*8, 40*8, 44*8, 48*8, 52*8, 56*8, 60*8 },
	4*16*8
};


static struct GfxDecodeInfo williams2_gfxdecodeinfo[] =
{
	{ REGION_GFX1, 0, &williams2_layout, 16, 8 },
	{ -1 } /* end of array */
};



/*************************************
 *
 *	Sound definitions
 *
 *************************************/

static struct DACinterface dac_interface =
{
	1,
	{ 50 }
};


static struct hc55516_interface sinistar_cvsd_interface =
{
	1,
	{ 80 },
};



/*************************************
 *
 *	Machine driver
 *
 *************************************/

static MACHINE_DRIVER_START( defender )

	/* basic machine hardware */
	MDRV_CPU_ADD_TAG("main", M6809, 1000000)
	MDRV_CPU_MEMORY(defender_readmem,defender_writemem)

	MDRV_CPU_ADD_TAG("sound", M6808, 3579000/4)
	MDRV_CPU_FLAGS(CPU_AUDIO_CPU)
	MDRV_CPU_MEMORY(sound_readmem,sound_writemem)

	MDRV_FRAMES_PER_SECOND(60)
	MDRV_VBLANK_DURATION(DEFAULT_60HZ_VBLANK_DURATION)

	MDRV_MACHINE_INIT(defender)
	MDRV_NVRAM_HANDLER(generic_0fill)

	/* video hardware */
	MDRV_VIDEO_ATTRIBUTES(VIDEO_TYPE_RASTER)
	MDRV_SCREEN_SIZE(304, 256)
	MDRV_VISIBLE_AREA(6, 298-1, 7, 247-1)
	MDRV_PALETTE_LENGTH(16)

	MDRV_VIDEO_START(williams)
	MDRV_VIDEO_UPDATE(williams)

	/* sound hardware */
	MDRV_SOUND_ADD(DAC, dac_interface)
MACHINE_DRIVER_END


static MACHINE_DRIVER_START( williams )

	/* basic machine hardware */
	MDRV_IMPORT_FROM(defender)

	MDRV_CPU_MODIFY("main")
	MDRV_CPU_MEMORY(williams_readmem,williams_writemem)

	MDRV_MACHINE_INIT(williams)
MACHINE_DRIVER_END


static MACHINE_DRIVER_START( sinistar )

	/* basic machine hardware */
	MDRV_IMPORT_FROM(williams)

	/* sound hardware */
	MDRV_SOUND_ADD(HC55516, sinistar_cvsd_interface)
MACHINE_DRIVER_END


static MACHINE_DRIVER_START( playball )

	/* basic machine hardware */
	MDRV_IMPORT_FROM(sinistar)

	/* video hardware */
	MDRV_VISIBLE_AREA(6, 298-1, 8, 239-1)
MACHINE_DRIVER_END


static MACHINE_DRIVER_START( blaster )

	/* basic machine hardware */
	MDRV_IMPORT_FROM(williams)

	MDRV_CPU_MODIFY("main")
	MDRV_CPU_MEMORY(blaster_readmem,blaster_writemem)

	/* video hardware */
	MDRV_PALETTE_LENGTH(16+240)

	MDRV_VIDEO_START(blaster)
MACHINE_DRIVER_END


static MACHINE_DRIVER_START( williams2 )

	/* basic machine hardware */
	MDRV_CPU_ADD_TAG("main", M6809, 1000000)
	MDRV_CPU_MEMORY(williams2_readmem,williams2_writemem)

	MDRV_CPU_ADD_TAG("sound", M6808, 3579000/4)
	MDRV_CPU_FLAGS(CPU_AUDIO_CPU)
	MDRV_CPU_MEMORY(williams2_sound_readmem,williams2_sound_writemem)

	MDRV_FRAMES_PER_SECOND(60)
	MDRV_VBLANK_DURATION(DEFAULT_60HZ_VBLANK_DURATION)
	MDRV_MACHINE_INIT(williams2)
	MDRV_NVRAM_HANDLER(generic_0fill)

	/* video hardware */
	MDRV_VIDEO_ATTRIBUTES(VIDEO_TYPE_RASTER)
	MDRV_SCREEN_SIZE(288, 256)
	MDRV_VISIBLE_AREA(4, 288-1, 8, 248-1)
	MDRV_GFXDECODE(williams2_gfxdecodeinfo)
	MDRV_PALETTE_LENGTH(16+8*16)

	MDRV_VIDEO_START(williams2)
	MDRV_VIDEO_UPDATE(williams2)

	/* sound hardware */
	MDRV_SOUND_ADD_TAG("wmsdac", DAC, dac_interface)
MACHINE_DRIVER_END


static MACHINE_DRIVER_START( joust2 )

	/* basic machine hardware */
	MDRV_IMPORT_FROM(williams2)
	MDRV_IMPORT_FROM(williams_cvsd_sound)

	MDRV_MACHINE_INIT(joust2)

	/* sound hardware */
	MDRV_SOUND_REMOVE("wmsdac")
MACHINE_DRIVER_END



/*************************************
 *
 *	ROM definitions
 *
 *************************************/

ROM_START( defender )
	ROM_REGION( 0x14000, REGION_CPU1, 0 )
	ROM_LOAD( "defend.1",     0x0d000, 0x0800, CRC(c3e52d7e) SHA1(a57f5278ffe44248fc73f9925d107f4024ad981a) )
	ROM_LOAD( "defend.4",     0x0d800, 0x0800, CRC(9a72348b) SHA1(ed6ce796702ff32209ced3cb1ba3837dbafa526f) )
	ROM_LOAD( "defend.2",     0x0e000, 0x1000, CRC(89b75984) SHA1(a9481478da38f99efb67f0ecf82d084e14b93b42) )
	ROM_LOAD( "defend.3",     0x0f000, 0x1000, CRC(94f51e9b) SHA1(a24cfc55de56a72758c76fe2a55f1ec6c353b16f) )
	/* bank 0 is the place for CMOS ram */
	ROM_LOAD( "defend.9",     0x10000, 0x0800, CRC(6870e8a5) SHA1(67ccc194b1753a18af0c85f5e603355549c4f727) )
	ROM_LOAD( "defend.12",    0x10800, 0x0800, CRC(f1f88938) SHA1(26e48dfeefa0766837b1e762695b9532dbc8bc5e) )
	ROM_LOAD( "defend.8",     0x11000, 0x0800, CRC(b649e306) SHA1(9d7bc3c89e5a53c575946f06702c722b864b1ff0) )
	ROM_LOAD( "defend.11",    0x11800, 0x0800, CRC(9deaf6d9) SHA1(59b018ba0f3fe6eadfd387dc180ac281460358bc) )
	ROM_LOAD( "defend.7",     0x12000, 0x0800, CRC(339e092e) SHA1(2f89951dbe55d80df43df8dcf497171f73e726d3) )
	ROM_LOAD( "defend.10",    0x12800, 0x0800, CRC(a543b167) SHA1(9292b94b0d74e57e03aada4852ad1997c34122ff) )
	ROM_RELOAD(               0x13800, 0x0800 )
	ROM_LOAD( "defend.6",     0x13000, 0x0800, CRC(65f4efd1) SHA1(a960fd1559ed74b81deba434391e49fc6ec389ca) )

	ROM_REGION( 0x10000, REGION_CPU2, 0 )
	ROM_LOAD( "defend.snd",   0xf800, 0x0800, CRC(fefd5b48) SHA1(ceb0d18483f0691978c604db94417e6941ad7ff2) )
ROM_END


ROM_START( defendg )
	ROM_REGION( 0x14000, REGION_CPU1, 0 )
	ROM_LOAD( "defeng01.bin", 0x0d000, 0x0800, CRC(6111d74d) SHA1(2a335bdce8269f75012df44b446cb261ddd5924c) )
	ROM_LOAD( "defeng04.bin", 0x0d800, 0x0800, CRC(3cfc04ce) SHA1(8ee65c7daed4d6956d0e15ada4dc414c28376012) )
	ROM_LOAD( "defeng02.bin", 0x0e000, 0x1000, CRC(d184ab6b) SHA1(ed61a95b04f6162aedba8a72bc46005b77283955) )
	ROM_LOAD( "defeng03.bin", 0x0f000, 0x1000, CRC(788b76d7) SHA1(92987207770a870b5be61c820e9e229801f1fa7a) )
	/* bank 0 is the place for CMOS ram */
	ROM_LOAD( "defeng09.bin", 0x10000, 0x0800, CRC(f57caa62) SHA1(c8c91b96fd3bc98eddcc1503159050dae5755001) )
	ROM_LOAD( "defeng12.bin", 0x10800, 0x0800, CRC(33db686f) SHA1(34bc7fa10b7996efcc53d3a891b2983874269828) )
	ROM_LOAD( "defeng08.bin", 0x11000, 0x0800, CRC(9a9eb3d2) SHA1(306a3a24931e1aa5fcfd71e3f117cc726d0920ac) )
	ROM_LOAD( "defeng11.bin", 0x11800, 0x0800, CRC(5ca4e860) SHA1(031188c009b8fca92703a0cc0c2bb44976212ae9) )
	ROM_LOAD( "defeng07.bin", 0x12000, 0x0800, CRC(545c3326) SHA1(98199df5206c261061b0108c68ab9128fa0779eb) )
	ROM_LOAD( "defeng10.bin", 0x12800, 0x0800, CRC(941cf34e) SHA1(411dcb18b67585982672ff687a9249f4890faa1b) )
	ROM_RELOAD(               0x13800, 0x0800 )
	ROM_LOAD( "defeng06.bin", 0x13000, 0x0800, CRC(3af34c05) SHA1(71f3ced06a373fa4805c856bd9fc97760787a920) )

	ROM_REGION( 0x10000, REGION_CPU2, 0 )
	ROM_LOAD( "defend.snd",   0xf800, 0x0800, CRC(fefd5b48) SHA1(ceb0d18483f0691978c604db94417e6941ad7ff2) )
ROM_END


ROM_START( defendw )
	ROM_REGION( 0x14000, REGION_CPU1, 0 )
	ROM_LOAD( "wb01.bin",     0x0d000, 0x1000, CRC(0ee1019d) SHA1(a76247e825b8267abfd195c12f96348fe10d4cbc) )
	ROM_LOAD( "defeng02.bin", 0x0e000, 0x1000, CRC(d184ab6b) SHA1(ed61a95b04f6162aedba8a72bc46005b77283955) )
	ROM_LOAD( "wb03.bin",     0x0f000, 0x1000, CRC(a732d649) SHA1(b681882c02c5870ad613edc77255969a5f796422) )
	/* bank 0 is the place for CMOS ram */
	ROM_LOAD( "defeng09.bin", 0x10000, 0x0800, CRC(f57caa62) SHA1(c8c91b96fd3bc98eddcc1503159050dae5755001) )
	ROM_LOAD( "defeng12.bin", 0x10800, 0x0800, CRC(33db686f) SHA1(34bc7fa10b7996efcc53d3a891b2983874269828) )
	ROM_LOAD( "defeng08.bin", 0x11000, 0x0800, CRC(9a9eb3d2) SHA1(306a3a24931e1aa5fcfd71e3f117cc726d0920ac) )
	ROM_LOAD( "defeng11.bin", 0x11800, 0x0800, CRC(5ca4e860) SHA1(031188c009b8fca92703a0cc0c2bb44976212ae9) )
	ROM_LOAD( "defeng07.bin", 0x12000, 0x0800, CRC(545c3326) SHA1(98199df5206c261061b0108c68ab9128fa0779eb) )
	ROM_LOAD( "defeng10.bin", 0x12800, 0x0800, CRC(941cf34e) SHA1(411dcb18b67585982672ff687a9249f4890faa1b) )
	ROM_RELOAD(               0x13800, 0x0800 )
	ROM_LOAD( "defeng06.bin", 0x13000, 0x0800, CRC(3af34c05) SHA1(71f3ced06a373fa4805c856bd9fc97760787a920) )

	ROM_REGION( 0x10000, REGION_CPU2, 0 )
	ROM_LOAD( "defend.snd",   0xf800, 0x0800, CRC(fefd5b48) SHA1(ceb0d18483f0691978c604db94417e6941ad7ff2) )
ROM_END


ROM_START( defndjeu )
	ROM_REGION( 0x15000, REGION_CPU1, 0 )
	ROM_LOAD( "15", 0x0d000, 0x1000, CRC(706a24bd) )
	ROM_LOAD( "16", 0x0e000, 0x1000, CRC(03201532) )
	ROM_LOAD( "17", 0x0f000, 0x1000, CRC(25287eca) )
	/* bank 0 is the place for CMOS ram */
	ROM_LOAD( "18", 0x10000, 0x1000, CRC(e99d5679) )
	ROM_LOAD( "19", 0x11000, 0x1000, CRC(769f5984) )
	ROM_LOAD( "20", 0x12000, 0x1000, CRC(12fa0788) )
	ROM_LOAD( "21", 0x13000, 0x1000, CRC(bddb71a3) )
	ROM_RELOAD(     0x14000, 0x1000 )

	ROM_REGION( 0x10000, REGION_CPU2, 0 )
	ROM_LOAD( "s", 0xf800, 0x0800, CRC(cb79ae42) )
ROM_END


ROM_START( defcmnd )
	ROM_REGION( 0x15000, REGION_CPU1, 0 )
	ROM_LOAD( "defcmnda.1",   0x0d000, 0x1000, CRC(68effc1d) SHA1(459fd95cdf94233e1a4302d1c166e0f7cc239579) )
	ROM_LOAD( "defcmnda.2",   0x0e000, 0x1000, CRC(1126adc9) SHA1(526cf1ca3a7eefd6115d74ac9af1a50774cc258e) )
	ROM_LOAD( "defcmnda.3",   0x0f000, 0x1000, CRC(7340209d) SHA1(d2cdab8ac4830ac027655ed7fe54314c5b87fdb3) )
	/* bank 0 is the place for CMOS ram */
	ROM_LOAD( "defcmnda.10",  0x10000, 0x0800, CRC(3dddae75) SHA1(f45fcf3e5ca9bf3edd692b4ee1e96f9f1d388522) )
	ROM_LOAD( "defcmnda.7",   0x10800, 0x0800, CRC(3f1e7cf8) SHA1(87afb4b1158e64039129bd8a9653bc61ab3e1e37) )
	ROM_LOAD( "defcmnda.9",   0x11000, 0x0800, CRC(8882e1ff) SHA1(9d54a39230acd01e0555f67ba2a3c9c6d66b59a1) )
	ROM_LOAD( "defcmnda.6",   0x11800, 0x0800, CRC(d068f0c5) SHA1(d32a4232756ca05972780cb35b0add12b31e8283) )
	ROM_LOAD( "defcmnda.8",   0x12000, 0x0800, CRC(fef4cb77) SHA1(96202e97f3392bc043a252e78d1c42b51c38d269) )
	ROM_LOAD( "defcmnda.5",   0x12800, 0x0800, CRC(49b50b40) SHA1(91cf841271a2f7d06f81477b4a450eb4580c7ca5) )
	ROM_LOAD( "defcmnda.4",   0x13000, 0x0800, CRC(43d42a1b) SHA1(b13d59940646451c00b49bbe4a41b9e2df4d7758) )

	ROM_REGION( 0x10000, REGION_CPU2, 0 )
	ROM_LOAD( "defcmnda.snd", 0xf800, 0x0800, CRC(f122d9c9) SHA1(70092fc354a2efbe7365be922fa36309b50d5c6f) )
ROM_END


ROM_START( defence )
	ROM_REGION( 0x15000, REGION_CPU1, 0 )
	ROM_LOAD( "1",            0x0d000, 0x1000, CRC(ebc93622) SHA1(bd1c098e91b24409925d01aa25de013451dba8e6) )
	ROM_LOAD( "2",            0x0e000, 0x1000, CRC(2a4f4f44) SHA1(8c0519fcb631e05e967cf0953ab2749183655594) )
	ROM_LOAD( "3",            0x0f000, 0x1000, CRC(a4112f91) SHA1(aad7ae81da7c20c7f4c1ef41697c8900a0c81f8e) )
	/* bank 0 is the place for CMOS ram */
	ROM_LOAD( "0",            0x10000, 0x0800, CRC(7a1e5998) SHA1(c133f43427540b39a383db7f46298942420d138a) )
	ROM_LOAD( "7",            0x10800, 0x0800, CRC(4c2616a3) SHA1(247411e2bb6618f77df6ea74aef1743fafb491a3) )
	ROM_LOAD( "9",            0x11000, 0x0800, CRC(7b146003) SHA1(04746f1b037bf6549fd53cff8f8c37136fce099e) )
	ROM_LOAD( "6",            0x11800, 0x0800, CRC(6d748030) SHA1(060ddf95eeb1318695a25c8c082a670fcdf117e7) )
	ROM_LOAD( "8",            0x12000, 0x0800, CRC(52d5438b) SHA1(087268ca30a42c00dbeceb4df901ddf80ae50125) )
	ROM_LOAD( "5",            0x12800, 0x0800, CRC(4a270340) SHA1(317fcc3156a099dbe48a0658757a9d6c4c54b23a) )
	ROM_LOAD( "4",            0x13000, 0x0800, CRC(e13f457c) SHA1(c706babc0005dfeb3c1b880047da6ec04bce407d) )

	ROM_REGION( 0x10000, REGION_CPU2, 0 )
	ROM_LOAD( "defcmnda.snd", 0xf800, 0x0800, CRC(f122d9c9) SHA1(70092fc354a2efbe7365be922fa36309b50d5c6f) )
ROM_END


ROM_START( mayday )
	ROM_REGION( 0x15000, REGION_CPU1, 0 )
	ROM_LOAD( "ic03-3.bin",  0x0d000, 0x1000, CRC(a1ff6e62) SHA1(c3c60ce94c6bdc4b07e45f386eff9a4aa4816953) )
	ROM_LOAD( "ic02-2.bin",  0x0e000, 0x1000, CRC(62183aea) SHA1(3843fe055ab6d3bb5a3362f57a63ce99e36cec47) )
	ROM_LOAD( "ic01-1.bin",  0x0f000, 0x1000, CRC(5dcb113f) SHA1(c41d671c336c68824771b7c4f0ffce39f1b6cd62) )
	/* bank 0 is the place for CMOS ram */
	ROM_LOAD( "ic04-4.bin",  0x10000, 0x1000, CRC(ea6a4ec8) SHA1(eaedc11968d88fd6f3c5b40c8d15d64ca6d0a1ab) )
	ROM_LOAD( "ic05-5.bin",  0x11000, 0x1000, CRC(0d797a3e) SHA1(289d2ecfebd7d71430d6624f3c9fbc91f9ef05cc) )
	ROM_LOAD( "ic06-6.bin",  0x12000, 0x1000, CRC(ee8bfcd6) SHA1(f68c44fdc18d57070aea604e261fb7b9407345a2) )
	ROM_LOAD( "ic07-7d.bin", 0x13000, 0x1000, CRC(d9c065e7) SHA1(ceeb58d1dfe14106271f17cf1c689b812216c3c0) )
	ROM_RELOAD(              0x14000, 0x1000 )

	ROM_REGION( 0x10000, REGION_CPU2, 0 )
	ROM_LOAD( "ic28-8.bin",  0xf800, 0x0800, CRC(fefd5b48) SHA1(ceb0d18483f0691978c604db94417e6941ad7ff2) )
ROM_END


ROM_START( maydaya )
	ROM_REGION( 0x15000, REGION_CPU1, 0 )
	ROM_LOAD( "mayday.c", 0x0d000, 0x1000, CRC(872a2f2d) SHA1(5823e889151b34e3fa739775440c788cca0b44c6) )
	ROM_LOAD( "mayday.b", 0x0e000, 0x1000, CRC(c4ab5e22) SHA1(757fd9311cffea420b1de8f574e84c13c0aac77d) )
	ROM_LOAD( "mayday.a", 0x0f000, 0x1000, CRC(329a1318) SHA1(4aa1d05ca05f37460eccb450ae61c21d86348f02) )
	/* bank 0 is the place for CMOS ram */
	ROM_LOAD( "mayday.d", 0x10000, 0x1000, CRC(c2ae4716) SHA1(582f763eda7d7d51ed0580045d6c617246b104b7) )
	ROM_LOAD( "mayday.e", 0x11000, 0x1000, CRC(41225666) SHA1(6d9c0347ff85bf9f9ae4648976c3ee971fec0f53) )
	ROM_LOAD( "mayday.f", 0x12000, 0x1000, CRC(c39be3c0) SHA1(91ac61e20d325a3a018ffe57bd79bfdfdc5a3cbd) )
	ROM_LOAD( "mayday.g", 0x13000, 0x1000, CRC(2bd0f106) SHA1(ac74d74a54d5b464e4c82b5b46ad7d20cdb7b6d7) )
	ROM_RELOAD(           0x14000, 0x1000 )

	ROM_REGION( 0x10000, REGION_CPU2, 0 )
	ROM_LOAD( "ic28-8.bin", 0xf800, 0x0800, CRC(fefd5b48) SHA1(ceb0d18483f0691978c604db94417e6941ad7ff2) )
ROM_END


ROM_START( maydayb )
	ROM_REGION( 0x15000, REGION_CPU1, 0 )
	ROM_LOAD( "ic03-3.bin",  0x0d000, 0x1000, CRC(a1ff6e62) SHA1(c3c60ce94c6bdc4b07e45f386eff9a4aa4816953) )
	ROM_LOAD( "ic02-2.bin",  0x0e000, 0x1000, CRC(62183aea) SHA1(3843fe055ab6d3bb5a3362f57a63ce99e36cec47) )
	ROM_LOAD( "ic01-1.bin",  0x0f000, 0x1000, CRC(5dcb113f) SHA1(c41d671c336c68824771b7c4f0ffce39f1b6cd62) )
	/* bank 0 is the place for CMOS ram */
	ROM_LOAD( "rom7.bin",    0x10000, 0x1000, CRC(0c3ca687) SHA1(a83f17c20f5767f092300266dd494bd0abf267bb) )
	ROM_LOAD( "ic05-5.bin",  0x11000, 0x1000, CRC(0d797a3e) SHA1(289d2ecfebd7d71430d6624f3c9fbc91f9ef05cc) )
	ROM_LOAD( "ic06-6.bin",  0x12000, 0x1000, CRC(ee8bfcd6) SHA1(f68c44fdc18d57070aea604e261fb7b9407345a2) )
	ROM_LOAD( "ic07-7d.bin", 0x13000, 0x1000, CRC(d9c065e7) SHA1(ceeb58d1dfe14106271f17cf1c689b812216c3c0) )
	ROM_RELOAD(              0x14000, 0x1000 )

	ROM_REGION( 0x10000, REGION_CPU2, 0 )
	ROM_LOAD( "ic28-8.bin",  0xf800, 0x0800, CRC(fefd5b48) SHA1(ceb0d18483f0691978c604db94417e6941ad7ff2) )

	ROM_REGION( 0x2000, REGION_USER1, 0 )
	ROM_LOAD( "rom11.bin",   0x0000, 0x0800, CRC(7e113979) SHA1(ac908afb6aa756fc4db1ffddbd3688aa07080693) )
	ROM_LOAD( "rom12.bin",   0x0800, 0x0800, CRC(a562c506) SHA1(a0bae41732f05caa80b9c13fba6ae4f01647e680) )
	ROM_LOAD( "rom6a.bin",   0x1000, 0x0800, CRC(8e4e981f) SHA1(685c1fca9373f4129c7c6b86f18900a1bd324019) )
	ROM_LOAD( "rom8-sos.bin",0x1800, 0x0800, CRC(6a9b383f) SHA1(10e71a3bb9492b6c34ff06760dd55c442611ca75) )
ROM_END


ROM_START( colony7 )
	ROM_REGION( 0x14000, REGION_CPU1, 0 )
	ROM_LOAD( "cs03.bin",     0x0d000, 0x1000, CRC(7ee75ae5) SHA1(1d268d83c2b0c7897d9e783f5da4e1d892709ba4) )
	ROM_LOAD( "cs02.bin",     0x0e000, 0x1000, CRC(c60b08cb) SHA1(8cf91a2c2c04199b2870bb11e10fa6ffef5b877f) )
	ROM_LOAD( "cs01.bin",     0x0f000, 0x1000, CRC(1bc97436) SHA1(326692de3491925bbeea9b0e880d9133f0e6440c) )
	/* bank 0 is the place for CMOS ram */
	ROM_LOAD( "cs06.bin",     0x10000, 0x0800, CRC(318b95af) SHA1(cb276ef440436f6000a2d20252f3197a67965167) )
	ROM_LOAD( "cs04.bin",     0x10800, 0x0800, CRC(d740faee) SHA1(aad164e72ebb0de18487c5397ea33d300cf93423) )
	ROM_LOAD( "cs07.bin",     0x11000, 0x0800, CRC(0b23638b) SHA1(b577c0cefa3ea2df436ed0fa1efa8ecd04ff78b0) )
	ROM_LOAD( "cs05.bin",     0x11800, 0x0800, CRC(59e406a8) SHA1(b64081ca83b6f57ac8fb71b1f8618083f19b99de) )
	ROM_LOAD( "cs08.bin",     0x12000, 0x0800, CRC(3bfde87a) SHA1(f5047927833be97324c861aa93a8e95b457058c4) )
	ROM_RELOAD(           0x12800, 0x0800 )

	ROM_REGION( 0x10000, REGION_CPU2, 0 )
	ROM_LOAD( "cs11.bin",     0xf800, 0x0800, CRC(6032293c) SHA1(dd2c6afc1149a879d49e93d1a2fa8e1f6d0b043b) )
ROM_END


ROM_START( colony7a )
	ROM_REGION( 0x14000, REGION_CPU1, 0 )
	ROM_LOAD( "cs03a.bin",    0x0d000, 0x1000, CRC(e0b0d23b) SHA1(4c50e00a71b3b2bf8d032a3cb496e5473204a8d6) )
	ROM_LOAD( "cs02a.bin",    0x0e000, 0x1000, CRC(370c6f41) SHA1(4e13a4cf9c1a3b1c354443c41549b59581d8b357) )
	ROM_LOAD( "cs01a.bin",    0x0f000, 0x1000, CRC(ba299946) SHA1(42e5d6ad0505f5a951d92165c9e2fa4e86659469) )
	/* bank 0 is the place for CMOS ram */
	ROM_LOAD( "cs06.bin",     0x10000, 0x0800, CRC(318b95af) SHA1(cb276ef440436f6000a2d20252f3197a67965167) )
	ROM_LOAD( "cs04.bin",     0x10800, 0x0800, CRC(d740faee) SHA1(aad164e72ebb0de18487c5397ea33d300cf93423) )
	ROM_LOAD( "cs07.bin",     0x11000, 0x0800, CRC(0b23638b) SHA1(b577c0cefa3ea2df436ed0fa1efa8ecd04ff78b0) )
	ROM_LOAD( "cs05.bin",     0x11800, 0x0800, CRC(59e406a8) SHA1(b64081ca83b6f57ac8fb71b1f8618083f19b99de) )
	ROM_LOAD( "cs08.bin",     0x12000, 0x0800, CRC(3bfde87a) SHA1(f5047927833be97324c861aa93a8e95b457058c4) )
	ROM_RELOAD(            0x12800, 0x0800 )

	ROM_REGION( 0x10000, REGION_CPU2, 0 )
	ROM_LOAD( "cs11.bin",     0xf800, 0x0800, CRC(6032293c) SHA1(dd2c6afc1149a879d49e93d1a2fa8e1f6d0b043b) )
ROM_END


ROM_START( stargate )
	ROM_REGION( 0x10000, REGION_CPU1, 0 )
	ROM_LOAD( "01",           0x0000, 0x1000, CRC(88824d18) SHA1(f003a5a9319c4eb8991fa2aae3f10c72d6b8e81a) )
	ROM_LOAD( "02",           0x1000, 0x1000, CRC(afc614c5) SHA1(087c6da93318e8dc922d3d22e0a2af7b9759701c) )
	ROM_LOAD( "03",           0x2000, 0x1000, CRC(15077a9d) SHA1(7badb4318b208f49d7fa65e915d0aa22a1e37915) )
	ROM_LOAD( "04",           0x3000, 0x1000, CRC(a8b4bf0f) SHA1(6b4d47c2899fe9f14f9dab5928499f12078c437d) )
	ROM_LOAD( "05",           0x4000, 0x1000, CRC(2d306074) SHA1(54f871983699113e31bb756d4ca885c26c2d66b4) )
	ROM_LOAD( "06",           0x5000, 0x1000, CRC(53598dde) SHA1(54b02d944caf95283c9b6f0160e75ea8c4ccc97b) )
	ROM_LOAD( "07",           0x6000, 0x1000, CRC(23606060) SHA1(a487ffcd4920d1056b87469735f7e1002f6a2e49) )
	ROM_LOAD( "08",           0x7000, 0x1000, CRC(4ec490c7) SHA1(8726ebaf048db9608dfe365bf434ed5ca9452db7) )
	ROM_LOAD( "09",           0x8000, 0x1000, CRC(88187b64) SHA1(efacc4a6d4b2af9a236c9d520de6d605c79cc5a8) )
	ROM_LOAD( "10",           0xd000, 0x1000, CRC(60b07ff7) SHA1(ba833f48ddfc1bd04ddb41b1d1c840d66ee7da30) )
	ROM_LOAD( "11",           0xe000, 0x1000, CRC(7d2c5daf) SHA1(6ca39f493eb8b370154ad46ef01976d352c929e1) )
	ROM_LOAD( "12",           0xf000, 0x1000, CRC(a0396670) SHA1(c46872550e0ca031453c6513f8f0448ecc9b5572) )

	ROM_REGION( 0x10000, REGION_CPU2, 0 )
	ROM_LOAD( "sg.snd",       0xf800, 0x0800, CRC(2fcf6c4d) SHA1(9c4334ac3ff15d94001b22fc367af40f9deb7d57) )
ROM_END


ROM_START( joust )
	ROM_REGION( 0x10000, REGION_CPU1, 0 )
	ROM_LOAD( "joust.wg1",    0x0000, 0x1000, CRC(fe41b2af) SHA1(0443e00ae2eb3e66cf805562ee04309487bb0ba4) )
	ROM_LOAD( "joust.wg2",    0x1000, 0x1000, CRC(501c143c) SHA1(5fda266d43cbbf42eeae1a078b5209d9408ab99f) )
	ROM_LOAD( "joust.wg3",    0x2000, 0x1000, CRC(43f7161d) SHA1(686da120aa4bd4a41f3d93e8c79ebb343977851a) )
	ROM_LOAD( "joust.wg4",    0x3000, 0x1000, CRC(db5571b6) SHA1(cb1c3285344e2cfbe0a81ab9b51758c40da8a23f) )
	ROM_LOAD( "joust.wg5",    0x4000, 0x1000, CRC(c686bb6b) SHA1(d9cac4c46820e1a451a145864bca7a35cfab7d37) )
	ROM_LOAD( "joust.wg6",    0x5000, 0x1000, CRC(fac5f2cf) SHA1(febaa8cf5c3a0af901cd12d0b7909f6fec3beadd) )
	ROM_LOAD( "joust.wg7",    0x6000, 0x1000, CRC(81418240) SHA1(5ad14aa65e71c3856dcdb04c99edda92e406a3e3) )
	ROM_LOAD( "joust.wg8",    0x7000, 0x1000, CRC(ba5359ba) SHA1(f4ee13d5a95ed3e1050a3927a3a0ccf86ed7752d) )
	ROM_LOAD( "joust.wg9",    0x8000, 0x1000, CRC(39643147) SHA1(d95d3b746133eac9dcc9ee05eabecb797023f1a5) )
	ROM_LOAD( "joust.wga",    0xd000, 0x1000, CRC(3f1c4f89) SHA1(90864a8ab944df45287bf0f68ad3a85194077a82) )
	ROM_LOAD( "joust.wgb",    0xe000, 0x1000, CRC(ea48b359) SHA1(6d38003d56bebeb1f5b4d2287d587342847aa195) )
	ROM_LOAD( "joust.wgc",    0xf000, 0x1000, CRC(c710717b) SHA1(7d01764e8251c60b3cab96f7dc6dcc1c624f9d12) )

	ROM_REGION( 0x10000, REGION_CPU2, 0 )
	ROM_LOAD( "joust.snd",    0xf000, 0x1000, CRC(f1835bdd) SHA1(af7c066d2949d36b87ea8c425ca7d12f82b5c653) )
ROM_END


ROM_START( joustwr )
	ROM_REGION( 0x10000, REGION_CPU1, 0 )
	ROM_LOAD( "joust.wg1",    0x0000, 0x1000, CRC(fe41b2af) SHA1(0443e00ae2eb3e66cf805562ee04309487bb0ba4) )
	ROM_LOAD( "joust.wg2",    0x1000, 0x1000, CRC(501c143c) SHA1(5fda266d43cbbf42eeae1a078b5209d9408ab99f) )
	ROM_LOAD( "joust.wg3",    0x2000, 0x1000, CRC(43f7161d) SHA1(686da120aa4bd4a41f3d93e8c79ebb343977851a) )
	ROM_LOAD( "joust.wg4",    0x3000, 0x1000, CRC(db5571b6) SHA1(cb1c3285344e2cfbe0a81ab9b51758c40da8a23f) )
	ROM_LOAD( "joust.wg5",    0x4000, 0x1000, CRC(c686bb6b) SHA1(d9cac4c46820e1a451a145864bca7a35cfab7d37) )
	ROM_LOAD( "joust.wg6",    0x5000, 0x1000, CRC(fac5f2cf) SHA1(febaa8cf5c3a0af901cd12d0b7909f6fec3beadd) )
	ROM_LOAD( "joust.wr7",    0x6000, 0x1000, CRC(e6f439c4) SHA1(ff8f1d54f3ac91101ab9f5f115baeca4f2670186) )
	ROM_LOAD( "joust.wg8",    0x7000, 0x1000, CRC(ba5359ba) SHA1(f4ee13d5a95ed3e1050a3927a3a0ccf86ed7752d) )
	ROM_LOAD( "joust.wg9",    0x8000, 0x1000, CRC(39643147) SHA1(d95d3b746133eac9dcc9ee05eabecb797023f1a5) )
	ROM_LOAD( "joust.wra",    0xd000, 0x1000, CRC(2039014a) SHA1(b9a76ecf01404585f833f76c54aa5a88a0215715) )
	ROM_LOAD( "joust.wgb",    0xe000, 0x1000, CRC(ea48b359) SHA1(6d38003d56bebeb1f5b4d2287d587342847aa195) )
	ROM_LOAD( "joust.wgc",    0xf000, 0x1000, CRC(c710717b) SHA1(7d01764e8251c60b3cab96f7dc6dcc1c624f9d12) )

	ROM_REGION( 0x10000, REGION_CPU2, 0 )
	ROM_LOAD( "joust.snd",    0xf000, 0x1000, CRC(f1835bdd) SHA1(af7c066d2949d36b87ea8c425ca7d12f82b5c653) )
ROM_END


ROM_START( joustr )
	ROM_REGION( 0x10000, REGION_CPU1, 0 )
	ROM_LOAD( "joust.wg1",    0x0000, 0x1000, CRC(fe41b2af) SHA1(0443e00ae2eb3e66cf805562ee04309487bb0ba4) )
	ROM_LOAD( "joust.wg2",    0x1000, 0x1000, CRC(501c143c) SHA1(5fda266d43cbbf42eeae1a078b5209d9408ab99f) )
	ROM_LOAD( "joust.wg3",    0x2000, 0x1000, CRC(43f7161d) SHA1(686da120aa4bd4a41f3d93e8c79ebb343977851a) )
	ROM_LOAD( "joust.sr4",    0x3000, 0x1000, CRC(ab347170) SHA1(ad50c83fcfa958f2673cae04bd811095f9ee08c0) )
	ROM_LOAD( "joust.wg5",    0x4000, 0x1000, CRC(c686bb6b) SHA1(d9cac4c46820e1a451a145864bca7a35cfab7d37) )
	ROM_LOAD( "joust.sr6",    0x5000, 0x1000, CRC(3d9a6fac) SHA1(0c81394ae96a2fcfa4c953d38e43f3ef415fe4fc) )
	ROM_LOAD( "joust.sr7",    0x6000, 0x1000, CRC(0a70b3d1) SHA1(eb78b694aa29f777f3c7e7104e568f865930c0ec) )
	ROM_LOAD( "joust.sr8",    0x7000, 0x1000, CRC(a7f01504) SHA1(0ca3211d060befc102bda2e97d163de7fb12a6f6) )
	ROM_LOAD( "joust.sr9",    0x8000, 0x1000, CRC(978687ad) SHA1(25e651af3e3be08d6293aab427a0843e9333a629) )
	ROM_LOAD( "joust.sra",    0xd000, 0x1000, CRC(c0c6e52a) SHA1(f14ff16195027f3e199e79e43741f0849c17fd10) )
	ROM_LOAD( "joust.srb",    0xe000, 0x1000, CRC(ab11bcf9) SHA1(efb09e92a621d6c4d6cde2f166e8c988c64d81ae) )
	ROM_LOAD( "joust.src",    0xf000, 0x1000, CRC(ea14574b) SHA1(7572d118b2343646054e558f0bd48e4959d84ce7) )

	ROM_REGION( 0x10000, REGION_CPU2, 0 )
	ROM_LOAD( "joust.snd",    0xf000, 0x1000, CRC(f1835bdd) SHA1(af7c066d2949d36b87ea8c425ca7d12f82b5c653) )
ROM_END


ROM_START( robotron )
	ROM_REGION( 0x10000, REGION_CPU1, 0 )
	ROM_LOAD( "robotron.sb1", 0x0000, 0x1000, CRC(66c7d3ef) SHA1(f6d60e26c209c1df2cc01ac07ad5559daa1b7118) )
	ROM_LOAD( "robotron.sb2", 0x1000, 0x1000, CRC(5bc6c614) SHA1(4d6e82bc29f49100f7751ccfc6a9ff35695b84b3) )
	ROM_LOAD( "robotron.sb3", 0x2000, 0x1000, CRC(e99a82be) SHA1(06a8c8dd0b4726eb7f0bb0e89c8533931d75fc1c) )
	ROM_LOAD( "robotron.sb4", 0x3000, 0x1000, CRC(afb1c561) SHA1(aaf89c19fd8f4e8750717169eb1af476aef38a5e) )
	ROM_LOAD( "robotron.sb5", 0x4000, 0x1000, CRC(62691e77) SHA1(79b4680ce19bd28882ae823f0e7b293af17cbb91) )
	ROM_LOAD( "robotron.sb6", 0x5000, 0x1000, CRC(bd2c853d) SHA1(f76ec5432a7939b33a27be1c6855e2dbe6d9fdc8) )
	ROM_LOAD( "robotron.sb7", 0x6000, 0x1000, CRC(49ac400c) SHA1(06eae5138254723819a5e93cfd9e9f3285fcddf5) )
	ROM_LOAD( "robotron.sb8", 0x7000, 0x1000, CRC(3a96e88c) SHA1(7ae38a609ed9a6f62ca003cab719740ed7651b7c) )
	ROM_LOAD( "robotron.sb9", 0x8000, 0x1000, CRC(b124367b) SHA1(fd9d75b866f0ebbb723f84889337e6814496a103) )
	ROM_LOAD( "robotron.sba", 0xd000, 0x1000, CRC(13797024) SHA1(d426a50e75dabe936de643c83a548da5e399331c) )
	ROM_LOAD( "robotron.sbb", 0xe000, 0x1000, CRC(7e3c1b87) SHA1(f8c6cbe3688f256f41a121255fc08f575f6a4b4f) )
	ROM_LOAD( "robotron.sbc", 0xf000, 0x1000, CRC(645d543e) SHA1(fad7cea868ebf17347c4bc5193d647bbd8f9517b) )

	ROM_REGION( 0x10000, REGION_CPU2, 0 )
	ROM_LOAD( "robotron.snd", 0xf000, 0x1000, CRC(c56c1d28) SHA1(15afefef11bfc3ab78f61ab046701db78d160ec3) )
ROM_END


ROM_START( robotryo )
	ROM_REGION( 0x10000, REGION_CPU1, 0 )
	ROM_LOAD( "robotron.sb1", 0x0000, 0x1000, CRC(66c7d3ef) SHA1(f6d60e26c209c1df2cc01ac07ad5559daa1b7118) )
	ROM_LOAD( "robotron.sb2", 0x1000, 0x1000, CRC(5bc6c614) SHA1(4d6e82bc29f49100f7751ccfc6a9ff35695b84b3) )
	ROM_LOAD( "robotron.yo3", 0x2000, 0x1000, CRC(67a369bc) SHA1(5a912d485e686de5e3175d3fc0e5daad36f4b836) )
	ROM_LOAD( "robotron.yo4", 0x3000, 0x1000, CRC(b0de677a) SHA1(02013e00513dd74e878a01791cbcca92712e2c80) )
	ROM_LOAD( "robotron.yo5", 0x4000, 0x1000, CRC(24726007) SHA1(8b4ed881f64e3ce73ac1a9ae2c184721c1ab37cc) )
	ROM_LOAD( "robotron.yo6", 0x5000, 0x1000, CRC(028181a6) SHA1(41c4d9ece2ae8a103b7151fc4ff576796303318d) )
	ROM_LOAD( "robotron.yo7", 0x6000, 0x1000, CRC(4dfcceae) SHA1(46fe1b1162d6054eb502852d065fc2e8c694b09d) )
	ROM_LOAD( "robotron.sb8", 0x7000, 0x1000, CRC(3a96e88c) SHA1(7ae38a609ed9a6f62ca003cab719740ed7651b7c) )
	ROM_LOAD( "robotron.sb9", 0x8000, 0x1000, CRC(b124367b) SHA1(fd9d75b866f0ebbb723f84889337e6814496a103) )
	ROM_LOAD( "robotron.yoa", 0xd000, 0x1000, CRC(4a9d5f52) SHA1(d5ae801e60ed829e7ef5c54a18aefca54eae827f) )
	ROM_LOAD( "robotron.yob", 0xe000, 0x1000, CRC(2afc5e7f) SHA1(f3405be9ad2287f3921e7dbd9c5313c91fa7f8d6) )
	ROM_LOAD( "robotron.yoc", 0xf000, 0x1000, CRC(45da9202) SHA1(81b3b2a72a3c871e8d7b9348056622c90a20d876) )

	ROM_REGION( 0x10000, REGION_CPU2, 0 )
	ROM_LOAD( "robotron.snd", 0xf000, 0x1000, CRC(c56c1d28) SHA1(15afefef11bfc3ab78f61ab046701db78d160ec3) )
ROM_END


ROM_START( spdball )
	ROM_REGION( 0x10000, REGION_CPU1, 0 )
	ROM_LOAD( "speedbal.01", 0x0000, 0x1000, CRC(7f4801bb) SHA1(8f22396170571189b1d088d73331d6a713c76f41) )
	ROM_LOAD( "speedbal.02", 0x1000, 0x1000, CRC(5cd5e489) SHA1(83c1bce945ecbaa4a59e0023198e574d9069680c) )
	ROM_LOAD( "speedbal.03", 0x2000, 0x1000, CRC(280e11a4) SHA1(4ef321e1744955a9a54c1e4b1f88c01c01e7b7c8) )
	ROM_LOAD( "speedbal.04", 0x3000, 0x1000, CRC(3469cbbf) SHA1(70b46cf686438441484ffeca0fa1398c15c8811e) )
	ROM_LOAD( "speedbal.05", 0x4000, 0x1000, CRC(87373c89) SHA1(a3cd72f4b517d5d727059a7d911b79ced27e9f93) )
	ROM_LOAD( "speedbal.06", 0x5000, 0x1000, CRC(48779a0d) SHA1(9cdfc12d1021b5d66acd38ab61f385219be39f4f) )
	ROM_LOAD( "speedbal.07", 0x6000, 0x1000, CRC(2e5d8db6) SHA1(7a13d60267ce12a6a4b20322c2ed1f39762bc663) )
	ROM_LOAD( "speedbal.08", 0x7000, 0x1000, CRC(c173cedf) SHA1(603c4c7cdc712d54a86b59470651d00b369293d8) )
	ROM_LOAD( "speedbal.09", 0x8000, 0x1000, CRC(415f424b) SHA1(f7e59385a67319ba152488762af1b42fc62ab264) )
	ROM_LOAD( "speedbal.10", 0xd000, 0x1000, CRC(4a3add93) SHA1(6939dd6cb6751a0406f364223029eff99040f9e2) )
	ROM_LOAD( "speedbal.11", 0xe000, 0x1000, CRC(1fbcfaa5) SHA1(fccdebbab172b141bbaec6f520b378d21c72f67a) )
	ROM_LOAD( "speedbal.12", 0xf000, 0x1000, CRC(f3458f41) SHA1(366fb880b4dc68849d6ea7a9dab55efa9c566123) )

	ROM_REGION( 0x10000, REGION_CPU2, 0 )
	ROM_LOAD( "speedbal.snd", 0xf000, 0x1000, CRC(78de20e2) SHA1(ece6e04b1d57167faf7aaee0829e7c31eb560437) )

	ROM_REGION( 0x1000, REGION_USER1, ROMREGION_DISPOSE )
	ROM_LOAD( "mystery.rom", 0x00000, 0x1000, CRC(dcb6a070) SHA1(6a6fcddf5b46eef187dcf5d9b60e03e9375e7276) )
ROM_END


ROM_START( bubbles )
	ROM_REGION( 0x10000, REGION_CPU1, 0 )
	ROM_LOAD( "bubbles.1b",   0x0000, 0x1000, CRC(8234f55c) SHA1(4d60942320c03ae50b0b17267062a321cf49e240) )
	ROM_LOAD( "bubbles.2b",   0x1000, 0x1000, CRC(4a188d6a) SHA1(2788c4a21659799e59ab82bc8d1864a3abe3b6d7) )
	ROM_LOAD( "bubbles.3b",   0x2000, 0x1000, CRC(7728f07f) SHA1(2a2c6dd8c2196dcd5e71b38554a56ee03d2aa454) )
	ROM_LOAD( "bubbles.4b",   0x3000, 0x1000, CRC(040be7f9) SHA1(de4d212cd2967b2dcd7b2c09dea2c1b06ce4c5bd) )
	ROM_LOAD( "bubbles.5b",   0x4000, 0x1000, CRC(0b5f29e0) SHA1(ae52f8c69c8b821abb458288c8ee0bc6c28fe535) )
	ROM_LOAD( "bubbles.6b",   0x5000, 0x1000, CRC(4dd0450d) SHA1(d55aa8fb8f2974ce5ba7155b01bc3e3622f202af) )
	ROM_LOAD( "bubbles.7b",   0x6000, 0x1000, CRC(e0a26ec0) SHA1(2da6213df6c15735a8bbd6750cfb1a1b6232a6f5) )
	ROM_LOAD( "bubbles.8b",   0x7000, 0x1000, CRC(4fd23d8d) SHA1(9d71caa30bc3f4151789279d21651e5a4fe4a484) )
	ROM_LOAD( "bubbles.9b",   0x8000, 0x1000, CRC(b48559fb) SHA1(551a49a12353044dbbf28dba2bd860c2d00c50bd) )
	ROM_LOAD( "bubbles.10b",  0xd000, 0x1000, CRC(26e7869b) SHA1(db428e79fc325ae3c8cab460267c27cdbc35a3bd) )
	ROM_LOAD( "bubbles.11b",  0xe000, 0x1000, CRC(5a5b572f) SHA1(f0c3a330abf9c8cfb6007ee372409450d2a15a93) )
	ROM_LOAD( "bubbles.12b",  0xf000, 0x1000, CRC(ce22d2e2) SHA1(be4b9800c846660ce2b2ddd75ad872dcf174979a) )

	ROM_REGION( 0x10000, REGION_CPU2, 0 )
	ROM_LOAD( "bubbles.snd",  0xf000, 0x1000, CRC(689ce2aa) SHA1(b70d2553f731f9a20ddaf9af2f93b7e9c44d4d99) )
ROM_END


ROM_START( bubblesr )
	ROM_REGION( 0x10000, REGION_CPU1, 0 )
	ROM_LOAD( "bubblesr.1b",  0x0000, 0x1000, CRC(dda4e782) SHA1(ad6825ebc05931942ce1042f18e18e3873083abc) )
	ROM_LOAD( "bubblesr.2b",  0x1000, 0x1000, CRC(3c8fa7f5) SHA1(fd3db6c2abab7000d586ef1a4e425329da292144) )
	ROM_LOAD( "bubblesr.3b",  0x2000, 0x1000, CRC(f869bb9c) SHA1(ce276fc33136a527eefbbf35c2bcf1f0b9858740) )
	ROM_LOAD( "bubblesr.4b",  0x3000, 0x1000, CRC(0c65eaab) SHA1(c622906cbda07421a7024955f3b9e8d173f4b6cb) )
	ROM_LOAD( "bubblesr.5b",  0x4000, 0x1000, CRC(7ece4e13) SHA1(c6ec7145c2d3bf51877c7fb995d9732b09e04cf0) )
	ROM_LOAD( "bubbles.6b",   0x5000, 0x1000, CRC(4dd0450d) SHA1(d55aa8fb8f2974ce5ba7155b01bc3e3622f202af) )
	ROM_LOAD( "bubbles.7b",   0x6000, 0x1000, CRC(e0a26ec0) SHA1(2da6213df6c15735a8bbd6750cfb1a1b6232a6f5) )
	ROM_LOAD( "bubblesr.8b",  0x7000, 0x1000, CRC(598b9bd6) SHA1(993cc3fac58310d0e617e58e3a0753002b987df1) )
	ROM_LOAD( "bubbles.9b",   0x8000, 0x1000, CRC(b48559fb) SHA1(551a49a12353044dbbf28dba2bd860c2d00c50bd) )
	ROM_LOAD( "bubblesr.10b", 0xd000, 0x1000, CRC(8b396db0) SHA1(88cab59ce7f07dfa15d1485d12ebab96d777ca65) )
	ROM_LOAD( "bubblesr.11b", 0xe000, 0x1000, CRC(096af43e) SHA1(994e60c1e684ae46ea791b274995d21ff5052e56) )
	ROM_LOAD( "bubblesr.12b", 0xf000, 0x1000, CRC(5c1244ef) SHA1(25b0f359c28291894381d73f4ba3a2b991a547f0) )

	ROM_REGION( 0x10000, REGION_CPU2, 0 )
	ROM_LOAD( "bubbles.snd",  0xf000, 0x1000, CRC(689ce2aa) SHA1(b70d2553f731f9a20ddaf9af2f93b7e9c44d4d99) )
ROM_END


ROM_START( bubblesp )
	ROM_REGION( 0x10000, REGION_CPU1, 0 )
	ROM_LOAD( "bub_prot.1b",  0x0000, 0x1000, CRC(6466a746) SHA1(ed67d879d82ef05bcd2b655f761f84bc0cf08897) )
	ROM_LOAD( "bub_prot.2b",  0x1000, 0x1000, CRC(cca04357) SHA1(98f879675c02e7ad5532da30f663714913a059b9) )
	ROM_LOAD( "bub_prot.3b",  0x2000, 0x1000, CRC(7aaff9e5) SHA1(8b377ec5c595a4e062bdc8fb8ca99b52a6bd9298) )
	ROM_LOAD( "bub_prot.4b",  0x3000, 0x1000, CRC(4e264f01) SHA1(a6fd2d0613f78c45b3873e06efa2dd99530ed0c8) )
	ROM_LOAD( "bub_prot.5b",  0x4000, 0x1000, CRC(121b0be6) SHA1(75ed718b9e83c32390ee0fe2c34e0300ecd98a85) )
	ROM_LOAD( "bub_prot.6b",  0x5000, 0x1000, CRC(80e90b25) SHA1(92c83b4333f4f0f65638b1827ace01b02c490339) )
	ROM_LOAD( "bubbles.7b",   0x6000, 0x1000, CRC(e0a26ec0) SHA1(2da6213df6c15735a8bbd6750cfb1a1b6232a6f5) )
	ROM_LOAD( "bub_prot.8b",  0x7000, 0x1000, CRC(96fb19c8) SHA1(3b1720e5efe2adc1f633216419bdf00c7e7b817d) )
	ROM_LOAD( "bub_prot.9b",  0x8000, 0x1000, CRC(be7e1028) SHA1(430b33c8d83ee6756a3ef9298792b71066c88326) )
	ROM_LOAD( "bub_prot.10b", 0xd000, 0x1000, CRC(89a565df) SHA1(1f02c17222f7303218962fada6c6f867414551cf) )
	ROM_LOAD( "bub_prot.11b", 0xe000, 0x1000, CRC(5a0c36a7) SHA1(2b9dd9006e57ff8214ad4e6b10a4b72e736d472c) )
	ROM_LOAD( "bub_prot.12b", 0xf000, 0x1000, CRC(2bfd3438) SHA1(2427a5614e98a9499e4d19f9d6e25f2b73896bf5) )

	ROM_REGION( 0x10000, REGION_CPU2, 0 )
	ROM_LOAD( "bubbles.snd",  0xf000, 0x1000, CRC(689ce2aa) SHA1(b70d2553f731f9a20ddaf9af2f93b7e9c44d4d99) )
ROM_END


ROM_START( splat )
	ROM_REGION( 0x10000, REGION_CPU1, 0 )
	ROM_LOAD( "splat.01",     0x0000, 0x1000, CRC(1cf26e48) SHA1(6ba4de6cc7d1359ed450da7bae1000552373f873) )
	ROM_LOAD( "splat.02",     0x1000, 0x1000, CRC(ac0d4276) SHA1(710aba98909d5d63c4b9b08579021f9c026b3111) )
	ROM_LOAD( "splat.03",     0x2000, 0x1000, CRC(74873e59) SHA1(727c9da682fd10353f3969ef02e9f1826d8cb77a) )
	ROM_LOAD( "splat.04",     0x3000, 0x1000, CRC(70a7064e) SHA1(7e6440585462b68b62d6d571d83635bf17149f1a) )
	ROM_LOAD( "splat.05",     0x4000, 0x1000, CRC(c6895221) SHA1(6f88ba8ac72d9301760d6e2512549f70b5373c65) )
	ROM_LOAD( "splat.06",     0x5000, 0x1000, CRC(ea4ab7fd) SHA1(288a361691a7f147ff3346627a10531d613ad017) )
	ROM_LOAD( "splat.07",     0x6000, 0x1000, CRC(82fd8713) SHA1(c4d42b111a0357700ac2bf700117d75ffb3c5be5) )
	ROM_LOAD( "splat.08",     0x7000, 0x1000, CRC(7dded1b4) SHA1(73df546dd60870f63a8c3deffea2b2d13149a48b) )
	ROM_LOAD( "splat.09",     0x8000, 0x1000, CRC(71cbfe5a) SHA1(bf22bedeceffdccc340637098070b32e9c13cf68) )
	ROM_LOAD( "splat.10",     0xd000, 0x1000, CRC(d1a1f632) SHA1(de4f5ba2b92c47757dfd2ca810bf8f87338223f7) )
	ROM_LOAD( "splat.11",     0xe000, 0x1000, CRC(ca8cde95) SHA1(8e12f6d9eaf397646691ec5d02963b32973cb32e) )
	ROM_LOAD( "splat.12",     0xf000, 0x1000, CRC(5bee3e60) SHA1(b4ee99fb6c353093faf1e088bab82fec66e785bc) )

	ROM_REGION( 0x10000, REGION_CPU2, 0 )
	ROM_LOAD( "splat.snd",    0xf000, 0x1000, CRC(a878d5f3) SHA1(f3347a354cb54ca228fe0971f0ae3bc778e2aecf) )
ROM_END


ROM_START( sinistar )
	ROM_REGION( 0x10000, REGION_CPU1, 0 )
	ROM_LOAD( "sinistar.01",  0x0000, 0x1000, CRC(f6f3a22c) SHA1(026d8cab07734fa294a5645edbe65a904bcbc302) )
	ROM_LOAD( "sinistar.02",  0x1000, 0x1000, CRC(cab3185c) SHA1(423d1e3b0c07333ec582529bc4d0b7baf591820a) )
	ROM_LOAD( "sinistar.03",  0x2000, 0x1000, CRC(1ce1b3cc) SHA1(5bc03d7249529d827dc60c087e074ab3e4ea7361) )
	ROM_LOAD( "sinistar.04",  0x3000, 0x1000, CRC(6da632ba) SHA1(72c0c3d5a5ca87ca4d95fcedaf834206e4633950) )
	ROM_LOAD( "sinistar.05",  0x4000, 0x1000, CRC(b662e8fc) SHA1(828a89d2ea13d8a362dae708f86bff54cb231887) )
	ROM_LOAD( "sinistar.06",  0x5000, 0x1000, CRC(2306183d) SHA1(703e29e6446856615760a4897c0f5d79cc7bdfb2) )
	ROM_LOAD( "sinistar.07",  0x6000, 0x1000, CRC(e5dd918e) SHA1(bf4e2ada6a59d246218544d822ba5355da925924) )
	ROM_LOAD( "sinistar.08",  0x7000, 0x1000, CRC(4785a787) SHA1(8c7eca656b2c23b0da41a8c7ce51a2735cab85a4) )
	ROM_LOAD( "sinistar.09",  0x8000, 0x1000, CRC(50cb63ad) SHA1(96e28e4fef98fff2649741a266fa590e0313e3b0) )
	ROM_LOAD( "sinistar.10",  0xe000, 0x1000, CRC(3d670417) SHA1(81802622bee8dbea5c0f08019d87d941dcdbe292) )
	ROM_LOAD( "sinistar.11",  0xf000, 0x1000, CRC(3162bc50) SHA1(2f38e572ab9c731e38dfe9bad3cc8222a775c5ea) )

	ROM_REGION( 0x10000, REGION_CPU2, 0 )
	ROM_LOAD( "speech.ic7",   0xb000, 0x1000, CRC(e1019568) SHA1(442f4f3ccd2e1db2136d2ffb121ea442921f87ca) )
	ROM_LOAD( "speech.ic5",   0xc000, 0x1000, CRC(cf3b5ffd) SHA1(d5d51c550581c9d46ab331dd4fd32541a2ef598e) )
	ROM_LOAD( "speech.ic6",   0xd000, 0x1000, CRC(ff8d2645) SHA1(16fa2a602acbbc182dd96bab113ab18356f3daf0) )
	ROM_LOAD( "speech.ic4",   0xe000, 0x1000, CRC(4b56a626) SHA1(44430cd5c110ec751b0bfb8ae99b26d443350db1) )
	ROM_LOAD( "sinistar.snd", 0xf000, 0x1000, CRC(b82f4ddb) SHA1(c70c7dd6e88897920d7709a260f27810f66aade1) )
ROM_END


ROM_START( sinista1 )
	ROM_REGION( 0x10000, REGION_CPU1, 0 )
	ROM_LOAD( "sinrev1.01",   0x0000, 0x1000, CRC(3810d7b8) SHA1(dcd690cbc958a2f97f022765315d77fb7c7d8e8b) )
	ROM_LOAD( "sinistar.02",  0x1000, 0x1000, CRC(cab3185c) SHA1(423d1e3b0c07333ec582529bc4d0b7baf591820a) )
	ROM_LOAD( "sinrev1.03",   0x2000, 0x1000, CRC(7c984ca9) SHA1(b32b7d15194051db5d29acf95b049e2eccf6d393) )
	ROM_LOAD( "sinrev1.04",   0x3000, 0x1000, CRC(cc6c4f24) SHA1(b4375544e02a19458c6fcc85edb31025c0b8eb71) )
	ROM_LOAD( "sinrev1.05",   0x4000, 0x1000, CRC(12285bfe) SHA1(6d433103332ddda2f2af23febc0b15aa93db1f31) )
	ROM_LOAD( "sinrev1.06",   0x5000, 0x1000, CRC(7a675f35) SHA1(3a7e9fdb2aef52dc29d33799694737038802b6e0) )
	ROM_LOAD( "sinrev1.07",   0x6000, 0x1000, CRC(b0463243) SHA1(95d597856a1942bd176f5f62db0d691f8f2f2932) )
	ROM_LOAD( "sinrev1.08",   0x7000, 0x1000, CRC(909040d4) SHA1(5361cc378bdace0799227e901341747dce9bb029) )
	ROM_LOAD( "sinrev1.09",   0x8000, 0x1000, CRC(cc949810) SHA1(2d2d1cccd7e43b63e424c34ab5215a412e2b9809) )
	ROM_LOAD( "sinrev1.10",   0xe000, 0x1000, CRC(ea87a53f) SHA1(4e4bad5315a8f5740c926ee5681879919a5be37f) )
	ROM_LOAD( "sinrev1.11",   0xf000, 0x1000, CRC(88d36e80) SHA1(bb9adaf5b73f9874e52dc2f5fd35e22f8b4fc258) )

	ROM_REGION( 0x10000, REGION_CPU2, 0 )
	ROM_LOAD( "speech.ic7",   0xb000, 0x1000, CRC(e1019568) SHA1(442f4f3ccd2e1db2136d2ffb121ea442921f87ca) )
	ROM_LOAD( "speech.ic5",   0xc000, 0x1000, CRC(cf3b5ffd) SHA1(d5d51c550581c9d46ab331dd4fd32541a2ef598e) )
	ROM_LOAD( "speech.ic6",   0xd000, 0x1000, CRC(ff8d2645) SHA1(16fa2a602acbbc182dd96bab113ab18356f3daf0) )
	ROM_LOAD( "speech.ic4",   0xe000, 0x1000, CRC(4b56a626) SHA1(44430cd5c110ec751b0bfb8ae99b26d443350db1) )
	ROM_LOAD( "sinistar.snd", 0xf000, 0x1000, CRC(b82f4ddb) SHA1(c70c7dd6e88897920d7709a260f27810f66aade1) )
ROM_END


ROM_START( sinista2 )
	ROM_REGION( 0x10000, REGION_CPU1, 0 )
	ROM_LOAD( "sinistar.01",  0x0000, 0x1000, CRC(f6f3a22c) SHA1(026d8cab07734fa294a5645edbe65a904bcbc302) )
	ROM_LOAD( "sinistar.02",  0x1000, 0x1000, CRC(cab3185c) SHA1(423d1e3b0c07333ec582529bc4d0b7baf591820a) )
	ROM_LOAD( "sinistar.03",  0x2000, 0x1000, CRC(1ce1b3cc) SHA1(5bc03d7249529d827dc60c087e074ab3e4ea7361) )
	ROM_LOAD( "sinistar.04",  0x3000, 0x1000, CRC(6da632ba) SHA1(72c0c3d5a5ca87ca4d95fcedaf834206e4633950) )
	ROM_LOAD( "sinistar.05",  0x4000, 0x1000, CRC(b662e8fc) SHA1(828a89d2ea13d8a362dae708f86bff54cb231887) )
	ROM_LOAD( "sinistar.06",  0x5000, 0x1000, CRC(2306183d) SHA1(703e29e6446856615760a4897c0f5d79cc7bdfb2) )
	ROM_LOAD( "sinistar.07",  0x6000, 0x1000, CRC(e5dd918e) SHA1(bf4e2ada6a59d246218544d822ba5355da925924) )
	ROM_LOAD( "sinrev2.08",   0x7000, 0x1000, CRC(d7ecee45) SHA1(f9552035409bce0a36ed93a677b28f8cd361f8f1) )
	ROM_LOAD( "sinistar.09",  0x8000, 0x1000, CRC(50cb63ad) SHA1(96e28e4fef98fff2649741a266fa590e0313e3b0) )
	ROM_LOAD( "sinistar.10",  0xe000, 0x1000, CRC(3d670417) SHA1(81802622bee8dbea5c0f08019d87d941dcdbe292) )
	ROM_LOAD( "sinrev2.11",   0xf000, 0x1000, CRC(792c8b00) SHA1(1f847ca8a67595927c36d69cead02813c2431c7b) )

	ROM_REGION( 0x10000, REGION_CPU2, 0 )
	ROM_LOAD( "speech.ic7",   0xb000, 0x1000, CRC(e1019568) SHA1(442f4f3ccd2e1db2136d2ffb121ea442921f87ca) )
	ROM_LOAD( "speech.ic5",   0xc000, 0x1000, CRC(cf3b5ffd) SHA1(d5d51c550581c9d46ab331dd4fd32541a2ef598e) )
	ROM_LOAD( "speech.ic6",   0xd000, 0x1000, CRC(ff8d2645) SHA1(16fa2a602acbbc182dd96bab113ab18356f3daf0) )
	ROM_LOAD( "speech.ic4",   0xe000, 0x1000, CRC(4b56a626) SHA1(44430cd5c110ec751b0bfb8ae99b26d443350db1) )
	ROM_LOAD( "sinistar.snd", 0xf000, 0x1000, CRC(b82f4ddb) SHA1(c70c7dd6e88897920d7709a260f27810f66aade1) )
ROM_END


ROM_START( playball )
	ROM_REGION( 0x10000, REGION_CPU1, 0 )
	ROM_LOAD( "playball.01",  0x0000, 0x1000, CRC(7ba8fd71) SHA1(9b77996238c67aead8c2cfc7f964f8cf9c6182eb) )
	ROM_LOAD( "playball.02",  0x1000, 0x1000, CRC(2387c3d4) SHA1(19d9da6af317595d0f3336e886154e0b8467cb3e) )
	ROM_LOAD( "playball.03",  0x2000, 0x1000, CRC(d34cc5fd) SHA1(d1f6d321c1a6a04a06813c77a3e079836a05956c) )
	ROM_LOAD( "playball.04",  0x3000, 0x1000, CRC(f68c3a8e) SHA1(f9cc7250254b9adceff883d3f6ee01c475d859ec) )
	ROM_LOAD( "playball.05",  0x4000, 0x1000, CRC(a3f20810) SHA1(678d2a5a06263cc5f74f4cb92287cf4d7a8b934f) )
	ROM_LOAD( "playball.06",  0x5000, 0x1000, CRC(f213e48e) SHA1(05b54f5121a887bc24fbe30f322277ae94474c14) )
	ROM_LOAD( "playball.07",  0x6000, 0x1000, CRC(9b5574e9) SHA1(1dddd33cd3f13694d7ba6a73e5090594c6677d5b) )
	ROM_LOAD( "playball.08",  0x7000, 0x1000, CRC(b2d2074a) SHA1(2defb2ffaca782606f792020f9c96d41abd77518) )
	ROM_LOAD( "playball.09",  0x8000, 0x1000, CRC(c4566d0f) SHA1(7848ea87d2d1693ade9129846024fbedc4145cbb) )
	ROM_LOAD( "playball.10",  0xd000, 0x1000, CRC(18787b52) SHA1(621754c1eab68de12763616b7bf01948cdce0221) )
	ROM_LOAD( "playball.11",  0xe000, 0x1000, CRC(1dd5c8f2) SHA1(17d0380ea05d9ddd17576691d0e5179ae7a71200) )
	ROM_LOAD( "playball.12",  0xf000, 0x1000, CRC(a700597b) SHA1(5ba07409ae9315b9ee65530f61155c394bfc69ad) )

	ROM_REGION( 0x10000, REGION_CPU2, 0 )
	ROM_LOAD( "speech.ic4",   0xb000, 0x1000, CRC(7e4fc798) SHA1(4636ab25238503370063f51f86f37d0e49c0d3b6) )
	ROM_LOAD( "speech.ic5",   0xc000, 0x1000, CRC(ddfe860c) SHA1(f847a0a6438af5dc646b7abe994530e6d1cbb803) )
	ROM_LOAD( "speech.ic6",   0xd000, 0x1000, CRC(8bfebf87) SHA1(d6829f78e1a2aee85673a42f7f6b78679847b616) )
	ROM_LOAD( "speech.ic7",   0xe000, 0x1000, CRC(db351db6) SHA1(94d807df61b5015f5fa78a500e2a58277db95c1f) )
	ROM_LOAD( "playball.snd", 0xf000, 0x1000, CRC(f3076f9f) SHA1(436fb1a6456535cd27f85c941ff79c0465b71555) )
ROM_END


ROM_START( lottofun )
	ROM_REGION( 0x10000, REGION_CPU1, 0 )
	ROM_LOAD( "vl4e.dat",     0x0000, 0x1000, CRC(5e9af236) SHA1(6f26c9be6da6f1195a4569f003a010d3f2e0c24d) )
	ROM_LOAD( "vl4c.dat",     0x1000, 0x1000, CRC(4b134ae2) SHA1(86756e1d8de113571857818a98d347789c003339) )
	ROM_LOAD( "vl4a.dat",     0x2000, 0x1000, CRC(b2f1f95a) SHA1(89166cdf4aff5e5a8cc4ea6ba589ce095de82f57) )
	ROM_LOAD( "vl5e.dat",     0x3000, 0x1000, CRC(c8681c55) SHA1(ac63e53a958f63bd0a05f36303c1aa777aee799d) )
	ROM_LOAD( "vl5c.dat",     0x4000, 0x1000, CRC(eb9351e0) SHA1(c66477ca0b3ed95708eb478fb992833beda1a4f8) )
	ROM_LOAD( "vl5a.dat",     0x5000, 0x1000, CRC(534f2fa1) SHA1(c034aa037ef6bc7cd2ed85da7531fd8efb7083e4) )
	ROM_LOAD( "vl6e.dat",     0x6000, 0x1000, CRC(befac592) SHA1(548cb1f0bc178eeada144c443545f7545c90b6a6) )
	ROM_LOAD( "vl6c.dat",     0x7000, 0x1000, CRC(a73d7f13) SHA1(833ff14c33635b61e1bd45b2878a4f6c9e18bf82) )
	ROM_LOAD( "vl6a.dat",     0x8000, 0x1000, CRC(5730a43d) SHA1(8acadf105dc373bf2b3087ccc1667b872452c913) )
	ROM_LOAD( "vl7a.dat",     0xd000, 0x1000, CRC(fb2aec2c) SHA1(73dc6a6dfe9ba51e3612b6d912bd7af1d5782296) )
	ROM_LOAD( "vl7c.dat",     0xe000, 0x1000, CRC(9a496519) SHA1(ae98dadcb63a33c796a3e3083d4b5bc957873cbc) )
	ROM_LOAD( "vl7e.dat",     0xf000, 0x1000, CRC(032cab4b) SHA1(87bdd0fd58b12e39efaadcf6e82744886a9292e9) )

	ROM_REGION( 0x10000, REGION_CPU2, 0 )
	ROM_LOAD( "vl2532.snd",   0xf000, 0x1000, CRC(214b8a04) SHA1(45f06b44a605cca6b293b20cfea4763b469254b8) )
ROM_END


ROM_START( blaster )
	ROM_REGION( 0x3c000, REGION_CPU1, 0 )
	ROM_LOAD( "blaster.11",   0x04000, 0x2000, CRC(6371e62f) SHA1(dc4173d2ee88757a6ac0838acaee325eadc2c4fb) )
	ROM_LOAD( "blaster.12",   0x06000, 0x2000, CRC(9804faac) SHA1(e61218fe190ad268af48d611d140d8f4cd38e4c7) )
	ROM_LOAD( "blaster.17",   0x08000, 0x1000, CRC(bf96182f) SHA1(e25a02508eecf79ea1ae5d45278a60becc6c7dcc) )
	ROM_LOAD( "blaster.16",   0x0d000, 0x1000, CRC(54a40b21) SHA1(663c7b539e6f1f065a4ecae7bb0477c71951223f) )
	ROM_LOAD( "blaster.13",   0x0e000, 0x2000, CRC(f4dae4c8) SHA1(211dcbe085a30419d649afe10ca7c4017d909bd7) )

	ROM_LOAD( "blaster.15",   0x00000, 0x4000, CRC(1ad146a4) SHA1(5ab3d9618023b59bc329a9eeef986901867a639b) )
	ROM_LOAD( "blaster.8",    0x10000, 0x4000, CRC(f110bbb0) SHA1(314dea232a3706509399348c7415f933c64cea1b) )
	ROM_LOAD( "blaster.9",    0x14000, 0x4000, CRC(5c5b0f8a) SHA1(224f89c85b2b1ca511d006180b8d994fccbdfb6b) )
	ROM_LOAD( "blaster.10",   0x18000, 0x4000, CRC(d47eb67f) SHA1(5dcde8be1a7b1927b90ffab3219dc47c5b2f20e4) )
	ROM_LOAD( "blaster.6",    0x1c000, 0x4000, CRC(47fc007e) SHA1(3a80b9b7ae460e9732f7c1cdd465a5b06ded970f) )
	ROM_LOAD( "blaster.5",    0x20000, 0x4000, CRC(15c1b94d) SHA1(5d97628541eb8933870c3ffd3646b7aaf8af6af5) )
	ROM_LOAD( "blaster.14",   0x24000, 0x4000, CRC(aea6b846) SHA1(04cb4b5eb000471a0cec377a5236ac8c83529528) )
	ROM_LOAD( "blaster.7",    0x28000, 0x4000, CRC(7a101181) SHA1(5f1581911ea7fe3e63ce1b9c50b1d3bf081dbf81) )
	ROM_LOAD( "blaster.1",    0x2c000, 0x4000, CRC(8d0ea9e7) SHA1(34f8e2e99748bed29285f7e4929bb920960ab03e) )
	ROM_LOAD( "blaster.2",    0x30000, 0x4000, CRC(03c4012c) SHA1(53f0adc91e5f1ac58b08b3a6d2de8de5a40bebab) )
	ROM_LOAD( "blaster.4",    0x34000, 0x4000, CRC(fc9d39fb) SHA1(126d43a64471bbf4b40aeda8913d50e82d254f9c) )
	ROM_LOAD( "blaster.3",    0x38000, 0x4000, CRC(253690fb) SHA1(06cb2ef95bb06b3618392e298aa690e1f75bc977) )

	ROM_REGION( 0x10000, REGION_CPU2, 0 )
	ROM_LOAD( "blaster.18",   0xf000, 0x1000, CRC(c33a3145) SHA1(6ffe2da7b70c0b576fbc1790a33eecdbb9ee3d02) )

	ROM_REGION( 0x0800, REGION_PROMS, 0 )		/* color PROM data */
	ROM_LOAD( "blaster.col",  0x0000, 0x0800, CRC(bac50bc4) SHA1(80a48eb97c6f02703210d00498f9669c36e64326) )
ROM_END


ROM_START( blastkit )
	ROM_REGION( 0x3c000, REGION_CPU1, 0 )
	ROM_LOAD( "blastkit.11",  0x04000, 0x2000, CRC(b7df4914) SHA1(81f7a89dfde06c160f2c8974eec701f2298ec434) )
	ROM_LOAD( "blastkit.12",  0x06000, 0x2000, CRC(8b1e26ab) SHA1(7d30800a9302f5a83792499d8df536693d01f75d) )
	ROM_LOAD( "blastkit.17",  0x08000, 0x1000, CRC(577d1e9a) SHA1(0064124a65490e0473dfb0081ec28b7ee43a04b5) )
	ROM_LOAD( "blastkit.16",  0x0d000, 0x1000, CRC(414b2abf) SHA1(2bde972d225d6e93e44751f542cee584d57f7983) )
	ROM_LOAD( "blastkit.13",  0x0e000, 0x2000, CRC(9c64db76) SHA1(c14508cb2f964af93631779db3adaa960fcc7559) )

	ROM_LOAD( "blaster.15",   0x00000, 0x4000, CRC(1ad146a4) SHA1(5ab3d9618023b59bc329a9eeef986901867a639b) )
	ROM_LOAD( "blaster.8",    0x10000, 0x4000, CRC(f110bbb0) SHA1(314dea232a3706509399348c7415f933c64cea1b) )
	ROM_LOAD( "blaster.9",    0x14000, 0x4000, CRC(5c5b0f8a) SHA1(224f89c85b2b1ca511d006180b8d994fccbdfb6b) )
	ROM_LOAD( "blaster.10",   0x18000, 0x4000, CRC(d47eb67f) SHA1(5dcde8be1a7b1927b90ffab3219dc47c5b2f20e4) )
	ROM_LOAD( "blaster.6",    0x1c000, 0x4000, CRC(47fc007e) SHA1(3a80b9b7ae460e9732f7c1cdd465a5b06ded970f) )
	ROM_LOAD( "blaster.5",    0x20000, 0x4000, CRC(15c1b94d) SHA1(5d97628541eb8933870c3ffd3646b7aaf8af6af5) )
	ROM_LOAD( "blaster.14",   0x24000, 0x4000, CRC(aea6b846) SHA1(04cb4b5eb000471a0cec377a5236ac8c83529528) )
	ROM_LOAD( "blastkit.7",   0x28000, 0x4000, CRC(6fcc2153) SHA1(00e7b6846c15400315d94e2c7d1c99b1a737c285) )
	ROM_LOAD( "blaster.1",    0x2c000, 0x4000, CRC(8d0ea9e7) SHA1(34f8e2e99748bed29285f7e4929bb920960ab03e) )
	ROM_LOAD( "blaster.2",    0x30000, 0x4000, CRC(03c4012c) SHA1(53f0adc91e5f1ac58b08b3a6d2de8de5a40bebab) )
	ROM_LOAD( "blastkit.4",   0x34000, 0x4000, CRC(f80e9ff5) SHA1(e232d96b6e07c7b4240fa4dd2cb9be4745a1be4b) )
	ROM_LOAD( "blastkit.3",   0x38000, 0x4000, CRC(20e851f9) SHA1(efc288ef0333812a6282f22aade8e43e9a827533) )

	ROM_REGION( 0x10000, REGION_CPU2, 0 )
	ROM_LOAD( "blaster.18",   0xf000, 0x1000, CRC(c33a3145) SHA1(6ffe2da7b70c0b576fbc1790a33eecdbb9ee3d02) )

	ROM_REGION( 0x0800, REGION_PROMS, 0 )		/* color PROM data */
	ROM_LOAD( "blaster.col",  0x0000, 0x0800, CRC(bac50bc4) SHA1(80a48eb97c6f02703210d00498f9669c36e64326) )
ROM_END


ROM_START( tshoot )
	ROM_REGION( 0x48000, REGION_CPU1, 0 )
	ROM_LOAD( "rom18.cpu", 0x0d000, 0x1000, CRC(effc33f1) SHA1(cd1b16b4a4a46ce9d550d10b465b8cf1ab3c5273) )	/* IC55 */
	ROM_LOAD( "rom2.cpu",  0x0e000, 0x1000, CRC(fd982687) SHA1(70be1ea57ea0a1e75b1bd988492a9c0244e8b91f) )	/* IC9	*/
	ROM_LOAD( "rom3.cpu",  0x0f000, 0x1000, CRC(9617054d) SHA1(8795b97a6391aa3804f68dc2d2b33866dc17f34c) )	/* IC10 */

	ROM_LOAD( "rom11.cpu", 0x10000, 0x2000, CRC(60d5fab8) SHA1(fe75e46dedb7ca153470d6a39cea0a721e5b7b39) )	/* IC18 */
	ROM_LOAD( "rom9.cpu",  0x12000, 0x2000, CRC(a4dd4a0e) SHA1(bb2f38c5ef2f3398b6ba605ffa0c30c89387bf14) )	/* IC16 */
	ROM_LOAD( "rom7.cpu",  0x14000, 0x2000, CRC(f25505e6) SHA1(d075ff89b6379ad7a47d9723ed1c21468b9d1dae) )	/* IC14 */
	ROM_LOAD( "rom5.cpu",  0x16000, 0x2000, CRC(94a7c0ed) SHA1(11f46e1ca7d79b4244ea0f60e0fba44186f1ebde) )	/* IC12 */

	ROM_LOAD( "rom17.cpu", 0x20000, 0x2000, CRC(b02d1ccd) SHA1(b08b6d9affb6f3e50a11fd9397fe4255927de3b6) )	/* IC26 */
	ROM_LOAD( "rom15.cpu", 0x22000, 0x2000, CRC(11709935) SHA1(ae25bbadbbcab9f3cba2bb4bb92d5217705b38e3) )	/* IC24 */

	ROM_LOAD( "rom10.cpu", 0x30000, 0x2000, CRC(0f32bad8) SHA1(7a2f559697d252ceec3a2f55fe51bc755e4bb65a) )	/* IC17 */
	ROM_LOAD( "rom8.cpu",  0x32000, 0x2000, CRC(e9b6cbf7) SHA1(6cd6b1e1c5e8e253e779afff8ad1ff90d6116fc9) )	/* IC15 */
	ROM_LOAD( "rom6.cpu",  0x34000, 0x2000, CRC(a49f617f) SHA1(759d25e33a09204664880329b86724805a1fe0e8) )	/* IC13 */
	ROM_LOAD( "rom4.cpu",  0x36000, 0x2000, CRC(b026dc00) SHA1(8a068997aa19e152d64db47528893046d338389c) )	/* IC11 */

	ROM_LOAD( "rom16.cpu", 0x40000, 0x2000, CRC(69ce38f8) SHA1(a2cd678e71bfa5e6a3594d8699660c7fa8b52001) )	/* IC25 */
	ROM_LOAD( "rom14.cpu", 0x42000, 0x2000, CRC(769a4ae5) SHA1(1cdfae2d889848d69f68f990714d027cfbca1853) )	/* IC23 */
	ROM_LOAD( "rom13.cpu", 0x44000, 0x2000, CRC(ec016c9b) SHA1(f2e40abd14b8b4944b792dd453ebe92eb64355ae) )	/* IC21 */
	ROM_LOAD( "rom12.cpu", 0x46000, 0x2000, CRC(98ae7afa) SHA1(6a904408419f576352bd2f895727fd17c0541ff8) )	/* IC19 */

	/* sound CPU */
	ROM_REGION( 0x10000, REGION_CPU2, 0 )
	ROM_LOAD( "rom1.cpu", 0xe000, 0x2000, CRC(011a94a7) SHA1(9f54a742a87ba56b9517e33e556f57dce6eb2eab) )		/* IC8	*/

	ROM_REGION( 0xc000, REGION_GFX1, ROMREGION_DISPOSE )
	ROM_LOAD( "rom20.cpu", 0x00000, 0x2000, CRC(c6e1d253) SHA1(c408a29f75ba2958e229996f903400b3d95e3bd3) )	/* IC57 */
	ROM_LOAD( "rom21.cpu", 0x04000, 0x2000, CRC(9874e90f) SHA1(85282823cc862341adf9642d2d5d05972da6dff0) )	/* IC58 */
	ROM_LOAD( "rom19.cpu", 0x08000, 0x2000, CRC(b9ce4d2a) SHA1(af5332f340d3c3ae02e77923d6e8f0dd92547728) )	/* IC41 */
ROM_END


ROM_START( mysticm )
	ROM_REGION( 0x48000, REGION_CPU1, 0 )
	ROM_LOAD( "mm02_2.a09", 0x0e000, 0x1000, CRC(3a776ea8) SHA1(1fef5f5cef5e10606c97ac9c365f000a88d51314) )	/* IC9	*/
	ROM_LOAD( "mm03_2.a10", 0x0f000, 0x1000, CRC(6e247c75) SHA1(4daf5206d29b887cd1a78528fac4b0cd8ec7f39b) )	/* IC10 */

	ROM_LOAD( "mm11_1.a18", 0x10000, 0x2000, CRC(f537968e) SHA1(2660a480d0bba5fe25885453115ef1015f8bdea9) )	/* IC18 */
	ROM_LOAD( "mm09_1.a16", 0x12000, 0x2000, CRC(3bd12f6c) SHA1(7925a92c486c994e8f34c8ed52bf81a34cf44f68) )	/* IC16 */
	ROM_LOAD( "mm07_1.a14", 0x14000, 0x2000, CRC(ea2a2a68) SHA1(71855c874cd5032f47fafc67e2d1667f956cd9b5) )	/* IC14 */
	ROM_LOAD( "mm05_1.a12", 0x16000, 0x2000, CRC(b514eef3) SHA1(0f9309768c416dd98e9c02121cc750993a2923ea) )	/* IC12 */

	ROM_LOAD( "mm18_1.a26", 0x20000, 0x2000, CRC(9b391a81) SHA1(b3f34e5d468fe4a4de2d4e771e2fa08de6596f26) )	/* IC26 */
	ROM_LOAD( "mm16_1.a24", 0x22000, 0x2000, CRC(399e175d) SHA1(e17301e4159e5a6d83c3ca62c93eb70f34b948df) )	/* IC24 */
	ROM_LOAD( "mm14_1.a22", 0x24000, 0x2000, CRC(191153b1) SHA1(fcd8aa6ad6506ba51a01f777f6a3b94e9c051b1c) )	/* IC22 */

	ROM_LOAD( "mm10_1.a17", 0x30000, 0x2000, CRC(d6a37509) SHA1(4b1f52954ca208ccc040c017873777fbf7fbd1f2) )	/* IC17 */
	ROM_LOAD( "mm08_1.a15", 0x32000, 0x2000, CRC(6f1a64f2) SHA1(4183b658b257d7fe35e1d7271f76d3358df5a7a2) )	/* IC15 */
	ROM_LOAD( "mm06_1.a13", 0x34000, 0x2000, CRC(2e6795d4) SHA1(8b074f6a7a4b5a9705de498684180815581faea2) )	/* IC13 */
	ROM_LOAD( "mm04_1.a11", 0x36000, 0x2000, CRC(c222fb64) SHA1(b4c51d2b1664ef3267df1dee9e4888acf847c286) )	/* IC11 */

	ROM_LOAD( "mm17_1.a25", 0x40000, 0x2000, CRC(d36f0a96) SHA1(9830955ca7e46b5b0dba98b4d2ea325bbbebe3c7) )	/* IC25 */
	ROM_LOAD( "mm15_1.a23", 0x42000, 0x2000, CRC(cd5d99da) SHA1(41a37903503c14fb9c801c51afa2f97c83b79f8b) )	/* IC23 */
	ROM_LOAD( "mm13_1.a21", 0x44000, 0x2000, CRC(ef4b79db) SHA1(346057cb8c4593df44fb36771553e60610fe1a0c) )	/* IC21 */
	ROM_LOAD( "mm12_1.a19", 0x46000, 0x2000, CRC(a1f04bf0) SHA1(389bdb7c9e395af9275abfb20c3ab51bc12dc4db) )	/* IC19 */

	/* sound CPU */
	ROM_REGION( 0x10000, REGION_CPU2, 0 )
	ROM_LOAD( "mm01_1.a08", 0x0e000, 0x2000, CRC(65339512) SHA1(144625d2905c953383bcc90cd2435d332394883f) )	/* IC8	*/

	ROM_REGION( 0xc000, REGION_GFX1, ROMREGION_DISPOSE )
	ROM_LOAD( "mm20_1.b57", 0x00000, 0x2000, CRC(5c0f4f46) SHA1(7dedbbeda2f34a2eac9fb14277874d9d66f468c7) )	/* IC57 */
	ROM_LOAD( "mm21_1.b58", 0x04000, 0x2000, CRC(cb90b3c5) SHA1(f28cca2c3ff23d6c9e2952a1b08ab2875655ec70) )	/* IC58 */
	ROM_LOAD( "mm19_1.b41", 0x08000, 0x2000, CRC(e274df86) SHA1(9876a487c5efa350ced31acbc39df22c8d414677) )	/* IC41 */
ROM_END


ROM_START( inferno )
	ROM_REGION( 0x48000, REGION_CPU1, 0 )
	ROM_LOAD( "ic9.inf",  0x0e000, 0x1000, CRC(1a013185) SHA1(9079c082ec043714f9d8ea92bc81d0b93d2ce715) )		/* IC9	*/
	ROM_LOAD( "ic10.inf", 0x0f000, 0x1000, CRC(dbf64a36) SHA1(54326bc527797f0a3a55764073eb40030aec1aae) ) 	/* IC10 */

	ROM_LOAD( "ic18.inf", 0x10000, 0x2000, CRC(95bcf7b1) SHA1(66687a3962109a25e26ae00bddd33ed973981b91) )		/* IC18 */
	ROM_LOAD( "ic16.inf", 0x12000, 0x2000, CRC(8bc4f935) SHA1(12da6faa71e5984047fa14f32af5bb865f228cb2) )		/* IC16 */
	ROM_LOAD( "ic14.inf", 0x14000, 0x2000, CRC(a70508a7) SHA1(930bb9af3b6ba9fdf3e7c32f6b5ffae9acd6cee3) )		/* IC14 */
	ROM_LOAD( "ic12.inf", 0x16000, 0x2000, CRC(7ffb87f9) SHA1(469f5ae39ad8531c4c11e9d10ab57686e7f54aef) )		/* IC12 */

	ROM_LOAD( "ic17.inf", 0x30000, 0x2000, CRC(b4684139) SHA1(c1d6ecd3dc8191250ef70e6972dad234c0d8f739) ) 	/* IC17 */
	ROM_LOAD( "ic15.inf", 0x32000, 0x2000, CRC(128a6ad6) SHA1(357438e50663d6cb96dabfa5110c17836584e15f) ) 	/* IC15 */
	ROM_LOAD( "ic13.inf", 0x34000, 0x2000, CRC(83a9e4d6) SHA1(4937e4d1c516da837213e40a1da862578c8dd272) ) 	/* IC13 */
	ROM_LOAD( "ic11.inf", 0x36000, 0x2000, CRC(c2e9c909) SHA1(21f0b9bf6ef3a9466ea9afde1c7efde9ed04ce5b) ) 	/* IC11 */

	ROM_LOAD( "ic25.inf", 0x40000, 0x2000, CRC(103a5951) SHA1(57c8caa1e9d5e245052822d08add9343fd622e04) ) 	/* IC25 */
	ROM_LOAD( "ic23.inf", 0x42000, 0x2000, CRC(c04749a0) SHA1(b203e8d1df556e10b4ecad4733448f889c63e261) ) 	/* IC23 */
	ROM_LOAD( "ic21.inf", 0x44000, 0x2000, CRC(c405f853) SHA1(6bd74d065a6043849e083c2822925b82c6fedb00) ) 	/* IC21 */
	ROM_LOAD( "ic19.inf", 0x46000, 0x2000, CRC(ade7645a) SHA1(bfaab1840e3171df895a2333a30b9dac214b3351) ) 	/* IC19 */

	/* sound CPU */
	ROM_REGION( 0x10000, REGION_CPU2, 0 )
	ROM_LOAD( "ic8.inf", 0x0e000, 0x2000, CRC(4e3123b8) SHA1(f453feed3ae3b6430db49eb4325f62eecfee9f5e) )		/* IC8	*/

	ROM_REGION( 0xc000, REGION_GFX1, ROMREGION_DISPOSE )
	ROM_LOAD( "ic57.inf", 0x00000, 0x2000, CRC(65a4ef79) SHA1(270c58901e83665bc388cd9cb92022c55e8eae50) ) 	/* IC57 */
	ROM_LOAD( "ic58.inf", 0x04000, 0x2000, CRC(4bb1c2a0) SHA1(9e8d214b8d1dbe4c2369e4047e165c9e692098a5) ) 	/* IC58 */
	ROM_LOAD( "ic41.inf", 0x08000, 0x2000, CRC(f3f7238f) SHA1(3810f1afd318ec37271c099c989b142b85d8da51) ) 	/* IC41 */
ROM_END


ROM_START( joust2 )
	ROM_REGION( 0x48000, REGION_CPU1, 0 )
	ROM_LOAD( "ic55_r1.cpu", 0x0D000, 0x1000, CRC(08b0d5bd) SHA1(b58da478aef36ae20fcfee48151d5d556e16b7b9) )	/* IC55 ROM02 */
	ROM_LOAD( "ic09_r2.cpu", 0x0E000, 0x1000, CRC(951175ce) SHA1(ac70df125bb438f9fccc082276df4a76ff693e16) )	/* IC09 ROM03 */
	ROM_LOAD( "ic10_r2.cpu", 0x0F000, 0x1000, CRC(ba6e0f6c) SHA1(431cbf38e919011d030f41008e1ad45e7e0ec38b) )	/* IC10 ROM04 */

	ROM_LOAD( "ic18_r1.cpu", 0x10000, 0x2000, CRC(9dc986f9) SHA1(5ce479936536ef713cdfc8fc8190d338c46d171e) )	/* IC18 ROM11 */
	ROM_LOAD( "ic16_r2.cpu", 0x12000, 0x2000, CRC(56e2b550) SHA1(01211d389ca384987d56c26596aa8c1adffdf8dd) )	/* IC16 ROM09 */
	ROM_LOAD( "ic14_r2.cpu", 0x14000, 0x2000, CRC(f3bce576) SHA1(30ee1b212879b3b55b47c9064f123fb77c8f3089) )	/* IC14 ROM07 */
	ROM_LOAD( "ic12_r2.cpu", 0x16000, 0x2000, CRC(5f8b4919) SHA1(1215a314c07ef4f244e862743035626cac1d9538) )	/* IC12 ROM05 */

	ROM_LOAD( "ic26_r1.cpu", 0x20000, 0x2000, CRC(4ef5e805) SHA1(98b93388ab4a4fa6eeceee3386fa46f5a307b8cb) )	/* IC26 ROM19 */
	ROM_LOAD( "ic24_r1.cpu", 0x22000, 0x2000, CRC(4861f063) SHA1(6db00cce230bf4bdfdfbfe59e0dc2d916b84d0dc) )	/* IC24 ROM17 */
	ROM_LOAD( "ic22_r1.cpu", 0x24000, 0x2000, CRC(421aafa8) SHA1(06187ba8fef3e89eb399d7040015212bd5f86853) )	/* IC22 ROM15 */
	ROM_LOAD( "ic20_r1.cpu", 0x26000, 0x2000, CRC(3432ff55) SHA1(aec0f83b92369de8a830ec298ac490a51bc29f26) )	/* IC20 ROM13 */

	ROM_LOAD( "ic17_r1.cpu", 0x30000, 0x2000, CRC(3e01b597) SHA1(17d09482636d6cda2f3266152396f0461121e748) )	/* IC17 ROM10 */
	ROM_LOAD( "ic15_r1.cpu", 0x32000, 0x2000, CRC(ff26fb29) SHA1(5ad498db71c384c1928ec965ba3cad48af428f19) )	/* IC15 ROM08 */
	ROM_LOAD( "ic13_r2.cpu", 0x34000, 0x2000, CRC(5f107db5) SHA1(c413a2e58853ccda602515b9668a6a620294ba49) )	/* IC13 ROM06 */

	ROM_LOAD( "ic25_r1.cpu", 0x40000, 0x2000, CRC(47580af5) SHA1(d2728f32f02b549c7e9691c668f0097e327a1d2d) )	/* IC25 ROM18 */
	ROM_LOAD( "ic23_r1.cpu", 0x42000, 0x2000, CRC(869b5942) SHA1(a3f4bab4c0db71589e9be2bbf1f94052ef2f56da) )	/* IC23 ROM16 */
	ROM_LOAD( "ic21_r1.cpu", 0x44000, 0x2000, CRC(0bbd867c) SHA1(f2db9fc57b6afb762715617345e8c3dcb89b6cc2) )	/* IC21 ROM14 */
	ROM_LOAD( "ic19_r1.cpu", 0x46000, 0x2000, CRC(b9221ed1) SHA1(428ea8f3e2fa58d875f581f5de6e0d05ed855a45) )	/* IC19 ROM12 */

	/* sound CPU */
	ROM_REGION( 0x10000, REGION_CPU2, 0 )
	ROM_LOAD( "ic08_r1.cpu", 0x0E000, 0x2000, CRC(84517c3c) SHA1(de0b6473953783c091ddcc7aaa89fc1ec3b9d378) )	/* IC08 ROM08 */

	/* sound board */
	ROM_REGION( 0x70000, REGION_CPU3, 0 )
	ROM_LOAD( "u04_r1.snd", 0x10000, 0x8000, CRC(3af6b47d) SHA1(aff19d65a4d9c249dec6a9e04a4066fada0f8fa1) )	/* IC04 ROM23 */
	ROM_LOAD( "u19_r1.snd", 0x30000, 0x8000, CRC(e7f9ed2e) SHA1(6b9ef5189650f0b6b2866da7f532cdf851f02ead) )	/* IC19 ROM24 */
	ROM_LOAD( "u20_r1.snd", 0x50000, 0x8000, CRC(c85b29f7) SHA1(b37e1890bd0dfa0c7db19fc878450718b60c1ca0) )	/* IC20 ROM25 */

	ROM_REGION( 0xc000, REGION_GFX1, ROMREGION_DISPOSE )
	ROM_LOAD( "ic57_r1.vid", 0x00000, 0x4000, CRC(572c6b01) SHA1(651df3223c1dc42543f57a7204ae492eb15a4999) )	/* IC57 ROM20 */
	ROM_LOAD( "ic58_r1.vid", 0x04000, 0x4000, CRC(aa94bf05) SHA1(3412dd181e2c12dc2dd1caabfe7e737005b0ccd7) )	/* IC58 ROM21 */
	ROM_LOAD( "ic41_r1.vid", 0x08000, 0x4000, CRC(c41e3daa) SHA1(fafe76bebd6eaf2cd124c1030e3a58eb5a6cddc6) )	/* IC41 ROM22 */
ROM_END



/*************************************
 *
 *	Driver initialization
 *
 *************************************/

static DRIVER_INIT( defender )
{
	static const UINT32 bank[8] = { 0x0c000, 0x10000, 0x11000, 0x12000, 0x0c000, 0x0c000, 0x0c000, 0x13000 };
	defender_bank_list = bank;

	/* CMOS configuration */
	CONFIGURE_CMOS(0xc400, 0x100);

	/* PIA configuration */
	CONFIGURE_PIAS(defender_pia_0_intf, williams_pia_1_intf, williams_snd_pia_intf);
}


static DRIVER_INIT( defndjeu )
{
/*
	Note: Please do not remove these comments in BETA versions. They are
			helpful to get the games working. When they will work, useless
			comments may be removed as desired.

	The encryption in this game is one of the silliest I have ever seen.
	I just wondered if the ROMs were encrypted, and figured out how they
	were in just about 5 mins...
	Very simple: bits 0 and 7 are swapped in the ROMs (not sound).

	Game does not work due to bad ROMs 16 and 20. However, the others are
	VERY similar (if not nearly SAME) to MAYDAY and DEFENSE ones (and NOT
	DEFENDER), although MAYDAY ROMs are more similar than DEFENSE ones...
	By putting MAYDAY ROMs and encrypting them, I got a first machine test
	and then, reboot... The test was the random graphic drawings, which
	were bad. Each time the full screen was drawn, the game rebooted.
	Unfortunately, I don't remember which roms I took to get that, and I
	could not get the same result anymore (I did not retry ALL the
	possibilities I did at 01:30am). :-(

	ROM equivalences (not including the sound ROM):

	MAYDAY      MAYDAY (Alternate)    DEFENSE       JEUTEL's Defender
	-----------------------------------------------------------------
	ROMC.BIN    IC03-3.BIN            DFNDR-C.ROM           15
	ROMB.BIN    IC02-2.BIN            DFNDR-B.ROM           16
	ROMA.BIN    IC01-1.BIN            DFNDR-A.ROM           17
	ROMG.BIN    IC07-7D.BIN           DFNDR-G.ROM           18
	ROMF.BIN    IC06-6.BIN            DFNDR-F.ROM           19
	ROME.BIN    IC05-5.BIN            DFNDR-E.ROM           20
	ROMD.BIN    IC04-4.BIN            DFNDR-D.ROM           21
*/
	static const UINT32 bank[8] = { 0x0c000, 0x10000, 0x11000, 0x12000, 0x0c000, 0x0c000, 0x0c000, 0x14000 };
	UINT8 *rom = memory_region(REGION_CPU1);
	int i;

	defender_bank_list = bank;

	/* CMOS configuration */
	CONFIGURE_CMOS(0xc400, 0x100);

	/* PIA configuration */
	CONFIGURE_PIAS(williams_pia_0_intf, williams_pia_1_intf, williams_snd_pia_intf);

	for (i = 0xd000; i < 0x15000; i++)
		rom[i] = BITSWAP8(rom[i],0,6,5,4,3,2,1,7);

}

#if 0
static DRIVER_INIT( defcmnd )
{
	static const UINT32 bank[8] = { 0x0c000, 0x10000, 0x11000, 0x12000, 0x13000, 0x0c000, 0x0c000, 0x14000 };
	defender_bank_list = bank;

	/* CMOS configuration */
	CONFIGURE_CMOS(0xc400, 0x100);

	/* PIA configuration */
	CONFIGURE_PIAS(williams_pia_0_intf, williams_pia_1_intf, williams_snd_pia_intf);
}
#endif

static DRIVER_INIT( mayday )
{
	static const UINT32 bank[8] = { 0x0c000, 0x10000, 0x11000, 0x12000, 0x0c000, 0x0c000, 0x0c000, 0x13000 };
	defender_bank_list = bank;

	/* CMOS configuration */
	CONFIGURE_CMOS(0xc400, 0x100);

	/* PIA configuration */
	CONFIGURE_PIAS(williams_pia_0_intf, williams_pia_1_intf, williams_snd_pia_intf);

	/* install a handler to catch protection checks */
	mayday_protection = install_mem_read_handler(0, 0xa190, 0xa191, mayday_protection_r);
}


static DRIVER_INIT( colony7 )
{
	static const UINT32 bank[8] = { 0x0c000, 0x10000, 0x11000, 0x12000, 0x0c000, 0x0c000, 0x0c000, 0x0c000 };
	defender_bank_list = bank;

	/* CMOS configuration */
	CONFIGURE_CMOS(0xc400, 0x100);

	/* PIA configuration */
	CONFIGURE_PIAS(williams_pia_0_intf, williams_pia_1_intf, williams_snd_pia_intf);
}


static DRIVER_INIT( stargate )
{
	/* CMOS configuration */
	CONFIGURE_CMOS(0xcc00, 0x400);

	/* PIA configuration */
	CONFIGURE_PIAS(stargate_pia_0_intf, williams_pia_1_intf, williams_snd_pia_intf);
}


static DRIVER_INIT( joust )
{
	/* CMOS configuration */
	CONFIGURE_CMOS(0xcc00, 0x400);

	/* video configuration */
	CONFIGURE_BLITTER(4, 0, 0);

	/* PIA configuration */
	CONFIGURE_PIAS(williams_muxed_pia_0_intf, williams_pia_1_intf, williams_snd_pia_intf);
}


static DRIVER_INIT( robotron )
{
	/* CMOS configuration */
	CONFIGURE_CMOS(0xcc00, 0x400);

	/* video configuration */
	CONFIGURE_BLITTER(4, 0, 0);

	/* PIA configuration */
	CONFIGURE_PIAS(williams_pia_0_intf, williams_pia_1_intf, williams_snd_pia_intf);
}


static DRIVER_INIT( spdball )
{
	/* CMOS configuration */
	CONFIGURE_CMOS(0xcc00, 0x400);

	/* video configuration */
	CONFIGURE_BLITTER(4, 0, 0);

	/* PIA configuration */
	CONFIGURE_PIAS(williams_pia_0_intf, williams_pia_1_intf, williams_snd_pia_intf);
	pia_config(3, PIA_STANDARD_ORDERING, &spdball_pia_3_intf);

	/* install extra input handlers */
	install_mem_read_handler (0, 0xc800, 0xc800, input_port_5_r);
	install_mem_read_handler (0, 0xc801, 0xc801, input_port_6_r);
	install_mem_read_handler (0, 0xc802, 0xc802, input_port_7_r);
	install_mem_read_handler (0, 0xc803, 0xc803, input_port_8_r);
	install_mem_read_handler (0, 0xc808, 0xc80b, pia_3_r);
	install_mem_write_handler(0, 0xc808, 0xc80b, pia_3_w);
}


static DRIVER_INIT( bubbles )
{
	/* CMOS configuration */
	CONFIGURE_CMOS(0xcc00, 0x400);

	/* video configuration */
	CONFIGURE_BLITTER(4, 0, 0);

	/* PIA configuration */
	CONFIGURE_PIAS(williams_pia_0_intf, williams_pia_1_intf, williams_snd_pia_intf);
}


static DRIVER_INIT( splat )
{
	/* CMOS configuration */
	CONFIGURE_CMOS(0xcc00, 0x400);

	/* video configuration */
	CONFIGURE_BLITTER(0, 0, 0);

	/* PIA configuration */
	CONFIGURE_PIAS(williams_dual_muxed_pia_0_intf, williams_pia_1_intf, williams_snd_pia_intf);
}


static DRIVER_INIT( sinistar )
{
	/* CMOS configuration */
	CONFIGURE_CMOS(0xcc00, 0x400);

	/* video configuration */
	CONFIGURE_BLITTER(4, 0, 1);

	/* PIA configuration */
	CONFIGURE_PIAS(williams_49way_pia_0_intf, williams_pia_1_intf, sinistar_snd_pia_intf);

	/* install RAM instead of ROM in the Dxxx slot */
	install_mem_read_handler (0, 0xd000, 0xdfff, MRA_RAM);
	install_mem_write_handler(0, 0xd000, 0xdfff, MWA_RAM);
}


static DRIVER_INIT( playball )
{
	/* CMOS configuration */
	CONFIGURE_CMOS(0xcc00, 0x400);

	/* video configuration */
	CONFIGURE_BLITTER(4, 0, 1);

	/* PIA configuration */
	CONFIGURE_PIAS(williams_pia_0_intf, playball_pia_1_intf, sinistar_snd_pia_intf);

	/* install RAM instead of ROM in the Dxxx slot */
	install_mem_read_handler (0, 0xd000, 0xdfff, MRA_RAM);
	install_mem_write_handler(0, 0xd000, 0xdfff, MWA_RAM);
}


static DRIVER_INIT( lottofun )
{
	/* CMOS configuration */
	CONFIGURE_CMOS(0xcc00, 0x400);

	/* video configuration */
	CONFIGURE_BLITTER(4, 0, 0);

	/* PIA configuration */
	CONFIGURE_PIAS(lottofun_pia_0_intf, williams_pia_1_intf, williams_snd_pia_intf);
}


static DRIVER_INIT( blaster )
{
	/* CMOS configuration */
	CONFIGURE_CMOS(0xcc00, 0x400);

	/* video configuration */
	CONFIGURE_BLITTER(0, 1, 0);

	/* PIA configuration */
	CONFIGURE_PIAS(williams_49way_pia_0_intf, williams_pia_1_intf, williams_snd_pia_intf);
}


static DRIVER_INIT( blastkit )
{
	/* CMOS configuration */
	CONFIGURE_CMOS(0xcc00, 0x400);

	/* video configuration */
	CONFIGURE_BLITTER(0, 1, 0);

	/* PIA configuration */
	CONFIGURE_PIAS(williams_49way_muxed_pia_0_intf, williams_pia_1_intf, williams_snd_pia_intf);
}


static DRIVER_INIT( mysticm )
{
	static const UINT8 tilemap_colors[] = { 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };

	/* CMOS configuration */
	CONFIGURE_CMOS(0xcc00, 0x400);

	/* video configuration */
	CONFIGURE_BLITTER(0, 0, 0);
	CONFIGURE_TILEMAP(0x7f, tilemap_colors, 1, 0, 1);

	/* PIA configuration */
	CONFIGURE_PIAS(mysticm_pia_0_intf, williams2_pia_1_intf, williams2_snd_pia_intf);

	/* install RAM instead of ROM in the Dxxx slot */
	install_mem_read_handler (0, 0xd000, 0xdfff, MRA_RAM);
	install_mem_write_handler(0, 0xd000, 0xdfff, MWA_RAM);
}


static DRIVER_INIT( tshoot )
{
	static const UINT8 tilemap_colors[] = { 0, 0, 1, 1, 2, 2, 3, 3, 4, 4, 5, 5, 6, 6, 7, 7 };

	/* CMOS configuration */
	CONFIGURE_CMOS(0xcc00, 0x400);

	/* video configuration */
	CONFIGURE_BLITTER(0, 0, 0);
	CONFIGURE_TILEMAP(0x7f, tilemap_colors, 1, 0, 0);

	/* PIA configuration */
	CONFIGURE_PIAS(tshoot_pia_0_intf, williams2_pia_1_intf, tshoot_snd_pia_intf);
}


static DRIVER_INIT( inferno )
{
	static const UINT8 tilemap_colors[] = { 0, 0, 1, 1, 2, 2, 3, 3, 4, 4, 5, 5, 6, 6, 7, 7 };

	/* CMOS configuration */
	CONFIGURE_CMOS(0xcc00, 0x400);

	/* video configuration */
	CONFIGURE_BLITTER(0, 0, 0);
	CONFIGURE_TILEMAP(0x7f, tilemap_colors, 1, 0, 0);

	/* PIA configuration */
	CONFIGURE_PIAS(williams2_muxed_pia_0_intf, williams2_pia_1_intf, williams2_snd_pia_intf);

	/* install RAM instead of ROM in the Dxxx slot */
	install_mem_read_handler (0, 0xd000, 0xdfff, MRA_RAM);
	install_mem_write_handler(0, 0xd000, 0xdfff, MWA_RAM);
	
	/* hack! the sound CPU programs the DDRA register to $80 instead of $00, meaning */
	/* that the top bit gets chopped off; not sure how to fix this for real */
	memory_region(REGION_CPU2)[0xe015] = 0;
}


static DRIVER_INIT( joust2 )
{
	static const UINT8 tilemap_colors[] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };

	/* CMOS configuration */
	CONFIGURE_CMOS(0xcc00, 0x400);

	/* video configuration */
	CONFIGURE_BLITTER(0, 0, 0);
	CONFIGURE_TILEMAP(0xff, tilemap_colors, 0, -2, 0);

	/* PIA configuration */
	CONFIGURE_PIAS(williams2_muxed_pia_0_intf, joust2_pia_1_intf, williams2_snd_pia_intf);

	/* expand the sound ROMs */
	memcpy(&memory_region(REGION_CPU3)[0x18000], &memory_region(REGION_CPU3)[0x10000], 0x08000);
	memcpy(&memory_region(REGION_CPU3)[0x20000], &memory_region(REGION_CPU3)[0x10000], 0x10000);
	memcpy(&memory_region(REGION_CPU3)[0x38000], &memory_region(REGION_CPU3)[0x30000], 0x08000);
	memcpy(&memory_region(REGION_CPU3)[0x40000], &memory_region(REGION_CPU3)[0x30000], 0x10000);
	memcpy(&memory_region(REGION_CPU3)[0x58000], &memory_region(REGION_CPU3)[0x50000], 0x08000);
	memcpy(&memory_region(REGION_CPU3)[0x60000], &memory_region(REGION_CPU3)[0x50000], 0x10000);
}



/*************************************
 *
 *	Game drivers
 *
 *************************************/

GAME( 1980, defender, 0,        defender, defender, defender, ROT0,   "Williams", "Defender (Red label)" )
GAME( 1980, defendg,  defender, defender, defender, defender, ROT0,   "Williams", "Defender (Green label)" )
GAME( 1980, defendw,  defender, defender, defender, defender, ROT0,   "Williams", "Defender (White label)" )
GAMEX(1980, defndjeu, defender, defender, defender, defndjeu, ROT0,   "Jeutel", "Defender ? (bootleg)", GAME_NOT_WORKING )
GAME( 1980, defcmnd,  defender, defender, defender, defender, ROT0,   "bootleg", "Defense Command (set 1)" )
GAME( 1981, defence,  defender, defender, defender, defender, ROT0,   "Outer Limits", "Defence Command" )

GAME( 1980, mayday,   0,        defender, defender, mayday,   ROT0,   "<unknown>", "Mayday (set 1)" )
GAME( 1980, maydaya,  mayday,   defender, defender, mayday,   ROT0,   "<unknown>", "Mayday (set 2)" )
GAME( 1980, maydayb,  mayday,   defender, defender, mayday,   ROT0,   "<unknown>", "Mayday (set 3)" )

GAME( 1981, colony7,  0,        defender, colony7,  colony7,  ROT270, "Taito", "Colony 7 (set 1)" )
GAME( 1981, colony7a, colony7,  defender, colony7,  colony7,  ROT270, "Taito", "Colony 7 (set 2)" )

GAME( 1981, stargate, 0,        williams, stargate, stargate, ROT0,   "Williams", "Stargate" )

GAME( 1982, robotron, 0,        williams, robotron, robotron, ROT0,   "Williams", "Robotron (Solid Blue label)" )
GAME( 1982, robotryo, robotron, williams, robotron, robotron, ROT0,   "Williams", "Robotron (Yellow/Orange label)" )

GAME( 1982, joust,    0,        williams, joust,    joust,    ROT0,   "Williams", "Joust (White/Green label)" )
GAME( 1982, joustr,   joust,    williams, joust,    joust,    ROT0,   "Williams", "Joust (Solid Red label)" )
GAME( 1982, joustwr,  joust,    williams, joust,    joust,    ROT0,   "Williams", "Joust (White/Red label)" )

GAME( 1982, bubbles,  0,        williams, bubbles,  bubbles,  ROT0,   "Williams", "Bubbles" )
GAME( 1982, bubblesr, bubbles,  williams, bubbles,  bubbles,  ROT0,   "Williams", "Bubbles (Solid Red label)" )
GAME( 1982, bubblesp, bubbles,  williams, bubbles,  bubbles,  ROT0,   "Williams", "Bubbles (prototype version)" )

GAME( 1982, splat,    0,        williams, splat,    splat,    ROT0,   "Williams", "Splat!" )

GAME( 1982, sinistar, 0,        sinistar, sinistar, sinistar, ROT270, "Williams", "Sinistar (revision 3)" )
GAME( 1982, sinista1, sinistar, sinistar, sinistar, sinistar, ROT270, "Williams", "Sinistar (prototype version)" )
GAME( 1982, sinista2, sinistar, sinistar, sinistar, sinistar, ROT270, "Williams", "Sinistar (revision 2)" )

GAME( 1983, playball, 0,        playball, playball, playball, ROT270, "Williams", "PlayBall! (prototype)" )

GAME( 1983, blaster,  0,        blaster,  blaster,  blaster,  ROT0,   "Williams", "Blaster" )
GAME( 1983, blastkit, blaster,  blaster,  blastkit, blastkit, ROT0,   "Williams", "Blaster (kit)" )

GAME( 1985, spdball,  0,        williams, spdball,  spdball,  ROT0,   "Williams", "Speed Ball (prototype)" )

GAME( 1983, mysticm,  0,        williams2,mysticm,  mysticm,  ROT0,   "Williams", "Mystic Marathon" )
GAME( 1984, tshoot,   0,        williams2,tshoot,   tshoot,   ROT0,   "Williams", "Turkey Shoot" )
GAME( 1984, inferno,  0,        williams2,inferno,  inferno,  ROT0,   "Williams", "Inferno" )
GAME( 1986, joust2,   0,        joust2,   joust2,   joust2,   ROT270, "Williams", "Joust 2 - Survival of the Fittest (set 1)" )

GAME( 1987, lottofun, 0,        williams, lottofun, lottofun, ROT0,   "H.A.R. Management", "Lotto Fun" )
