#ifndef STATS_H
#define STATS_H

/* Structure used by 'get_stats' function */
struct stats
{
  unsigned long user_ticks;
  unsigned long system_ticks;
  unsigned long blocked_ticks;
  unsigned long ready_ticks;
  unsigned long elapsed_total_ticks;
  unsigned long total_trans; /* Number of times the process has got the CPU: READY->RUN transitions */
  unsigned long remaining_ticks;
};

void reset_stats(struct stats *t);
void user_to_system_stats(struct stats *t);
void system_to_user_stats(struct stats *t);
void system_to_ready_stats(struct stats *t);
void ready_to_system_stats(struct stats *t);

#endif /* !STATS_H */
