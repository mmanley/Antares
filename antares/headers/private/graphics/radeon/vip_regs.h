/*
	Copyright (c) 2002, Thomas Kurschel


	Part of Radeon driver
		
	VIP host port registers
*/

#ifndef _VIP_REGS_H
#define _VIP_REGS_H

#define	RADEON_VIPH_TIMEOUT_STAT						0xc50
#		define RADEON_VIPH_TIMEOUT_STAT_VIPH_REG_STAT	(1 << 4)
#		define RADEON_VIPH_TIMEOUT_STAT_VIPH_REG_AK		(1 << 4)
#		define RADEON_VIPH_TIMEOUT_STAT_AK_MASK			0xff
#		define RADEON_VIPH_TIMEOUT_STAT_VIPH_REGR_DIS	(1 << 24)

#define	RADEON_VIPH_CONTROL								0xc40
#		define RADEON_VIPH_CONTROL_VIPH_REG_RDY			(1 << 13)
#		define RADEON_VIPH_CONTROL_VIPH_MAX_WAIT_SHIFT	16
#		define RADEON_VIPH_CONTROL_VIPH_DMA_MODE		(1 << 20)
#		define RADEON_VIPH_CONTROL_VIPH_EN				(1 << 21)

#define	RADEON_VIPH_REG_ADDR							0x80
#define	RADEON_VIPH_REG_DATA							0x84

#define	RADEON_VIPH_DV_LAT								0xc44
#		define RADEON_VIPH_DV_LAT_VIPH_DV0_LAT_SHIFT	16
#		define RADEON_VIPH_DV_LAT_VIPH_DV1_LAT_SHIFT	20
#		define RADEON_VIPH_DV_LAT_VIPH_DV2_LAT_SHIFT	24
#		define RADEON_VIPH_DV_LAT_VIPH_DV3_LAT_SHIFT	28

#define RADEON_VIPH_DMA_CHUNK							0xc48
#		define RADEON_VIPH_DMA_CHUNK_VIPH_CH0_CHUNK_SHIFT	0
#		define RADEON_VIPH_DMA_CHUNK_VIPH_CH1_CHUNK_SHIFT	4
#		define RADEON_VIPH_DMA_CHUNK_VIPH_CH2_CHUNK_SHIFT	6
#		define RADEON_VIPH_DMA_CHUNK_VIPH_CH3_CHUNK_SHIFT	8

#define RADEON_VIP_VENDOR_DEVICE_ID                 	0x0000


#endif
