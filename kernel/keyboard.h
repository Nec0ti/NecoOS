/**
 * NecoOS - Keyboard Driver Header
 * 
 * Defines functions for keyboard input handling
 */

 #ifndef NECOOS_KEYBOARD_H
 #define NECOOS_KEYBOARD_H
 
 #include <stdint.h>
 
 // Keyboard scan codes
 #define KEYBOARD_KEY_ESC        0x01
 #define KEYBOARD_KEY_BACKSPACE  0x0E
 #define KEYBOARD_KEY_TAB        0x0F
 #define KEYBOARD_KEY_ENTER      0x1C
 #define KEYBOARD_KEY_LCTRL      0x1D
 #define KEYBOARD_KEY_LSHIFT     0x2A
 #define KEYBOARD_KEY_RSHIFT     0x36
 #define KEYBOARD_KEY_LALT       0x38
 #define KEYBOARD_KEY_CAPSLOCK   0x3A
 #define KEYBOARD_KEY_F1         0x3B
 #define KEYBOARD_KEY_F2         0x3C
 #define KEYBOARD_KEY_F3         0x3D
 #define KEYBOARD_KEY_F4         0x3E
 #define KEYBOARD_KEY_F5         0x3F
 #define KEYBOARD_KEY_F6         0x40
 #define KEYBOARD_KEY_F7         0x41
 #define KEYBOARD_KEY_F8         0x42
 #define KEYBOARD_KEY_F9         0x43
 #define KEYBOARD_KEY_F10        0x44
 #define KEYBOARD_KEY_NUMLOCK    0x45
 #define KEYBOARD_KEY_SCROLLLOCK 0x46
 #define KEYBOARD_KEY_HOME       0x47
 #define KEYBOARD_KEY_UP         0x48
 #define KEYBOARD_KEY_PGUP       0x49
 #define KEYBOARD_KEY_LEFT       0x4B
 #define KEYBOARD_KEY_RIGHT      0x4D
 #define KEYBOARD_KEY_END        0x4F
 #define KEYBOARD_KEY_DOWN       0x50
 #define KEYBOARD_KEY_PGDN       0x51
 #define KEYBOARD_KEY_INS        0x52
 #define KEYBOARD_KEY_DEL        0x53
 
 // Keyboard buffer size
 #define KEYBOARD_BUFFER_SIZE 64
 
 // Function prototypes
 void keyboard_init();
 void keyboard_handler(uint32_t irq);
 int keyboard_read_char();
 int keyboard_read_line(char *buffer, int size);
 int keyboard_is_key_pressed(uint8_t scancode);
 const char *keyboard_get_key_name(uint8_t scancode);
 
 #endif /* NECOOS_KEYBOARD_H */
