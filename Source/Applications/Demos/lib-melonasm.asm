[bits 32]

%ifidn __OUTPUT_FORMAT__, bin
; create a MelonBinary output

%define MEM_ORIGIN 0x10000000

dd 0xFEEDBEEF		; magic number ^^
dd end - start
dd MEM_ORIGIN

; the ($-$$) permits not taking into account the header above
[org MEM_ORIGIN - ($-$$)]

%elifidn __OUTPUT_FORMAT__, elf
; create an elf object

[global start]

%endif

%define SC_PUTCH 0xFFFFFF01
%define SC_SLEEP 0xFFFFFF02
%define SC_WHEX 0xFFFFFF03
