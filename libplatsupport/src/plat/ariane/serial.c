/*
 * Copyright 2019, Data61, CSIRO (ABN 41 687 119 230)
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#include <string.h>
#include <stdlib.h>
#include <platsupport/serial.h>
#include "../../chardev.h"

static void uart_handle_irq(ps_chardevice_t *dev)
{
    /* nothing to do, interrupts are not used */
}

#define UART_RBR_OFFSET     0   /* In:  Recieve Buffer Register */
#define UART_THR_OFFSET     0   /* Out: Transmitter Holding Register */
#define UART_DLL_OFFSET     0   /* Out: Divisor Latch Low */
#define UART_IER_OFFSET     1   /* I/O: Interrupt Enable Register */
#define UART_DLM_OFFSET     1   /* Out: Divisor Latch High */
#define UART_FCR_OFFSET     2   /* Out: FIFO Control Register */
#define UART_IIR_OFFSET     2   /* I/O: Interrupt Identification Register */
#define UART_LCR_OFFSET     3   /* Out: Line Control Register */
#define UART_MCR_OFFSET     4   /* Out: Modem Control Register */
#define UART_LSR_OFFSET     5   /* In:  Line Status Register */
#define UART_MSR_OFFSET     6   /* In:  Modem Status Register */
#define UART_SCR_OFFSET     7   /* I/O: Scratch Register */
#define UART_MDR1_OFFSET    8   /* I/O:  Mode Register */

#define UART_LSR_FIFOE      0x80    /* Fifo error */
#define UART_LSR_TEMT       0x40    /* Transmitter empty */
#define UART_LSR_THRE       0x20    /* Transmit-hold-register empty */
#define UART_LSR_BI     0x10    /* Break interrupt indicator */
#define UART_LSR_FE     0x08    /* Frame error indicator */
#define UART_LSR_PE     0x04    /* Parity error indicator */
#define UART_LSR_OE     0x02    /* Overrun error indicator */
#define UART_LSR_DR     0x01    /* Receiver data ready */
#define UART_LSR_BRK_ERROR_BITS 0x1E    /* BI, FE, PE, OE bits */

int uart_putchar(ps_chardevice_t *dev, int c)
{
    void *vaddr = dev->vaddr;
    volatile uint32_t *lsr = vaddr + UART_LSR_OFFSET;
    while ((*lsr & UART_LSR_THRE) == 0)
        ;

    volatile uint32_t *thr = vaddr + UART_THR_OFFSET;
    *thr = c;
    return c;
}

int uart_getchar(ps_chardevice_t *dev)
{
    return -1;
}

int uart_init(const struct dev_defn *defn,
              const ps_io_ops_t *ops,
              ps_chardevice_t *dev)
{
    memset(dev, 0, sizeof(*dev));

    /* Map device. */
    void *vaddr = chardev_map(defn, ops);
    if (vaddr == NULL) {
        return -1;
    }

    /* Set up all the device properties. */
    dev->id         = defn->id;
    dev->vaddr      = (void *)vaddr;
    dev->read       = &uart_read;
    dev->write      = &uart_write;
    dev->handle_irq = &uart_handle_irq;
    dev->irqs       = defn->irqs;
    dev->ioops      = *ops;
    dev->flags      = SERIAL_AUTO_CR;

    return 0;
}
