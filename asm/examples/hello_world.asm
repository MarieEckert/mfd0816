section code at 0x1100
	; The terminal device sits at address 0x1000 on the GIO Bus,
	; reading from there gets you keyboard input (or simply 0 if there is none)
	; and writing displays something!
	define TERMINAL,	0x1000

			ld	acl,	msg			; Get the address of "msg" defined in data.
	print:	out	[acl],	TERMINAL	; Write whatever lies under that address.
			inc	acl					; Go to the next character in msg.
			cmp	[acl],	0			; Continue printing if its not the NULL
			jnz	print				; terminator.
	halt:	jmp	halt				; Spin forever otherwise

section data at 0xd000
	msg:	ds	"Hello, World!" db 0	; Our message

section vectors at 0xfffc
			dw	0		; interrupt vector
			dw	code	; reset vector
