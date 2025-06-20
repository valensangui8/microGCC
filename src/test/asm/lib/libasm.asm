; ==========================================
; file:  libprint.asm      (NASM, ELF-64)
; ==========================================

        global  print_number
        global  print_str
	global init_array
; ───────────────────────────────────────────
; print_number
;   Prints the 64-bit *signed* integer passed
;   by the caller (via stack) and returns the
;   number of bytes written in EAX/RAX.
;   Stack on entry:
;       [rsp]      return address
;       [rsp+8]    int64 number
; ───────────────────────────────────────────
section .text
print_number:
        push    rbp
        mov     rbp, rsp
        push    rbx                    ; callee-saved we will use

        mov     rax, [rbp+16]          ; fetch the argument (int64)

        ; ---- prepare buffer (21 bytes is enough for sign + 20-digit int64)
        lea     rsi, [rel _buf+21]     ; rsi -> end of buffer
        mov     byte [rsi], 0          ; null terminator
        mov     rbx, 10                ; divisor
        xor     ecx, ecx               ; ecx=0 -> positive unless we set it later

        test    rax, rax               ; number negative?
        jns     .convert
        neg     rax
        mov     cl, 1                  ; remember sign

.convert:                               ; convert digits (backwards)
        xor     rdx, rdx
        div     rbx                    ; rax=quotient, rdx=remainder
        dec     rsi
        add     dl, '0'
        mov     [rsi], dl
        test    rax, rax
        jnz     .convert

        cmp     cl, 1                  ; need a minus?
        jne     .ready
        dec     rsi
        mov     byte [rsi], '-'

.ready:
        ; ---- write(1, rsi, len)
        mov     rax, 1                 ; syscall write
        mov     rdi, 1                 ; STDOUT
        mov     rdx, _buf+21
        sub     rdx, rsi               ; length = end - start
        syscall

        pop     rbx
        pop     rbp
        ret                            ; RAX already contains bytes written

; ───────────────────────────────────────────
; print_str
;   Prints an arbitrary buffer.
;   Stack on entry:
;       [rsp]      return address
;       [rsp+8]    pointer to buffer (char*)
;       [rsp+16]   size   (int64)
;   Returns bytes written in RAX.
; ───────────────────────────────────────────
print_str:
        push    rbp
        mov     rbp, rsp

        mov     rax, 1                 ; syscall write
        mov     rdi, 1                 ; STDOUT
        mov     rsi, [rbp+16]          ; ptr
        mov     rdx, [rbp+24]          ; size
        syscall

        pop     rbp
        ret

; ───────────────────────────────────────────


; ───────────────────────────────────────────
; init_array
; Inicializa un arreglo con el mensaje, hasta maxlen
; Stack on entry:
;   [rsp]       return address
;   [rsp+8]     char* arr
;   [rsp+16]    long maxlen
; ───────────────────────────────────────────
init_array:
    push rbp
    mov rbp, rsp
    push rbx
    push rcx
    push rdx
    push r8

    mov rbx, [rbp+16]     ; arr pointer
    mov rcx, [rbp+24]     ; max length
    xor r8, r8            ; index = 0

.copy_loop:
    cmp rcx, 1
    jl .done              ; if maxlen < 1, skip

    mov al, [message + r8]
    cmp al, 0
    je .terminate

    mov [rbx + r8], al
    inc r8
    dec rcx
    jmp .copy_loop

.terminate:
    mov byte [rbx + r8], 0
    jmp .cleanup

.done:
    ; if we reached maxlen, force null-termination
    mov byte [rbx + r8], 0

.cleanup:
    pop r8
    pop rdx
    pop rcx
    pop rbx
    pop rbp
    ret



section .data
message db "Hola, bienvenidos a microGCC", 10, 0


section .bss
_buf    resb    22                     ; sign + 20 digits + terminator

