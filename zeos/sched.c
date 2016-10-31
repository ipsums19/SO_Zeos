/*
 * sched.c - initializes struct for task 0 anda task 1
 */

#include <sched.h>
#include <mm.h>
#include <io.h>

/**
 * Container for the Task array and 2 additional pages (the first and the last one)
 * to protect against out of bound accesses.
 */
union task_union protected_tasks[NR_TASKS+2]
  __attribute__((__section__(".data.task")));

union task_union *task = &protected_tasks[1]; /* == union task_union task[NR_TASKS] */

#if 1
struct task_struct *list_head_to_task_struct(struct list_head *l)
{
  return list_entry( l, struct task_struct, list);
}
#endif

extern struct list_head blocked;
struct list_head freequeue;
struct list_head readyqueue;
struct task_struct *idle_task;


/* get_DIR - Returns the Page Directory address for task 't' */
page_table_entry * get_DIR (struct task_struct *t)
{
	return t->dir_pages_baseAddr;
}

/* get_PT - Returns the Page Table address for task 't' */
page_table_entry * get_PT (struct task_struct *t)
{
	return (page_table_entry *)(((unsigned int)(t->dir_pages_baseAddr->bits.pbase_addr))<<12);
}

int get_quantum(struct task_struct *t)
{
    return t->quantum;
}

void set_quantum(struct task_struct *t, int new_quantum)
{
    t->quantum = new_quantum;
}

void sched_next_rr()
{
    union task_union *next_union;
    struct list_head *next_queue = list_first(&readyqueue);
    if(next_queue == NULL)
        next_union = (union task_union *) idle_task;
    else
    {
        list_del(next_queue);
        next_union = (union task_union *) list_head_to_task_struct(next_queue);
    }
    task_switch(next_union);
    set_quantum(current(), DEFAULT_QUANTUM);
}

void update_process_state_rr(struct task_struct *t, struct list_head *dest)
{
    if(dest == NULL)
    {
        t->state = ST_RUN;
        /*ready_to_system_stats(t);*/
    }
    else if(t->PID != 0)
    {
        /*if(dest == &readyqueue) system_to_ready_stats(t);*/
        t->state = ST_READY;
        list_add_tail(&t->list, dest);
    }
}

int needs_sched_rr()
{
    return (get_quantum(current()) <= 0 && !(list_empty(&readyqueue)));
}

void update_sched_data_rr()
{
    if(get_quantum(current()) > 0) current()->quantum--;
}

void schedule()
{
    update_sched_data_rr();
    if(needs_sched_rr())
    {
        update_process_state_rr(list_head_to_task_struct(list_first(&readyqueue)), NULL);
        update_process_state_rr(current(), &readyqueue);
        sched_next_rr();
    }
}

int allocate_DIR(struct task_struct *t)
{
	int pos;

	pos = ((int)t-(int)task)/sizeof(union task_union);

	t->dir_pages_baseAddr = (page_table_entry*) &dir_pages[pos];

	return 1;
}

void cpu_idle(void)
{
	__asm__ __volatile__("sti": : :"memory");

	while(1)
	{
	;
	}
}

void init_idle (void)
{
    struct task_struct *pcb;
    union task_union *task;
    pcb = list_head_to_task_struct(list_first(&freequeue));
    task = (union task_union*) pcb;
    list_del(&pcb->list);

    pcb->PID = 0;
    pcb->esp = 0;
    pcb->quantum = 0;
    reset_stats(&pcb->process_stats);
    allocate_DIR(pcb);

    task->stack[KERNEL_STACK_SIZE-1] = (unsigned long)&cpu_idle;
    task->stack[KERNEL_STACK_SIZE-2] = 0;
    pcb->esp = task->stack[KERNEL_STACK_SIZE-2];
    idle_task = pcb;
}

void init_task1(void)
{
    struct task_struct *pcb;
    union task_union *task;
    pcb = list_head_to_task_struct(list_first(&freequeue));
    list_del(&pcb->list);
    task = (union task_union*) pcb;

    pcb->PID = 1;
    reset_stats(pcb);
    allocate_DIR(pcb);
    set_user_pages(pcb);
    tss.esp0 = (DWord) &task->stack[KERNEL_STACK_SIZE];
    set_cr3(get_DIR(pcb));
}


void init_sched(){
    INIT_LIST_HEAD(&readyqueue);
    INIT_LIST_HEAD(&freequeue);
    int i;
    for(i = 0; i < NR_TASKS; ++i)
        list_add_tail(&task[i].task.list, &freequeue);
}

struct task_struct* current()
{
  int ret_value;

  __asm__ __volatile__(
  	"movl %%esp, %0"
	: "=g" (ret_value)
  );
  return (struct task_struct*)(ret_value&0xfffff000);
}

void inner_task_switch(union task_union *new)
{
    tss.esp0 = (DWord) &new->stack[KERNEL_STACK_SIZE];
    set_cr3(get_DIR(&new->task));

    __asm__ __volatile__(
        "movl %%ebp, %0"
        : "=g" (current()->esp)
    );
    __asm__ __volatile__(
        "movl %%esp, %0"
        :: "g" (new->task.esp)
    );
    __asm__ __volatile__(
        "popl %ebp\n"
        "ret\n"
    );
}

void task_switch(union task_union *new)
{
    __asm__ __volatile__(
            "pushl %esi\n"
            "pushl %edi\n"
            "pushl %ebx\n"
            );

    inner_task_switch(new);

    __asm__ __volatile__(
            "popl %ebx\n"
            "popl %edi\n"
            "popl %esi\n"
            );
}
