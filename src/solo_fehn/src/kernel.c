/*
 * kernel.c - Kernel entry point
 *
 * Assignment 5 adds the music player on top of all earlier subsystems.
 * Boot flow:
 *   GDT -> IDT -> IRQs -> keyboard -> heap -> paging -> PIT -> music demo.
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
#include <song.h>

void main(uint32_t mb_magic, uint32_t mb_info_addr) {
    (void)mb_magic;
    (void)mb_info_addr;

    terminal_initialize();

    printf("UiAOS booting...\n");

    gdt_install();
    idt_install();
    irq_install();
    keyboard_install();
    init_kernel_memory(&end);
    init_paging();
    init_pit();

    printf("[init] All subsystems online (GDT, IDT, IRQ, keyboard, heap, paging, PIT).\n\n");

    /* --- Assignment 5: music player --- */
    Song songs[] = {
        { music_1, music_1_length },
    };
    uint32_t n_songs = sizeof(songs) / sizeof(Song);

    SongPlayer* player = create_song_player();
    printf("[music] Player created at 0x%x\n", (unsigned int)player);

    /* Play each song twice, then drop into the keyboard prompt. The
       assignment example uses while(1); we use a small finite count so
       the demo finishes in reasonable time and the kernel goes idle
       afterwards. */
    for (int round = 0; round < 2; round++) {
        for (uint32_t i = 0; i < n_songs; i++) {
            printf("\nPlaying song %d (round %d)...\n", (int)(i + 1), round + 1);
            player->play_song(&songs[i]);
            printf("Finished playing the song.\n");
            sleep_interrupt(500);
        }
    }

    /* Tidy up. */
    free(player);

    __asm__ volatile ("sti");
    printf("\nMusic demo complete. Type something on the keyboard:\n> ");

    for (;;) {
        __asm__ volatile ("hlt");
    }
}
