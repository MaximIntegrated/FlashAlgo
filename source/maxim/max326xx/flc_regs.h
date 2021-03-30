/*******************************************************************************
* Copyright (C) 2014 Maxim Integrated Products, Inc., All Rights Reserved.
*
* Permission is hereby granted, free of charge, to any person obtaining a
* copy of this software and associated documentation files (the "Software"),
* to deal in the Software without restriction, including without limitation
* the rights to use, copy, modify, merge, publish, distribute, sublicense,
* and/or sell copies of the Software, and to permit persons to whom the
* Software is furnished to do so, subject to the following conditions:
*
* The above copyright notice and this permission notice shall be included
* in all copies or substantial portions of the Software.
*
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
* OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
* MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
* IN NO EVENT SHALL MAXIM INTEGRATED BE LIABLE FOR ANY CLAIM, DAMAGES
* OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,
* ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
* OTHER DEALINGS IN THE SOFTWARE.
*
* Except as contained in this notice, the name of Maxim Integrated
* Products, Inc. shall not be used except as stated in the Maxim Integrated
* Products, Inc. Branding Policy.
*
* The mere transfer of this software does not imply any licenses
* of trade secrets, proprietary technology, copyrights, patents,
* trademarks, maskwork rights, or any other form of intellectual
* property whatsoever. Maxim Integrated Products, Inc. retains all
* ownership rights.
*
********************************************************************************
*/

#ifndef _MXC_FLC_REGS_H_
#define _MXC_FLC_REGS_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

/*
    If types are not defined elsewhere (CMSIS) define them here
*/
#ifndef __IO
#define __IO volatile
#endif
#ifndef __I
#define __I  volatile const
#endif
#ifndef __O
#define __O  volatile
#endif
#ifndef __R
#define __R  volatile const
#endif

/*                                  Offset  Description
                                    ====================== */
typedef struct {
    __IO uint32_t addr;         /* 0x0000   Flash Address Register */
    __IO uint32_t clkdiv;     	/* 0x0004   Clock Divide Register */
    __IO uint32_t cn;         	/* 0x0008   Control Register */
    __R  uint32_t rsv0[6];      /* 0x000C   Reserved */
    __IO uint32_t intr;       	/* 0x0024   Interrupt Register */
    __R  uint32_t rsv1[2];      /* 0x0028   Reserved */
    __IO uint32_t data0;      	/* 0x0030   Data Register 0 */
    __IO uint32_t data1;        /* 0x0034   Data Register 1 */
    __IO uint32_t data2;        /* 0x0038   Data Register 2 */
    __IO uint32_t data3;        /* 0x003C   Data Register 3 */
} mxc_flc_regs_t;

/*  CN  ******************************************************************  */

#define MXC_F_FLC_CN_WR_POS             (0)
#define MXC_F_FLC_CN_WR                 (0x00000001UL << MXC_F_FLC_CN_WR_POS)
#define MXC_F_FLC_CN_ME_POS             (1)
#define MXC_F_FLC_CN_ME                 (0x00000001UL << MXC_F_FLC_CN_ME_POS)
#define MXC_F_FLC_CN_PGE_POS            (2)
#define MXC_F_FLC_CN_PGE                (0x00000001UL << MXC_F_FLC_CN_PGE_POS)
#define MXC_F_FLC_CN_WDTH_POS           (4)
#define MXC_F_FLC_CN_WDTH               (0x00000001UL << MXC_F_FLC_CN_WDTH_POS)
#define MXC_F_FLC_CN_ERASE_CODE_POS     (8)
#define MXC_F_FLC_CN_ERASE_CODE         (0x000000FFUL << MXC_F_FLC_CN_ERASE_CODE_POS)
#define MXC_S_FLC_CN_CODE_PAGE_ERASE    (0x00000055UL << MXC_F_FLC_CN_ERASE_CODE_POS)
#define MXC_S_FLC_CN_CODE_MASS_ERASE    (0x000000AAUL << MXC_F_FLC_CN_ERASE_CODE_POS)
#define MXC_F_FLC_CN_PEND_POS           (24)
#define MXC_F_FLC_CN_PEND               (0x00000001UL << MXC_F_FLC_CN_PEND_POS)
#define MXC_F_FLC_CN_BRST_POS           (27)
#define MXC_F_FLC_CN_BRST               (0x00000001UL << MXC_F_FLC_CN_BRST_POS)
#define MXC_F_FLC_CN_UNLOCK_POS         (28)
#define MXC_F_FLC_CN_UNLOCK             (0x0000000FUL << MXC_F_FLC_CN_UNLOCK_POS)

/*  INT  ************************************************************  */

#define MXC_F_FLC_INTR_AF_POS     (1)
#define MXC_F_FLC_INTR_AF         (0x00000001UL << MXC_F_FLC_INTR_AF_POS)

#ifdef __cplusplus
}
#endif

#endif /* _MXC_FLC_REGS_H_ */
