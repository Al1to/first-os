#include "./keyboard.h"

volatile bool input;

#define KEYBOARD_BUFFER_SIZE 128

typedef struct {
    volatile int head;
    volatile int tail;
    char buffer[KEYBOARD_BUFFER_SIZE];
} buffer_t;

buffer_t buffer;
static char input_buffer[KEYBOARD_BUFFER_SIZE];

void buffer_push(char data) {
    buffer.buffer[buffer.tail] = data;
    buffer.tail = (buffer.tail + 1) % KEYBOARD_BUFFER_SIZE;
}

void buffer_pop() {
    buffer.tail = (buffer.tail - 1) % KEYBOARD_BUFFER_SIZE;
}

bool caps_on;
bool caps_lock;

const uint32_t UNKNOWN   = 0xFFFFFFFF;
const uint32_t ESC       = 0xFFFFFFFF - 1;
const uint32_t CTRL      = 0xFFFFFFFF - 2;
const uint32_t LSHFT     = 0xFFFFFFFF - 3;
const uint32_t RSHFT     = 0xFFFFFFFF - 4;
const uint32_t ALT       = 0xFFFFFFFF - 5;
const uint32_t F1        = 0xFFFFFFFF - 6;
const uint32_t F2        = 0xFFFFFFFF - 7;
const uint32_t F3        = 0xFFFFFFFF - 8;
const uint32_t F4        = 0xFFFFFFFF - 9;
const uint32_t F5        = 0xFFFFFFFF - 10;
const uint32_t F6        = 0xFFFFFFFF - 11;
const uint32_t F7        = 0xFFFFFFFF - 12;
const uint32_t F8        = 0xFFFFFFFF - 13;
const uint32_t F9        = 0xFFFFFFFF - 14;
const uint32_t F10       = 0xFFFFFFFF - 15;
const uint32_t F11       = 0xFFFFFFFF - 16;
const uint32_t F12       = 0xFFFFFFFF - 17;
const uint32_t SCRLCK    = 0xFFFFFFFF - 18;
const uint32_t HOME      = 0xFFFFFFFF - 19;
const uint32_t UP        = 0xFFFFFFFF - 20;
const uint32_t LEFT      = 0xFFFFFFFF - 21;
const uint32_t RIGHT     = 0xFFFFFFFF - 22;
const uint32_t DOWN      = 0xFFFFFFFF - 23;
const uint32_t PGUP      = 0xFFFFFFFF - 24;
const uint32_t PGDOWN    = 0xFFFFFFFF - 25;
const uint32_t END       = 0xFFFFFFFF - 26;
const uint32_t INS       = 0xFFFFFFFF - 27;
const uint32_t DEL       = 0xFFFFFFFF - 28;
const uint32_t CAPS      = 0xFFFFFFFF - 29;
const uint32_t NONE      = 0xFFFFFFFF - 30;
const uint32_t ALTGR     = 0xFFFFFFFF - 31;
const uint32_t NUMLCK    = 0xFFFFFFFF - 32;

const uint32_t lowercase[128] = {
    UNKNOWN,ESC,'1','2','3','4','5','6','7','8',
    '9','0','-','=','\b','\t','q','w','e','r',
    't','y','u','i','o','p','[',']','\n',CTRL,
    'a','s','d','f','g','h','j','k','l',';',
    '\'','`',LSHFT,'\\','z','x','c','v','b','n','m',',',
    '.','/',RSHFT,'*',ALT,' ',CAPS,F1,F2,F3,F4,F5,F6,F7,F8,F9,F10,NUMLCK,SCRLCK,HOME,UP,PGUP,'-',LEFT,UNKNOWN,RIGHT,
    '+',END,DOWN,PGDOWN,INS,DEL,UNKNOWN,UNKNOWN,UNKNOWN,F11,F12,UNKNOWN,UNKNOWN,UNKNOWN,UNKNOWN,UNKNOWN,UNKNOWN,
    UNKNOWN,UNKNOWN,UNKNOWN,UNKNOWN,UNKNOWN,UNKNOWN,UNKNOWN,UNKNOWN,UNKNOWN,UNKNOWN,UNKNOWN,UNKNOWN,UNKNOWN,UNKNOWN,
    UNKNOWN,UNKNOWN,UNKNOWN,UNKNOWN,UNKNOWN,UNKNOWN,UNKNOWN,UNKNOWN,UNKNOWN,UNKNOWN,UNKNOWN,UNKNOWN,UNKNOWN,UNKNOWN,
    UNKNOWN,UNKNOWN,UNKNOWN,UNKNOWN,UNKNOWN
};

const uint32_t uppercase[128] = {
    UNKNOWN,ESC,'!','@','#','$','%','^','&','*','(',')','_','+','\b','\t','Q','W','E','R',
    'T','Y','U','I','O','P','{','}','\n',CTRL,'A','S','D','F','G','H','J','K','L',':','"','~',LSHFT,'|','Z','X','C',
    'V','B','N','M','<','>','?',RSHFT,'*',ALT,' ',CAPS,F1,F2,F3,F4,F5,F6,F7,F8,F9,F10,NUMLCK,SCRLCK,HOME,UP,PGUP,'-',
    LEFT,UNKNOWN,RIGHT,'+',END,DOWN,PGDOWN,INS,DEL,UNKNOWN,UNKNOWN,UNKNOWN,F11,F12,UNKNOWN,UNKNOWN,UNKNOWN,UNKNOWN,
    UNKNOWN,UNKNOWN,UNKNOWN,UNKNOWN,UNKNOWN,UNKNOWN,UNKNOWN,UNKNOWN,UNKNOWN,UNKNOWN,UNKNOWN,UNKNOWN,UNKNOWN,UNKNOWN,
    UNKNOWN,UNKNOWN,UNKNOWN,UNKNOWN,UNKNOWN,UNKNOWN,UNKNOWN,UNKNOWN,UNKNOWN,UNKNOWN,UNKNOWN,UNKNOWN,UNKNOWN,UNKNOWN,
    UNKNOWN,UNKNOWN,UNKNOWN,UNKNOWN,UNKNOWN,UNKNOWN,UNKNOWN
};

void print_pci_keyboard_data(pci_t pci, uint8_t i, uint8_t j, uint8_t k) {
	// (void)i;
	// (void)j;
	// (void)k;
	// if (pci.vendorID != 0xFFFF && pci.class == 1 && pci.subclass == 6) {
	// 	/*if (k==0) {
	// 	  printf("Detected SATA Host on port %X:%X\n", i,j);
	// 	  dprintf("Detected SATA Host on port %X:%X\n", i,j);
	// 	} else {
	// 	  printf("Detected SATA Host on port %X:%X.%u\n",i,j,k);
	// 	  dprintf("Detected SATA Host on port %X:%X.%u\n",i,j,k);
	// 	}*/
	// 	// identity_map((void *)pci.BAR5);
	// 	// initialize_abar((HBAData *)pci.BAR5);
	// }
	vga_print("Detected Keyboard on port i:j\n");
}

void keyboard_init(void) {
    pci_register_driver(print_pci_keyboard_data, 9, 0);
    buffer.head = buffer.tail = 0;
    input = false;
    caps_on = false;
    irq_install_handler(1, &keyboard_handler);
}

void keyboard_handler(struct int_regs *regs) {
    if (!input) return;
    char scan_code = in_port_b(0x60) & 0x7F;
    char press     = in_port_b(0x60) & 0x80;

    switch (scan_code) {
        case 1:
        case 29:
        case 56:
        case 59:
        case 60:
        case 61:
        case 62:
        case 63:
        case 64:
        case 65:
        case 66:
        case 67: 
        case 68:
        case 87:
        case 88:
            break;
            
        // case 75:                                             // left
        //     position cur_pos_left = vga_get_pos();
        //     position stop_delete_left = get_terminal_stop_delete();
        //     if (( cur_pos_left.row != stop_delete_left.row
        //        || (cur_pos_left.col-1) != stop_delete_left.col) 
        //        && press == 0) {
        //         cur_pos_left.col -= 1;
        //         vga_set_pos(cur_pos_left);
        //     }
        //     break; 
            
        case 28:                                                // enter
            if (press == 0) {
                vga_print("\n");
                buffer_push('\0');
                input = false;
            }
            break;
        case 42:                                                // shift
            if (press == 0) caps_on == true;
            else            caps_on == false;
            break;
            
        case 58:                                                // caps_lock
            if (!caps_lock && press == 0)     caps_lock = true;
            else if (caps_lock && press == 0) caps_lock = false;
            break;
            
        case 14:                                                // backspace
            position cur_pos_bs = vga_get_pos();
            position stop_delete_bs = get_terminal_stop_delete();
            if (( cur_pos_bs.row != stop_delete_bs.row
               || cur_pos_bs.col != stop_delete_bs.col) 
               && press == 0) {
                vga_print("\b");
                buffer_pop();
            }
            break;

        default:
            if (press == 0) {
                if (caps_on || caps_lock) {
                    vga_printf("%c", uppercase[scan_code]);
                    buffer_push(uppercase[scan_code]);
                } else { 
                    vga_printf("%c", lowercase[scan_code]);
                    buffer_push(lowercase[scan_code]);
                }
            }
            break;
    }
}

char* wait_keyboard_input(void) { 
    input = true;
    while (input);

    int size = (buffer.tail + KEYBOARD_BUFFER_SIZE - buffer.head) % KEYBOARD_BUFFER_SIZE;
    for (int i = 0; i < size; i++) {
        input_buffer[i] = buffer.buffer[(buffer.head + i) % KEYBOARD_BUFFER_SIZE];
    }

    buffer.head = buffer.tail % KEYBOARD_BUFFER_SIZE;
    
    return input_buffer;
}