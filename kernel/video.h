/**
 * NecoOS - Video Driver Header
 * 
 * Defines functions for VGA text mode output
 */

 #ifndef NECOOS_VIDEO_H
 #define NECOOS_VIDEO_H
 
 #include <stdint.h>
 
 // VGA text mode colors
 enum vga_color {
     VGA_COLOR_BLACK = 0,
     VGA_COLOR_BLUE = 1,
     VGA_COLOR_GREEN = 2,
     VGA_COLOR_CYAN = 3,
     VGA_COLOR_RED = 4,
     VGA_COLOR_MAGENTA = 5,
     VGA_COLOR_BROWN = 6,
     VGA_COLOR_LIGHT_GREY = 7,
     VGA_COLOR_DARK_GREY = 8,
     VGA_COLOR_LIGHT_BLUE = 9,
     VGA_COLOR_LIGHT_GREEN = 10,
     VGA_COLOR_LIGHT_CYAN = 11,
     VGA_COLOR_LIGHT_RED = 12,
     VGA_COLOR_LIGHT_MAGENTA = 13,
     VGA_COLOR_LIGHT_BROWN = 14,
     VGA_COLOR_WHITE = 15,
 };
 
 // VGA text mode dimensions
 #define VGA_WIDTH 80
 #define VGA_HEIGHT 25
 #define VGA_MEMORY 0xB8000
 
 // Function prototypes
 void video_init();
 void video_clear_screen();
 void video_set_cursor(int x, int y);
 void video_get_cursor(int *x, int *y);
 void video_print_char(char c);
 void video_print_string(const char *str);
 void video_print_int(int num);
 void video_set_color(uint8_t fg, uint8_t bg);
 void video_scroll();
 
 #endif /* NECOOS_VIDEO_H */
