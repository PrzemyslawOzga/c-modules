#include "memory_usage_logger.h"

#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/sched/signal.h>
#include <linux/mm.h>

struct task_struct *task;
struct mm_struct *mm;

static void print_memory_usage(struct task_struct *task, struct mm_struct *mm)
{
	printk(KERN_INFO "--------------------------------------------\n");
	printk(KERN_INFO "Process: %s (PID: %d)\n", task->comm, task->pid);
	printk(KERN_INFO " Total Virtual Memory: %.2lu MB\n",
	       (mm->total_vm * PAGE_SIZE) / (1024 * 1024));
	printk(KERN_INFO " RSS (Resident Set Size): %.2lu MB\n",
	       (get_mm_rss(mm) * PAGE_SIZE) / (1024 * 1024));
}

static void get_memory_usage(void)
{
	for_each_process(task) {
		mm = task->mm;
		if (mm) {
			print_memory_usage(task, mm);
		}
	}
}

static int __init memory_usage_info_init(void)
{
	printk(KERN_INFO "Start building module...\n");
	get_memory_usage();
	printk(KERN_INFO "Module built successfully.\n");

	return 0;
}

static void __exit memory_usage_info_exit(void)
{
	printk(KERN_INFO "VMA logger exit.\n");
}

module_init(memory_usage_info_init);
module_exit(memory_usage_info_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Przemyslaw Ozga <przemyslaw.ozga@intel.com>");
MODULE_DESCRIPTION(
	"A module that monitors and manages memory allocation. Name VMA means Virtual Memory Area");
