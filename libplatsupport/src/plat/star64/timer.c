#include <assert.h>
#include <errno.h>
#include <stdint.h>
#include <stdlib.h>
#include <platsupport/plat/timer.h>

#define STARFIVE_TIMER_MAX_TICKS        0xffffffff

int starfive_timer_init(starfive_timer_t *timer, uint64_t channel) {
    assert(timer);
    assert(timer->vaddr);
    if (timer == NULL || timer->vaddr == NULL) {
        return EINVAL;
    }

    timer->regs = timer->vaddr + sizeof(starfive_timer_regs_t) * channel;

    /* Set the timer to continous mode */
    timer->regs->ctrl = 0;
    timer->regs->load = STARFIVE_TIMER_MAX_TICKS;
    timer->regs->enable = 1;

    return 0;
}

uint32_t starfive_timer_get_time(starfive_timer_t *timer) {
    assert(timer);
    assert(timer->regs);

    return timer->regs->value;
}
