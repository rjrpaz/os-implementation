
;;
;; add this to export sections

EXPORT Enable_Paging
EXPORT flushTLB


;;
;; Add this to code section

;
; Start paging
;	load crt3 with the passed page directory pointer
;	enable paging bit in cr2
align 8
Enable_Paging:
	mov	eax, [esp+4]
	mov	cr3, eax
	mov	eax, cr3
	mov	cr3, eax
	mov	ebx, cr0
	or	ebx, 0x80000000
	mov	cr0, ebx
	ret


;
; Change PDBR 
;	load crt3 with the passed page directory pointer
align 8
SetPDBR:
	mov	eax, [esp+4]
	mov	cr3, eax
	ret

;
; flush TLB - just need to re-load cr3 to force this to happen
;
align 8
flushTLB:
	mov	eax, cr3
	mov	cr3, eax
	ret

