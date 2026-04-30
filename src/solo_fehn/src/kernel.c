/*
 * kernel.c - Kernel entry point
 *
 * Assignment 4 adds memory management, paging, and the PIT-based sleep
 * functions.  Boot flow now:
 *
 *   1. Clear the screen (terminal).
 *   2. Install GDT, IDT, IRQs, keyboard.
 *   3. Initialise the kernel heap (init_kernel_memory).
 *   4. Enable paging (init_paging).
 *   5. Print the memory layout.
 *   6. Initialise the PIT (so sleep_* and IRQ0 work).
 *   7. Demonstrate malloc and the sleep functions.
 *   8. Idle on hlt while the keyboard handler picks up keystrokes.
 */

#include <libc/stdint.h>
#include <libc/stdio.h>
#include <gdt.h>
#include <idt.h>
#include <irq.h>
#include <keyboard.h>
#include <terminal.h>
#include <memory.h>
#include <paging.h>
#include <pit.h>

void main(uint32_t mb_magic, uint32_t mb_info_addr) {
    (void)mb_magic;
    (void)mb_info_addr;

    terminal_initialize();

    printf("UiAOS booting...\n");

    gdt_install();
    printf("[init] GDT installed.\n");

    idt_install();
    printf("[init] IDT installed (32 ISRs registered).\n");

    irq_install();
    printf("[init] PICs remapped, 16 IRQs registered.\n");

    keyboard_install();
    printf("[init] Keyboard handler attached to IRQ1.\n");

    /* --- Assignment 4: memory and paging --- */
    init_kernel_memory(&end);
    init_paging();
    printf("[init] Paging enabled (identity-mapped first 4 MiB).\n");
    print_memory_layout();

    /* Demonstrate the kernel heap. */
    void* a = malloc(12345);
    void* b = malloc(54321);
    void* c = malloc(13331);
    printf("[mem] malloc(12345) = 0x%x\n", (unsigned int)a);
    printf("[mem] malloc(54321) = 0x%x\n", (unsigned int)b);
    printf("[mem] malloc(13331) = 0x%x\n", (unsigned int)c);
    print_memory_layout();

    /* --- Assignment 4: PIT --- */
    init_pit();
    printf("[init] PIT running at %d Hz.\n", (int)TARGET_FREQUENCY);

    /* Demonstrate sleep functions. */
    int counter = 0;
    for (int n = 0; n < 2; n++) {
        printf("[%d]: Sleeping with busy-waiting (HIGH CPU).\n", counter);
        sleep_busy(1000);
        printf("[%d]: Slept using busy-waiting.\n", counter++);

        printf("[%d]: Sleeping with interrupts (LOW CPU).\n", counter);
        sleep_interrupt(1000);
        printf("[%d]: Slept using interrupts.\n", counter++);
    }

    printf("\nInterrupts enabled. Type something on the keyboard:\n> ");

    for (;;) {
        __asm__ volatile ("hlt");
    }
}
