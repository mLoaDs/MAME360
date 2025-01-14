/*
  "Simple" 156 based Data East Hardware
 Data East
  DE-0409-1
  DE-0491-1
 Mitchell
  MT5601-0 (slightly different component arrangement to Deco PCBs)
 Games Supported
  Data East:
    Joe & Mac Returns (Japanese version called Caveman Ninja 2 might exist)
    Chain Reaction / Magical Drop / Magical Drop Plus 1
  Mitchell games:
    Charlie Ninja
    Party Time: Gonta the Diver II / Ganbare! Gonta!! 2
    Osman / Cannon Dancer
  The DECO 156 is a 32-bit custom encrypted ARM5 chip connected to
  16-bit hardware. Only ROM and System Work RAM is accessed via all
  32 data lines.
  Info from Charles MacDonald:
  - The sound effects 6295 is clocked at exactly half the rate of the
  music 6295.
  - Both have the SS pin pulled high to select the sample rate of the
  ADPCM data. Depends on the input clock though, the rates are described
  in the data sheet.
  - Both are connected directly to their ROMs with no swapping on the
  address or data lines. The music ROM is a 16-bit ROM in byte mode.
  The 156 data bus has pull-up resistors so reading unused locations will
  return $FFFF.
  I traced out all the connections and confirmed that both video chips (52
  and 141) really are on the lower 16 bits of the 32-bit data bus, same
  with the palette RAM. Just the program ROM and 4K internal RAM to the
  223 should be accessed as 32-bit. Not sure why that part isn't working
  right though.
  Each game has a 512K block of memory that is decoded in the same way.
  One of the PALs controls where this block starts at, for example
  0x380000 for Magical Drop and 0x180000 for Osman:
    000000-00FFFF : Main RAM (16K)
    010000-01FFFF : Sprite RAM (8K)
    020000-02FFFF : Palette RAM (4K)
    030000-03FFFF : Read player inputs, write EEPROM and OKI banking
    040000-04FFFF : PF1,2 control registers
    050000-05FFFF : PF1,2 name tables
    060000-06FFFF : PF1,2 row scroll
    070000-07FFFF : Control register
  The ordering of items within the block does not change and the size of
  each region is always 64K. If any RAM or other I/O has to be mirrored,
  it likely fills out the entire 64K range.
  The control register (marked as MWA_NOP in the driver) pulls one of the
  DE156 pins high for a brief moment and low again. Perhaps it triggers an
  interrupt or reset? It doesn't seem to be connected to anything else, at
  least on my board.
  The sprite chip has 16K RAM but the highest address line is tied to
  ground, so only 8K is available. This is correctly implemented in the
  driver, I'm mentioning it to confirm it isn't banked or anything like that.
  Notes:
  Magical Drop / Magical Drop Plus / Chain Reaction
  Random crashes at 7mhz..
-- check this ---
Are the OKI M6295 clocks from Heavy Smash are correct at least for the Mitchell games:
 - OKI M6295(1) clock: 1.000MHz (28 / 28), sample rate = 1000000 / 132
 - OKI M6295(2) clock: 2.000MHz (28 / 14), sample rate = 2000000 / 132
*/
#include "driver.h"
#include "decocrpt.h"
#include "cpu/arm/arm.h"
#include "machine/eeprom.h"
#include "deco16ic.h"
#include "vidhrdw/generic.h"

static UINT32 *simpl156_systemram;
static const UINT8 *simpl156_default_eeprom = NULL;

static void draw_sprites(struct mame_bitmap *bitmap, const struct rectangle *cliprect)
{
	int offs;


	for (offs = (0x1400/4)-4;offs >= 0;offs -= 4) // 0x1400 for charlien
	{
		int x,y,sprite,colour,multi,fx,fy,inc,flash,mult, pri;

		sprite = spriteram32[offs+1]&0xffff;

		y = spriteram32[offs]&0xffff;
		flash=y&0x1000;
		if (flash && (cpu_getcurrentframe() & 1)) continue;

		x = spriteram32[offs+2]&0xffff;
		colour = (x >>9) & 0x1f;

		pri = (x&0xc000); // 2 bits or 1?

		switch (pri&0xc000) {
		case 0x0000: pri=0; break;
		case 0x4000: pri=0xf0; break;
		case 0x8000: pri=0xf0|0xcc; break;
		case 0xc000: pri=0xf0|0xcc; break; /*  or 0xf0|0xcc|0xaa ? */
		}

		fx = y & 0x2000;
		fy = y & 0x4000;
		multi = (1 << ((y & 0x0600) >> 9)) - 1;	/* 1x, 2x, 4x, 8x height */

		x = x & 0x01ff;
		y = y & 0x01ff;
		if (x >= 320) x -= 512;
		if (y >= 256) y -= 512;
		y = 240 - y;
		x = 304 - x;

		if (x>320) continue;

		sprite &= ~multi;
		if (fy)
			inc = -1;
		else
		{
			sprite += multi;
			inc = 1;
		}

		if (1) // flip screen iq_132
		{
			y=240-y;
			x=304-x;
			if (fx) fx=0; else fx=1;
			if (fy) fy=0; else fy=1;
			mult=16;
		}
		else 
			mult=-16;

		while (multi >= 0)
		{
			pdrawgfx(bitmap,Machine->gfx[2],
					sprite - multi * inc,
					colour,
					fx,fy,
					x,y + mult * multi,
					&Machine->visible_area,TRANSPARENCY_PEN,0,pri);

			multi--;
		}
	}
}


VIDEO_UPDATE( simpl156 )
{
	fillbitmap(priority_bitmap,0,cliprect);

	deco16_pf12_update(deco16_pf1_rowscroll,deco16_pf2_rowscroll);

	fillbitmap(bitmap,256,cliprect);

	deco16_tilemap_2_draw(bitmap,cliprect,0,2);
	deco16_tilemap_1_draw(bitmap,cliprect,0,4);

	draw_sprites(bitmap,cliprect);
}

static int simpl156_bank_callback(const int bank)
{
	return ((bank>>4)&0x7) * 0x1000;
}

VIDEO_START( simpl156 )
{
	deco16_pf1_data = (data16_t*)auto_malloc(0x2000);
	deco16_pf2_data = (data16_t*)auto_malloc(0x2000);
	deco16_pf1_rowscroll = (data16_t*)auto_malloc(0x800);
	deco16_pf2_rowscroll = (data16_t*)auto_malloc(0x800);
	deco16_pf12_control = (data16_t*)auto_malloc(0x10);

	paletteram16 =  (data16_t*)auto_malloc(0x1000);

	deco16_1_video_init();

	deco16_set_tilemap_bank_callback(0, simpl156_bank_callback);
	deco16_set_tilemap_bank_callback(1, simpl156_bank_callback);

	return 0;
}


INPUT_PORTS_START( simpl156 )
	PORT_START	/* 16bit */
	PORT_BIT( 0x0001, IP_ACTIVE_LOW, IPT_COIN1 )
	PORT_BIT( 0x0002, IP_ACTIVE_LOW, IPT_COIN2 )
	PORT_BIT( 0x0004, IP_ACTIVE_LOW, IPT_SERVICE1 )
	PORT_SERVICE_NO_TOGGLE( 0x0008, IP_ACTIVE_LOW )
	PORT_BIT( 0x00f0, IP_ACTIVE_HIGH, IPT_VBLANK ) // all bits? check..
	PORT_BIT( 0x0100, IP_ACTIVE_HIGH, IPT_SPECIAL ) // eeprom?..

	PORT_START	/* 16bit */
	PORT_BIT( 0x0001, IP_ACTIVE_LOW, IPT_JOYSTICK_UP | IPF_PLAYER1 | IPF_8WAY )
	PORT_BIT( 0x0002, IP_ACTIVE_LOW, IPT_JOYSTICK_DOWN | IPF_PLAYER1 | IPF_8WAY )
	PORT_BIT( 0x0004, IP_ACTIVE_LOW, IPT_JOYSTICK_LEFT | IPF_PLAYER1 | IPF_8WAY )
	PORT_BIT( 0x0008, IP_ACTIVE_LOW, IPT_JOYSTICK_RIGHT | IPF_PLAYER1 | IPF_8WAY )
	PORT_BIT( 0x0010, IP_ACTIVE_LOW, IPT_BUTTON1 | IPF_PLAYER1 )
	PORT_BIT( 0x0020, IP_ACTIVE_LOW, IPT_BUTTON2 | IPF_PLAYER1 )
	PORT_BIT( 0x0040, IP_ACTIVE_LOW, IPT_BUTTON3 | IPF_PLAYER1 )
	PORT_BIT( 0x0080, IP_ACTIVE_LOW, IPT_START1 )
	PORT_BIT( 0x0100, IP_ACTIVE_LOW, IPT_JOYSTICK_UP | IPF_PLAYER2 | IPF_8WAY )
	PORT_BIT( 0x0200, IP_ACTIVE_LOW, IPT_JOYSTICK_DOWN | IPF_PLAYER2 | IPF_8WAY )
	PORT_BIT( 0x0400, IP_ACTIVE_LOW, IPT_JOYSTICK_LEFT | IPF_PLAYER2 | IPF_8WAY )
	PORT_BIT( 0x0800, IP_ACTIVE_LOW, IPT_JOYSTICK_RIGHT | IPF_PLAYER2 | IPF_8WAY )
	PORT_BIT( 0x1000, IP_ACTIVE_LOW, IPT_BUTTON1 | IPF_PLAYER2 )
	PORT_BIT( 0x2000, IP_ACTIVE_LOW, IPT_BUTTON2 | IPF_PLAYER2 )
	PORT_BIT( 0x4000, IP_ACTIVE_LOW, IPT_BUTTON3 | IPF_PLAYER2 )
	PORT_BIT( 0x8000, IP_ACTIVE_LOW, IPT_START2 )
INPUT_PORTS_END


static READ32_HANDLER( simpl156_inputs_read )
{
	// use a fake vblank switch otherwise the games will crap out...

	UINT32 returndata;
	static UINT32 vblank_fake;

	vblank_fake ^= 0xf0;

	returndata = readinputport(0) | 0xffff0000;
	returndata |= (EEPROM_read_bit() << 8);
	returndata &= ~0xf0;
	returndata |= vblank_fake;

	return returndata;
}

static READ32_HANDLER( simpl156_palette_r )
{
	return paletteram16[offset]^0xffff0000;
}

static WRITE32_HANDLER( simpl156_palette_w )
{
	UINT16 dat;
	int color;

	data &=0x0000ffff;
	mem_mask &=0x0000ffff;

	COMBINE_DATA(&paletteram16[offset]);
	color = offset;

	dat = paletteram16[offset]&0xffff;

#define pal5bit(n)	((((n) & 0x1f) << 3) | (((n) & 0x1f) >> 2))

	palette_set_color(color,pal5bit(dat >> 0),pal5bit(dat >> 5),pal5bit(dat >> 10));

#undef pal5bit
}


static READ32_HANDLER(  simpl156_system_r )
{
	UINT32 returndata;

	returndata = readinputport(1);

	return returndata;
}

static WRITE32_HANDLER( simpl156_eeprom_w )
{
	OKIM6295_set_bank_base(1, 0x40000 * (data & 0x7) );

	EEPROM_set_clock_line((data & 0x20) ? ASSERT_LINE : CLEAR_LINE);
	EEPROM_write_bit(data & 0x10);
	EEPROM_set_cs_line((data & 0x40) ? CLEAR_LINE : ASSERT_LINE);
}

/* we need to throw away bits for all ram accesses as the devices are connected as 16-bit */

static READ32_HANDLER( simpl156_spriteram_r )
{
	return spriteram32[offset]^0xffff0000;
}

static WRITE32_HANDLER( simpl156_spriteram_w )
{
	data &=0x0000ffff;
	mem_mask &=0x0000ffff;

	COMBINE_DATA(&spriteram32[offset]);
}

static UINT32*simpl156_mainram;


static READ32_HANDLER( simpl156_mainram_r )
{
	return simpl156_mainram[offset]^0xffff0000;
}

static WRITE32_HANDLER( simpl156_mainram_w )
{
	data &=0x0000ffff;
	mem_mask &=0x0000ffff;

	COMBINE_DATA(&simpl156_mainram[offset]);
}

static READ32_HANDLER( simpl156_pf1_rowscroll_r )
{
	return deco16_pf1_rowscroll[offset]^0xffff0000;
}

static WRITE32_HANDLER( simpl156_pf1_rowscroll_w )
{
	data &=0x0000ffff;
	mem_mask &=0x0000ffff;

	COMBINE_DATA(&deco16_pf1_rowscroll[offset]);
}

static READ32_HANDLER( simpl156_pf2_rowscroll_r )
{
	return deco16_pf2_rowscroll[offset]^0xffff0000;
}

static WRITE32_HANDLER( simpl156_pf2_rowscroll_w )
{
	data &=0x0000ffff;
	mem_mask &=0x0000ffff;

	COMBINE_DATA(&deco16_pf2_rowscroll[offset]);
}

static READ32_HANDLER ( simpl156_pf12_control_r )
{
	return deco16_pf12_control[offset]^0xffff0000;
}

static WRITE32_HANDLER( simpl156_pf12_control_w )
{
	data &=0x0000ffff;
	mem_mask &=0x0000ffff;

	COMBINE_DATA(&deco16_pf12_control[offset]);
}

static READ32_HANDLER( simpl156_pf1_data_r )
{
	return deco16_pf1_data[offset]^0xffff0000;
}

static WRITE32_HANDLER( simpl156_pf1_data_w )
{
	data &=0x0000ffff;
	mem_mask &=0x0000ffff;

	deco16_pf1_data_w(offset,data,mem_mask);
}

static READ32_HANDLER( simpl156_pf2_data_r )
{
	return deco16_pf2_data[offset]^0xffff0000;
}

static WRITE32_HANDLER( simpl156_pf2_data_w )
{
	data &=0x0000ffff;
	mem_mask &=0x0000ffff;
	deco16_pf2_data_w(offset,data,mem_mask);
}

READ32_HANDLER( simpl156_6295_0_r )
{
	return OKIM6295_status_0_r(0);
}

READ32_HANDLER( simpl156_6295_1_r )
{
	return OKIM6295_status_1_r(0);
}

// write?
WRITE32_HANDLER( simpl156_6295_0_w )
{
	if( ACCESSING_LSB32 ) {
		OKIM6295_data_0_w(0, data & 0xff);
	}
}

WRITE32_HANDLER( simpl156_6295_1_w )
{
	if( ACCESSING_LSB32 ) {
		OKIM6295_data_1_w(0, data & 0xff);
	}
}

static MEMORY_READ32_START( joemacr_readmem )
    { 0x000000, 0x07ffff, MRA32_ROM },
	{ 0x100000, 0x107fff, simpl156_mainram_r }, // main ram
	{ 0x110000, 0x111fff, simpl156_spriteram_r },
	{ 0x120000, 0x120fff, simpl156_palette_r },
	{ 0x130000, 0x130003, simpl156_system_r }, // eeprom
	{ 0x140000, 0x14001f, simpl156_pf12_control_r },
	{ 0x150000, 0x151fff, simpl156_pf1_data_r },
	{ 0x152000, 0x153fff, simpl156_pf1_data_r },
	{ 0x154000, 0x155fff, simpl156_pf2_data_r },
	{ 0x160000, 0x161fff, simpl156_pf1_rowscroll_r },
	{ 0x164000, 0x165fff, simpl156_pf2_rowscroll_r },
	{ 0x180000, 0x180003, simpl156_6295_0_r },
	{ 0x1c0000, 0x1c0003, simpl156_6295_1_r },
	{ 0x200000, 0x200003, simpl156_inputs_read },
	{ 0x201000, 0x201fff, MRA32_RAM },  // work ram (32-bit)
MEMORY_END

static MEMORY_WRITE32_START( joemacr_writemem )
    { 0x000000, 0x07ffff, MWA32_ROM },
	{ 0x100000, 0x107fff, simpl156_mainram_w, &simpl156_mainram },// main ram
	{ 0x110000, 0x111fff, simpl156_spriteram_w, &spriteram32, &spriteram_size },
	{ 0x120000, 0x120fff, simpl156_palette_w },
	{ 0x130000, 0x130003, simpl156_eeprom_w },
	{ 0x140000, 0x14001f, simpl156_pf12_control_w },
	{ 0x150000, 0x151fff, simpl156_pf1_data_w },
	{ 0x152000, 0x153fff, simpl156_pf1_data_w },
	{ 0x154000, 0x155fff, simpl156_pf2_data_w },
	{ 0x160000, 0x161fff, simpl156_pf1_rowscroll_w },
	{ 0x164000, 0x164fff, simpl156_pf2_rowscroll_w },
	{ 0x170000, 0x170003, MWA32_NOP }, // ?
	{ 0x180000, 0x180003, simpl156_6295_0_w },
	{ 0x1c0000, 0x1c0003, simpl156_6295_1_w },
	{ 0x201000, 0x201fff, MWA32_RAM, &simpl156_systemram }, // work ram (32-bit)
MEMORY_END

static MEMORY_READ32_START( mitchell156_readmem )
    { 0x000000, 0x07ffff, MRA32_ROM },
	{ 0x100000, 0x100003, simpl156_6295_0_r },
	{ 0x140000, 0x140003, simpl156_6295_1_r },
	{ 0x180000, 0x187fff, simpl156_mainram_r }, // main ram
	{ 0x190000, 0x191fff, simpl156_spriteram_r },
	{ 0x1a0000, 0x1a0fff, simpl156_palette_r },
	{ 0x1b0000, 0x1b0003, simpl156_system_r },
	{ 0x1c0000, 0x1c001f, simpl156_pf12_control_r },
	{ 0x1d0000, 0x1d1fff, simpl156_pf1_data_r },
	{ 0x1d2000, 0x1d3fff, simpl156_pf1_data_r },
	{ 0x1d4000, 0x1d5fff, simpl156_pf2_data_r },
	{ 0x1e0000, 0x1e1fff, simpl156_pf1_rowscroll_r },
	{ 0x1e4000, 0x1e5fff, simpl156_pf2_rowscroll_r },
	{ 0x200000, 0x200003, simpl156_inputs_read },
	{ 0x201000, 0x201fff, MRA32_RAM }, /* work ram (32-bit)*/
  { 0x202000, 0x202fff, MRA32_RAM }, /* work ram (32-bit) mirror needed for Osman */
MEMORY_END

static MEMORY_WRITE32_START( mitchell156_writemem )
    { 0x000000, 0x07ffff, MWA32_ROM },
	{ 0x100000, 0x100003, simpl156_6295_0_w },
	{ 0x140000, 0x140003, simpl156_6295_1_w },
	{ 0x180000, 0x187fff, simpl156_mainram_w, &simpl156_mainram }, // main ram
	{ 0x190000, 0x191fff, simpl156_spriteram_w, &spriteram32, &spriteram_size },
	{ 0x1a0000, 0x1a0fff, simpl156_palette_w },
	{ 0x1b0000, 0x1b0003, simpl156_eeprom_w },
	{ 0x1c0000, 0x1c001f, simpl156_pf12_control_w },
	{ 0x1d0000, 0x1d1fff, simpl156_pf1_data_w },
	{ 0x1d2000, 0x1d3fff, simpl156_pf1_data_w },
	{ 0x1d4000, 0x1d5fff, simpl156_pf2_data_w },
	{ 0x1e0000, 0x1e1fff, simpl156_pf1_rowscroll_w },
	{ 0x1e4000, 0x1e5fff, simpl156_pf2_rowscroll_w },
	{ 0x1f0000, 0x1f0003, MWA32_NOP }, /* ?*/
	{ 0x201000, 0x201fff, MWA32_RAM, &simpl156_systemram }, /* work ram (32-bit)*/
  { 0x202000, 0x202fff, MWA32_RAM, &simpl156_systemram }, /* work ram (32-bit) mirror needed for Osman */
MEMORY_END


static MEMORY_READ32_START( chainrec_readmem )
    { 0x000000, 0x07ffff, MRA32_ROM },
	{ 0x200000, 0x200003, simpl156_inputs_read },
	{ 0x201000, 0x201fff, MRA32_RAM },  // work ram (32-bit)
	{ 0x3c0000, 0x3c0003, simpl156_6295_1_r },
	{ 0x400000, 0x407fff, simpl156_mainram_r }, // main ram
	{ 0x410000, 0x411fff, simpl156_spriteram_r },
	{ 0x420000, 0x420fff, simpl156_palette_r },
	{ 0x430000, 0x430003, simpl156_system_r },
	{ 0x440000, 0x44001f, simpl156_pf12_control_r },
	{ 0x450000, 0x451fff, simpl156_pf1_data_r },
	{ 0x452000, 0x453fff, simpl156_pf1_data_r },
	{ 0x454000, 0x455fff, simpl156_pf2_data_r },
	{ 0x460000, 0x461fff, simpl156_pf1_rowscroll_r },
	{ 0x464000, 0x465fff, simpl156_pf2_rowscroll_r },
	{ 0x480000, 0x480003, simpl156_6295_0_r },
MEMORY_END

static MEMORY_WRITE32_START( chainrec_writemem )
    { 0x000000, 0x07ffff, MWA32_ROM },
	{ 0x201000, 0x201fff, MWA32_RAM, &simpl156_systemram },// work ram (32-bit)
	{ 0x3c0000, 0x3c0003, simpl156_6295_1_w },
	{ 0x400000, 0x407fff, simpl156_mainram_w, &simpl156_mainram }, // main ram
	{ 0x410000, 0x411fff, simpl156_spriteram_w, &spriteram32, &spriteram_size },
	{ 0x420000, 0x420fff, simpl156_palette_w },
	{ 0x430000, 0x430003, simpl156_eeprom_w },
	{ 0x440000, 0x44001f, simpl156_pf12_control_w },
	{ 0x450000, 0x451fff, simpl156_pf1_data_w },
	{ 0x452000, 0x453fff, simpl156_pf1_data_w },
	{ 0x454000, 0x455fff, simpl156_pf2_data_w },
	{ 0x460000, 0x461fff, simpl156_pf1_rowscroll_w },
	{ 0x464000, 0x465fff, simpl156_pf2_rowscroll_w },
	{ 0x470000, 0x470003, MWA32_NOP }, // ?
	{ 0x480000, 0x480003, simpl156_6295_0_w },
MEMORY_END


static MEMORY_READ32_START( magdrop_readmem )
    { 0x000000, 0x07ffff, MRA32_ROM },
	{ 0x200000, 0x200003, simpl156_inputs_read },
	{ 0x201000, 0x201fff, MRA32_RAM },  // work ram (32-bit)
	{ 0x340000, 0x340003, simpl156_6295_1_r },
	{ 0x380000, 0x387fff, simpl156_mainram_r }, // main ram
	{ 0x390000, 0x391fff, simpl156_spriteram_r },
	{ 0x3a0000, 0x3a0fff, simpl156_palette_r },
	{ 0x3b0000, 0x3b0003, simpl156_system_r },
	{ 0x3c0000, 0x3c001f, simpl156_pf12_control_r },
	{ 0x3d0000, 0x3d1fff, simpl156_pf1_data_r },
	{ 0x3d2000, 0x3d3fff, simpl156_pf1_data_r },
	{ 0x3d4000, 0x3d5fff, simpl156_pf2_data_r },
	{ 0x3e0000, 0x3e1fff, simpl156_pf1_rowscroll_r },
	{ 0x3e4000, 0x3e5fff, simpl156_pf2_rowscroll_r },
	{ 0x400000, 0x400003, simpl156_6295_0_r },
MEMORY_END

static MEMORY_WRITE32_START( magdrop_writemem )
    { 0x000000, 0x07ffff, MWA32_ROM },
	{ 0x201000, 0x201fff, MWA32_RAM, &simpl156_systemram }, // work ram (32-bit)
	{ 0x340000, 0x340003, simpl156_6295_1_w },
	{ 0x380000, 0x387fff, simpl156_mainram_w, &simpl156_mainram },// main ram
	{ 0x390000, 0x391fff, simpl156_spriteram_w, &spriteram32, &spriteram_size },
	{ 0x3a0000, 0x3a0fff, simpl156_palette_w },
	{ 0x3b0000, 0x3b0003, simpl156_eeprom_w },
	{ 0x3c0000, 0x3c001f, simpl156_pf12_control_w },
	{ 0x3d0000, 0x3d1fff, simpl156_pf1_data_w },
	{ 0x3d2000, 0x3d3fff, simpl156_pf1_data_w },
	{ 0x3d4000, 0x3d5fff, simpl156_pf2_data_w },
	{ 0x3e0000, 0x3e1fff, simpl156_pf1_rowscroll_w },
	{ 0x3e4000, 0x3e5fff, simpl156_pf2_rowscroll_w },
	{ 0x3f0000, 0x3f0003, MWA32_NOP }, // ?
	{ 0x400000, 0x400003, simpl156_6295_0_w },
MEMORY_END


static MEMORY_READ32_START( magdropp_readmem )
    { 0x000000, 0x07ffff, MRA32_ROM },
	{ 0x200000, 0x200003, simpl156_inputs_read },
	{ 0x201000, 0x201fff, MRA32_RAM }, // work ram (32-bit)
	{ 0x4c0000, 0x4c0003, simpl156_6295_1_r },
	{ 0x680000, 0x687fff, simpl156_mainram_r }, // main ram
	{ 0x690000, 0x691fff, simpl156_spriteram_r },
	{ 0x6a0000, 0x6a0fff, simpl156_palette_r },
	{ 0x6b0000, 0x6b0003, simpl156_system_r },
	{ 0x6c0000, 0x6c001f, simpl156_pf12_control_r },
	{ 0x6d0000, 0x6d1fff, simpl156_pf1_data_r },
	{ 0x6d2000, 0x6d3fff, simpl156_pf1_data_r },
	{ 0x6d4000, 0x6d5fff, simpl156_pf2_data_r },
	{ 0x6e0000, 0x6e1fff, simpl156_pf1_rowscroll_r },
	{ 0x6e4000, 0x6e5fff, simpl156_pf2_rowscroll_r },
	{ 0x780000, 0x780003, simpl156_6295_0_r },
MEMORY_END

static MEMORY_WRITE32_START( magdropp_writemem )
    { 0x000000, 0x07ffff, MWA32_ROM },
	{ 0x201000, 0x201fff, MWA32_RAM, &simpl156_systemram }, // work ram (32-bit)
	{ 0x4c0000, 0x4c0003, simpl156_6295_1_w },
	{ 0x680000, 0x687fff, simpl156_mainram_w, &simpl156_mainram }, // main ram
	{ 0x690000, 0x691fff, simpl156_spriteram_w, &spriteram32, &spriteram_size },
	{ 0x6a0000, 0x6a0fff, simpl156_palette_w },
	{ 0x6b0000, 0x6b0003, simpl156_eeprom_w },
	{ 0x6c0000, 0x6c001f, simpl156_pf12_control_w },
	{ 0x6d0000, 0x6d1fff, simpl156_pf1_data_w },
	{ 0x6d2000, 0x6d3fff, simpl156_pf1_data_w },
	{ 0x6d4000, 0x6d5fff, simpl156_pf2_data_w },
	{ 0x6e0000, 0x6e1fff, simpl156_pf1_rowscroll_w },
	{ 0x6e4000, 0x6e5fff, simpl156_pf2_rowscroll_w },
	{ 0x6f0000, 0x6f0003, MWA32_NOP }, // ?
	{ 0x780000, 0x780003, simpl156_6295_0_w },
MEMORY_END


static struct GfxLayout tile_8x8_layout =
{
	8,8,
	RGN_FRAC(1,2),
	4,
	{ RGN_FRAC(1,2)+8,RGN_FRAC(1,2)+0,RGN_FRAC(0,2)+8,RGN_FRAC(0,2)+0 },
	{ 0, 1, 2, 3, 4, 5, 6, 7 },
	{ 0*16, 1*16, 2*16, 3*16, 4*16, 5*16, 6*16, 7*16 },
	8*16
};

static struct GfxLayout tile_16x16_layout =
{
	16,16,
	RGN_FRAC(1,2),
	4,
	{ RGN_FRAC(1,2)+8,RGN_FRAC(1,2)+0,RGN_FRAC(0,2)+8,RGN_FRAC(0,2)+0 },
	{ 256,257,258,259,260,261,262,263,0, 1, 2, 3, 4, 5, 6, 7 },
	{ 0*16, 1*16, 2*16, 3*16, 4*16, 5*16, 6*16, 7*16,8*16,9*16,10*16,11*16,12*16,13*16,14*16,15*16 },
	32*16
};

static struct GfxLayout spritelayout =
{
	16,16,
	RGN_FRAC(1,1),
	4,
	{ 24,8,16,0 },
	{ 512,513,514,515,516,517,518,519, 0, 1, 2, 3, 4, 5, 6, 7 },
	{ 0*32, 1*32, 2*32, 3*32, 4*32, 5*32, 6*32, 7*32,
	  8*32, 9*32,10*32,11*32,12*32,13*32,14*32,15*32},
	32*32
};

static struct GfxDecodeInfo gfxdecodeinfo[] =
{
	{ REGION_GFX1, 0, &tile_8x8_layout,     0, 32 },
	{ REGION_GFX1, 0, &tile_16x16_layout,     0, 32 },
	{ REGION_GFX2, 0, &spritelayout,     0x200, 32 },
	{ -1 }	/* end of array */
};

static NVRAM_HANDLER( simpl156 )
{
	if (read_or_write)
		EEPROM_save(file);
	else
	{
		EEPROM_init(&eeprom_interface_93C46);// 93c45

		if (file) EEPROM_load(file);
		else
		{
			if (simpl156_default_eeprom)	{ /* Set the EEPROM to Factory Defaults */

				data8_t eeprom_data[0x100]; // lazy
				memcpy (eeprom_data, simpl156_default_eeprom, 0x80);

				EEPROM_set_data(eeprom_data,0x100);
			}
		}
	}
}

static INTERRUPT_GEN( simpl156_vbl_interrupt )
{
	cpu_set_irq_line(0, ARM_IRQ_LINE, HOLD_LINE);
}

static struct OKIM6295interface adpcm_6295_interface =
{
	2,          	/* 2 chips */
	{ 1006875/132, 2013750/132 },      /* 10847 Hz frequency (1.431815MHz / 132) */
	{ REGION_SOUND1, REGION_SOUND2 },  /* memory */
	{ 100, 60 }
};

static struct OKIM6295interface mitchell156_6295_interface =
{
	2,          	/* 2 chips */
	{ 1006875/132, 1006875/132 },      /* 10847 Hz frequency (1.431815MHz / 132) */
	{ REGION_SOUND1, REGION_SOUND2 },  /* memory */
	{ 60, 20 }
};

static MACHINE_DRIVER_START( mitchell156 )
	/* basic machine hardware */

	MDRV_CPU_ADD(ARM, 28000000 /* /4 */)	/*DE156*/ /* 7.000 MHz */ /* measured at 7.. seems to need 28? */
	MDRV_CPU_MEMORY(mitchell156_readmem,mitchell156_writemem)
	MDRV_CPU_VBLANK_INT(simpl156_vbl_interrupt,1)

	MDRV_NVRAM_HANDLER(simpl156) // 93C45

	MDRV_FRAMES_PER_SECOND(58)
	MDRV_VBLANK_DURATION(DEFAULT_60HZ_VBLANK_DURATION)

	/* video hardware */
	MDRV_VIDEO_ATTRIBUTES(VIDEO_TYPE_RASTER)
	MDRV_SCREEN_SIZE(64*8, 32*8)
	MDRV_VISIBLE_AREA(0*8, 40*8-1, 1*8, 31*8-1)

	MDRV_GFXDECODE(gfxdecodeinfo)
	MDRV_PALETTE_LENGTH(4096)

	MDRV_VIDEO_START(simpl156)
	MDRV_VIDEO_UPDATE(simpl156)

	MDRV_SOUND_ATTRIBUTES(SOUND_SUPPORTS_STEREO)
	MDRV_SOUND_ADD(OKIM6295, mitchell156_6295_interface)
MACHINE_DRIVER_END

static MACHINE_DRIVER_START( joemacr )
	/* basic machine hardware */

	MDRV_CPU_ADD(ARM, 28000000 /* /4 */)	/*DE156*/ /* 7.000 MHz */ /* measured at 7.. seems to need 28? */
	MDRV_CPU_MEMORY(joemacr_readmem,joemacr_writemem)
	MDRV_CPU_VBLANK_INT(simpl156_vbl_interrupt,1)

	MDRV_NVRAM_HANDLER(simpl156) // 93C45

	MDRV_FRAMES_PER_SECOND(58)
	MDRV_VBLANK_DURATION(TIME_IN_USEC(800))

	/* video hardware */
	MDRV_VIDEO_ATTRIBUTES(VIDEO_TYPE_RASTER)
	MDRV_SCREEN_SIZE(64*8, 32*8)
	MDRV_VISIBLE_AREA(0*8, 40*8-1, 1*8, 31*8-1)

	MDRV_GFXDECODE(gfxdecodeinfo)
	MDRV_PALETTE_LENGTH(4096)

	MDRV_VIDEO_START(simpl156)
	MDRV_VIDEO_UPDATE(simpl156)

	MDRV_SOUND_ATTRIBUTES(SOUND_SUPPORTS_STEREO)
	MDRV_SOUND_ADD(OKIM6295, adpcm_6295_interface)
MACHINE_DRIVER_END

static MACHINE_DRIVER_START( chainrec )
	/* basic machine hardware */

	MDRV_CPU_ADD(ARM, 28000000 /* /4 */)	/*DE156*/ /* 7.000 MHz */ /* measured at 7.. seems to need 28? */
	MDRV_CPU_MEMORY(chainrec_readmem,chainrec_writemem)
	MDRV_CPU_VBLANK_INT(simpl156_vbl_interrupt,1)

	MDRV_NVRAM_HANDLER(simpl156) // 93C45

	MDRV_FRAMES_PER_SECOND(58)
	MDRV_VBLANK_DURATION(TIME_IN_USEC(800))

	/* video hardware */
	MDRV_VIDEO_ATTRIBUTES(VIDEO_TYPE_RASTER)
	MDRV_SCREEN_SIZE(64*8, 32*8)
	MDRV_VISIBLE_AREA(0*8, 40*8-1, 1*8, 31*8-1)

	MDRV_GFXDECODE(gfxdecodeinfo)
	MDRV_PALETTE_LENGTH(4096)

	MDRV_VIDEO_START(simpl156)
	MDRV_VIDEO_UPDATE(simpl156)

	MDRV_SOUND_ATTRIBUTES(SOUND_SUPPORTS_STEREO)
	MDRV_SOUND_ADD(OKIM6295, adpcm_6295_interface) // not right!!
MACHINE_DRIVER_END

static MACHINE_DRIVER_START( magdrop )
	/* basic machine hardware */

	MDRV_CPU_ADD(ARM, 28000000 /* /4 */)	/*DE156*/ /* 7.000 MHz */ /* measured at 7.. seems to need 28? */
	MDRV_CPU_MEMORY(magdrop_readmem,magdrop_writemem)
	MDRV_CPU_VBLANK_INT(simpl156_vbl_interrupt,1)

	MDRV_NVRAM_HANDLER(simpl156) // 93C45

	MDRV_FRAMES_PER_SECOND(58)
	MDRV_VBLANK_DURATION(TIME_IN_USEC(800))

	/* video hardware */
	MDRV_VIDEO_ATTRIBUTES(VIDEO_TYPE_RASTER)
	MDRV_SCREEN_SIZE(64*8, 32*8)
	MDRV_VISIBLE_AREA(0*8, 40*8-1, 1*8, 31*8-1)

	MDRV_GFXDECODE(gfxdecodeinfo)
	MDRV_PALETTE_LENGTH(4096)

	MDRV_VIDEO_START(simpl156)
	MDRV_VIDEO_UPDATE(simpl156)

	MDRV_SOUND_ATTRIBUTES(SOUND_SUPPORTS_STEREO)
	MDRV_SOUND_ADD(OKIM6295, adpcm_6295_interface) // not right!!
MACHINE_DRIVER_END

static MACHINE_DRIVER_START( magdropp )
	/* basic machine hardware */

	MDRV_CPU_ADD(ARM, 28000000 /* /4 */)	/*DE156*/ /* 7.000 MHz */ /* measured at 7.. seems to need 28? */
	MDRV_CPU_MEMORY(magdropp_readmem,magdropp_writemem)
	MDRV_CPU_VBLANK_INT(simpl156_vbl_interrupt,1)

	MDRV_NVRAM_HANDLER(simpl156) // 93C45

	MDRV_FRAMES_PER_SECOND(58)
	MDRV_VBLANK_DURATION(TIME_IN_USEC(800))

	/* video hardware */
	MDRV_VIDEO_ATTRIBUTES(VIDEO_TYPE_RASTER)
	MDRV_SCREEN_SIZE(64*8, 32*8)
	MDRV_VISIBLE_AREA(0*8, 40*8-1, 1*8, 31*8-1)

	MDRV_GFXDECODE(gfxdecodeinfo)
	MDRV_PALETTE_LENGTH(4096)

	MDRV_VIDEO_START(simpl156)
	MDRV_VIDEO_UPDATE(simpl156)

	MDRV_SOUND_ATTRIBUTES(SOUND_SUPPORTS_STEREO)
	MDRV_SOUND_ADD(OKIM6295, adpcm_6295_interface) // not right!!
MACHINE_DRIVER_END

static void simpl156_common_init(void)
{
	UINT8 *rom = memory_region(REGION_SOUND2);
	int length = memory_region_length(REGION_SOUND2);
	UINT8 *buf1 = (UINT8*)malloc(length);

	UINT32 x;

	/* hmm low address line goes to banking chip instead? */
	for (x=0;x<length;x++)
	{
		UINT32 addr;

		addr = BITSWAP24 (x,23,22,21,0, 20,
		                    19,18,17,16,
		                    15,14,13,12,
		                    11,10,9, 8,
		                    7, 6, 5, 4,
		                    3, 2, 1 );

		buf1[addr] = rom[x];
	}

	memcpy(rom,buf1,length);

	free (buf1);

	deco56_decrypt(REGION_GFX1);

	deco156_decrypt();
}

/*

Joe and Mac Returns
Data East 1994

This is a higher quality bootleg made with genuine DECO chips/parts.

+-------------------------------+
|        07.u46           04.u12|
| VOL  M6296 M6295    52  03.u11|
|J  06.u45      62256           |
|A              62256     02.u8h|
|M    PAL  141            01.u8l|
|M     62256                    |
|A 223 62256  PAL  28MHz        |
|             62256         156 |
|  SW1        62256   223       |
|      93C45  05.u29            |
+-------------------------------+

All roms are socketted eproms, no labels, just a number in pencel.

05.u29  27c4096
01.u8l  27c4096
02.u8h  27c4096
03.u11  27c4096
04.u12  27c4096
06.u45  27c160
07.u46  27c020

*/

ROM_START( joemacr )
	ROM_REGION( 0x80000, REGION_CPU1, 0 ) /* DE156 code (encrypted) */
	ROM_LOAD( "05.u29",    0x000000, 0x080000,  CRC(74e9a158) SHA1(eee447303ac0884e152b89f59a9694afade87336) )

	ROM_REGION( 0x100000, REGION_GFX1, 0 )
	ROM_LOAD( "01.u8l",    0x000000, 0x080000,  CRC(4da4a2c1) SHA1(1ed4bd4337d8b185b56e326e662a8715e4d09e17) )
	ROM_LOAD( "02.u8h",    0x080000, 0x080000,  CRC(642c08db) SHA1(9a541fd56ae34c24f803e08869702be6fafd81d1) )

	ROM_REGION( 0x100000, REGION_GFX2, 0 )
	ROM_LOAD16_BYTE( "mbn01",    0x000001, 0x080000, CRC(a3a37353) SHA1(c4509c8268afb647c20e71b42ae8ebd2bdf075e6) ) /* 03.u11 */
	ROM_LOAD16_BYTE( "mbn02",    0x000000, 0x080000, CRC(aa2230c5) SHA1(43b7ac5c69cde1840a5255a8897e1c5d5f89fd7b) ) /* 04.u12 */

	ROM_REGION( 0x80000, REGION_SOUND1, 0 ) /* Oki samples */
	ROM_LOAD( "mbn04",    0x00000, 0x40000,  CRC(dcbd4771) SHA1(2a1ab6b0fc372333c7eb17aab077fe1ca5ba1dea) ) /* 07.u46 */

	ROM_REGION( 0x200000, REGION_SOUND2, 0 ) /* samples? (banked?) */
	ROM_LOAD( "mbn03",    0x00000, 0x200000, CRC(70b71a2a) SHA1(45851b0692de73016fc9b913316001af4690534c) ) /* 06.u45 */
ROM_END

/*

Joe and Mac Returns
Data East 1994

DE-0491-1

156         MW00
      223              223
             141

  MBN00

  MBN01   52   MBN03  M6295
  MBN02        MBN04  M6295

*/

ROM_START( joemacra )
	ROM_REGION( 0x80000, REGION_CPU1, 0 ) /* DE156 code (encrypted) */
	ROM_LOAD( "mw00",    0x000000, 0x080000,  CRC(e1b78f40) SHA1(e611c317ada5a049a5e05d69c051e22a43fa2845) )

	ROM_REGION( 0x100000, REGION_GFX1, 0 ) // rebuilt with roms from other set
	ROM_LOAD( "mbn00",    0x000000, 0x100000, CRC(11b2dac7) SHA1(71a50f606caddeb0ef266e2d3df9e429a4873f21) )

	ROM_REGION( 0x100000, REGION_GFX2, 0 )
	ROM_LOAD16_BYTE( "mbn01",    0x000001, 0x080000, CRC(a3a37353) SHA1(c4509c8268afb647c20e71b42ae8ebd2bdf075e6) )
	ROM_LOAD16_BYTE( "mbn02",    0x000000, 0x080000, CRC(aa2230c5) SHA1(43b7ac5c69cde1840a5255a8897e1c5d5f89fd7b) )

	ROM_REGION( 0x80000, REGION_SOUND1, 0 ) /* Oki samples */
	ROM_LOAD( "mbn04",    0x00000, 0x40000,  CRC(dcbd4771) SHA1(2a1ab6b0fc372333c7eb17aab077fe1ca5ba1dea) )

	ROM_REGION( 0x200000, REGION_SOUND2, 0 ) /* samples? (banked?) */
	ROM_LOAD( "mbn03",    0x00000, 0x200000, CRC(70b71a2a) SHA1(45851b0692de73016fc9b913316001af4690534c) )
ROM_END

/*

Chain Reaction
Data East 1995

DEC-22VO
DE-0409-1

156           E1
       223    2063     93C45
              2063
                              223
               141
       28MHz         5864
                     5864
            6264
            6264
  MCC-00

  U5* U6*    52      MCC-03   AD-65
  U3* U4*            MCC-04   AD-65

* NOTE: These 4 roms are on a sub-board connecting through mcc-01 & mcc-02 sockets

*/

ROM_START( chainrec )
	ROM_REGION( 0x80000, REGION_CPU1, 0 ) /* DE156 code (encrypted) */
	ROM_LOAD( "e1",    0x000000, 0x080000, CRC(8a8340ef) SHA1(4aaee56127b73453b862ff2a33dc241eeabf5658) ) /* No DECO ID number on label */

	ROM_REGION( 0x100000, REGION_GFX1, 0 )
	ROM_LOAD( "mcc-00",    0x000000, 0x100000, CRC(646b03ec) SHA1(9a2fc11b1575032b5a784d88c3a90913068d1e69) )

	ROM_REGION( 0x200000, REGION_GFX2, 0 )
	ROM_LOAD32_BYTE( "u3",    0x000000, 0x080000, CRC(92659721) SHA1(b446ce98ec9c2c16375ef00639cfb463b365b8f7) ) /* No DECO ID numbers on labels */
	ROM_LOAD32_BYTE( "u4",    0x000002, 0x080000, CRC(e304eb32) SHA1(61a647ec89695a6b25ff924bdc6d29cbd7aca82b) )
	ROM_LOAD32_BYTE( "u5",    0x000001, 0x080000, CRC(1b6f01ea) SHA1(753fc670707432e317d035b09b0bad0762fea731) )
	ROM_LOAD32_BYTE( "u6",    0x000003, 0x080000, CRC(531a56f2) SHA1(89602bb873a3b110bffc216f921ba228e53380f9) )

	ROM_REGION( 0x80000, REGION_SOUND1, 0 ) /* Oki samples */
	ROM_LOAD( "mcc-04",    0x00000, 0x40000,  CRC(86ee6ade) SHA1(56ad3f432c7f430f19fcba7c89940c63da165906) )

	ROM_REGION( 0x200000, REGION_SOUND2, 0 ) /* samples? (banked?) */
	ROM_LOAD( "mcc-03",    0x00000, 0x100000, CRC(da2ebba0) SHA1(96d31dea4c7226ee1d386b286919fa334388c7a1) )
ROM_END

/*

Magical Drop / Magical Drop Plus 1
Data East 1995

DE-0409-1

156           E1
       223    2063     93C45
              2063
                              223
               141
       28MHz         5864
                     5864
            6264
            6264
  MCC-00

  mcc-01     52      MCC-03   AD-65
  mcc-02             MCC-04   AD-65


*/

ROM_START( magdrop )
	ROM_REGION( 0x80000, REGION_CPU1, 0 ) /* DE156 code (encrypted) */
	ROM_LOAD( "re00-2.e1",    0x000000, 0x080000,  CRC(7138f10f) SHA1(ca93c3c2dc9a7dd6901c8429a6bf6883076a9b8f) )

	ROM_REGION( 0x100000, REGION_GFX1, 0 )
	ROM_LOAD( "mcc-00",    0x000000, 0x100000, CRC(646b03ec) SHA1(9a2fc11b1575032b5a784d88c3a90913068d1e69) )

	ROM_REGION( 0x200000, REGION_GFX2, 0 )
	ROM_LOAD16_BYTE( "mcc-01.a13",    0x000001, 0x100000, CRC(13d88745) SHA1(0ce4ec1481f31be860ee80322de6e32f9a566229) )
	ROM_LOAD16_BYTE( "mcc-02.a14",    0x000000, 0x100000, CRC(d0f97126) SHA1(3848a6f00d0e57aaf383298c4d111eb63a88b073) )

	ROM_REGION( 0x80000, REGION_SOUND1, 0 ) /* Oki samples */
	ROM_LOAD( "mcc-04",    0x00000, 0x40000,  CRC(86ee6ade) SHA1(56ad3f432c7f430f19fcba7c89940c63da165906) )

	ROM_REGION( 0x200000, REGION_SOUND2, 0 ) /* samples? (banked?) */
	ROM_LOAD( "mcc-03",    0x00000, 0x100000, CRC(da2ebba0) SHA1(96d31dea4c7226ee1d386b286919fa334388c7a1) )

	ROM_REGION( 0x80, REGION_USER1, 0 ) /* eeprom */
	ROM_LOAD( "93c45.2h",    0x00, 0x80, CRC(16ce8d2d) SHA1(1a6883c75d34febbd92a16cfe204ff12550c85fd) )
ROM_END

ROM_START( magdropp )
	ROM_REGION( 0x80000, REGION_CPU1, 0 ) /* DE156 code (encrypted) */
	ROM_LOAD( "rz00-1.e1",    0x000000, 0x080000,  CRC(28caf639) SHA1(a17e792c82e65009e21680094acf093c0c4f1021) )

	ROM_REGION( 0x100000, REGION_GFX1, 0 )
	ROM_LOAD( "mcc-00",    0x000000, 0x100000, CRC(646b03ec) SHA1(9a2fc11b1575032b5a784d88c3a90913068d1e69) )

	ROM_REGION( 0x200000, REGION_GFX2, 0 )
	ROM_LOAD16_BYTE( "mcc-01.a13",    0x000001, 0x100000, CRC(13d88745) SHA1(0ce4ec1481f31be860ee80322de6e32f9a566229) )
	ROM_LOAD16_BYTE( "mcc-02.a14",    0x000000, 0x100000, CRC(d0f97126) SHA1(3848a6f00d0e57aaf383298c4d111eb63a88b073) )

	ROM_REGION( 0x80000, REGION_SOUND1, 0 ) /* Oki samples */
	ROM_LOAD( "mcc-04",    0x00000, 0x40000,  CRC(86ee6ade) SHA1(56ad3f432c7f430f19fcba7c89940c63da165906) )

	ROM_REGION( 0x200000, REGION_SOUND2, 0 ) /* samples? (banked?) */
	ROM_LOAD( "mcc-03",    0x00000, 0x100000, CRC(da2ebba0) SHA1(96d31dea4c7226ee1d386b286919fa334388c7a1) )

	ROM_REGION( 0x80, REGION_USER1, 0 ) /* eeprom */
	ROM_LOAD( "eeprom.2h",    0x00, 0x80, CRC(d13d9edd) SHA1(e98ee2b696f0a7a8752dc30ef8b41bfe6598cbe4) )
ROM_END

/*

Charlie Ninja
(c)1995 Mitchell Corp.

DEC-22VO (board is manufactured by DECO)
MT5601-0
------------------------------------------------------------
|                 MBR03.14H                    MBR-01.14A  |
|                                                          |
|     OKI M6295   ND01-0.13H            52                 |
|                                                          |
--|   OKI M6295    MBR02.12F                               |
  |                                                        |
--|                               CY7C185-25   MBR-00.9A   |
|                                 CY7C185-25               |
|                                                          |
|                                                          |
|J                     GAL16V8           28.000MHz         |
|A                  CY7C185-25                             |
|M                  CY7C185-25     141                     |
|M                                                         |
|A          223                                            |
|                                             GAL16V8      |
|                                GAL16V8                   |
|                             CY7C185-25   223             |
--|                93C45.2H   CY7C185-25                   |
  |                                                  156   |
--| TESTSW                  ND00-1.1E                      |
|                                                          |
------------------------------------------------------------

eprom 1e   27c4002  labeled ND 00-1
eprom 13h  27c2001  labeled ND 01-0

maskrom 14a  27c800  labeled MBR-01
maskrom 14h  27c800  labeled MBR-03
maskrom 12f  27c800  labeled MBR-02
maskrom 9a   27c160  labeled MBR-00

*/

ROM_START( charlien )
	ROM_REGION( 0x80000, REGION_CPU1, 0 ) /* DE156 code (encrypted) */
	ROM_LOAD( "nd00-1.1e",    0x000000, 0x080000,  CRC(f18f4b23) SHA1(cb0c159b4dde3a3c5f295f270485996811e5e4d2) )

	ROM_REGION( 0x200000, REGION_GFX1, 0 )
	ROM_LOAD( "mbr-00.9a",    0x000000, 0x080000, CRC(ecf2c7f0) SHA1(3c735a4eef2bc49f16ac9365a5689101f43c13e9) )
	ROM_CONTINUE( 0x100000, 0x080000)
	ROM_CONTINUE( 0x080000, 0x080000)
	ROM_CONTINUE( 0x180000, 0x080000)

	ROM_REGION( 0x800000, REGION_GFX2, 0 )
	ROM_LOAD16_BYTE( "mbr-01.14a",    0x000001, 0x100000, CRC(46c90215) SHA1(152acdeea34ec1db3f761066a0c1ff6e43e47f9d) )
	ROM_LOAD16_BYTE( "mbr-03.14h",    0x000000, 0x100000, CRC(c448a68a) SHA1(4b607dfee269abdfeb710b74b73ef87dc2b30e8c) )

	ROM_REGION( 0x80000, REGION_SOUND1, 0 ) /* Oki samples */
	ROM_LOAD( "nd01-0.13h",    0x00000, 0x40000,  CRC(635a100a) SHA1(f6ec70890892e7557097ccd519de37247bb8c98d) )

	ROM_REGION( 0x200000, REGION_SOUND2, 0 ) /* samples? (banked?) */
	ROM_LOAD( "mbr-02.12f",    0x00000, 0x100000, CRC(4f67d333) SHA1(608f921bfa6b7020c0ce72e5229b3f1489208b23) ) // 00, 01, 04, 05
ROM_END

/*

Osman / Cannon Dancer
(c)1996 Mitchell Corp.

The game is very similar to Strider as it was programmed by some of the same programmers.

PCB Layout
----------

DEC-22VO (board is manufactured by DECO)
MT5601-0
------------------------------------------------------------
|                 MCF04.14H     MCF-03.14D     MCF-02.14A  |
|                                                          |
|     OKI M6295   SA01-0.13H            52     MCF-01.13A  |
|                                                          |
--|   OKI M6295    MCF05.12F                               |
  |                                                        |
--|                               CY7C185-25   MCF-00.9A   |
|                                 CY7C185-25               |
|                                                          |
|                                                          |
|J                     GAL16V8           28.000MHz         |
|A                  CY7C185-25                             |
|M                  CY7C185-25     141                     |
|M                                                         |
|A          223                                            |
|                                             GAL16V8      |
|                                GAL16V8                   |
|                             CY7C185-25   223             |
--|                93C45.2H   CY7C185-25                   |
  |                                                  156   |
--| TESTSW                  SA00-0.1E                      |
|                                                          |
------------------------------------------------------------

*/

ROM_START( osman )
	ROM_REGION( 0x80000, REGION_CPU1, 0 ) /* DE156 code (encrypted) */
	ROM_LOAD( "sa00-0.1e",    0x000000, 0x080000, CRC(ec6b3257) SHA1(10a42a680ce122ab030eaa2ccd99d302cb77854e) )

	ROM_REGION( 0x200000, REGION_GFX1, 0 )
	ROM_LOAD( "mcf-00.9a",    0x000000, 0x080000, CRC(247712dc) SHA1(bcb765afd7e756b68131c97c30d210de115d6b50) )
	ROM_CONTINUE( 0x100000, 0x080000)
	ROM_CONTINUE( 0x080000, 0x080000)
	ROM_CONTINUE( 0x180000, 0x080000)

	ROM_REGION( 0x800000, REGION_GFX2, 0 )
	ROM_LOAD16_BYTE( "mcf-02.14a",    0x000001, 0x200000, CRC(21251b33) SHA1(d252fe5c6eef8cbc9327e4176b4868b1cb17a738) )
	ROM_LOAD16_BYTE( "mcf-04.14h",    0x000000, 0x200000, CRC(4fa55577) SHA1(e229ba9cce46b92ce255aa33b974e19b214c4017) )
	ROM_LOAD16_BYTE( "mcf-01.13a",    0x400001, 0x200000, CRC(83881e25) SHA1(ae82cf0f704e6efea94c6c1d276d4e3e5b3ebe43) )
	ROM_LOAD16_BYTE( "mcf-03.14d",    0x400000, 0x200000, CRC(faf1d51d) SHA1(675dbbfe15b8010d54b2b3af26d42cdd753c2ce2) )

	ROM_REGION( 0x80000, REGION_SOUND1, 0 ) /* Oki samples */
	ROM_LOAD( "sa01-0.13h",    0x00000, 0x40000,  CRC(cea8368e) SHA1(1fcc641381fdc29bd50d3a4b23e67647f79e505a))

	ROM_REGION( 0x200000, REGION_SOUND2, 0 ) /* samples? (banked?) */
	ROM_LOAD( "mcf-05.12f",    0x00000, 0x200000, CRC(f007d376) SHA1(4ba20e5dabeacc3278b7f30c4462864cbe8f6984) )
ROM_END

/* NOTE: Cannon Dancer uses IDENTICAL roms to Osman. Region is contained in the eeprom settings which we set in the INIT function */

ROM_START( candance )
	ROM_REGION( 0x80000, REGION_CPU1, 0 ) /* DE156 code (encrypted) */
	ROM_LOAD( "sa00-0.1e",    0x000000, 0x080000, CRC(ec6b3257) SHA1(10a42a680ce122ab030eaa2ccd99d302cb77854e) )

	ROM_REGION( 0x200000, REGION_GFX1, 0 )
	ROM_LOAD( "mcf-00.9a",    0x000000, 0x080000, CRC(247712dc) SHA1(bcb765afd7e756b68131c97c30d210de115d6b50) )
	ROM_CONTINUE( 0x100000, 0x080000)
	ROM_CONTINUE( 0x080000, 0x080000)
	ROM_CONTINUE( 0x180000, 0x080000)

	ROM_REGION( 0x800000, REGION_GFX2, 0 )
	ROM_LOAD16_BYTE( "mcf-02.14a",    0x000001, 0x200000, CRC(21251b33) SHA1(d252fe5c6eef8cbc9327e4176b4868b1cb17a738) )
	ROM_LOAD16_BYTE( "mcf-04.14h",    0x000000, 0x200000, CRC(4fa55577) SHA1(e229ba9cce46b92ce255aa33b974e19b214c4017) )
	ROM_LOAD16_BYTE( "mcf-01.13a",    0x400001, 0x200000, CRC(83881e25) SHA1(ae82cf0f704e6efea94c6c1d276d4e3e5b3ebe43) )
	ROM_LOAD16_BYTE( "mcf-03.14d",    0x400000, 0x200000, CRC(faf1d51d) SHA1(675dbbfe15b8010d54b2b3af26d42cdd753c2ce2) )

	ROM_REGION( 0x80000, REGION_SOUND1, 0 ) /* Oki samples */
	ROM_LOAD( "sa01-0.13h",    0x00000, 0x40000,  CRC(cea8368e) SHA1(1fcc641381fdc29bd50d3a4b23e67647f79e505a))

	ROM_REGION( 0x200000, REGION_SOUND2, 0 ) /* samples? (banked?) */
	ROM_LOAD( "mcf-05.12f",    0x00000, 0x200000, CRC(f007d376) SHA1(4ba20e5dabeacc3278b7f30c4462864cbe8f6984) )
ROM_END


/* some default eeproms */

static const UINT8 chainrec_eeprom[128] = {
	0x52, 0x54, 0x00, 0x50, 0x00, 0x00, 0x39, 0x11, 0x41, 0x54, 0x00, 0x43, 0x00, 0x50, 0xFF, 0xFF,
	0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
	0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
	0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
	0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
	0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
	0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
	0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF
};

/*
  Cannon Dancer / Osman are odd, they don't init their own Eeprom...
  Roms on boths games are identical, and the Eeprom contains several settings the user isn't
  permitted to change, including region and button config..  the first 2 bytes must match the
  last first 2 bytes in the last block of 8 bytes, and the at 0x20 there must be 0088 or the
  game won't boot

  The second byte of Eeprom contains the following

  (Switch 0 in test mode)

   -bss llfr (byte 0x01 / 0x79 of eeprom)

  *- = unknown / unused  (no observed effect)
  *b = button config (2 buttons, or 3 buttons)
   s = number of special weapons (bombs)
   l = lives
   f = flip (screen rotation)
  *r = region


  other settings

  Switch 1  (byte 0x00 / 0x78 of eeprom)

  ppdd -ecs

  p = number of players (health bar?)
  d = difficulty
 *- = unknown / unused  (no observed effect)
  e = extend score
  c = continue
  s = demo sound

 Switch 2  (byte 0x7d of eeprom)

  cccC CCxb

  ccc = coin1
  CCC = coin 2
 *x = breaks attract mode / game..
  b = blood


  items marked * the user can't configure in test mode

  I don't know if any of the other bytes in the eeprom are tested / used.

  1 in eeprom is 0 in test mode

  Both games are currently configured to 3 buttons, its possible the game was never
  released with a 2 button configuration.

   NOTE: an actual read of the eeprom appears to be byteswapped vs. this data / the file
         MAME outputs

*/

static const UINT8 osman_eeprom[128] = {
	0xFF, 0xBE, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
	0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
	0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
	0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
	0x00, 0x88, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
	0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
	0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
	0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xBE, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF
};
static const UINT8 candance_eeprom[128] = {
	0xFF, 0xBF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
	0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
	0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
	0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
	0x00, 0x88, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
	0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
	0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
	0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xBF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF
};

/* Everything seems more stable if we run the CPU speed x4 and use Idle skips.. maybe it has an internal multipler? */
static READ32_HANDLER( joemacr_speedup_r )
{
	if (activecpu_get_pc()==0x284)  cpu_spinuntil_time(TIME_IN_USEC(400));
	return simpl156_systemram[0x18/4];
}

static DRIVER_INIT (joemacr)
{
	install_mem_read32_handler(0, 0x0201018, 0x020101b, joemacr_speedup_r );
	simpl156_common_init();
}

static READ32_HANDLER( chainrec_speedup_r )
{
	if (activecpu_get_pc()==0x2d4)  cpu_spinuntil_time(TIME_IN_USEC(400));
	return simpl156_systemram[0x18/4];
}

static DRIVER_INIT (chainrec)
{
	install_mem_read32_handler(0, 0x0201018, 0x020101b, chainrec_speedup_r );
	simpl156_common_init();
	simpl156_default_eeprom = chainrec_eeprom;
}


static READ32_HANDLER( charlien_speedup_r )
{
	if (activecpu_get_pc()==0xc8c8)  cpu_spinuntil_time(TIME_IN_USEC(400));
	return simpl156_systemram[0x10/4];
}

static DRIVER_INIT (charlien)
{
	install_mem_read32_handler(0, 0x0201010, 0x0201013, charlien_speedup_r );

	simpl156_default_eeprom = osman_eeprom;

	simpl156_common_init();
}

static READ32_HANDLER( osman_speedup_r )
{
	if (activecpu_get_pc()==0x5974)  cpu_spinuntil_time(TIME_IN_USEC(400));
	return simpl156_systemram[0x10/4];
}

static DRIVER_INIT (osman)
{
	install_mem_read32_handler(0, 0x0201010, 0x0201013, osman_speedup_r );

	simpl156_default_eeprom = osman_eeprom;

	simpl156_common_init();
}

static DRIVER_INIT (candance)
{
    install_mem_read32_handler(0, 0x0201010, 0x0201013, osman_speedup_r );

	simpl156_default_eeprom = candance_eeprom;

	simpl156_common_init();
}

/* Data East games running on the DE-0409-1 or DE-0491-1 PCB */
GAME( 1994, joemacr,  0,        joemacr,     simpl156, joemacr,  ROT0,  "Data East Corporation", "Joe & Mac Returns (World, Version 1.1)" ) /* bootleg board with genuine DECO parts */
GAME( 1994, joemacra, joemacr,  joemacr,     simpl156, joemacr,  ROT0,  "Data East Corporation", "Joe & Mac Returns (World, Version 1.0)" )
GAME( 1995, chainrec, 0,        chainrec,    simpl156, chainrec, ROT0,  "Data East Corporation", "Chain Reaction (World, Version 2.2)" )
GAME( 1995, magdrop,  chainrec, magdrop,     simpl156, chainrec, ROT0,  "Data East Corporation", "Magical Drop (Japan, Version 1.1)" )
GAME( 1995, magdropp, chainrec, magdropp,    simpl156, chainrec, ROT0,  "Data East Corporation", "Magical Drop Plus 1 (Japan, Version 2.1)" )

/* Mitchell games running on the DEC-22VO / MT5601-0 PCB */
GAME( 1995, charlien, 0,        mitchell156, simpl156, charlien, ROT0,  "Mitchell", "Charlie Ninja" ) /* language in service mode */
GAME( 1996, osman,    0,        mitchell156, simpl156, osman,    ROT0,  "Mitchell", "Osman (World)" )
GAME( 1996, candance, osman,    mitchell156, simpl156, candance, ROT0,  "Mitchell", "Cannon Dancer (Japan)" )
