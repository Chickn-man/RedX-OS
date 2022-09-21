[org 0x7c00]
mov bp, 0x7c00
mov sp, bp
mov bx, Welcome
call Print

mov bx, WIP
call Print

mov bx, WIP2
call Print
hl:
  hlt
jmp hl

Welcome:
  db "Welcome to RedX OS!",0

WIP:
  db "The legacy bios bootloader for RedX OS is curently unfinished.",0

WIP2:
  db "Please boot with EFI firmware.",0
Print:
  push ax
  push bx
  mov ah, 0x0e
  .Loop:
  cmp [bx], byte 0
  je .Exit
    mov al, [bx]
    int 0x10
    inc bx
    jmp .Loop
  .Exit:
  mov al, 0xa
  int 0x10
  mov al, 0xd
  int 0x10
  pop ax
  pop bx
  ret
times 510-($-$$) db 0
dw 0xaa55