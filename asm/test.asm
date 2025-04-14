addressing absolute

section code at 0x1100
	_entry:	mov	0x1000,	sp			; initialise stack pointer
			ld		acl,	[sp]	; load the word from the top of the
									; stack into acl
			ld		bcl,	[[0x1330]]
			ld		ccl,	[[bcl]]

			call	test
			jmp		_entry
	test:	ret

section data at 0xd000
					define			bufferSize, 1 ;	0x400
	buffer:			times			bufferSize
								db	0xff
	text:			ds "hello, world! \\ \" \\ \\\"" db 0

section reset_vector at 0xffe4
	ds "RESET VECTOR; NO TOUCHING!"
	reset_vector:	dw code
