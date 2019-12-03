; Print.s
; Student names: change this to your names or look very silly
; Last modification date: change this to the last modification date or look very silly
; Runs on LM4F120 or TM4C123
; EE319K lab 7 device driver for any LCD
;
; As part of Lab 7, students need to implement these LCD_OutDec and LCD_OutFix
; This driver assumes two low-level LCD functions
; ST7735_OutChar   outputs a single 8-bit ASCII character
; ST7735_OutString outputs a null-terminated string 

w	EQU	0		;w variable
x	EQU 4		;x variable
y	EQU	8		;y variable
z	EQU	12		;z variable
msb1 EQU 0
msb2 EQU 4
msb3 EQU 8
msb4 EQU 12
msb5 EQU 16
msb6 EQU 20
msb7 EQU 24
msb8 EQU 28
msb9 EQU 32
msb10 EQU 36

    IMPORT   ST7735_OutChar
    IMPORT   ST7735_OutString
    EXPORT   LCD_OutDec
    EXPORT   LCD_OutFix
	PRESERVE8 {TRUE}
    AREA    |.text|, CODE, READONLY, ALIGN=2
    THUMB

  

;-----------------------LCD_OutDec-----------------------
; Output a 32-bit number in unsigned decimal format
; Input: R0 (call by value) 32-bit unsigned number
; Output: none
; Invariables: This function must not permanently modify registers R4 to R11
LCD_OutDec
	PUSH{LR}
	CMP R0,#0
	BEQ Zero

	SUB SP,SP,#40		;allocates 10 entries
	
	LDR R2,=1000000000
	UDIV R1,R0,R2
	STR R1,[SP,#msb1]
	MUL R1,R1,R2
	SUB R0,R0,R1	
	
	LDR R2,=100000000
	UDIV R1,R0,R2
	STR R1,[SP,#msb2]
	MUL R1,R1,R2
	SUB R0,R0,R1
	
	LDR R2,=10000000
	UDIV R1,R0,R2
	STR R1,[SP,#msb3]
	MUL R1,R1,R2
	SUB R0,R0,R1

	LDR R2,=1000000
	UDIV R1,R0,R2
	STR R1,[SP,#msb4]
	MUL R1,R1,R2
	SUB R0,R0,R1
	
	LDR R2,=100000
	UDIV R1,R0,R2
	STR R1,[SP,#msb5]
	MUL R1,R1,R2
	SUB R0,R0,R1
	
	MOV R2, #10000
	UDIV R1,R0,R2
	STR R1,[SP,#msb6]
	MUL R1,R1,R2
	SUB R0,R0,R1
	
	MOV R2, #1000
	UDIV R1,R0,R2
	STR R1,[SP,#msb7]
	MUL R1,R1,R2
	SUB R0,R0,R1
	
	MOV R2, #100
	UDIV R1,R0,R2
	STR R1,[SP,#msb8]
	MUL R1,R1,R2
	SUB R0,R0,R1
	
	MOV R2, #10
	UDIV R1,R0,R2
	STR R1,[SP,#msb9]
	MUL R1,R1,R2
	SUB R0,R0,R1
	
	STR R0,[SP,#msb10]
	
	MOV R2,#40
	MOV R12,#0
	
Loop1
	LDR R1,[SP,#0]
	SUBS R1,R1,#0
	BEQ Decrement
	SUB R2,R2,R12
	B Loop2
	
Decrement
	ADD R12,R12,#4
	ADD SP,SP,#4
	B Loop1

Loop2
	LDR R0,[SP,#0]
	ADD R0,R0,#0x30
	PUSH{R1,R2}
	BL ST7735_OutChar
	POP{R1,R2}
	ADD SP,SP,#4
	SUB R2,R2,#4
	SUBS R2,R2,#0
	BNE Loop2
	B Done
	
Zero
	ADD R0,R0,#0x30
	BL ST7735_OutChar

Done
	POP{LR}
      BX  LR
;* * * * * * * * End of LCD_OutDec * * * * * * * *

; -----------------------LCD _OutFix----------------------
; Output characters to LCD display in fixed-point format
; unsigned decimal, resolution 0.001, range 0.000 to 9.999
; Inputs:  R0 is an unsigned 32-bit number
; Outputs: none
; E.g., R0=0,    then output "0.000 "
;       R0=3,    then output "0.003 "
;       R0=89,   then output "0.089 "
;       R0=123,  then output "0.123 "
;       R0=9999, then output "9.999 "
;       R0>9999, then output "*.*** "
; Invariables: This function must not permanently modify registers R4 to R11
LCD_OutFix
	PUSH{LR}
	SUB SP,SP,#16		;allocate space
	MOV R1,#1000
	CMP R0,R1
	BCS Special			;check if input is greater than 999
	
	MOV R2, #100
	UDIV R1,R0,R2		;100s place
	STR R1,[SP,#w]
	MUL R1,R1,R2
	SUB R0,R0,R1
	
	MOV R1,#0x2E		;decimal point
	STR R1,[SP,#x]
	
	MOV R2, #10
	UDIV R1,R0,R2		;10s place
	STR R1,[SP,#y]
	MUL R1,R1,R2
	SUB R0,R0,R1

	STR R0,[SP,#z]		;1s place
	
	POP{R0-R3}			;deallocation
	
	ADD R0,R0,#0x30
	PUSH{R0-R3}
	BL ST7735_OutChar	;output 1s place
	POP{R0-R3}
	
	MOV R0,R1			
	PUSH{R0-R3}
	BL ST7735_OutChar	;output Decimal point
	POP{R0-R3}
	
	MOV R0,R2
	ADD R0,R0,#0x30
	PUSH{R0-R3}
	BL ST7735_OutChar	;output tenths place
	POP{R0-R3}
	
	MOV R0,R3
	ADD R0,R0,#0x30
	PUSH{R0-R3}
	BL ST7735_OutChar	;output hundredths place
	POP{R0-R3}
	B NotSpecial
	
Special
	MOV R0,#0x2A
	BL ST7735_OutChar
	
	MOV R0,#0x2E
	BL ST7735_OutChar
	
	MOV R0,#0x2A
	BL ST7735_OutChar
	
	MOV R0,#0x2A
	BL ST7735_OutChar

NotSpecial
	POP{LR}
     BX   LR
 
     ALIGN
;* * * * * * * * End of LCD_OutFix * * * * * * * *

     ALIGN                           ; make sure the end of this section is aligned
     END                             ; end of file
