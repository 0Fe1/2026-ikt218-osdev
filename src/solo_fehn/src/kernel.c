/*
 * kernel.c - Kernel entry point for Assignment 2
 *
 * Called from multiboot2.asm after Limine bootloader hands over control.
 * Sets up the GDT, initializes the VGA text terminal, and prints "Hello World".
 */

#include <libc/stdint.h>
#include <libc/stdio.h>
#include <gdt.h>
#include <terminal.h>

void main(uint32_t mb_magic, uint32_t mb_info_addr) {
    /* mb_magic and mb_info_addr come from the Multiboot2 bootloader.
       We don't use them yet, but mark them as used so the compiler stays quiet. */
    (void)mb_magic;
    (void)mb_info_addr;

    /* Clear the screen and move the cursor to the top-left. */
    terminal_initialize();

    /* Install our own Global Descriptor Table.
       After this returns, the CPU is using OUR segment descriptors,
       not the ones Limine set up. */
    gdt_install();

    /* Print Hello World using our printf implementation. */
    printf("Hello World!\n");
    printf("GDT installed with %d entries (NULL, Code, Data).\n", 3);

    /* Halt the CPU forever. We don't have interrupts or a scheduler yet. */
    while (1) {
        __asm__ volatile ("hlt");
    }
}
