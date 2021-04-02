; My first program
mov  a, 6
mov  b, 6 
mov  c, 4
mov	 d,a ;dsf
call function ;sdf
jmp	mitak
mitak0: msg  '(', d, ' * ', d, ' * ', b, ' * ', c, ") = ", a   ;output message
end

mitak:
	sub a,a
	msg	'Hello Workld NAzar\n'
	jmp	mitak0

function:
    mul  a, a
	mul  a, b
	mul  a, c
    ret