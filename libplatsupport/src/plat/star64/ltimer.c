/*
 * Copyright 2023, UNSW
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#include <platsupport/plat/timer.h>
#include <platsupport/ltimer.h>
#include <platsupport/io.h>
#include <utils/io.h>

typedef struct {
    starfive_timer_t timer;
    irq_id_t timer_irq_id;
    ltimer_callback_fn_t user_callback;
    void *user_callback_token;
    ps_io_ops_t ops;
} starfive_ltimer_t;

/* See timer.h for an explanation of the driver. */

/* Each channel IRQ is edge-triggered. */
static ps_irq_t irqs[] = {
    {
        .type = PS_TRIGGER,
        .trigger.number = CHANNEL_0_IRQ,
        .trigger.trigger = 1
    }
};

static pmem_region_t pmems[] = {
    {
        .type = PMEM_TYPE_DEVICE,
        .base_addr = TIMER_BASE,
        .length = 0x10000
    },
};

static int get_time(void *data, uint64_t *time)
{
    assert(data);
    assert(time);
    starfive_ltimer_t *starfive_ltimer = (starfive_ltimer_t *) data;
    *time = starfive_timer_get_time(&starfive_ltimer->timer);

    return 0;
}

static int set_timeout(void *data, uint64_t ns, timeout_type_t type)
{
}

static int reset(void *data)
{
    // just stop timer
}

static void destroy(void *data)
{
    // stop timer and do teardown stuff
}

int ltimer_default_init(ltimer_t *ltimer, ps_io_ops_t ops, ltimer_callback_fn_t callback, void *callback_token)
{
    printf("in ltimer_default_init!\n");
    assert(ltimer);

    ltimer->get_time = get_time;
    ltimer->set_timeout = set_timeout;
    ltimer->reset = reset;
    ltimer->destroy = destroy;

    int error;

    error = ps_calloc(&ops.malloc_ops, 1, sizeof(starfive_ltimer_t), &ltimer->data);
    if (error) {
        return error;
    }

    starfive_ltimer_t *starfive_ltimer = ltimer->data;

    starfive_ltimer->ops = ops;
    starfive_ltimer->user_callback = callback;
    starfive_ltimer->user_callback_token = callback_token;
    starfive_ltimer->timer_irq_id = PS_INVALID_IRQ_ID;

    starfive_ltimer->timer.vaddr = ps_pmem_map(&ops, pmems[0], false, PS_MEM_NORMAL);
    if (starfive_ltimer->timer.vaddr == NULL) {
        destroy(ltimer->data);
        return EINVAL;
    }

    starfive_timer_init(&starfive_ltimer->timer, 0);

    printf("get_time: %d\n", starfive_timer_get_time(&starfive_ltimer->timer));
    int i = 0;
    // while (i < 10000) {
    //     printf("get_time: %d\n", starfive_timer_get_time(&starfive_ltimer->timer));
    //     i++;
    // }

    return 0;
}
