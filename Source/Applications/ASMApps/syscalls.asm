[bits 32]

dd 0xFEEDBEEF		; magic number ^^
dd end - start

%define SC_PUTCH 0xFFFFFF01
%define SC_SLEEP 0xFFFFFF02
