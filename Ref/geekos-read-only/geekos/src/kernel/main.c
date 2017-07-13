/*
 * GeekOS entry point and main function
 * Copyright (C) 2001-2008, David H. Hovemeyer <david.hovemeyer@gmail.com>
 *
 * This code is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License version 2 only, as
 * published by the Free Software Foundation.
 *   
 * This code is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
 * version 2 for more details (a copy is included in the LICENSE file that
 * accompanied this code).
 *  
 * You should have received a copy of the GNU General Public License version
 * 2 along with this work; if not, write to the Free Software Foundation,
 * Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA.
 */

#include <geekos/boot.h>
#include <geekos/version.h>
#include <geekos/cons.h>
#include <geekos/mem.h>
#include <geekos/vm.h>
#include <geekos/int.h>
#include <geekos/irq.h>
#include <geekos/thread.h>
#include <geekos/workqueue.h>
#include <geekos/timer.h>

static void test_thread(ulong_t arg)
{
	cons_printf("Test thread: arg=%lx\n", arg);
	thread_exit(42);
}

static void busy_wait(u32_t ticks)
{
	u32_t last = g_numticks;
	while (g_numticks < last + ticks) {
		/* wait */
	}
}

static void busy_thread(ulong_t arg)
{
	busy_wait(90);
	while (1) {
		cons_printf("A");
		busy_wait(180);
	}
}

void geekos_main(u32_t loader_magic, struct multiboot_info *boot_record)
{
	/* Initialize kernel */
	mem_clear_bss();
	cons_init();
	cons_clear();
	PANIC_IF(loader_magic != MB_LOADER_MAGIC, "Unrecognized magic");
	cons_printf("GeekOS %d.%d.%d on %s\n", GEEKOS_MAJOR, GEEKOS_MINOR, GEEKOS_PATCH, GEEKOS_ARCH);
	mem_init(boot_record);
	int_init();
	vm_init_paging(boot_record);
	irq_init();
	thread_init();
	workqueue_init();
	timer_init();

	/* TODO: spawn init process */
	{
		struct thread *thread = thread_create(&test_thread, 0xdeadbeefUL, THREAD_ATTACHED);
		int exitcode;

		cons_printf("Starting new thread %p\n", thread);
		thread_yield();
		exitcode = thread_join(thread);
		cons_printf("Thread exited with code %d\n", exitcode);
	}

	thread_create(&busy_thread, 0, THREAD_DETACHED);

	/* see if timer is ticking */
	while (1) {
		busy_wait(180);
		cons_printf("B");
	}

#if 0
	HALT();
#endif
}
