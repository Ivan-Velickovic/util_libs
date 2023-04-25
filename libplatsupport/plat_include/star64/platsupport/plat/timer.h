/*
 * Copyright 2023, UNSW
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */
#pragma once

#include <stdint.h>

/*
 * The JH7110 SoC contains a timer with four 32-bit counters. Each one of these
 * counters is referred to as a "channel".
 */

#define CHANNEL_0_IRQ 69
#define CHANNEL_1_IRQ 70
#define CHANNEL_2_IRQ 71
#define CHANNEL_3_IRQ 72

#define TIMER_BASE 0x13050000

typedef struct {
    /* Registers */
    uint32_t ctrl;
    uint32_t load;
    uint32_t enable;
    uint32_t reload;
    uint32_t value;
    uint32_t intclr;
    uint32_t intmask;
} starfive_timer_regs_t;

typedef struct {
    volatile starfive_timer_regs_t *regs;
    void *vaddr;
} starfive_timer_t;

int starfive_timer_init(starfive_timer_t *timer, uint64_t channel);
uint32_t starfive_timer_get_time(starfive_timer_t *timer);
