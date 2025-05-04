/**
 * NecoOS - Interrupt Management Implementation
 * 
 * Implements structures and functions for interrupt handling
 */

 #include "interrupts.h"
 #include "video.h"
 #include "../lib/stdlib.h"
 
 // IDT entry structure
 struct idt_entry {
     uint16_t base_lo;  // Lower 16 bits of handler function address
     uint16_t selector; // Kernel segment selector
     uint8_t  zero;     // Always zero
     uint8_t  flags;    // Flags (access rights)
     uint16_t base_hi;  // Upper 16 bits of handler function address
 } __attribute__((packed));
 
 // IDT pointer structure
 struct idt_ptr {
     uint16_t limit;    // Size of the IDT - 1
     uint32_t base;     // Base address of the IDT
 } __attribute__((packed));
 
 // Interrupt handler function pointers
 static isr_t interrupt_handlers[IDT_ENTRIES];
 
 // IDT entries array
 static struct idt_entry idt_entries[IDT_ENTRIES];
 
 // IDT pointer
 static struct idt_ptr idt_ptr;
 
 // PIC port addresses
 #define PIC1_COMMAND  0x20
 #define PIC1_DATA     0x21
 #define PIC2_COMMAND  0xA0
 #define PIC2_DATA     0xA1
 
 // PIC commands
 #define PIC_EOI       0x20  // End Of Interrupt
 #define ICW1_ICW4     0x01  // Expect ICW4
 #define ICW1_SINGLE   0x02  // Single (cascade) mode
 #define ICW1_INTERVAL4 0x04 // Call address interval 4
 #define ICW1_LEVEL    0x08  // Level triggered (edge) mode
 #define ICW1_INIT     0x10  // Initialization command
 #define ICW4_8086     0x01  // 8086/88 mode
 #define ICW4_AUTO     0x02  // Auto (normal) EOI
 #define ICW4_BUF_SLAVE 0x08 // Buffered mode/slave
 #define ICW4_BUF_MASTER 0x0C // Buffered mode/master
 #define ICW4_SFNM     0x10  // Special fully nested mode
 
 // Forward declarations for assembly interrupt handlers
 extern void isr0();
 extern void isr1();
 extern void isr2();
 extern void isr3();
 extern void isr4();
 extern void isr5();
 extern void isr6();
 extern void isr7();
 extern void isr8();
 extern void isr9();
 extern void isr10();
 extern void isr11();
 extern void isr12();
 extern void isr13();
 extern void isr14();
 extern void isr15();
 extern void isr16();
 extern void isr17();
 extern void isr18();
 extern void isr19();
 extern void isr20();
 extern void isr21();
 extern void isr22();
 extern void isr23();
 extern void isr24();
 extern void isr25();
 extern void isr26();
 extern void isr27();
 extern void isr28();
 extern void isr29();
 extern void isr30();
 extern void isr31();
 extern void irq0();
 extern void irq1();
 extern void irq2();
 extern void irq3();
 extern void irq4();
 extern void irq5();
 extern void irq6();
 extern void irq7();
 extern void irq8();
 extern void irq9();
 extern void irq10();
 extern void irq11();
 extern void irq12();
 extern void irq13();
 extern void irq14();
 extern void irq15();
 
 // Helper function to set an IDT gate
 static void idt_set_gate(uint8_t num, uint32_t base, uint16_t selector, uint8_t flags) {
     idt_entries[num].base_lo = base & 0xFFFF;
     idt_entries[num].base_hi = (base >> 16) & 0xFFFF;
     idt_entries[num].selector = selector;
     idt_entries[num].zero = 0;
     idt_entries[num].flags = flags;
 }
 
 // Helper function to load the IDT
 static void idt_load() {
     asm volatile("lidt %0" : : "m"(idt_ptr));
 }
 
 /**
  * Initialize the Interrupt Descriptor Table
  */
 void idt_init() {
     // Set up the IDT pointer
     idt_ptr.limit = sizeof(struct idt_entry) * IDT_ENTRIES - 1;
     idt_ptr.base = (uint32_t)&idt_entries;
     
     // Clear the IDT and interrupt handlers
     memset(&idt_entries, 0, sizeof(struct idt_entry) * IDT_ENTRIES);
     memset(&interrupt_handlers, 0, sizeof(isr_t) * IDT_ENTRIES);
     
     // Set up CPU exception handlers (ISRs 0-31)
     // Flags: 0x8E = 10001110b (Present, Ring 0, 32-bit Interrupt Gate)
     idt_set_gate(0, (uint32_t)isr0, 0x08, 0x8E);
     idt_set_gate(1, (uint32_t)isr1, 0x08, 0x8E);
     idt_set_gate(2, (uint32_t)isr2, 0x08, 0x8E);
     idt_set_gate(3, (uint32_t)isr3, 0x08, 0x8E);
     idt_set_gate(4, (uint32_t)isr4, 0x08, 0x8E);
     idt_set_gate(5, (uint32_t)isr5, 0x08, 0x8E);
     idt_set_gate(6, (uint32_t)isr6, 0x08, 0x8E);
     idt_set_gate(7, (uint32_t)isr7, 0x08, 0x8E);
     idt_set_gate(8, (uint32_t)isr8, 0x08, 0x8E);
     idt_set_gate(9, (uint32_t)isr9, 0x08, 0x8E);
     idt_set_gate(10, (uint32_t)isr10, 0x08, 0x8E);
     idt_set_gate(11, (uint32_t)isr11, 0x08, 0x8E);
     idt_set_gate(12, (uint32_t)isr12, 0x08, 0x8E);
     idt_set_gate(13, (uint32_t)isr13, 0x08, 0x8E);
     idt_set_gate(14, (uint32_t)isr14, 0x08, 0x8E);
     idt_set_gate(15, (uint32_t)isr15, 0x08, 0x8E);
     idt_set_gate(16, (uint32_t)isr16, 0x08, 0x8E);
     idt_set_gate(17, (uint32_t)isr17, 0x08, 0x8E);
     idt_set_gate(18, (uint32_t)isr18, 0x08, 0x8E);
     idt_set_gate(19, (uint32_t)isr19, 0x08, 0x8E);
     idt_set_gate(20, (uint32_t)isr20, 0x08, 0x8E);
     idt_set_gate(21, (uint32_t)isr21, 0x08, 0x8E);
     idt_set_gate(22, (uint32_t)isr22, 0x08, 0x8E);
     idt_set_gate(23, (uint32_t)isr23, 0x08, 0x8E);
     idt_set_gate(24, (uint32_t)isr24, 0x08, 0x8E);
     idt_set_gate(25, (uint32_t)isr25, 0x08, 0x8E);
     idt_set_gate(26, (uint32_t)isr26, 0x08, 0x8E);
     idt_set_gate(27, (uint32_t)isr27, 0x08, 0x8E);
     idt_set_gate(28, (uint32_t)isr28, 0x08, 0x8E);
     idt_set_gate(29, (uint32_t)isr29, 0x08, 0x8E);
     idt_set_gate(30, (uint32_t)isr30, 0x08, 0x8E);
     idt_set_gate(31, (uint32_t)isr31, 0x08, 0x8E);
     
     // Set up IRQ handlers (IRQs 0-15 mapped to IDT entries 32-47)
     idt_set_gate(32, (uint32_t)irq0, 0x08, 0x8E);
     idt_set_gate(33, (uint32_t)irq1, 0x08, 0x8E);
     idt_set_gate(34, (uint32_t)irq2, 0x08, 0x8E);
     idt_set_gate(35, (uint32_t)irq3, 0x08, 0x8E);
     idt_set_gate(36, (uint32_t)irq4, 0x08, 0x8E);
     idt_set_gate(37, (uint32_t)irq5, 0x08, 0x8E);
     idt_set_gate(38, (uint32_t)irq6, 0x08, 0x8E);
     idt_set_gate(39, (uint32_t)irq7, 0x08, 0x8E);
     idt_set_gate(40, (uint32_t)irq8, 0x08, 0x8E);
     idt_set_gate(41, (uint32_t)irq9, 0x08, 0x8E);
     idt_set_gate(42, (uint32_t)irq10, 0x08, 0x8E);
     idt_set_gate(43, (uint32_t)irq11, 0x08, 0x8E);
     idt_set_gate(44, (uint32_t)irq12, 0x08, 0x8E);
     idt_set_gate(45, (uint32_t)irq13, 0x08, 0x8E);
     idt_set_gate(46, (uint32_t)irq14, 0x08, 0x8E);
     idt_set_gate(47, (uint32_t)irq15, 0x08, 0x8E);
     
     // Load the IDT
     idt_load();
 }
 
 /**
  * Initialize the Programmable Interrupt Controllers
  */
 void pic_init() {
     // Save masks
     uint8_t a1 = inb(PIC1_DATA);
     uint8_t a2 = inb(PIC2_DATA);
     
     // Start the initialization sequence (in cascade mode)
     outb(PIC1_COMMAND, ICW1_INIT | ICW1_ICW4);
     io_wait();
     outb(PIC2_COMMAND, ICW1_INIT | ICW1_ICW4);
     io_wait();
     
     // Set vector offsets
     outb(PIC1_DATA, 32);  // ICW2: Master PIC vector offset (IDT entries 32-39)
     io_wait();
     outb(PIC2_DATA, 40);  // ICW2: Slave PIC vector offset (IDT entries 40-47)
     io_wait();
     
     // Tell Master PIC that there is a slave PIC at IRQ2
     outb(PIC1_DATA, 4);   // ICW3: tell Master PIC that there is a slave PIC at IRQ2 (0000 0100)
     io_wait();
     outb(PIC2_DATA, 2);   // ICW3: tell Slave PIC its cascade identity (0000 0010)
     io_wait();
     
     // Set operation mode (8086 mode)
     outb(PIC1_DATA, ICW4_8086);
     io_wait();
     outb(PIC2_DATA, ICW4_8086);
     io_wait();
     
     // Restore saved masks
     outb(PIC1_DATA, a1);
     outb(PIC2_DATA, a2);
 }
 
 /**
  * Register an interrupt handler
  */
 void register_interrupt_handler(uint8_t n, isr_t handler) {
     interrupt_handlers[n] = handler;
 }
 
 /**
  * Generic interrupt handler - called from assembly
  */
 void isr_handler(uint32_t interrupt_number) {
     // Check if we have a registered handler for this interrupt
     if (interrupt_handlers[interrupt_number]) {
         interrupt_handlers[interrupt_number](interrupt_number);
     } else {
         // No handler registered, just print a message
         video_print_string("Unhandled exception: ");
         video_print_int(interrupt_number);
         video_print_string("\n");
     }
 }
 
 /**
  * Generic IRQ handler - called from assembly
  */
 void irq_handler(uint32_t interrupt_number) {
     // Send EOI (End of Interrupt) signal to PICs
     if (interrupt_number >= 40) {  // If it came from the slave PIC
         outb(PIC2_COMMAND, PIC_EOI);  // Send to slave
     }
     outb(PIC1_COMMAND, PIC_EOI);  // Send to master (always)
     
     // Call the registered handler if there is one
     if (interrupt_handlers[interrupt_number]) {
         interrupt_handlers[interrupt_number](interrupt_number - 32);
     }
 }
