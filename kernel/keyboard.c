/**
 * NecoOS - Keyboard Driver Implementation
 * 
 * Implements functions for keyboard input handling
 */

 #include "keyboard.h"
 #include "interrupts.h"
 #include "video.h"
 #include "../lib/stdlib.h"
 
 // Keyboard controller ports
 #define KEYBOARD_DATA_PORT    0x60
 #define KEYBOARD_STATUS_PORT  0x64
 #define KEYBOARD_COMMAND_PORT 0x64
 
 // Key states
 #define KEY_RELEASED 0x80  // Bit that indicates key release
 
 // Modifier keys state
 static uint8_t shift_pressed = 0;
 static uint8_t ctrl_pressed = 0;
 static uint8_t alt_pressed = 0;
 static uint8_t caps_lock = 0;
 
 // Keyboard buffer
 static char keyboard_buffer[KEYBOARD_BUFFER_SIZE];
 static int buffer_start = 0;
 static int buffer_end = 0;
 static int buffer_count = 0;
 
 // US QWERTY keyboard layout
 static const char keymap_lower[128] = {
     0, 27, '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '-', '=', '\b',
     '\t', 'q', 'w', 'e', 'r', 't', 'y', 'u', 'i', 'o', 'p', '[', ']', '\n',
     0, 'a', 's', 'd', 'f', 'g', 'h', 'j', 'k', 'l', ';', '\'', '`',
     0, '\\', 'z', 'x', 'c', 'v', 'b', 'n', 'm', ',', '.', '/', 0,
     '*', 0, ' ', 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
     '-', 0, 0, 0, '+', 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
 };
 
 static const char keymap_upper[128] = {
     0, 27, '!', '@', '#', '$', '%', '^', '&', '*', '(', ')', '_', '+', '\b',
     '\t', 'Q', 'W', 'E', 'R', 'T', 'Y', 'U', 'I', 'O', 'P', '{', '}', '\n',
     0, 'A', 'S', 'D', 'F', 'G', 'H', 'J', 'K', 'L', ':', '"', '~',
     0, '|', 'Z', 'X', 'C', 'V', 'B', 'N', 'M', '<', '>', '?', 0,
     '*', 0, ' ', 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
     '-', 0, 0, 0, '+', 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
 };
 
 /**
  * Add a character to the keyboard buffer
  */
 static void buffer_put(char c) {
     // Check if buffer is full
     if (buffer_count >= KEYBOARD_BUFFER_SIZE) {
         return;
     }
     
     // Add character to buffer
     keyboard_buffer[buffer_end] = c;
     buffer_end = (buffer_end + 1) % KEYBOARD_BUFFER_SIZE;
     buffer_count++;
 }
 
 /**
  * Get a character from the keyboard buffer
  */
 static int buffer_get() {
     // Check if buffer is empty
     if (buffer_count == 0) {
         return -1;
     }
     
     // Get character from buffer
     char c = keyboard_buffer[buffer_start];
     buffer_start = (buffer_start + 1) % KEYBOARD_BUFFER_SIZE;
     buffer_count--;
     
     return c;
 }
 
 /**
  * Keyboard interrupt handler
  */
 void keyboard_handler(uint32_t irq) {
     uint8_t scancode = inb(KEYBOARD_DATA_PORT);
     uint8_t key_release = scancode & KEY_RELEASED;
     scancode &= ~KEY_RELEASED;  // Remove release bit
     
     // Handle special keys
     switch (scancode) {
         case KEYBOARD_KEY_LSHIFT:
         case KEYBOARD_KEY_RSHIFT:
             shift_pressed = !key_release;
             break;
             
         case KEYBOARD_KEY_LCTRL:
             ctrl_pressed = !key_release;
             break;
             
         case KEYBOARD_KEY_LALT:
             alt_pressed = !key_release;
             break;
             
         case KEYBOARD_KEY_CAPSLOCK:
             if (!key_release) {
                 caps_lock = !caps_lock;
             }
             break;
             
         default:
             // Handle normal keys (only on key press, not release)
             if (!key_release && scancode < 128) {
                 // Determine which character map to use
                 const char *keymap = ((shift_pressed && !caps_lock) || 
                                      (!shift_pressed && caps_lock)) ? 
                                       keymap_upper : keymap_lower;
                 
                 // Get the character and add it to the buffer
                 char c = keymap[scancode];
                 if (c != 0) {
                     // Handle Ctrl+C and other control combinations
                     if (ctrl_pressed) {
                         if (c >= 'a' && c <= 'z') {
                             c = c - 'a' + 1;  // Ctrl+A = 1, Ctrl+B = 2, etc.
                         }
                     }
                     
                     // Add to buffer
                     buffer_put(c);
                     
                     // Echo the character to the screen (only printable chars)
                     if (c >= 32 && c <= 126) {
                         video_print_char(c);
                     } else if (c == '\t') {
                         video_print_char('\t');
                     } else if (c == '\b') {
                        video_print_char('\b');
                    } else if (c == '\n' || c == '\r') {
                        video_print_char('\n');
                    }
             }
             break;
     }
 }
 
 /**
  * Initialize the keyboard driver
  */
 void keyboard_init() {
     // Register keyboard interrupt handler (IRQ 1)
     register_interrupt_handler(33, keyboard_handler);
     
     // Reset keyboard buffer
     buffer_start = 0;
     buffer_end = 0;
     buffer_count = 0;
     
     // Reset modifier keys state
     shift_pressed = 0;
     ctrl_pressed = 0;
     alt_pressed = 0;
     caps_lock = 0;
 }
 
 /**
  * Read a character from the keyboard buffer (non-blocking)
  * Returns the character read, or -1 if buffer is empty
  */
 int keyboard_read_char() {
     return buffer_get();
 }
 
 /**
  * Read a line from the keyboard
  * Returns the number of characters read
  */
 int keyboard_read_line(char *buffer, int size) {
     int i = 0;
     int c;
     
     // Wait for a character
     while ((c = keyboard_read_char()) == -1) {
         // Just wait for input
         asm volatile("hlt");
     }
     
     // Read until newline or until buffer is full
     while (c != '\n' && c != '\r' && i < size - 1) {
         // Handle backspace
         if (c == '\b') {
             if (i > 0) {
                 i--;
             }
         } 
         // Handle normal characters
         else if (c >= 32 || c == '\t') {
             buffer[i++] = c;
         }
         
         // Wait for next character
         while ((c = keyboard_read_char()) == -1) {
             asm volatile("hlt");
         }
     }
     
     // Null-terminate the string
     buffer[i] = '\0';
     
     return i;
 }
 
 /**
  * Check if a key is currently pressed
  */
 int keyboard_is_key_pressed(uint8_t scancode) {
     switch(scancode) {
         case KEYBOARD_KEY_LSHIFT:
         case KEYBOARD_KEY_RSHIFT:
             return shift_pressed;
         case KEYBOARD_KEY_LCTRL:
             return ctrl_pressed;
         case KEYBOARD_KEY_LALT:
             return alt_pressed;
         default:
             return 0;  // We don't track other keys' state
     }
 }
 
 /**
  * Get the name of a key from its scancode
  */
 const char *keyboard_get_key_name(uint8_t scancode) {
     static const char *key_names[] = {
         "Unknown", "Esc", "1", "2", "3", "4", "5", "6", "7", "8", "9", "0",
         "-", "=", "Backspace", "Tab", "Q", "W", "E", "R", "T", "Y", "U", "I",
         "O", "P", "[", "]", "Enter", "LCtrl", "A", "S", "D", "F", "G", "H",
         "J", "K", "L", ";", "'", "`", "LShift", "\\", "Z", "X", "C", "V",
         "B", "N", "M", ",", ".", "/", "RShift", "*", "LAlt", "Space"
     };
     
     if (scancode < sizeof(key_names) / sizeof(key_names[0])) {
         return key_names[scancode];
     } else {
         return "Unknown";
     }
}
