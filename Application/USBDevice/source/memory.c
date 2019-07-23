/*----------------------------------------------------------------------------
 *      Name:    MEMORY.C
 *      Purpose: USB Mass Storage Demo
 *      Version: V1.10
 *----------------------------------------------------------------------------
 *      This software is supplied "AS IS" without any warranties, express,
 *      implied or statutory, including but not limited to the implied
 *      warranties of fitness for purpose, satisfactory quality and
 *      noninfringement. Keil extends you a royalty-free right to reproduce
 *      and distribute executable files created using this software for use
 *      on NXP Semiconductors LPC family microcontroller devices only. Nothing
 *      else gives you the right to use this software.
 *
 *      Copyright (c) 2005-2009 Keil Software.
 *---------------------------------------------------------------------------*/
#ifdef __BUILD_WITH_EXAMPLE__
#include "lpc177x_8x_libcfg.h"
#else
#include "lpc177x_8x_libcfg_default.h"
#endif /* __BUILD_WITH_EXAMPLE__ */

#ifdef _USB_DEV_MASS_STORAGE
#include "LPC177x_8x.h"

#include "lpc_types.h"

#include "usb.h"
#include "usbcfg.h"
#include "usbhw.h"
#include "usbcore.h"
#include "mscuser.h"

#include "memory.h"

extern uint8_t Memory[MSC_MemorySize];         /* MSC Memory in RAM */


/* Main Program */

void USB_Message_Connect(void)
{
    uint32_t n;

    for (n = 0; n < MSC_ImageSize; n++) {     /* Copy Initial Disk Image */
        Memory[n] = DiskImage[n];             /*   from Flash to RAM     */
    }

    USB_Init();                               /* USB Initialization */
    USB_Connect(TRUE);                        /* USB Connect */
}


/**
 * @}
 */
#endif /*_USB_DEV_MASS_STORAGE*/
