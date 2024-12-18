#include "keylogger_interrupt.h"

#include <linux/init.h>
#include <linux/kernel.h>

#define DEBUG_MSG(fmt, ...)                                    \
	do {                                                   \
		if (debug_enabled)                             \
			printk(KERN_DEBUG fmt, ##__VA_ARGS__); \
	} while (0)

// ----------------- Module Init/Exit -----------------
static int __init main_module(void)
{
	return 0;
}

static void __exit exit_module(void)
{
}

module_init(main_module);
module_exit(exit_module);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Przemyslaw Ozga <przemyslaw.ozga@intel.com>");
MODULE_DESCRIPTION(
	"A kernel module that logs keystrokes and handles buffer operations");