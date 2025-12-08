/****************************************************************************
 * arch/arm/src/nrf51/nrf51_start.c
 *
 * Minimal start-up logic for the QEMU micro:bit target.
 *
 * SPDX-License-Identifier: Apache-2.0
 ****************************************************************************/

#include <nuttx/config.h>
#include <nuttx/init.h>

#include "arm_internal.h"

/* The idle task stack is placed above .bss */
#define IDLE_STACK ((uintptr_t)_ebss + CONFIG_IDLETHREAD_STACKSIZE)

const uintptr_t g_idle_topstack = IDLE_STACK;

void __start(void)
{
  const uint32_t *src;
  uint32_t *dest;

  /* Clear .bss */
  for (dest = (uint32_t *)_sbss; dest < (uint32_t *)_ebss; )
    {
      *dest++ = 0;
    }

  /* Initialize .data */
  for (src = (uint32_t *)_eronly, dest = (uint32_t *)_sdata;
       dest < (uint32_t *)_edata; )
    {
      *dest++ = *src++;
    }

  /* Perform early serial init so that any debug output is visible */
#ifdef USE_EARLYSERIALINIT
  arm_earlyserialinit();
#endif

  /* Then start NuttX */
  nx_start();
}
