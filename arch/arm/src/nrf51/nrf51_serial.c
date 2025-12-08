/****************************************************************************
 * arch/arm/src/nrf51/nrf51_serial.c
 *
 * Semihosting backed serial for QEMU micro:bit.
 *
 * SPDX-License-Identifier: Apache-2.0
 ****************************************************************************/

#include <nuttx/config.h>
#include <nuttx/arch.h>
#include <nuttx/irq.h>
#include <nuttx/serial/serial.h>

#include <syscall.h>

#ifdef CONFIG_NRF51_HAVE_UART

#define SEMI_SYSLOG_WRITEC  0x03
#define SEMI_SYSLOG_READC   0x07

static int nrf51_setup(struct uart_dev_s *dev)
{
  return OK;
}

static void nrf51_shutdown(struct uart_dev_s *dev)
{
}

static int nrf51_attach(struct uart_dev_s *dev)
{
  return OK;
}

static void nrf51_detach(struct uart_dev_s *dev)
{
}

static void nrf51_txint(struct uart_dev_s *dev, bool enable)
{
}

static void nrf51_rxint(struct uart_dev_s *dev, bool enable)
{
}

static bool nrf51_rxavailable(struct uart_dev_s *dev)
{
  /* Semihost channel always blocks on receive. */
  return true;
}

static bool nrf51_txready(struct uart_dev_s *dev)
{
  return true;
}

static void nrf51_send(struct uart_dev_s *dev, int ch)
{
  smh_call(SEMI_SYSLOG_WRITEC, &ch);
}

static int nrf51_receive(struct uart_dev_s *dev, uint32_t *status)
{
  int ch = smh_call(SEMI_SYSLOG_READC, NULL);
  return ch;
}

static const struct uart_ops_s g_semihost_ops =
{
  .setup    = nrf51_setup,
  .shutdown = nrf51_shutdown,
  .attach   = nrf51_attach,
  .detach   = nrf51_detach,
  .txint    = nrf51_txint,
  .rxint    = nrf51_rxint,
  .rxavailable = nrf51_rxavailable,
  .rxflowcontrol = NULL,
  .txready  = nrf51_txready,
  .txempty  = nrf51_txready,
  .send     = nrf51_send,
  .receive  = nrf51_receive,
  .rxenable = NULL
};

static char g_semirx[1];
static char g_semitx[1];

static struct uart_dev_s g_semihost_uart =
{
  .recv =
    {
      .size   = 1,
      .buffer = g_semirx
    },
  .xmit =
    {
      .size   = 1,
      .buffer = g_semitx
    },
  .ops  = &g_semihost_ops,
};

void arm_earlyserialinit(void)
{
  uart_register("/dev/console", &g_semihost_uart);
}

void arm_serialinit(void)
{
}

int up_putc(int ch)
{
  smh_call(SEMI_SYSLOG_WRITEC, &ch);
  return ch;
}
#endif /* CONFIG_NRF51_HAVE_UART */
