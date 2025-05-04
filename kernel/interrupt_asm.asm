;
; NecoOS - Interrupt Handlers (Assembly)
;
; This file contains the assembly code for interrupt service routines (ISRs)
; and IRQ handlers. These handlers save the CPU state, call the C handler,
; and then restore the state.
;

[BITS 32]

; External function declarations
extern isr_handler   ; C function to handle ISRs
extern irq_handler   ; C function to handle IRQs

; Macro to create an ISR stub that does not push an error code
%macro ISR_NOERRCODE 1
global isr%1
isr%1:
    push 0          ; Push dummy error code (for consistency with error code ISRs)
    push %1         ; Push interrupt number
    jmp isr_common  ; Jump to common handler
%endmacro

; Macro to create an ISR stub that pushes an error code
%macro ISR_ERRCODE 1
global isr%1
isr%1:
    ; Error code is already pushed by the CPU
    push %1         ; Push interrupt number
    jmp isr_common  ; Jump to common handler
%endmacro

; Macro to create an IRQ stub
%macro IRQ 2
global irq%1
irq%1:
    push 0          ; Push dummy error code
    push %2         ; Push interrupt number (32 + IRQ number)
    jmp irq_common  ; Jump to common handler
%endmacro

; CPU exceptions
ISR_NOERRCODE 0    ; Division by zero
ISR_NOERRCODE 1    ; Debug
ISR_NOERRCODE 2    ; Non-maskable interrupt
ISR_NOERRCODE 3    ; Breakpoint
ISR_NOERRCODE 4    ; Overflow
ISR_NOERRCODE 5    ; Bound range exceeded
ISR_NOERRCODE 6    ; Invalid opcode
ISR_NOERRCODE 7    ; Device not available
ISR_ERRCODE   8    ; Double fault
ISR_NOERRCODE 9    ; Coprocessor segment overrun
ISR_ERRCODE   10   ; Invalid TSS
ISR_ERRCODE   11   ; Segment not present
ISR_ERRCODE   12   ; Stack-segment fault
ISR_ERRCODE   13   ; General protection fault
ISR_ERRCODE   14   ; Page fault
ISR_NOERRCODE 15   ; Reserved
ISR_NOERRCODE 16   ; x87 FPU floating-point error
ISR_ERRCODE   17   ; Alignment check
ISR_NOERRCODE 18   ; Machine check
ISR_NOERRCODE 19   ; SIMD floating-point exception
ISR_NOERRCODE 20   ; Virtualization exception
ISR_NOERRCODE 21   ; Reserved
ISR_NOERRCODE 22   ; Reserved
ISR_NOERRCODE 23   ; Reserved
ISR_NOERRCODE 24   ; Reserved
ISR_NOERRCODE 25   ; Reserved
ISR_NOERRCODE 26   ; Reserved
ISR_NOERRCODE 27   ; Reserved
ISR_NOERRCODE 28   ; Reserved
ISR_NOERRCODE 29   ; Reserved
ISR_ERRCODE   30   ; Security exception
ISR_NOERRCODE 31   ; Reserved

; IRQs
IRQ 0, 32   ; Timer (IRQ 0)
IRQ 1, 33   ; Keyboard (IRQ 1)
IRQ 2, 34   ; Cascade for PIC2 (IRQ 2)
IRQ 3, 35   ; COM2 (IRQ 3)
IRQ 4, 36   ; COM1 (IRQ 4)
IRQ 5, 37   ; LPT2 (IRQ 5)
IRQ 6, 38   ; Floppy disk (IRQ 6)
IRQ 7, 39   ; LPT1 / spurious interrupt (IRQ 7)
IRQ 8, 40   ; CMOS real-time clock (IRQ 8)
IRQ 9, 41   ; Free / ACPI (IRQ 9)
IRQ 10, 42  ; Free (IRQ 10)
IRQ 11, 43  ; Free (IRQ 11)
IRQ 12, 44  ; PS/2 mouse (IRQ 12)
IRQ 13, 45  ; FPU / coprocessor / inter-processor (IRQ 13)
IRQ 14, 46  ; Primary ATA hard disk (IRQ 14)
IRQ 15, 47  ; Secondary ATA hard disk (IRQ 15)

; Common ISR handler
isr_common:
    ; Save all registers
    pusha             ; Pushes edi, esi, ebp, esp, ebx, edx, ecx, eax
    
    ; Save data segment
    mov ax, ds
    push eax
    
    ; Load kernel data segment
    mov ax, 0x10      ; Kernel data segment is at offset 0x10 in GDT
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    
    ; Call C handler
    call isr_handler
    
    ; Restore data segment
    pop eax
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    
    ; Restore all registers
    popa
    
    ; Clean up error code and interrupt number
    add esp, 8
    
    ; Return from interrupt
    iret

; Common IRQ handler
irq_common:
    ; Save all registers
    pusha
    
    ; Save data segment
    mov ax, ds
    push eax
    
    ; Load kernel data segment
    mov ax, 0x10
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    
    ; Call C handler
    call irq_handler
    
    ; Restore data segment
    pop eax
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    
    ; Restore all registers
    popa
    
    ; Clean up error code and interrupt number
    add esp, 8
    
    ; Return from interrupt
    iret
