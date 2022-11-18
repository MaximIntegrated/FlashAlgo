/* CMSIS-DAP Interface Firmware
 * Copyright (c) 2009-2013 ARM Limited
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include <stdint.h>
#include <stdbool.h>
#include "FlashOS.h"
#include "flc_regs.h"

#define FLC_CN_UNLOCK_VALUE   0x20000000UL

typedef struct {
    uint32_t flc_base;      // flsah controller 0
    uint32_t flash_base;    // flash block0 addr
    uint32_t flash_size;    // flash block0 size
    
    uint32_t clkdiv_value;
    uint32_t burst_size;
    
    // flash block1 params, may not be exist
    uint32_t flc1_base;     // flash controller 1
    uint32_t flash1_base;   // flash block1 addr
    uint32_t flash1_size;   // flash block1 size
} device_cfg_t;

// Allocate space for the device config and fill it with some dummy values so the compiler keeps it.
volatile const device_cfg_t device_cfg = {
    0x11111111,
    0x22222222,
    0x33333333,
    0x44444444,
    0x55555555,
    0x0,
    0x0,
    0x0,
};

/******************************************************************************/
/*
 *  Check if the Flash controller is busy
 *    Parameter:      flc:  Pointer to Flash Controller
 *    Return Value:   0 - OK,  1 - Failed
 */
static bool IsBusy(mxc_flc_regs_t *flc)
{
    /* Make sure no flash operation is currently in progress */
    return flc->cn & (MXC_F_FLC_CN_WR | MXC_F_FLC_CN_ME | MXC_F_FLC_CN_PGE);
}

/******************************************************************************/
/*
 *  Initialize Flash Programming Functions
 *    Parameter:      adr:  Device Base Address
 *                    clk:  Clock Frequency (Hz)
 *                    fnc:  Function Code (1 - Erase, 2 - Program, 3 - Verify)
 *    Return Value:   0 - OK,  1 - Failed
 */
int Init(uint32_t adr, uint32_t clk, uint32_t fnc)
{
    mxc_flc_regs_t *flc = (mxc_flc_regs_t*)device_cfg.flc_base;

    /* Check if the flash controller is busy */
    if (IsBusy(flc)) {
        return 1;
    }
		
    /* Set flash clock divider to generate a 1MHz clock from the APB clock */
    flc->clkdiv = device_cfg.clkdiv_value;

    // Enable FLC1 if exist
    if (device_cfg.flc1_base) {
        flc = (mxc_flc_regs_t*)device_cfg.flc1_base;

        /* Check if the flash controller is busy */
        if (IsBusy(flc)) {
            return 1;
        }

        /* Set flash clock divider to generate a 1MHz clock from the APB clock */
        flc->clkdiv = device_cfg.clkdiv_value;	
    }
				
    return  0;
}

/******************************************************************************/
/*
 *  De-Initialize Flash Programming Functions
 *    Parameter:      fnc:  Function Code (1 - Erase, 2 - Program, 3 - Verify)
 *    Return Value:   0 - OK,  1 - Failed
 */
int UnInit(uint32_t fnc)
{
    mxc_flc_regs_t *flc = (mxc_flc_regs_t*)device_cfg.flc_base;

    /* Lock flash */
    flc->cn &= ~MXC_F_FLC_CN_UNLOCK;

    // Uninit FLC1 if exist
    if (device_cfg.flc1_base) {
        mxc_flc_regs_t *flc = (mxc_flc_regs_t*)device_cfg.flc1_base;

        /* Lock flash */
        flc->cn &= ~MXC_F_FLC_CN_UNLOCK;
    }

    return  0;
}

/******************************************************************************/
static int PrepareFLC(mxc_flc_regs_t *flc)
{
    /* Check if the flash controller is busy */
    if (IsBusy(flc)) {
        return 1;
    }

    /* Clear stale errors and disable interrupts */
    if (flc->intr != 0) {
        flc->intr = 0;
        if (flc->intr != 0) {
            flc->intr = MXC_F_FLC_INTR_AF;
            if (flc->intr != 0) {
                return 1;
            }
        }
    }

    /* Unlock flash */
    flc->cn = (flc->cn & ~MXC_F_FLC_CN_UNLOCK) | FLC_CN_UNLOCK_VALUE;

    return 0;
}

static int EraseFlashBlock(mxc_flc_regs_t *flc)
{
    /* Prepare for the flash operation */
    if (PrepareFLC(flc) != 0) {
        return 1;
    }

    /* Write mass erase code */
    flc->cn = (flc->cn & ~MXC_F_FLC_CN_ERASE_CODE) | MXC_S_FLC_CN_CODE_MASS_ERASE;

    /* Issue mass erase command */
    flc->cn |= MXC_F_FLC_CN_ME;

    /* Wait until mass erase is complete */
    while (flc->cn & MXC_F_FLC_CN_ME);

    /* Lock flash */
    flc->cn &= ~MXC_F_FLC_CN_UNLOCK;

    /* Check access violations */
    if (flc->intr & MXC_F_FLC_INTR_AF) {
        return 1;
    }

    return  0;
}

/******************************************************************************/
/*
 *  Erase complete Flash Memory
 *    Return Value:   0 - OK,  1 - Failed
 */
int EraseChip(void)
{
    mxc_flc_regs_t *flc = (mxc_flc_regs_t*)device_cfg.flc_base;

    if (EraseFlashBlock(flc) != 0) {
        return 1;
    }

    // Erase FLC1 if exist
    if (device_cfg.flc1_base) {
        mxc_flc_regs_t *flc = (mxc_flc_regs_t*)device_cfg.flc1_base;
        
        if (EraseFlashBlock(flc) != 0) {
            return 1;
        }       
    }

    return  0;
}

/******************************************************************************/
/*
 *  Erase Sector in Flash Memory
 *    Parameter:      address:  Sector Address
 *    Return Value:   0 - OK,  1 - Failed
 */
int EraseSector(uint32_t address)
{
    mxc_flc_regs_t *flc;

    if (device_cfg.flash1_base && (address >= device_cfg.flash1_base)) {
        address -= device_cfg.flash_size; // take away first block size
        flc = (mxc_flc_regs_t*)device_cfg.flc1_base;
    } else {
        flc = (mxc_flc_regs_t*)device_cfg.flc_base;
    }
    
    /* Prepare for the flash operation */
    if (PrepareFLC(flc) != 0) {
        return 1;
    }

    /* Write page erase code */
    flc->cn = (flc->cn & ~MXC_F_FLC_CN_ERASE_CODE) | MXC_S_FLC_CN_CODE_PAGE_ERASE;

    /* Issue page erase command */
    flc->addr = address;
    flc->cn |= MXC_F_FLC_CN_PGE;

    /* Wait until the page erase is complete */
    while (flc->cn & MXC_F_FLC_CN_PGE);

    /* Lock flash */
    flc->cn &= ~MXC_F_FLC_CN_UNLOCK;

    /* Check access violations */
    if (flc->intr & MXC_F_FLC_INTR_AF) {
        return 1;
    }

    return  0;
}

/******************************************************************************/
/*
 *  Program Page in Flash Memory
 *    Parameter:      address:  Page Start Address
 *                    size:     Page Size
 *                    buffer:   Page Data
 *    Return Value:   0 - OK,  1 - Failed
 */
int ProgramPage(uint32_t address, uint32_t size, unsigned char *buffer8)
{
    mxc_flc_regs_t *flc;
    uint32_t remaining = size;
    uint32_t *buffer = (uint32_t *)buffer8;

    if (device_cfg.flash1_base && (address >= device_cfg.flash1_base)) {
        address -= device_cfg.flash_size; // take away first block size
        flc = (mxc_flc_regs_t*)device_cfg.flc1_base;
    } else {
        flc = (mxc_flc_regs_t*)device_cfg.flc_base;
    }
    
    /* Only accept 32-bit aligned pointers */
    if ((uint32_t)buffer8 & 0x3) {
        return 1;
    }
    buffer = (uint32_t *)buffer8;

    /* Prepare for the flash operation */
    if (PrepareFLC(flc) != 0) {
        return 1;
    }

    while ((remaining >= 4) && ((address & 0x1F) != 0)) {
        flc->addr = address;
        flc->data0 = *buffer++;
        flc->cn |= MXC_F_FLC_CN_WR;

        /* Wait until flash write is complete */
        while (flc->cn & MXC_F_FLC_CN_WR);

        address += 4;
        remaining -= 4;
    }

    if ((device_cfg.burst_size == 128) && (remaining >= 16)) {
        while (remaining >= 16) {
            flc->addr = address;
            flc->data0 = *buffer++;
            flc->data1 = *buffer++;
            flc->data2 = *buffer++;
            flc->data3 = *buffer++;
            flc->cn |= MXC_F_FLC_CN_WR;

            /* Wait until flash write is complete */
            while (flc->cn & MXC_F_FLC_CN_WR);

            address += 16;
            remaining -= 16;
        }
    }

    if (remaining >= 4) {
        while (remaining >= 4) {
            flc->addr = address;
            flc->data0 = *buffer++;
            flc->cn |= MXC_F_FLC_CN_WR;

            /* Wait until flash write is complete */
            while (flc->cn & MXC_F_FLC_CN_WR);

            address += 4;
            remaining -= 4;
        }
    }

    if (remaining > 0) {
        uint32_t last_word;
        uint32_t mask;

        last_word = 0xffffffff;
        mask = 0xff;

        while (remaining > 0) {
            last_word &= (*buffer | ~mask);
            mask <<= 8;
            remaining--;
        }

        flc->addr = address;
        flc->data0 = last_word;
        flc->cn |= MXC_F_FLC_CN_WR;

        /* Wait until flash write is complete */
        while (flc->cn & MXC_F_FLC_CN_WR);
    }

    /* Lock flash */
    flc->cn &= ~MXC_F_FLC_CN_UNLOCK;

    /* Check access violations */
    if (flc->intr & MXC_F_FLC_INTR_AF) {
        return 1;
    }

    return 0;
}
