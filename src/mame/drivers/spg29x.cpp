// license:BSD-3-Clause
// copyright-holders:Sandro Ronco
/***************************************************************************

        Mattel HyperScan

        08/17/2013 Skeleton driver by Sandro Ronco

        SPG290 Interrupt:

        Vector      Source
          63        SPU FIQ
          62        SPU Beatirq
          61        SPU Envirq
          60        CD servo
          59        ADC gain overflow / ADC recorder / FIFO overflow
          58        General purpose ADC
          57        Timer base
          56        Timer
          55        TV vblanking start
          54        LCD vblanking start
          53        PPU vblanking start
          52        TV
          51        Sensor frame end
          50        Sensor coordinate hit
          49        Sensor motion frame end
          48        Sensor capture done + sensor debug IRQ
          47        TV coordinate hit
          46        PPU coordinate hit
          45        USB host+device
          44        SIO
          43        SPI
          42        Uart (IrDA)
          41        NAND
          40        SD
          39        I2C master
          38        I2S slave
          37        APBDMA CH1
          36        APBDMA CH2
          35        LDM_DMA
          34        BLN_DMA
          33        APBDMA CH3
          32        APBDMA CH4
          31        Alarm + HMS
          30        MP4
          29        C3 (ECC module)
          28        GPIO
          27        Bufctl (for debug) + TV/PPU vblanking end (for debug)
          26        RESERVED1
          25        RESERVED2
          24        RESERVED3


-------

        CPU die markings on Big Buck Hunter "SunplusmM LU9001"


****************************************************************************/

#include "emu.h"
#include "cpu/score/score.h"
#include "screen.h"
#include "softlist_dev.h"

#define LOG_SPG290_REGISTER_ACCESS  (1)



class spg29x_game_state : public driver_device
{
public:
	spg29x_game_state(const machine_config &mconfig, device_type type, const char *tag) :
		driver_device(mconfig, type, tag),
		m_maincpu(*this, "maincpu")
	{ }

	void spg29x(machine_config &config);
	void hyperscan(machine_config &config);

protected:
	virtual void machine_reset() override;

	required_device<score7_cpu_device> m_maincpu;
private:

	virtual void machine_start() override;
	virtual void device_timer(emu_timer &timer, device_timer_id id, int param, void *ptr) override;

	uint32_t spg290_screen_update(screen_device &screen, bitmap_rgb32 &bitmap, const rectangle &cliprect);
	DECLARE_READ32_MEMBER(spg290_regs_r);
	DECLARE_WRITE32_MEMBER(spg290_regs_w);
	void spg290_timers_update();
	DECLARE_WRITE_LINE_MEMBER(spg290_vblank_irq);
	inline uint32_t spg290_read_mem(uint32_t offset);
	inline void spg290_write_mem(uint32_t offset, uint32_t data);
	void spg290_argb1555(bitmap_rgb32 &bitmap, const rectangle &cliprect, uint16_t posy, uint16_t posx, uint16_t argb);
	void spg290_rgb565(bitmap_rgb32 &bitmap, const rectangle &cliprect, uint16_t posy, uint16_t posx, uint16_t rgb, uint32_t transrgb);
	void spg290_blit_sprite(bitmap_rgb32 &bitmap, const rectangle &cliprect, uint32_t control, uint32_t attribute, uint32_t *palettes, uint32_t buf_start);
	void spg290_blit_bitmap(bitmap_rgb32 &bitmap, const rectangle &cliprect, uint32_t control, uint32_t attribute, int posy, int posx, uint32_t nptr, uint32_t buf_start, uint32_t transrgb);
	void spg290_blit_character(bitmap_rgb32 &bitmap, const rectangle &cliprect, uint32_t control, uint32_t attribute, int posy, int posx, uint32_t nptr, uint32_t buf_start, uint32_t transrgb);

	void spg290_mem(address_map &map);
	void spg290_bios_mem(address_map &map);

	static const device_timer_id TIMER_SPG290 = 0;
	static const device_timer_id TIMER_I2C = 1;

	struct spg290_miu
	{
		uint32_t  status;
	};

	struct spg290_ppu
	{
		uint32_t  control;
		uint32_t  irq_control;
		uint32_t  irq_status;
		uint32_t  sprite_max;
		uint32_t  sprite_buf_start;
		uint32_t  frame_buff[3];
		uint32_t  palettes[0x200];
		uint32_t  tx_hoffset[0x200];
		uint32_t  tx_hcomp[0x200];
		uint32_t  transrgb;

		struct ppu_spite
		{
			uint32_t  control;
			uint32_t  attribute;
		} sprites[0x200];

		struct ppu_tx
		{
			uint32_t  control;
			uint32_t  attribute;
			uint32_t  posx;
			uint32_t  posy;
			uint32_t  nptr;
			uint32_t  buf_start[3];
		} txs[3];
	};

	struct spg290_timer
	{
		uint32_t  control;
		uint32_t  control2;
		uint16_t  preload;
		uint16_t  counter;
	};

	struct spg290_i2c
	{
		uint32_t  config;
		uint32_t  irq_control;
		uint32_t  clock;
		uint8_t   count;
		uint32_t  id;
		uint32_t  port_addr;
		uint32_t  wdata;
		uint32_t  rdata;
	};

	spg290_miu      m_miu;
	spg290_ppu      m_ppu;
	spg290_timer    m_timers[6];
	spg290_i2c      m_i2c;
	emu_timer *     m_update_timer;
	emu_timer *     m_i2c_timer;


};

class spg29x_nand_game_state : public spg29x_game_state
{
public:
	spg29x_nand_game_state(const machine_config& mconfig, device_type type, const char* tag) :
		spg29x_game_state(mconfig, type, tag)
	{ }

	void nand_init(int blocksize, int blocksize_stripped);
	void nand_jak_bbh();
	void nand_jak_bbsf();

protected:
	void machine_reset() override;

private:
	std::vector<uint8_t> m_strippedrom;
	int m_firstvector;
};

class spg29x_zone3d_game_state : public spg29x_game_state
{
public:
	spg29x_zone3d_game_state(const machine_config& mconfig, device_type type, const char* tag) :
		spg29x_game_state(mconfig, type, tag)
	{ }

	void init_zone3d();

protected:
	void machine_reset() override;

private:
};






#if LOG_SPG290_REGISTER_ACCESS
static void log_spg290_regs(device_t *device,uint8_t module, uint16_t reg, uint32_t mem_mask, bool write, uint32_t data=0)
{
	static const char *const modules_name[] =
	{
		"CSI", "PPU", "JPG", "TV", "LCD", "SPU", "CD", "MIU", "APBDMA", "BUFCTL","IRQCTL", "GPUBUF", "LDMDMA",
		"BLNDMA", "TPGBUF", "AHBDEC", "GPIO", "SPI", "SIO", "I2C", "I2S", "UART", "TIMERS/RTC", "WDOG", "SD",
		"FLASH", "ADC", "USB device", "USB host", "Reserved", "Reserved", "Reserved", "SFTCFG", "CKG", "MP4",
		"MIU2", "ECC"
	};

	if (module < 0x25)
		device->logerror("SPG: %-10s", modules_name[module]);
	else
		device->logerror("SPG: mod 0x%02x  ", module);

	if (!write)
		device->logerror(" R 0x%04x", reg);
	else
		device->logerror(" W 0x%04x = 0x%08x", reg, data);

	if (mem_mask != 0xffffffffu)
		device->logerror(" (0x%08x)\n",  mem_mask);
	else
		device->logerror("\n");
}
#endif


READ32_MEMBER(spg29x_game_state::spg290_regs_r)
{
	uint32_t addr = offset << 2;
	uint32_t data = 0;

	if (addr == 0x010000)           // PPU Control
	{
		data = m_ppu.control;
	}
	else if (addr == 0x010084)      // PPU IRQ Status
	{
		data = m_ppu.irq_status;
	}
	if (addr == 0x07006c)           // MIU Status
	{
		data = m_miu.status;
	}
	else if ((addr & 0xff0fff) == 0x160000)   // Timer X Status
	{
		int idx = (offset>>10) & 0x0f;
		data = m_timers[idx].control;
	}
	else if(addr == 0x130024)       // I2C interrupt
	{
		data = m_i2c.irq_control;
	}
	else if(addr == 0x130034)       // I2C write data
	{
		data = m_i2c.wdata;
	}
	else if(addr == 0x130038)       // I2C data data
	{
		data = m_i2c.rdata;
	}

#if LOG_SPG290_REGISTER_ACCESS
	//else
	{
		if (!machine().side_effects_disabled())
			log_spg290_regs(this,(offset >> 14) & 0xff, (offset<<2) & 0xffff, mem_mask, false);
	}
#endif

	return data;
}

WRITE32_MEMBER(spg29x_game_state::spg290_regs_w)
{
	uint32_t addr = offset << 2;

	if (addr == 0x010000)               // PPU Control
	{
		COMBINE_DATA(&m_ppu.control);
	}
	else if (addr == 0x010008)          // PPU Max Sprites
	{
		COMBINE_DATA(&m_ppu.sprite_max);
	}
	else if (addr == 0x010010)          // PPU Max Sprites
	{
		COMBINE_DATA(&m_ppu.transrgb);
	}
	else if (addr == 0x0100d0)          // Sprites buffer start
	{
		COMBINE_DATA(&m_ppu.sprite_buf_start);
	}
	else if (addr == 0x010020 || addr == 0x01003c || addr == 0x010058)   // Text Layers x pos
	{
		int idx = (((offset>>3) & 3) | ((offset>>4) & 1)) - 1;
		COMBINE_DATA(&m_ppu.txs[idx].posx);
	}
	else if (addr == 0x010024 || addr == 0x010040 || addr == 0x01005c)   // Text Layers y pos
	{
		int idx = (((offset>>3) & 3) | ((offset>>4) & 1)) - 1;
		COMBINE_DATA(&m_ppu.txs[idx].posy);
	}
	else if (addr == 0x010028 || addr == 0x010044 || addr == 0x010060)   // Text Layers attribute
	{
		int idx = ((offset>>3) & 3) - 1;
		COMBINE_DATA(&m_ppu.txs[idx].attribute);
	}
	else if (addr == 0x01002c || addr == 0x010048 || addr == 0x010064)   // Text Layers control
	{
		int idx = ((offset>>3) & 3) - 1;
		COMBINE_DATA(&m_ppu.txs[idx].control);
	}
	else if (addr == 0x010030 || addr == 0x01004c || addr == 0x010068)   // Text Layers number ptr
	{
		int idx = ((offset>>3) & 3) - 1;
		COMBINE_DATA(&m_ppu.txs[idx].nptr);
	}
	else if (addr == 0x010080)          // PPU IRQ Control
	{
		COMBINE_DATA(&m_ppu.irq_control);
	}
	else if (addr == 0x010084)          // PPU IRQ ack
	{
		if (ACCESSING_BITS_0_7)
			m_ppu.irq_status &= ~data;
	}
	else if (addr >= 0x0100a0 && addr <= 0x0100a8)          // Tx1 buffer start
	{
		COMBINE_DATA(&m_ppu.txs[0].buf_start[offset & 3]);
	}
	else if (addr >= 0x0100ac && addr <= 0x0100b4)          // Tx2 buffer start
	{
		COMBINE_DATA(&m_ppu.txs[1].buf_start[(offset+1) & 3]);
	}
	else if (addr >= 0x0100b8 && addr <= 0x0100c0)          // Tx3 buffer starts
	{
		COMBINE_DATA(&m_ppu.txs[2].buf_start[(offset+2) & 3]);
	}
	else if ((addr & 0xfff000) == 0x011000)                 // Palettes
	{
		COMBINE_DATA(&m_ppu.palettes[offset & 0x01ff]);
	}
	else if ((addr & 0xfff000) == 0x012000)                 // Tx horizontal offset
	{
		COMBINE_DATA(&m_ppu.tx_hoffset[offset & 0x01ff]);
	}
	else if ((addr & 0xfff000) == 0x013000)                 // Tx horizontal compression
	{
		COMBINE_DATA(&m_ppu.tx_hcomp[offset & 0x01ff]);
	}
	else if ((addr & 0xfff000) == 0x014000)                 // Sprite Control Registers
	{
		int idx = (offset>>1) & 0x1ff;
		if (offset & 1)
			COMBINE_DATA(&m_ppu.sprites[idx].attribute);
		else
			COMBINE_DATA(&m_ppu.sprites[idx].control);
	}
	else if (addr == 0x07005c)                  // MIU Status
	{
		COMBINE_DATA(&m_miu.status);
	}
	else if ((addr & 0xff0fff) == 0x160000)     // Timer X Control 1
	{
		int idx = (offset>>10) & 0x07;
		COMBINE_DATA(&m_timers[idx].control);

		if (ACCESSING_BITS_24_31)
			m_timers[idx].control &= ~(data & 0x04000000);  // Timers IRQ ack
	}
	else if ((addr & 0xff0fff) == 0x160004)     // Timer X Control 2
	{
		int idx = (offset>>10) & 0x07;
		COMBINE_DATA(&m_timers[idx].control2);
	}
	else if ((addr & 0xff0fff) == 0x160008)     // Timer X Preload
	{
		int idx = (offset>>10) & 0x07;
		COMBINE_DATA(&m_timers[idx].preload);
	}
	else if (addr == 0x2100e4)                  // Timer Source Clock Selection
	{
		const auto timers_clk = XTAL(27'000'000) / ((data & 0xff) + 1);
		m_update_timer->adjust(attotime::from_hz(timers_clk), 0, attotime::from_hz(timers_clk));
	}
	else if(addr == 0x130020)                   // I2C configuration
	{
		COMBINE_DATA(&m_i2c.config);
	}
	else if(addr == 0x130024)                   // I2C interrupt
	{
		COMBINE_DATA(&m_i2c.irq_control);

		if (ACCESSING_BITS_0_7)
			m_i2c.irq_control &= ~(data & 0x00000001);  // I2C IRQ ack
	}
	else if(addr == 0x130028)                   // I2C clock setting
	{
		COMBINE_DATA(&m_i2c.clock);
		const auto i2c_clk = XTAL(27'000'000) / ((m_i2c.clock & 0x3ff) + 1);
		m_i2c_timer->adjust(attotime::from_hz(i2c_clk), 0, attotime::from_hz(i2c_clk));
	}
	else if(addr == 0x13002c)                   // I2C ID
	{
		COMBINE_DATA(&m_i2c.id);
	}
	else if(addr == 0x130030)                   // I2C port address
	{
		COMBINE_DATA(&m_i2c.port_addr);
	}
	else if(addr == 0x130034)                   // I2C write data
	{
		COMBINE_DATA(&m_i2c.wdata);
	}
	else if(addr == 0x130038)                   // I2C data data
	{
		COMBINE_DATA(&m_i2c.rdata);
	}
	else if(addr == 0x150000)                   // UART data
	{
		if (ACCESSING_BITS_0_7)
			printf("%c", data & 0xff);
	}

#if LOG_SPG290_REGISTER_ACCESS
	//else
	{
		if (!machine().side_effects_disabled())
			log_spg290_regs(this,(offset >> 14) & 0xff, (offset<<2) & 0xffff, mem_mask, true, data);
	}
#endif
}

inline uint32_t spg29x_game_state::spg290_read_mem(uint32_t offset)
{
	return m_maincpu->space(0).read_dword(offset);
}

inline void spg29x_game_state::spg290_write_mem(uint32_t offset, uint32_t data)
{
	return m_maincpu->space(0).write_dword(offset, data);
}

void spg29x_game_state::spg290_argb1555(bitmap_rgb32 &bitmap, const rectangle &cliprect, uint16_t posy, uint16_t posx, uint16_t argb)
{
	if (!(argb & 0x8000) && cliprect.contains(posx, posy))
	{
		rgb_t color = rgb_t(pal5bit(argb >> 10), pal5bit(argb >> 5), pal5bit(argb >> 0));
		bitmap.pix32(posy, posx) = color;
	}
}

void spg29x_game_state::spg290_rgb565(bitmap_rgb32 &bitmap, const rectangle &cliprect, uint16_t posy, uint16_t posx, uint16_t rgb, uint32_t transrgb)
{
	if ((!(transrgb & 0x10000) || (transrgb & 0xffff) != rgb) && cliprect.contains(posx, posy))
	{
		rgb_t color = rgb_t(pal5bit(rgb >> 11), pal6bit(rgb >> 5), pal5bit(rgb >> 0));
		bitmap.pix32(posy, posx) = color;
	}
}

void spg29x_game_state::spg290_blit_sprite(bitmap_rgb32 &bitmap, const rectangle &cliprect, uint32_t control, uint32_t attribute, uint32_t *palettes, uint32_t buf_start)
{
	uint32_t sprite_base = buf_start + ((control & 0xffff) << 8);
	uint16_t sprite_x    = (control >> 16) & 0x3ff;
	uint16_t sprite_y    = (attribute >> 16) & 0x3ff;
	uint8_t sprite_hsize = 8 << ((attribute >> 4) & 0x03);
	uint8_t sprite_vsize = 8 << ((attribute >> 6) & 0x03);
	uint8_t bit_pixel    = ((attribute & 3) + 1) << 1;
	uint8_t pixel_word   = 32 / bit_pixel;
	uint8_t word_line    = sprite_hsize / pixel_word;
	uint8_t sprite_flip  = (attribute >> 2) & 0x03;

	for (int y=0; y < sprite_vsize; y++)
		for (int x=0; x < word_line; x++)
		{
			uint32_t data = spg290_read_mem(sprite_base + (y * pixel_word + x) * 4);

			for (int b=0; b < pixel_word; b++)
			{
				uint16_t pen = ((data >> (b * bit_pixel)) & ((1 << bit_pixel) - 1));
				uint16_t posx;
				if (sprite_flip & 0x01)
					posx = sprite_x + (sprite_hsize - (x * word_line + b));
				else
					posx = sprite_x + x * (word_line) + b;

				uint16_t posy;
				if (sprite_flip & 0x02)
					posy = sprite_y + (sprite_vsize - y);
				else
					posy = sprite_y + y;

				spg290_argb1555(bitmap, cliprect, posy, posx, palettes[pen]);
			}
		}
}

void spg29x_game_state::spg290_blit_bitmap(bitmap_rgb32 &bitmap, const rectangle &cliprect, uint32_t control, uint32_t attribute, int posy, int posx, uint32_t nptr, uint32_t buf_start, uint32_t transrgb)
{
	for (int y=0; y<512; y++)
	{
		int line = (control & 0x04) ? 0 : y;
		uint32_t tx_start = spg290_read_mem(nptr + (posy + line) * 4);

		for (int x=0; x < 1024>>1; x++)
		{
			uint32_t data = spg290_read_mem(buf_start + (tx_start + posx) * 2 + x * 4);

			for (int b=0; b < 2; b++)
			{
				uint16_t posx = x * 2 + b;
				uint16_t posy = y;
				uint16_t pix = (data >> (b*16)) & 0xffff;

				if (control & 0x0080)
					spg290_argb1555(bitmap, cliprect, posy, posx, pix);
				if (control & 0x1000)
					spg290_rgb565(bitmap, cliprect, posy, posx, pix, transrgb);
			}
		}
	}
}

void spg29x_game_state::spg290_blit_character(bitmap_rgb32 &bitmap, const rectangle &cliprect, uint32_t control, uint32_t attribute, int posy, int posx, uint32_t nptr, uint32_t buf_start, uint32_t transrgb)
{
	// TODO
}


uint32_t spg29x_game_state::spg290_screen_update(screen_device &screen, bitmap_rgb32 &bitmap, const rectangle &cliprect)
{
	if (m_ppu.control & 0x1000)
	{
		for (int depth=0; depth<4; depth++)
		{
			// draw the bitmap/text layers
			for (int l=0; l<3; l++)
				if ((m_ppu.txs[l].control & 0x08) && ((m_ppu.txs[l].attribute >> 13) & 3) == depth)
				{
					if (m_ppu.txs[l].control & 0x01)
						spg290_blit_bitmap(bitmap, cliprect, m_ppu.txs[l].control, m_ppu.txs[l].attribute, m_ppu.txs[l].posy & 0x1ff, m_ppu.txs[l].posx & 0x1ff, m_ppu.txs[l].nptr, m_ppu.txs[l].buf_start[0], m_ppu.transrgb);
					else
						spg290_blit_character(bitmap, cliprect, m_ppu.txs[l].control, m_ppu.txs[l].attribute, m_ppu.txs[l].posy & 0x1ff, m_ppu.txs[l].posx & 0x1ff, m_ppu.txs[l].nptr, m_ppu.txs[l].buf_start[0], m_ppu.transrgb);
				}

			// draw the sprites
			for (int i=0; i<=(m_ppu.sprite_max & 0x1ff); i++)
			{
				if (((m_ppu.sprites[i].attribute >> 13) & 3) == depth)
					spg290_blit_sprite(bitmap, cliprect, m_ppu.sprites[i].control, m_ppu.sprites[i].attribute, m_ppu.palettes, m_ppu.sprite_buf_start);
			}
		}
	}
	else
	{
		bitmap.fill(rgb_t::black(), cliprect);
	}

	return 0;
}

void spg29x_game_state::spg290_timers_update()
{
	for(auto & elem : m_timers)
		if (elem.control & 0x80000000)
		{
			if (((elem.control2 >> 30) & 0x03) == 0x00)
			{
				if (elem.counter == 0xffff)
				{
					elem.counter = elem.preload;
					if (elem.control & 0x08000000)
					{
						elem.control |= 0x04000000;
						m_maincpu->set_input_line_and_vector(0, HOLD_LINE, 56); // SCORE7
					}
				}
				else
					elem.counter++;
			}
			else
			{
				// TODO: capture, comparison and PWM mode

			}
		}
}

WRITE_LINE_MEMBER(spg29x_game_state::spg290_vblank_irq)
{
	if (state && m_ppu.irq_control & 0x01)      // VBlanking Start IRQ
	{
		m_ppu.irq_status |= 0x01;
		m_maincpu->set_input_line_and_vector(0, HOLD_LINE, 53); // SCORE7
	}
	else if (!state && m_ppu.irq_control & 0x02) // VBlanking End IRQ
	{
		m_ppu.irq_status |= 0x02;
		m_maincpu->set_input_line_and_vector(0, HOLD_LINE, 53); // SCORE7
	}
}


void spg29x_game_state::device_timer(emu_timer &timer, device_timer_id id, int param, void *ptr)
{
	switch (id)
	{
	case TIMER_SPG290:
		spg290_timers_update();
		break;
	case TIMER_I2C:
		if ((m_i2c.config & 0x40) && (m_i2c.config & 0x01))
		{
			// TODO: replace with real I2C emulation
			m_i2c.rdata = 0;

			m_maincpu->set_input_line_and_vector(0, HOLD_LINE, 39); // SCORE7
		}
		break;
	}
}

void spg29x_game_state::spg290_mem(address_map &map)
{
	map.global_mask(0x1fffffff);
	map(0x00000000, 0x00ffffff).ram().mirror(0x07000000);
	map(0x08000000, 0x09ffffff).rw(FUNC(spg29x_game_state::spg290_regs_r), FUNC(spg29x_game_state::spg290_regs_w));
	map(0x0a000000, 0x0a003fff).ram();                         // internal SRAM
	map(0x0b000000, 0x0b007fff).rom().region("spg290", 0);  // internal ROM
}

void spg29x_game_state::spg290_bios_mem(address_map& map)
{
	spg290_mem(map);
	map(0x10000000, 0x100fffff).rom().region("bios", 0).mirror(0x0e000000);
	map(0x11000000, 0x110fffff).rom().region("bios", 0).mirror(0x0e000000);
}

/* Input ports */
static INPUT_PORTS_START( hyperscan )
INPUT_PORTS_END


void spg29x_game_state::machine_start()
{
	m_update_timer = timer_alloc(TIMER_SPG290);
	m_i2c_timer = timer_alloc(TIMER_I2C);
}

void spg29x_game_state::machine_reset()
{
	memset(&m_ppu, 0, sizeof(spg290_ppu));
	memset(&m_miu, 0, sizeof(spg290_miu));
	memset(&m_i2c, 0, sizeof(m_i2c));
	m_i2c_timer->adjust(attotime::never, 0, attotime::never);

	// disable JTAG
	m_maincpu->set_state_int(SCORE_CR + 29, 0x20000000);
}

void spg29x_nand_game_state::machine_reset()
{
	spg29x_game_state::machine_reset();

	uint32_t bootstrap_ram_start = (m_strippedrom[m_firstvector+0] << 0) | (m_strippedrom[m_firstvector+1] << 8) | (m_strippedrom[m_firstvector+2] << 16) | (m_strippedrom[m_firstvector+3] << 24);
	uint32_t bootstrap_ram_end   = (m_strippedrom[m_firstvector+4] << 0) | (m_strippedrom[m_firstvector+5] << 8) | (m_strippedrom[m_firstvector+6] << 16) | (m_strippedrom[m_firstvector+7] << 24);
	uint32_t bootstrap_ram_boot  = (m_strippedrom[m_firstvector+8] << 0) | (m_strippedrom[m_firstvector+9] << 8) | (m_strippedrom[m_firstvector+10] << 16) | (m_strippedrom[m_firstvector+11] << 24);

	// there is a 0x01 at 0x26, possibly related to source location / block in NAND to copy from?

	logerror("NAND Bootstrap RAM start: %08x RAM end: %08x RAM boot: %08x", bootstrap_ram_start, bootstrap_ram_end, bootstrap_ram_boot);

	uint32_t sourceaddr = 0x10000;
	for (uint32_t addr = bootstrap_ram_start; addr <= bootstrap_ram_end; addr++)
	{
		address_space& mem = m_maincpu->space(AS_PROGRAM);
		uint8_t byte = m_strippedrom[sourceaddr];
		mem.write_byte(addr, byte);
		sourceaddr++;
	}

	// probably jumped to from internal ROM?
	m_maincpu->set_state_int(SCORE_PC, bootstrap_ram_boot);
}


void spg29x_zone3d_game_state::machine_reset()
{
	spg29x_game_state::machine_reset();

	uint8_t* rom = memregion("spi")->base();
	int size = memregion("spi")->bytes();

	uint32_t destaddr = 0x1dc;
	for (uint32_t addr = 0; addr < size; addr++)
	{
		address_space& mem = m_maincpu->space(AS_PROGRAM);
		uint8_t byte = rom[addr];
		mem.write_byte(addr+destaddr, byte);
	}

	m_maincpu->set_state_int(SCORE_PC, 0x1000);
}


void spg29x_game_state::spg29x(machine_config &config)
{
	/* basic machine hardware */
	SCORE7(config, m_maincpu, XTAL(27'000'000) * 4);   // 108MHz S+core 7
	m_maincpu->set_addrmap(AS_PROGRAM, &spg29x_game_state::spg290_mem);

	SOFTWARE_LIST(config, "cd_list").set_original("hyperscan");

	/* video hardware */
	screen_device &screen(SCREEN(config, "screen", SCREEN_TYPE_RASTER));
	screen.set_refresh_hz(50);
	screen.set_vblank_time(ATTOSECONDS_IN_USEC(2500)); /* not accurate */
	screen.set_screen_update(FUNC(spg29x_game_state::spg290_screen_update));
	screen.set_size(640, 480);
	screen.set_visarea(0, 640-1, 0, 480-1);
	screen.screen_vblank().set(FUNC(spg29x_game_state::spg290_vblank_irq));
}

void spg29x_game_state::hyperscan(machine_config &config)
{
	spg29x(config);
	m_maincpu->set_addrmap(AS_PROGRAM, &spg29x_game_state::spg290_bios_mem);
}

void spg29x_nand_game_state::nand_init(int blocksize, int blocksize_stripped)
{
	uint8_t* rom = memregion("nand")->base();
	int size = memregion("nand")->bytes();

	int numblocks = size / blocksize;

	m_strippedrom.resize(numblocks * blocksize_stripped);

	for (int i = 0; i < numblocks; i++)
	{
		const int base = i * blocksize;
		const int basestripped = i * blocksize_stripped;

		for (int j = 0; j < blocksize_stripped; j++)
		{
			m_strippedrom[basestripped + j] = rom[base + j];
		}
	}

	// debug to allow for easy use of unidasm.exe
	if (0)
	{
		FILE *fp;
		char filename[256];
		sprintf(filename,"stripped_%s", machine().system().name);
		fp=fopen(filename, "w+b");
		if (fp)
		{
			fwrite(&m_strippedrom[0], blocksize_stripped * numblocks, 1, fp);
			fclose(fp);
		}
	}
}

void spg29x_nand_game_state::nand_jak_bbh()
{
	nand_init(0x210, 0x200);
	m_firstvector = 0xc;
}

void spg29x_nand_game_state::nand_jak_bbsf()
{
	nand_init(0x210, 0x200);
	m_firstvector = 0x8;
}

void spg29x_zone3d_game_state::init_zone3d()
{

}


/* ROM definition */
ROM_START( hyprscan )
	ROM_REGION( 0x100000, "bios", ROMREGION_32BIT | ROMREGION_LE )
	ROM_LOAD32_DWORD("hyperscan.bin", 0x000000, 0x100000, CRC(ce346a14) SHA1(560cb747e7193e6781d4b8b0bd4d7b45d3d28690))

	ROM_REGION( 0x008000, "spg290", ROMREGION_32BIT | ROMREGION_LE )
	ROM_LOAD32_DWORD("spg290.bin", 0x000000, 0x008000, NO_DUMP)     // 256Kbit SPG290 internal ROM
ROM_END


ROM_START( jak_bbh )
	ROM_REGION( 0x4200000, "nand", 0 ) // ID returned C25A, read as what appears to be a compatible type.
	ROM_LOAD("bigbuckhunterpro_as_hy27us0812a_c25a.bin", 0x000000, 0x4200000, CRC(e2627540) SHA1(c8c6e5fbc4084fa695390bbb4e1e52e671f050da) )

	ROM_REGION( 0x008000, "spg290", ROMREGION_32BIT | ROMREGION_LE )
	ROM_LOAD32_DWORD("internal.rom", 0x000000, 0x008000, NO_DUMP)
ROM_END


ROM_START( jak_bbsf )
	ROM_REGION( 0x4200000, "nand", 0 )
	ROM_LOAD("bigbucksafari.bin", 0x000000, 0x4200000, CRC(dc5f9bf1) SHA1(27893c396d62f353ced52ef88fd9ade5c051598f) )

	ROM_REGION( 0x008000, "spg290", ROMREGION_32BIT | ROMREGION_LE )
	ROM_LOAD32_DWORD("internal.rom", 0x000000, 0x008000, NO_DUMP)
ROM_END

ROM_START( zone3d )
	ROM_REGION( 0x100000, "spi", 0 )
	ROM_LOAD("zone_25l8006e_c22014.bin", 0x000000, 0x100000, CRC(8c571771) SHA1(cdb46850286d31bf58d45b75ffc396ed774ac4fd) )

	/*
	model: Lexar SD
	revision: LX01
	serial number: 00000000XL10

	size: 362.00 MiB (741376 sectors * 512 bytes)
	unk1: 0000000000000007
	unk2: 00000000000000fa
	unk3: 01
	
	The SD card has no label, but there's some printing on the back:
	MMAGF0380M3085-WY
	TC00201106 by Taiwan

	--
	Dumped with hardware write blocker, so this image is correct, and hasn't been corrupted by Windows
	
	Image contains a FAT filesystem with a number of compressed? programs that presumably get loaded into RAM by
	the bootloader in the serial flash ROM
	*/

	DISK_REGION( "cfcard" )
	DISK_IMAGE( "zone3d", 0, SHA1(77971e2dbfb2ceac12f482d72539c2e042fd9108) )
	
	ROM_REGION( 0x008000, "spg290", ROMREGION_32BIT | ROMREGION_LE )
	ROM_LOAD32_DWORD("internal.rom", 0x000000, 0x008000, NO_DUMP)
ROM_END


/* Driver */

//    YEAR  NAME  PARENT  COMPAT  MACHINE    INPUT      CLASS            INIT        COMPANY   FULLNAME     FLAGS
COMP( 2006, hyprscan,   0,      0,      hyperscan, hyperscan, spg29x_game_state, empty_init, "Mattel", "HyperScan", MACHINE_NOT_WORKING | MACHINE_NO_SOUND )

// There were 1 player and 2 player versions for these JAKKS guns.  The 2nd gun appears to be simply a controller (no AV connectors) but as they were separate products with the 2 player verisons being released up to a year after the original, the code could differ.
// If they differ, it is currently uncertain which versions these ROMs are from
COMP( 2009, jak_bbh,    0,      0,      spg29x, hyperscan, spg29x_nand_game_state, nand_jak_bbh, "JAKKS Pacific Inc", "Big Buck Hunter Pro (JAKKS Pacific TV Game)", MACHINE_NOT_WORKING | MACHINE_NO_SOUND ) //has ISSI 404A (24C04)
COMP( 2011, jak_bbsf,   0,      0,      spg29x, hyperscan, spg29x_nand_game_state, nand_jak_bbsf,"JAKKS Pacific Inc", "Big Buck Safari (JAKKS Pacific TV Game)", MACHINE_NOT_WORKING | MACHINE_NO_SOUND ) // has ISSI 416A (24C16)

// ends up doing the fllowing, which causes a jump to 0xbf000024, where we have nothing mapped (internal ROM related, or thinks it's loaded code there?  This is the area Hyperscan uses as 'BIOS' not Internal ROM so could be RAM here)
// 000011D4: ldis r8, 0xbf00
// 000011D8: ori r8, 0x0024
// 000011DC: br r8
COMP( 201?, zone3d,    0,      0,      spg29x, hyperscan, spg29x_zone3d_game_state, init_zone3d,"Zone", "Zone 3D", MACHINE_NOT_WORKING | MACHINE_NO_SOUND )

