/**
 * NecoOS - Video Driver Implementation
 * 
 * Implements functions for VGA text mode output
 */

 #include "video.h"
 #include "../lib/stdlib.h"
 
 // Current cursor position
 static int cursor_x = 0;
 static int cursor_y = 0;
 
 // Current color attributes
 static uint8_t current_color = 0;
 
 // VGA memory pointer
 static uint16_t* vga_memory = (uint16_t*)VGA_MEMORY;
 
 /**
  * Creates a VGA entry with character and color attributes
  */
 static inline uint16_t vga_entry(unsigned char c, uint8_t color) {
     return (uint16_t)c | ((uint16_t)color << 8);
 }
 
 /**
  * Creates a color byte from foreground and background colors
  */
 static inline uint8_t vga_color(enum vga_color fg, enum vga_color bg) {
     return fg | (bg << 4);
 }
 
 /**
  * Updates the hardware cursor position
  */
 static void update_cursor() {
     // Calculate position
     uint16_t pos = cursor_y * VGA_WIDTH + cursor_x;
     
     // Set low byte of cursor position
     outb(0x3D4, 0x0F);
     outb(0x3D5, (uint8_t)(pos & 0xFF));
     
     // Set high byte of cursor position
     outb(0x3D4, 0x0E);
     outb(0x3D5, (uint8_t)((pos >> 8) & 0xFF));
 }
 
 /**
  * Initialize the video subsystem
  */
 void video_init() {
     // Default to light grey on black
     current_color = vga_color(VGA_COLOR_LIGHT_GREY, VGA_COLOR_BLACK);
     
     // Clear the screen
     video_clear_screen();
 }
 
 /**
  * Clear the screen and reset cursor position
  */
 void video_clear_screen() {
     // Fill the entire screen with spaces using current color
     for (int y = 0; y < VGA_HEIGHT; y++) {
         for (int x = 0; x < VGA_WIDTH; x++) {
             const int index = y * VGA_WIDTH + x;
             vga_memory[index] = vga_entry(' ', current_color);
         }
     }
     
     // Reset cursor position
     cursor_x = 0;
     cursor_y = 0;
     update_cursor();
 }
 
 /**
  * Set the cursor position
  */
 void video_set_cursor(int x, int y) {
     // Validate coordinates
     if (x >= 0 && x < VGA_WIDTH && y >= 0 && y < VGA_HEIGHT) {
         cursor_x = x;
         cursor_y = y;
         update_cursor();
     }
 }
 
 /**
  * Get the current cursor position
  */
 void video_get_cursor(int *x, int *y) {
     if (x) *x = cursor_x;
     if (y) *y = cursor_y;
 }
 
 /**
  * Print a single character
  */
 void video_print_char(char c) {
     // Handle special characters
     switch (c) {
         case '\n':  // Newline
             cursor_x = 0;
             cursor_y++;
             break;
             
         case '\r':  // Carriage return
             cursor_x = 0;
             break;
             
         case '\t':  // Tab
             cursor_x = (cursor_x + 8) & ~(8 - 1);
             break;
             
         case '\b':  // Backspace
             if (cursor_x > 0) {
                 cursor_x--;
                 // Erase the character
                 const int index = cursor_y * VGA_WIDTH + cursor_x;
                 vga_memory[index] = vga_entry(' ', current_color);
             }
             break;
             
         default:  // Regular character
             // Calculate position and write character
             const int index = cursor_y * VGA_WIDTH + cursor_x;
             vga_memory[index] = vga_entry(c, current_color);
             cursor_x++;
             break;
     }
     
     // Handle end of line
     if (cursor_x >= VGA_WIDTH) {
         cursor_x = 0;
         cursor_y++;
     }
     
     // Handle scrolling if necessary
     if (cursor_y >= VGA_HEIGHT) {
         video_scroll();
     }
     
     // Update hardware cursor
     update_cursor();
 }
 
 /**
  * Print a null-terminated string
  */
 void video_print_string(const char *str) {
     while (*str) {
         video_print_char(*str++);
     }
 }
 
 /**
  * Print an integer
  */
 void video_print_int(int num) {
     char buffer[16];
     itoa(num, buffer, 10);
     video_print_string(buffer);
 }
 
 /**
  * Set text foreground and background colors
  */
 void video_set_color(uint8_t fg, uint8_t bg) {
     current_color = vga_color(fg, bg);
 }
 
 /**
  * Scroll the screen up one line
  */
 void video_scroll() {
     // Move all lines up by one
     for (int y = 0; y < VGA_HEIGHT - 1; y++) {
         for (int x = 0; x < VGA_WIDTH; x++) {
             const int src_index = (y + 1) * VGA_WIDTH + x;
             const int dst_index = y * VGA_WIDTH + x;
             vga_memory[dst_index] = vga_memory[src_index];
         }
     }
     
     // Clear the last line
     for (int x = 0; x < VGA_WIDTH; x++) {
         const int index = (VGA_HEIGHT - 1) * VGA_WIDTH + x;
         vga_memory[index] = vga_entry(' ', current_color);
     }
     
     // Adjust cursor position
     cursor_y--;
 }
