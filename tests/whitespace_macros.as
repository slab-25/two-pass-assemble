mcro   spaced_macro
    mov   r1,    #1
    inc     r1
mcroend

mcro tabbed_macro
	mov	r2,	#2
	dec	r2
mcroend

START:    mov   r7,   #0
         spaced_macro
	 tabbed_macro
END:     stop