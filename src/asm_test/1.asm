.model small
.stack 100h
.data

.code
start:
    mov ax, @data
    mov ds, ax

    ; Call main function
    call main

    ; Exit program
    mov ax, 4C00h
    int 21h


main proc near
    ; Function prologue
    push bp
    mov bp, sp
    sub sp, 2  ; Reserve space for locals
    mov ax, 10
    mov [bp+8], ax  ; a
L0:
    mov ax, [bp+8]  ; a
    push ax
    mov ax, 0
    pop dx
    cmp dx, ax
    jne L2
    mov ax, 0
    jmp L3
L2:
    mov ax, 1
L3:
    cmp ax, 0
    je L1
    mov ax, [bp+8]  ; a
    push ax
    mov ax, 1
    pop dx
    xchg ax, dx
    sub ax, dx
    mov [bp+8], ax  ; a
    jmp L0
L1:
    ; Function epilogue
    mov sp, bp
    pop bp
    ret
main endp

end start
