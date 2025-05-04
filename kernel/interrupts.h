/**
 * NecoOS - Interrupt Management Header
 * 
 * Defines structures and functions for interrupt handling
 */

 #ifndef NECOOS_INTERRUPTS_H
 #define NECOOS_INTERRUPTS_H
 
 #include <stdint.h>
 
 // IDT entry count
 #define IDT_ENTRIES 256
 
 // Interrupt handler function type
 typedef void (*isr_t)(uint32_t);
 
 // Function prototypes
 void idt_init();
 void pic_init();
 void register_interrupt_handler(uint8_t n, isr_t handler);
 
 // Low-level port I/O functions
 static inline uint8_t inb(uint16_t port) {
     uint8_t ret;
     asm volatile("inb %1, %0" : "=a"(ret) : "Nd"(port));
     return ret;
 }
 
 static inline void outb(uint16_t port, uint8_t val) {
     asm volatile("outb %0, %1" : : "a"(val), "Nd"(port));
 }
 
 static inline void io_wait() {
     outb(0x80, 0);  // Write to an unused port - causes a delay
 }
 
 // Interrupt handler numbers
 enum {
     // Exceptions
     INT_DIVIDE_ERROR = 0,
     INT_DEBUG,
     INT_NMI,
     INT_BREAKPOINT,
     INT_OVERFLOW,
     INT_BOUND_RANGE,
     INT_INVALID_OPCODE,
     INT_DEVICE_NOT_AVAILABLE,
     INT_DOUBLE_FAULT,
     INT_COPROCESSOR_SEGMENT,
     INT_INVALID_TSS,
     INT_SEGMENT_NOT_PRESENT,
     INT_STACK_SEGMENT,
     INT_GENERAL_PROTECTION,
     INT_PAGE_FAULT,
     INT_RESERVED,
     INT_X87_FPU,
     INT_ALIGNMENT_CHECK,
     INT_MACHINE_CHECK,
     INT_SIMD_FP,
     INT_VIRTUALIZATION,
     INT_SECURITY_EXCEPTION = 30,
     
     // IRQs (mapped to IDT entries starting at 32)
     IRQ0 = 32,  // Timer
     IRQ1,       // Keyboard
     IRQ2,       // Cascade for 8259A Slave controller
     IRQ3,       // Serial Port COM2
     IRQ4,       // Serial Port COM1
     IRQ5,       // Parallel Port 2 or Sound Card
     IRQ6,       // Floppy Disk Controller
     IRQ7,       // Parallel Port 1
     IRQ8,       // Real Time Clock
     IRQ9,       // ACPI
     IRQ10,      // Open
     IRQ11,      // Open
     IRQ12,      // PS/2 Mouse
     IRQ13,      // FPU / Coprocessor
     IRQ14,      // Primary ATA Hard Disk
     IRQ15       // Secondary ATA Hard Disk
 };
 
 #endif /* NECOOS_INTERRUPTS_H */
