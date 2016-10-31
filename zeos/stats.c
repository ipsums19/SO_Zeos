#include <stats.h>
#include <utils.h>


void reset_stats(struct stats *t)
{
    t->user_ticks = 0;
    t->system_ticks = 0;
    t->blocked_ticks = 0;
    t->ready_ticks = 0;
    t->elapsed_total_ticks = get_ticks();
    t->total_trans = 0;
    t->remaining_ticks = 1000;
}

void user_to_system_stats(struct stats *t){
    t->user_ticks += get_ticks() - t->user_ticks;
    t->elapsed_total_ticks = get_ticks();
}

void system_to_user_stats(struct stats *t){
    t->system_ticks += get_ticks() - t->system_ticks;
    t->elapsed_total_ticks = get_ticks();
}

void system_to_ready_stats(struct stats *t){
    t->system_ticks += get_ticks() - t->system_ticks;
    t->elapsed_total_ticks = get_ticks();
}

void ready_to_system_stats(struct stats *t){
    t->ready_ticks += get_ticks() - t->ready_ticks;
    t->elapsed_total_ticks = get_ticks();
    t->total_trans++;
}


