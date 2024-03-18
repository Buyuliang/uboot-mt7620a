/*
 * (C) Copyright 2014
 * Martin Liu, martin.zy.liu@gmail.com
 *
 * (C) Copyright 2003
 * Kyle Harris, kharris@nexus-tech.net
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
#include "msdc.h"

#define DEBUG

static int curr_device = -1;

enum mmc_state {
	MMC_INVALID,
	MMC_READ,
	MMC_WRITE,
	MMC_ERASE,
};
static void print_mmcinfo(struct mmc_card *mmc)
{
	printf("Device: %s\n",  mmc_card_name(mmc));
	printf("Manufacturer ID: %x\n", mmc->raw_cid[0] >> 24);
	printf("OEM: %x\n", (mmc->raw_cid[0] >> 8) & 0xffff);
	printf("Name: %c%c%c%c%c \n", mmc->raw_cid[0] & 0xff,
			(mmc->raw_cid[1] >> 24), (mmc->raw_cid[1] >> 16) & 0xff,
			(mmc->raw_cid[1] >> 8) & 0xff, mmc->raw_cid[1] & 0xff);

	// printf("Tran Speed: %d\n", mmc->tran_speed);
	// printf("Rd Block Len: %d\n", mmc->read_bl_len);

	// printf("%s version %d.%d\n", IS_SD(mmc) ? "SD" : "MMC",
	// 		(mmc->version >> 4) & 0xf, mmc->version & 0xf);

	// printf("High Capacity: %s\n", mmc->high_capacity ? "Yes" : "No");
	// puts("Capacity: ");
	// print_size(mmc->capacity, "\n");

	// printf("Bus Width: %d-bit\n", mmc->bus_width);
}

int do_mmcinfo(cmd_tbl_t *cmdtp, int flag, int argc, char * const argv[])
{
	struct mmc_card *mmc;
    MSDC_CLR_BIT32(0x10000060, 0x1 << 19);
    MSDC_SET_BIT32(0x10000060, 0x1 << 18);
	if (curr_device < 0) {
		if (get_mmc_num() > 0)
			curr_device = 0;
		else {
			puts("No MMC device available\n");
			return 1;
		}
	}

	if (0 != mmc_init(curr_device)) {
		puts("MMC device initial failed\n");
		return 1;	
	}
	
	mmc = mmc_get_card(curr_device);

	if (mmc) {
		print_mmcinfo(mmc);
		return 0;
	} else {
		printf("no mmc device at slot %x\n", curr_device);
		return 1;
	}
}

U_BOOT_CMD(
	mmcinfo, 1, 0, do_mmcinfo,
	"mmcinfo - display MMC info\n",
	"    - device number of the device to dislay info of\n"
	""
);

int cmd_usage(cmd_tbl_t *cmdtp)
{
	printf ("Usage:\n%s\n", cmdtp->usage);
	return 1;
}

int do_mmcops(cmd_tbl_t *cmdtp, int flag, int argc, char * const argv[])
{
	enum mmc_state state;

	if (argc < 2)
		return cmd_usage(cmdtp);
	MSDC_CLR_BIT32(0x10000060, 0x1 << 19);
	MSDC_SET_BIT32(0x10000060, 0x1 << 18);
	if (curr_device < 0) {
		if (get_mmc_num() > 0)
			curr_device = 0;
		else {
			puts("No MMC device available\n");
			return 1;
		}
	}

	if (strcmp(argv[1], "rescan") == 0) {
		struct mmc_card *mmc = mmc_get_card(curr_device);

		if (!mmc) {
			printf("no mmc device at slot %x\n", curr_device);
			return 1;
		}

		mmc->has_init = 0;

		if (mmc_init(curr_device))
			return 1;
		else
			return 0;
	// }
	// else if (strncmp(argv[1], "part", 4) == 0) {
	// 	block_dev_desc_t *mmc_dev;
	// 	struct mmc_card *mmc = mmc_get_card(curr_device);

	// 	if (!mmc) {
	// 		printf("no mmc device at slot %x\n", curr_device);
	// 		return 1;
	// 	}
	// 	mmc_init(curr_device);

 //        /* set freq */
 //        mmc_set_clock(mmc->host, mmc_card_ddr(mmc), MSDC_MAX_SCLK);

 //        /* set buswidth */
 //        if (mmc_set_bus_width(mmc->host, mmc, HOST_BUS_WIDTH_4) != 0) {
 //            debug("%s %d", __FILE__, __LINE__);
 //            return 0;
 //        }

	// 	mmc_dev = mmc_get_dev(curr_device);
	// 	if (mmc_dev != NULL &&
	// 			mmc_dev->type != DEV_TYPE_UNKNOWN) {
	// 		debug("if_type: %d\n", mmc_dev->if_type);
	// 		debug("device number: %d\n", mmc_dev->dev);
	// 		debug("blksz: %d\n", mmc_dev->blksz);
	// 		debug("vendor: %s\n", mmc_dev->vendor);
	// 		debug("product: %s\n", mmc_dev->product);
	// 		debug("revision: %s\n", mmc_dev->revision);
	// 		debug("part_type: %d\n", mmc_dev->part_type );
	// 		debug("removable: %d\n", mmc_dev->removable);

	// 		print_part(mmc_dev);
	// 		return 0;
	// 	}

	// 	puts("get mmc type error!\n");
	// 	return 1;
	// } else if (strcmp(argv[1], "list") == 0) {
	// 	print_mmc_devices('\n');
	// 	return 0;
	} else if (strcmp(argv[1], "dev") == 0) {
		int dev, part = -1;
		struct mmc_card *mmc;

		if (argc == 2)
			dev = curr_device;
		else if (argc == 3)
			dev = simple_strtoul(argv[2], NULL, 10);
		else if (argc == 4) {
			dev = (int)simple_strtoul(argv[2], NULL, 10);
			part = (int)simple_strtoul(argv[3], NULL, 10);
			if (part > EXT_CSD_PART_ACCESS_MASK) {
				printf("#part_num shouldn't be larger"
					" than %d\n", EXT_CSD_PART_ACCESS_MASK);
				return 1;
			}
		} else
			return cmd_usage(cmdtp);

		mmc = mmc_get_card(dev);
		if (!mmc) {
			printf("no mmc device at slot %x\n", dev);
			return 1;
		}

		mmc_init(dev);
		// if (part != -1) {
		// 	int ret;
		// 	if (mmc->part_config == MMCPART_NOAVAILABLE) {
		// 		printf("Card doesn't support part_switch\n");
		// 		return 1;
		// 	}

		// 	if (part != mmc->part_num) {
		// 		ret = mmc_switch_part(dev, part);
		// 		if (!ret)
		// 			mmc->part_num = part;

		// 		printf("switch to partions #%d, %s\n",
		// 				part, (!ret) ? "OK" : "ERROR");
		// 	}
		// }
		curr_device = dev;
		// if (mmc->raw_ext_csd[EXT_CSD_PART_CFG] == EXT_CSD_PART_CFG_NOAVAILABLE)
			printf("mmc%d is current device\n", curr_device);
		// else
		// 	printf("mmc%d(part %d) is current device\n",
		// 		curr_device, mmc->part_num);

		return 0;
	}

	if (strcmp(argv[1], "read") == 0)
		state = MMC_READ;
	else if (strcmp(argv[1], "write") == 0)
		state = MMC_WRITE;
	else if (strcmp(argv[1], "erase") == 0)
		state = MMC_ERASE;
	else
		state = MMC_INVALID;

	if (state != MMC_INVALID) {
		struct mmc_card *mmc = mmc_get_card(curr_device);
		int idx = 2;
		u32 blk, cnt, n, err;
		void *addr;

		if (state != MMC_ERASE) {
			addr = (void *)simple_strtoul(argv[idx], NULL, 16);
			++idx;
		} else
			addr = 0;
		blk = simple_strtoul(argv[idx], NULL, 16);
		cnt = simple_strtoul(argv[idx + 1], NULL, 16);

		if (!mmc) {
			printf("no mmc device at slot %x\n", curr_device);
			return 1;
		}

		debug("[SD%d] %s: %d, %d ... \n",
				curr_device, argv[1] , blk, cnt);

		err = mmc_init(curr_device);
		if (err != MMC_ERR_NONE) {
			printf("[SD%d] Init failed with error code: %d", err);
		}

        /* set freq */
        // mmc_set_clock(mmc->host, mmc_card_ddr(mmc), MSDC_MAX_SCLK);

        /* set buswidth */
        if (mmc_set_bus_width(mmc->host, mmc, HOST_BUS_WIDTH_4) != 0) {
            debug("%s %d", __FILE__, __LINE__);
            return 0;
        }

        /* set mode */
        msdc_set_dmode(mmc->host, MSDC_MODE_PIO);

		switch (state) {
		case MMC_READ:

			// n = mmc->host->blk_read(curr_device, blk,
			// 			      cnt, addr);
			// int (*blk_read)(struct mmc_host *host, uchar *dst, ulong src, ulong nblks);
			n = mmc->host->blk_read(mmc->host, addr, blk, cnt);

			/* flush cache after read */
			flush_cache((ulong)addr, cnt * 512); /* FIXME */
			break;
		case MMC_WRITE:
		// int (*blk_write)(struct mmc_host *host, ulong dst, uchar *src, ulong nblks);
			n = mmc->host->blk_write(mmc->host, blk,
						      addr, cnt);
			break;
		// case MMC_ERASE:
		// 	n = mmc->host.block_erase(curr_device, blk, cnt);
		// 	break;
		default:
			return cmd_usage(cmdtp);
		}

	    MSG(OPS, "[SD%d] %s addr=0x%x, blk=%d, n=%d, cnt=%d\n", mmc->host->id, __FUNCTION__, 
	    	addr, blk, n, cnt );

	    if (0) {
			printf("[SD%d] %d blocks %s: %s\n", mmc->host->id,
					n, argv[1], (n == cnt) ? "OK" : "ERROR");
			return (n == cnt) ? 0 : 1;
		}

		printf("[SD%d] %d blocks %s: %s\n", mmc->host->id,
				cnt, argv[1], (n == MMC_ERR_NONE) ? "OK" : "ERROR");
		return n;
	}

	return cmd_usage(cmdtp);
}

U_BOOT_CMD(
	mmc, 6, 1, do_mmcops,
	"mmc 	- MMC sub system\n",
	"mmc read addr blk# cnt\n"
	"mmc write addr blk# cnt\n"
	"mmc erase blk# cnt\n"
	"mmc rescan\n"
	"mmc part - lists available partition on current mmc device\n"
	"mmc dev [dev] [part] - show or set current mmc device [partition]\n"
	"mmc list - lists available devices");
