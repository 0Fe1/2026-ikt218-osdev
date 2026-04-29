/*
 * kernel.c - Kernel entry point
 *
 * Assignment 2 added the GDT and the VGA terminal.
 * Assignment 3 adds the IDT, ISRs, IRQs (with PIC remapping) and a
 * PS/2 keyboard driver.
 *
 * Boot flow:
 *   1. Clear the screen.
 *   2. Install our own GDT (so segment registers are under our control).
 *   3. Install the IDT and the 32 CPU exception stubs.
 *   4. Remap the PICs and register the 16 IRQ stubs.
 *   5. Register the keyboard handler on IRQ1.
 *   6. Trigger three software interrupts to demonstrate ISR dispatch.
 *   7. Enable interrupts (sti) and idle on hlt while keyboard input
 *      arrives via IRQ1.
 */

#include <libc/stdint.h>
#include <libc/stdio.h>
#include <gdt.h>
#include <idt.h>
#include <irq.h>
#include <keyboard.h>
#include <terminal.h>

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

    /* Demonstrate ISR dispatch by triggering three software interrupts. */
    printf("\nTriggering three software interrupts:\n");
    __asm__ volatile ("int $0x0");      /* Division by zero */
    __asm__ volatile ("int $0x3");      /* Breakpoint */
    __asm__ volatile ("int $0x6");      /* Invalid opcode */

    /* Enable hardware interrupts and start accepting keyboard input. */
    __asm__ volatile ("sti");
    printf("\nInterrupts enabled. Type something on the keyboard:\n> ");

    for (;;) {
        __asm__ volatile ("hlt");
    }
}
