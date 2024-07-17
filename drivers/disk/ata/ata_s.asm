dd_dcr equ 0
dd_stLBA equ 16
dd_prtlen equ 48
dd_sbits equ 80
dd_tf equ 0 ; ??? я хз че это, какой-то task file

; global read_ata_st
read_ata_st:
	push edx
	push ecx
	push eax
	test ebx, ebx
	js short .reset
	cmp ebx, 0x3fffff
	stc
	jg short .r_don
	mov edx, [esi + dd_prtlen]
	dec edx
	cmp edx, ebp
	jb short .r_don
	cmp edx, ebx
	jb short .r_don
	sub edx, ebx
	inc edx
	cmp edx, ebp
	jb short .r_don
	mov dx, [esi + dd_dcr]
	in al, dx
	test al, 0x88
	je short .stat_ok
.reset:
	call srst_ata_st
	test ebx, ebx
	jns short .stat_ok
	xor ebx, ebx
	jmp short .r_don
.stat_ok:
	cmp ebp, 0xfffffff
	jg short .setreg
	mov eax, [esi + dd_stLBA]
	cmp eax, 0xfffffff
	jg short .setreg
	add eax, ebp
	cmp eax, 0xfffffff
	jg short .setreg
	add eax, ebx
	cmp eax, 0xfffffff
.setreg:
	mov dx, [esi + dd_tf]
	jle short .read28
.read48:
	test ebx, ebx
	je short .r_don
	call pio48_read
	je short .read48
	jmp short .r_don
.read28:
	test ebx, ebx
	je short .r_don
	call pio28_read
	je short .read28
.r_don:
	pop eax
	pop ecx
	pop edx
	ret

pio28_read:
	add ebp, [esi + dd_stLBA]
	mov ecx, ebp
	mov al, bl
	or dl, 2
	out dx, al
	mov al, cl
	inc edx
	out dx, al
	mov al, ch
	inc edx
	out dx, al
	bswap ecx
	mov al, ch
	inc edx
	out dx, al
	mov al, cl
	or al, byte [esi + dd_sbits]
	inc edx
	out dx, al

	inc edx
	mov al, 0x20
	out dx, al
	mov ecx, 4
.lp1:
	in al, dx
	test al, 0x80
	jne short .retry
	test al, 8
	jne short .data_rdy
.retry:
	dec ecx
	jg short .lp1
.pior_l:
	in al, dx
	test al, 0x80
	jne short .pior_l
	test al, 0x21
	jne short .fail
.data_rdy:
	sub dl, 7
	mov cx, 256
	rep insw
	or dl, 7
	in al, dx
	in al, dx
	in al, dx
	in al, dx
	inc ebp
	dec ebx
	test bl, bl
	jne short .pior_l

	sub dx, 7
	sub ebp, [esi + dd_stLBA]
	test al, 0x21
	je short .done
.fail:
	stc
.done:
	ret

pio48_read:
	xor eax, eax
	add ebp, [esi + dd_stLBA]
	adc ah, 0
	mov ecx, ebp
	bswap ecx
	or dl, 2
	mov al, bh
	out dx, al
	mov al, cl
	inc edx
	out dx, al
	inc edx
	mov al, ah
	out dx, al
	inc edx
	mov al, 0
	out dx, al

	sub dl, 3
	mov al, bl
	out dx, al
	mov ax, bp
	inc edx
	out dx, al
	mov al, ah
	inc edx
	out dx, al
	mov al, ch
	inc edx
	out dx, al

	mov al, byte [esi + dd_sbits]
	inc edx
	and al, 0x50
	out dx, al

	inc edx
	mov al, 0x24
	out dx, al
	mov ecx, 4
.lp1:
	in al, dx
	test al, 0x80
	jne short .retry
	test al, 8
	jne short .data_rdy
.retry:
	dec ecx
	jg short .lp1
.pior_l:
	in al, dx
	test al, 0x80
	jne short .pior_l
	test al, 0x21
	jne short .fail
.data_rdy:
	sub dl, 7
	mov cx, 256
	rep insw
	or dl, 7
	in al, dx
	in al, dx
	in al, dx
	in al, dx
	inc ebp
	dec ebx
	test bx, bx
	jne short .pior_l

	sub dx, 7
	sub ebp, [esi + dd_stLBA]
	test al, 0x21
	je short .done
.fail:
	stc
.done:
	ret

srst_ata_st:
	push eax
	mov al, 4
	out dx, al
	xor eax, eax
	out dx, al
	in al, dx
	in al, dx
	in al, dx
	in al, dx
.rdylp:
	in al, dx
	and al, 0xc0
	cmp al, 0x40
	jne short .rdylp
	pop eax
	ret