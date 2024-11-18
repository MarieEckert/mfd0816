extern _start

include "bc16.inc"

section text at 0x1100
addressing absolute
_reset:               mov 0x1000, sp            ; initialise stack pointer
                      sti                       ; enable interrupts

                      ; initialise the bc16
                      stb                       ; set the i/o bus to byte mode
                      out BC16_CT_VAL, DC16_CT_ADDR ; enter control mode
                      out 0x2, BC16_SET_WORDSIZE    ; set wordsize to two
                      clb ; set the i/o bus mode back to word mode, since bc16
                          ; now expects 16-bits per write

                      out BC16_CT_VAL, DC16_CT_ADDR ; leave control mode

                      ; jump into the user program
                      jmp _start

section vecs at 0xfffc
interrupt_vector:     dw isr
reset_vector:         dw _reset
