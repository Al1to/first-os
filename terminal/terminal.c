#include "./terminal.h"

position terminal_stop_delete;

void terminal_init(const char* prompt) {
    while(true) {
        vga_print("\n");
        // path = get_path();
        // vga_printc(8, path);
        vga_printc(7, prompt);
        terminal_stop_delete = vga_get_pos();
        char* command = wait_keyboard_input();
        vga_print(command);
        // ... execute
    }
}

position get_terminal_stop_delete() {
    return terminal_stop_delete;
}