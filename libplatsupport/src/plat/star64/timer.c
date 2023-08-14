#include <assert.h>
#include <errno.h>
#include <stdint.h>
#include <stdlib.h>
#include <platsupport/plat/timer.h>

#define CHANNEL_SIZE 0x40
#define TIMER_MAX_TICKS 0xffffffff

int starfive_handle_irq(starfive_timer_t *timer) {
    timer->regs->intclr = 1;
}

int starfive_timer_init(starfive_timer_t *timer, uint64_t channel) {
    assert(timer);
    assert(timer->vaddr);
    if (timer == NULL || timer->vaddr == NULL) {
        return EINVAL;
    }

    timer->regs = timer->vaddr + CHANNEL_SIZE * channel;

    printf("timer->regs: 0x%p\n", timer->regs);
    printf("timer->regs->load: 0x%p\n", &timer->regs->load);

    assert(((uintptr_t)&timer->regs->load - (uintptr_t)timer->regs) == 0x08);
    assert(((uintptr_t)&timer->regs->enable - (uintptr_t)timer->regs) == 0x10);
    assert(((uintptr_t)&timer->regs->value - (uintptr_t)timer->regs) == 0x18);
    printf("sizeof time->regs: 0x%lx\n", sizeof(timer->regs));
    assert(sizeof(starfive_timer_regs_t) == 0x28);

    /* Set the timer to continous mode */
    timer->regs->enable = 0;
    timer->regs->ctrl = 1;
    // timer->regs->load = TIMER_MAX_TICKS;
    timer->regs->enable = 1;

    timer->regs->enable = 0;
    timer->regs->intclr = 1;
    timer->regs->intmask = 0;
    timer->regs->enable = 1;

    return 0;
}

uint32_t starfive_timer_get_time(starfive_timer_t *timer) {
    assert(timer);
    assert(timer->regs);

    return timer->regs->value;
}
