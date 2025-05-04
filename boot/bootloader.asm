;
; NecoOS Bootloader
; 16-bit Real Mode bootloader that loads the kernel and switches to 32-bit protected mode
;

[BITS 16]                   ; 16-bit Real Mode
[ORG 0x7C00]                ; BIOS loads the bootloader at this memory address

; Constants
KERNEL_OFFSET equ 0x1000    ; Kernel will be loaded at this memory offset
STACK_BASE equ 0x9000       ; Stack pointer base address

; Boot entry point
boot_start:
    ; Setup segments and stack
    xor ax, ax              ; Set AX = 0
    mov ds, ax              ; Data Segment = 0
    mov es, ax              ; Extra Segment = 0
    mov ss, ax              ; Stack Segment = 0
    mov sp, STACK_BASE      ; Stack pointer = 0x9000
    
    ; Clear the screen
    mov ah, 0x00            ; Set video mode function
    mov al, 0x03            ; 80x25 text mode
    int 0x10                ; BIOS video interrupt
    
    ; Display boot message
    mov si, BOOT_MSG
    call print_string
    
    ; Load the kernel from disk
    call load_kernel
    
    ; Switch to protected mode
    call switch_to_pm
    
    ; We should never reach here
    jmp $

; Function to print a null-terminated string
; Input: SI = pointer to string
print_string:
    pusha                   ; Save all registers
    mov ah, 0x0E            ; BIOS teletype function
.loop:
    lodsb                   ; Load byte from SI into AL and increment SI
    test al, al             ; Check if AL = 0 (end of string)
    jz .done                ; If zero, we're done
    int 0x10                ; Print the character
    jmp .loop               ; Continue with next character
.done:
    popa                    ; Restore all registers
    ret

; Function to load the kernel from disk
load_kernel:
    mov si, KERNEL_LOAD_MSG
    call print_string
    
    ; Reset disk system
    xor ax, ax
    int 0x13
    
    ; Setup disk read parameters
    mov ah, 0x02            ; BIOS read disk function
    mov al, 32              ; Number of sectors to read (adjust as needed)
    mov ch, 0               ; Cylinder 0
    mov cl, 2               ; Start from sector 2 (sector 1 is the boot sector)
    mov dh, 0               ; Head 0
    mov dl, 0x80            ; Drive number (0x80 for first hard disk)
    
    ; Set buffer address for loading kernel
    mov bx, KERNEL_OFFSET
    
    ; Perform the read
    int 0x13
    
    ; Check for errors
    jc disk_error           ; Jump if carry flag is set (error)
    
    mov si, KERNEL_SUCCESS_MSG
    call print_string
    ret

; Handle disk read error
disk_error:
    mov si, DISK_ERROR_MSG
    call print_string
    jmp $                   ; Hang

; Global Descriptor Table for protected mode
gdt_start:

gdt_null:                   ; Null descriptor (required)
    dd 0x0
    dd 0x0

gdt_code:                   ; Code segment descriptor
    dw 0xFFFF               ; Limit (0-15)
    dw 0x0                  ; Base (0-15)
    db 0x0                  ; Base (16-23)
    db 10011010b            ; Flags (P=1, DPL=00, S=1, Type=1010)
    db 11001111b            ; Flags (G=1, D/B=1, L=0, AVL=0) + Limit (16-19)
    db 0x0                  ; Base (24-31)

gdt_data:                   ; Data segment descriptor
    dw 0xFFFF               ; Limit (0-15)
    dw 0x0                  ; Base (0-15)
    db 0x0                  ; Base (16-23)
    db 10010010b            ; Flags (P=1, DPL=00, S=1, Type=0010)
    db 11001111b            ; Flags (G=1, D/B=1, L=0, AVL=0) + Limit (16-19)
    db 0x0                  ; Base (24-31)

gdt_end:

gdt_descriptor:
    dw gdt_end - gdt_start - 1  ; GDT size
    dd gdt_start                ; GDT address

; GDT segment selectors
CODE_SEG equ gdt_code - gdt_start
DATA_SEG equ gdt_data - gdt_start

; Function to switch to protected mode
switch_to_pm:
    cli                     ; Disable interrupts
    
    ; Load the GDT
    lgdt [gdt_descriptor]
    
    ; Enable protected mode by setting the first bit of CR0
    mov eax, cr0
    or eax, 0x1
    mov cr0, eax
    
    ; Perform a far jump to flush the pipeline and load CS with CODE_SEG
    jmp CODE_SEG:init_pm
    
[BITS 32]                   ; From now on, we're in 32-bit protected mode

; Initialize protected mode
init_pm:
    ; Update segment registers
    mov ax, DATA_SEG
    mov ds, ax
    mov ss, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    
    ; Update stack pointer
    mov ebp, STACK_BASE
    mov esp, ebp
    
    ; Jump to the kernel
    call KERNEL_OFFSET
    
    ; We should never reach here
    jmp $

; Data
BOOT_MSG db 'NecoOS Bootloader starting...', 0x0D, 0x0A, 0
KERNEL_LOAD_MSG db 'Loading kernel into memory...', 0x0D, 0x0A, 0
KERNEL_SUCCESS_MSG db 'Kernel loaded successfully!', 0x0D, 0x0A, 0
DISK_ERROR_MSG db 'Error loading kernel from disk!', 0x0D, 0x0A, 0

; Padding and boot signature
times 510-($-$$) db 0   ; Fill the rest of the sector with zeros
dw 0xAA55               ; Boot signature
