/*
 * keyboard.c - PS/2 keyboard driver (Task 4 of Assignment 3)
 *
 * The keyboard controller reports key events on IRQ1.  Reading I/O port
 * 0x60 returns a "scancode" byte that identifies the key and whether it
 * was pressed or released (top bit set = release).  We translate the
 * lower 7 bits through a US-layout lookup table to get a printable ASCII
 * character, and feed it to the terminal via printf.
 *
 * No buffering, no shift/caps-lock state, no key-up handling beyond
 * ignoring it.  Enough for the assignment requirement of "read and print".
 */

#include <keyboard.h>
#include <irq.h>
#include <io.h>
#include <libc/stdint.h>
#include <libc/stdio.h>

#define KBD_DATA_PORT  0x60

/* US-layout scancode set 1 -> ASCII.  0x00 means "no character to print". */
static const char scancode_ascii[128] = {
    0,    27,  '1', '2', '3', '4', '5', '6', '7', '8',     /* 0x00-0x09 */
    '9',  '0', '-', '=', '\b','\t','q', 'w', 'e', 'r',     /* 0x0A-0x13 */
    't',  'y', 'u', 'i', 'o', 'p', '[', ']', '\n', 0,      /* 0x14-0x1D */
    'a',  's', 'd', 'f', 'g', 'h', 'j', 'k', 'l', ';',     /* 0x1E-0x27 */
    '\'', '`', 0,   '\\','z', 'x', 'c', 'v', 'b', 'n',     /* 0x28-0x31 */
    'm',  ',', '.', '/', 0,   '*', 0,   ' ', 0,   0,       /* 0x32-0x3B */
    /* The rest is function keys, numpad, etc. - all left as 0. */
};

static void keyboard_callback(registers_t* r) {
    (void)r;

    uint8_t scancode = inb(KBD_DATA_PORT);

    /* Top bit set = key release.  Skip those for now. */
    if (scancode & 0x80) {
        return;
    }

    char c = (scancode < 128) ? scancode_ascii[scancode] : 0;
    if (c != 0) {
        printf("%c", c);
    }
}

void keyboard_install(void) {
    irq_register_handler(1, keyboard_callback);
    irq_unmask(1);   /* allow IRQ1 to be delivered */
}
