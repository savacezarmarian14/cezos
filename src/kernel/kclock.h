#ifndef __KCLOCK_H__
#define __KCLOCK_H__

#define IO_RTC					0x70

#define MC_NVRAM_START			0x0e
#define MV_NVRAM_SIZE			50

#define NVRAM_BASELO			(MC_NVRAM_START + 7)	/* low byte; RTC off. 0x15 */
#define NVRAM_BASEHI			(MC_NVRAM_START + 8)	/* high byte; RTC off. 0x16 */

/* NVRAM bytes 9 & 10: extended memory size (between 1MB and 16MB) */
#define NVRAM_EXTLO				(MC_NVRAM_START + 9)	/* low byte; RTC off. 0x17 */
#define NVRAM_EXTHI				(MC_NVRAM_START + 10)	/* high byte; RTC off. 0x18 */

/* NVRAM bytes 38 and 39: extended memory size (between 16MB and 4G) */
#define NVRAM_EXT16LO			(MC_NVRAM_START + 38)	/* low byte; RTC off. 0x34 */
#define NVRAM_EXT16HI			(MC_NVRAM_START + 39)	/* high byte; RTC off. 0x35 */


unsigned int mc_read(unsigned int reg);
void 		 mc_write(unsigned int reg, unsigned int datum);


#endif // __KCLOCK_H__