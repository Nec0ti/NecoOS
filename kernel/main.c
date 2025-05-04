/**
 * NecoOS - Kernel Main
 * 
 * This is the main entry point for the NecoOS kernel.
 * It initializes all required subsystems and starts the main kernel loop.
 */

 #include "../lib/stdlib.h"
 #include "video.h"
 #include "keyboard.h"
 #include "interrupts.h"
 
 // Forward declarations
 void kernel_init();
 void kernel_main();
 
 /**
  * Entry point for the kernel.
  * This function is called by the bootloader after switching to protected mode.
  */
 void kernel_entry() {
     // Initialize the kernel
     kernel_init();
     
     // Start the main kernel loop
     kernel_main();
     
     // We should never reach here, but just in case
     for(;;) {
         halt();
     }
 }
 
 /**
  * Initialize all kernel subsystems
  */
 void kernel_init() {
     // Initialize video subsystem
     video_init();
     
     // Display welcome message
     video_clear_screen();
     video_print_string("NecoOS v0.1 - Kernel Initialized\n");
     video_print_string("--------------------------------\n\n");
     
     // Initialize interrupt descriptor table
     idt_init();
     
     // Initialize programmable interrupt controller
     pic_init();
     
     // Initialize keyboard subsystem
     keyboard_init();
     
     // Enable interrupts
     asm volatile("sti");
     
     // Initialize the shell
     shell_init();
     
     video_print_string("System initialization complete.\n");
     video_print_string("Type 'help' for available commands.\n\n");
 }
 
 /**
  * Main kernel loop
  * This is where the kernel spends most of its time.
  */
 void kernel_main() {
     // Start the shell
     shell_start();
     
     fs_t* fs = fs_init(512, 1024); // Initialize file system with 512-byte sectors and 1024 sectors
     shell_t* shell = shell_init(fs); // Initialize shell with file system
     shell_loop(shell); // Start shell loop
     
     video_print_string("\nKernel halted. System is now inactive.\n");
     
     // If we ever exit the main loop, halt the CPU
     for(;;) {
         halt();
     }
 }
 
 /**
  * Halts the CPU
  */
 void halt() {
     asm volatile("hlt");
 }
