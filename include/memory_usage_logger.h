#ifndef MEMORYLOGGER_H
#define MEMORYLOGGER_H

#include <linux/module.h>

static void print_memory_usage(struct task_struct *task, struct mm_struct *mm);
static void get_memory_usage(void);

#endif /* MEMORYLOGGER_H */
