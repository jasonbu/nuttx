/****************************************************************************
 * arch/arm/src/nrf51/chip.h
 *
 * SPDX-License-Identifier: Apache-2.0
 ****************************************************************************/

#ifndef __ARCH_ARM_SRC_NRF51_CHIP_H
#define __ARCH_ARM_SRC_NRF51_CHIP_H

/* The QEMU micro:bit target models an nRF51822 (Cortex-M0). The support here
 * is intentionally minimal and relies on semihosting for console I/O.
 */

#define NRF51_FLASH_BASE   0x00000000
#define NRF51_SRAM_BASE    0x20000000

#endif /* __ARCH_ARM_SRC_NRF51_CHIP_H */
