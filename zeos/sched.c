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

void sched_next_rr()
{
    struct task_struct *next_task = list_head_to_task_struct(list_first(&readyqueue));
    if(current()->PID != 0)
        list_add_tail(&current()->list, &readyqueue);
    union task_union *next_union = (union task_union *) next_task;
    task_switch(next_union);
    set_quantum(current(), DEFAULT_QUANTUM);
}

void update_process_state_rr(struct task_struct *t, struct list_head *dest)
{

}

int needs_sched_rr()
{
    return (get_quantum(current()) <= 0 && !(list_empty(&readyqueue)));
}

void update_sched_data_rr()
{
    if(current()->quantum > 0) current()->quantum--;
}

void schedule()
{
    update_sched_data_rr();
    if(needs_sched_rr())
    {
        update_process_state_rr(current(), &readyqueue);
        sched_next_rr();
    }
}

int get_quantum(struct task_struct *t)
{
    return t->quantum;
}

void set_quantum(struct task_struct *t, int new_quantum)
{
    t->quantum = new_quantum;
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
    list_del(list_first(&freequeue));

    pcb->PID = 0;
    pcb->esp = 0;
    pcb->quantum = 0;
    allocate_DIR(pcb);

    task->stack[KERNEL_STACK_SIZE-1] = (unsigned long)&cpu_idle;
    task->stack[KERNEL_STACK_SIZE-0] = 0;

    idle_task = pcb;
}

void init_task1(void)
{
    struct task_struct *pcb;
    union task_union *task;
    pcb = list_head_to_task_struct(list_first(&freequeue));
    task = (union task_union*) pcb;
    list_del(list_first(&freequeue));

    pcb->PID = 1;
    allocate_DIR(pcb);
    set_user_pages(pcb);
    tss.esp0 = (DWord) &task->stack[KERNEL_STACK_SIZE];
    set_cr3(get_DIR(pcb));
}


void init_sched(){
    INIT_LIST_HEAD(&readyqueue);
    INIT_LIST_HEAD(&freequeue);
    int i;
    for(i = 0; i < 10; ++i)
        list_add_tail(&task[i].task.list, &readyqueue);
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
