; ************************************************************************
 ;
 ; These assembly subroutines should be linked to the C programs
 ; listed at earlier in this handout.
 ;
 ;                    NAME
 ;                      SECTION
 ;
 ;  The routines in this module demonstrate the C parameter passing
 ; Rules.
 ;               TABLE OF CONTENTS
 ;
 ;    _trnsfr
 ;    _rvrse
 ;    _asccode (an ASCII table)
 ;
 ; ************************************************************************

     include dos.mac        ;always include this line in the .asm file
                      ; dos.mac contains macros defining 'pseg',
                      ; 'dseg', etc.
     extrn _echo:far       ;all procedure addresses are far pointers
                      ; in huge memory model. Leading underscore
                      ; is REQUIRED for anything that must link
                      ; with a 'C' module.
 ; ************************************************************************
     pseg             ; Program segment
     public      _trnsfr, _rvrse  ;allow procedures outside this file to call
                      ;these functions

_trnsfr proc     far        ;far due to huge memory model
;***************************************************************************
;*  PROCEDURE trnsfr just takes the variables "passed" to it by the  main   *
;*         C program and passes them to procedure 'echo'.                 *
;*            Parameter passing in C uses the system stack.  The stack for  *
;*            this call is as follows{remember, the call was 'trnsfr(i,a)'}:*
;*                                                                          *
;*                      SYSTEM STACK:                                       *
;*                 ________________________  __                             *
;*                |                        |   \                            *
;*                |high byte of a's segment|   |                            *
;*                |_ _ _ _ _ _ _ _ _ _ _ _ |   |                            *
;*                |                        |   |                            *
;*                |  low byte, a's segment |   |   the (far) pointer to the *
;*                |________________________|    \  string is passed, not    *
;*                |                        |    /  the string itself.       *
;*                | high byte, a's offset  |   |                            *
;*                |_ _ _ _ _ _ _ _ _ _ _ _ |   |   NOTE: parameters are     *
;*                |                        |   |    pushed in reverse order *
;*                |  low byte, a's offset  |   |                            *
;*                |________________________| __/                            *
;*                |                        |                                *
;*                | high byte of i's value |                                *
;*                |_ _ _ _ _ _ _ _ _ _ _ _ |                                *
;*                |                        |                                *
;*                | low byte of i's value  |                                *
;*                |________________________| __                             *
;*                |                        |   \                            *
;*                |    high byte of CS     |    |                           *
;*                |_ _ _ _ _ _ _ _ _ _ _ _ |    |                           *
;*                |                        |    |                           *
;*                |    low byte of CS      |    |                           *
;*                |________________________|     \     Return Address to    *
;*                |                        |     /     the calling procedure*
;*                |    high byte of IP     |    |                           *
;*                |_ _ _ _ _ _ _ _ _ _ _ _ |    |                           *
;*                |                        |    |                           *
;*                |    low byte of IP      |    |  <--- SS:SP points here   *
;*                |________________________| __/                            *
;*                |                        |                                *
;*                |                        |                                *
;*                |_ _ _ _ _ _ _ _ _ _ _ _ |                                *
;*                |                        |                                *
;*                |                        |                                *
;*                |________________________|                                *
;*                                                                          *
;*  calls:                                                                  *
;*    functions : none                                                      *
;*    procedures: echo (in the C module)                                    *
;*  called by:                                                              *
;*    functions : none                                                      *
;*    procedures: externally referenced (by main() in the C module)         *
;*  parameters:                                                             *
;*             i => an integer used for nothing                             *
;*             a => a string used for nothing                               *
;*  variables:                                                       *
;*   none                                                                 *
;***************************************************************************/

     pop   ax         ;Get the instruction pointer (IP)
     mov   cs:ipsave,ax     ;save it away so that we can return to the
                      ; caller   (NOTE: 'cs:' is necessary since
                      ;                 'ipsave' is in pseg)
     pop   ax         ;Get the CS (remember, in huge model, all
                      ; addresses need CS and IP)
     mov   cs:cssave,ax    ;save it away, too
                      ; Note: by stripping away our return address
                      ;       the stack after the 'call' statement
                      ;     will be set up for C parameter
                      ;     passing (echo takes the same parameters
                      ;     passed to trnsfr)
     call  _echo      ;call the C routine 'echo'
     push  ct:cssave       ;put the return address back on the stack
     push  cs:ipsave       ;
     ret              ; so we can exit back to 'main()'
ipsave     dw    ?          ;storage for offset of return address (IP)
cssave     dw    ?          ;storage for segment of rtn address (CS)
_trnsfr    endp
param1     equ   6          ;offset into the stack to access the 1st
                            ; parameter

_rvrse     proc  far
;***************************************************************************
;*  PROCEDURE rvrse just swaps the characters in the passed string.         *
;*            The system stack for this call ('rvrse(str4)') is:            *
;*                                                                          *
;*                                                                          *
;*                      SYSTEM STACK:                                       *
;*                                                                          *
;*                                                                          *
;*                 ________________________  __                             *
;*                |                        |   \                            *
;*                |high byte of str4's seg |   |                            *
;*                |_ _ _ _ _ _ _ _ _ _ _ _ |   |                            *
;*                |                        |   |                            *
;*                |  low byte, str4's seg  |   |                            *
;*                |________________________|    \  remember that a pointer  *
;*                |                        |    /  to a string is passed    *
;*                | high byte, str4's off  |   |                            *
;*                |_ _ _ _ _ _ _ _ _ _ _ _ |   |                            *
;*                |                        |   |                            *
;*                |  low byte, str4's off  |   |                            *
;*                |________________________| __/                            *
;*                |                        |   \                            *
;*                |    high byte of CS     |    |                           *
;*                |_ _ _ _ _ _ _ _ _ _ _ _ |    |                           *
;*                |                        |    |                           *
;*                |    low byte of CS      |    |                           *
;*                |________________________|     \     Return Address to    *
;*                |                        |     /     the calling procedure*
;*                |    high byte of IP     |    |                           *
;*                |_ _ _ _ _ _ _ _ _ _ _ _ |    |                           *
;*                |                        |    |                           *
;*                |    low byte of IP      |    |  <--- SS:SP points here   *
;*                |________________________| __/                            *
;*                |                        |                                *
;*                |(high byte, original BP)|                                *
;*                |_ _ _ _ _ _ _ _ _ _ _ _ |                                *
;*                |                        |                                *
;*                |(low byte, original BP) |     <--- SS:BP will point here *
;*                |________________________|                                *
;*                                                                          *
;*                                                                          *
;*  If the first steps in any assembler program are 'push bp','mov bp,sp'   *
;*  then bp can be used to access the parameters; bp+6 reaches parameter 1, *
;*  bp+8 reaches parameter 2, etc.                                          *
;*                                                                          *
;*                                                                          *
;*  calls:                                                                  *
;*    functions : none                                                      *
;*    procedures: none                                                      *
;*  called by:                                                              *
;*    functions : none                                                      *
;*    procedures: externally referenced (by main() in the C module)         *
;*  parameters:                                                             *
;*          str4 => a string whose characters will be swapped around        *
;*  variables:                                              *
;*   none                                                                *
;***************************************************************************/
     push  bp         ;save the bp
     mov   bp,sp      ;use bp to point to the parameters
     push  es         ;save es since we alter it
     push    si       ;save si since we alter it
     push  bx         ;save bx since we alter it
     push  ax         ;save ax since we alter it
     les   si,dword ptr [bp+param1]
                      ;load es & si with the 1st argument (the
                      ; pointer to the string)
     mov   ax,es:[si] ;access first two bytes of the string
     mov   bx,es:[si+2]     ;access second two bytes
     mov   es:[si],bx ;swap the bytes
     mov   es:[si+2],ax
     pop   ax         ;restore ax (note: when restoring, the pops
                      ;             should occur in the reverse
                      ;           order of the pushes)
     pop   bx              ;restore original bx
     pop   si              ;restore original si
     pop   es              ;restore original es
     pop   bp              ;restore original bp
     ret
_rvrse     endp

     endps            ; end of Program segment

     dseg             ; Data segment

     pblic      _asccode        ;A table of ASCII code values used in
                      ; the C procedure asctab()
_asccode db 01H,01H,01H,01H,01H,01H
     db 02H,03H,04H,05H,06H,02H
     db 02H,03H,04H,05H,06H,02H
     db 02H,03H,04H,05H,06H,02H
     db 02H,03H,04H,05H,06H,02H
     db 01H,01H,01H,01H,01H,01H

     endds            ; end of data segment

     end

