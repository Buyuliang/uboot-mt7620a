/*
 * (C) Copyright 2014
 * Robin He, gzhechu@gmail.com
 *
 *
 * See file CREDITS for list of people who contributed to this
 * project.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston,
 * MA 02111-1307 USA
 */

#include <common.h>
#include <command.h>
#include <rt_mmap.h>
#include <linux/types.h>
#include "cmd_gpio.h"

u32 FB_OFFSET_DATA 	= GPIO23_00_DATA;
u32 FB_OFFSET_DIR 	= GPIO23_00_DIR;
u32 FB_OFFSET_PAL 	= GPIO23_00_POL;
u32 FB_BIT_BASE_OFF	= 0;

// #define CMD_GPIO_DEBUG
#if 1
#define dprint(f, a...) 	printf(f, ##a)
#else
#define dprint(f, a...)	do {} while (0)
#endif

void pb(u32 x)
{
    u32 z;
    int y = 0;
    for (z = 0x80000000; z > 0; z >>= 1)
    {
        if ((x & z) == z) dprint("1");
	   	else dprint("0");
	   	if (0 == ++y%8) dprint(" ");
    }
	dprint("\n");
}

static inline void rt_gpio_w32(struct ralink_gpio_chip *rg, u8 reg, u32 val)
{
	// iowrite32(val, rg->membase + rg->regs[reg]);
	// dprint("rt_gpio_w32 rg->mem_offset: %d\n", rg->mem_offset);
	// dprint("rt_gpio_w32 rg->gpio_base: %d\n", rg->gpio_base);
	// dprint("rt_gpio_w32 rg->num_gpio: %d\n", rg->num_gpio);
	// dprint("rt_gpio_w32 reg: %08x %08x\n", reg, rg->regs[reg]);
	// dprint("rt_gpio_w32 val: %08x ", val); pb(val); 
	// dprint("rt_gpio_w32 write to: %08x\n", RALINK_PIO_BASE + rg->mem_offset + rg->regs[reg]);
	ra_outl(RALINK_PIO_BASE + rg->mem_offset + rg->regs[reg], val);
}

static inline u32 rt_gpio_r32(struct ralink_gpio_chip *rg, u8 reg)
{
	// return ioread32(rg->membase + rg->regs[reg]);
	// dprint("rt_gpio_r32 rg->mem_offset: %d\n", rg->mem_offset);
	// dprint("rt_gpio_r32 rg->gpio_base: %d\n", rg->gpio_base);
	// dprint("rt_gpio_r32 rg->num_gpio: %d\n", rg->num_gpio);
	// dprint("rt_gpio_r32 reg: %08x %08x\n", reg, rg->regs[reg]);
	// u32 val = ra_inl(RALINK_PIO_BASE + rg->mem_offset + rg->regs[reg]);
	// dprint("rt_gpio_r32 val: %08x ", val); pb(val); 
	// dprint("rt_gpio_r32 read from: %08x\n", RALINK_PIO_BASE + rg->mem_offset + rg->regs[reg]);
	return (ra_inl(RALINK_PIO_BASE + rg->mem_offset + rg->regs[reg]));
}

static void ralink_gpio_set(struct ralink_gpio_chip *rg, u32 offset, int value)
{
	offset = offset - rg->gpio_base;
	rt_gpio_w32(rg, (value) ? GPIO_REG_SET : GPIO_REG_RESET, BIT(offset));
}

static int ralink_gpio_get(struct ralink_gpio_chip *rg, u32 offset)
{
	offset = offset - rg->gpio_base;
	return !!(rt_gpio_r32(rg, GPIO_REG_DATA) & BIT(offset));
}

static int ralink_gpio_direction_input(struct ralink_gpio_chip *rg, unsigned offset)
{
	// unsigned long flags;
	u32 t;
	offset = offset - rg->gpio_base;
	// spin_lock_irqsave(&rg->lock, flags);
	t = rt_gpio_r32(rg, GPIO_REG_DIR);
	t &= ~BIT(offset);
	rt_gpio_w32(rg, GPIO_REG_DIR, t);
	// spin_unlock_irqrestore(&rg->lock, flags);

	return 0;
}

static int ralink_gpio_direction_output(struct ralink_gpio_chip *rg,
					unsigned offset)
{
	// unsigned long flags;
	u32 t;
	offset = offset - rg->gpio_base;
	// spin_lock_irqsave(&rg->lock, flags);
	// ralink_gpio_set(rg, offset, value);
	t = rt_gpio_r32(rg, GPIO_REG_DIR);
	t |= BIT(offset);
	rt_gpio_w32(rg, GPIO_REG_DIR, t);
	// spin_unlock_irqrestore(&rg->lock, flags);

	return 0;
}

u32 ralink_gpio_get_direction(struct ralink_gpio_chip *rg, u32 offset)
{
	offset = offset - rg->gpio_base;
	return !!(rt_gpio_r32(rg, GPIO_REG_DIR) & BIT(offset));
}

void ralink_gpio_info(struct ralink_gpio_chip *rg, u32 gpio)
{
	u32 offset = gpio - rg->gpio_base;
	u32 gpio_mode, pol, data, dir, edge, rmask, fmask;
	gpio_mode = ra_inl(RALINK_SYSCTL_BASE + 0x60);
	data = ra_inl(RALINK_PIO_BASE + rg->mem_offset + rg->regs[GPIO_REG_DATA]);
	dir = ra_inl(RALINK_PIO_BASE + rg->mem_offset + rg->regs[GPIO_REG_DIR]);
	pol = ra_inl(RALINK_PIO_BASE + rg->mem_offset + rg->regs[GPIO_REG_POL]);
	edge = ra_inl(RALINK_PIO_BASE + rg->mem_offset + rg->regs[GPIO_REG_EDGE]);
	rmask = ra_inl(RALINK_PIO_BASE + rg->mem_offset + rg->regs[GPIO_REG_RENA]);
	fmask = ra_inl(RALINK_PIO_BASE + rg->mem_offset + rg->regs[GPIO_REG_FENA]);
	// dprint("offset:\t %08x\n", rg->mem_offset);
	// dprint("base:\t %08d\n", rg->gpio_base);
	// dprint("ngpio:\t %08d\n", rg->num_gpio);	
#ifdef CMD_GPIO_DEBUG
	dprint("item \t address  value    31----24 23----16 FEDCBA98 76543210 \n");	
	dprint("mode:\t %08x %08x ", RALINK_SYSCTL_BASE + 0x60, gpio_mode);	
	pb(gpio_mode);
	dprint("edge:\t %08x %08x ", RALINK_PIO_BASE + rg->mem_offset + rg->regs[GPIO_REG_EDGE], edge);
	pb(edge); 
	dprint("rmask:\t %08x %08x ", RALINK_PIO_BASE + rg->mem_offset + rg->regs[GPIO_REG_RENA], rmask);
	pb(rmask); 
	dprint("fmask:\t %08x %08x ", RALINK_PIO_BASE + rg->mem_offset + rg->regs[GPIO_REG_FENA], fmask);
	pb(fmask); 
	dprint("data:\t %08x %08x ", RALINK_PIO_BASE + rg->mem_offset + rg->regs[GPIO_REG_DATA], data);
	pb(data); 
	dprint("dir:\t %08x %08x ", RALINK_PIO_BASE + rg->mem_offset + rg->regs[GPIO_REG_DIR], dir);
	pb(dir); 
	dprint("pol:\t %08x %08x ", RALINK_PIO_BASE + rg->mem_offset + rg->regs[GPIO_REG_POL], pol);
	pb(pol); 
#endif
	dprint("gpio: %d, ", gpio);
	dprint("offset: %d, ", offset);	
	dprint("value: %d, ", (data >> offset) & 1);
	if ((dir >> offset) & 1)
		dprint("direction: output, ");
	else
		dprint("direction: input, ");
	dprint("polarity: %d \t", (pol >> offset) & 1);
	dprint("\n");
}

int do_gpio(cmd_tbl_t *cmdtp, int flag, int argc, char * const argv[])
{
	if (argc < 2)
		return cmd_usage(cmdtp);

	u32 ret = -1;
	u32 gpio;
	gpio = simple_strtoul(argv[1], NULL, 16);

	struct ralink_gpio_chip rg0 = {0x00, 00, 24, 
		{0x00, 0x04, 0x08, 0x0c, 0x20, 0x24, 0x28, 0x2c, 0x30, 0x34}};
	struct ralink_gpio_chip rg1 = {0x38, 24, 16,
		{0x00, 0x04, 0x08, 0x0c, 0x10, 0x14, 0x18, 0x1c, 0x20, 0x24}};
	struct ralink_gpio_chip rg2 = {0x60, 40, 32,
		{0x00, 0x04, 0x08, 0x0c, 0x10, 0x14, 0x18, 0x1c, 0x20, 0x24}};
	struct ralink_gpio_chip rg3 = {0x88, 72, 1,
		{0x00, 0x04, 0x08, 0x0c, 0x10, 0x14, 0x18, 0x1c, 0x20, 0x24}};
	struct ralink_gpio_chip *rg;
	if (gpio > 72) 
	{
		return cmd_usage(cmdtp);
	}
	else if (gpio == 72) 
	{
		rg = &rg3;
	}
	else if (gpio >= 40)
	{
		rg = &rg2;
	}
	else if (gpio >= 24)
	{
		rg = &rg1;
	}
	else if (gpio >= 0) 
	{
		rg = &rg0;
	}
	else
		return cmd_usage(cmdtp);

	ra_or(RALINK_SYSCTL_BASE + 0x60, BIT(9));
	ra_or(RALINK_SYSCTL_BASE + 0x60, BIT(10));

	u32 value, dir;
	if (strcmp(argv[2], "info") == 0)
		ralink_gpio_info(rg, gpio);
	else if (strcmp(argv[2], "value") == 0)
	{
		if (3 == argc)
		{
			ret = ralink_gpio_get(rg, gpio);
		}
		else
		{
			value = simple_strtoul(argv[3], NULL, 16);
			ralink_gpio_set(rg, gpio, value);
		}
	}
	else if (strcmp(argv[2], "dir") == 0)
	{
		if (3 == argc)
			ret = ralink_gpio_get_direction(rg, gpio);
		else
		{
			dir = simple_strtoul(argv[3], NULL, 16);
			if (dir > 0)
				ralink_gpio_direction_output(rg, gpio);
			else
				ralink_gpio_direction_input(rg, gpio);
		}
	}
	ralink_gpio_info(rg, gpio);
	return ret;
}

U_BOOT_CMD(
	gpio, 6, 1, do_gpio,
	"gpio 	- GPIO sub system\n",
	"gpio get/set gpio data\n"
	"gpio format:\n"
	"  gpio <gpio> value [value] -- get/set gpio value 1:high 0:low\n"
	"  gpio <gpio> dir [dir] -- get/set gpio direction 1:out 0:in\n"
	"  gpio <gpio> info -- get info of gpio\n"
	"  NOTE -- <gpio> should between 0-72, [dir] and [value] could be 0/1\n");
