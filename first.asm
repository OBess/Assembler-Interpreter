; My first program
mov  a, 6
mov  b, 6 
mov  c, 4
mov  f, 13
mov  g, 10
mov	 d,a ;dsf
call function ;sdf
jmp	mitak
mitak0:
msg  '(', d, ' * ', d, ' * ', b, ' * ', c, ") = ", a, "\n"   ;output message
call dimas
msg 'Hello Dimas\n'

sub  g, f
msg  '\nResult: ',g
end

mitak:
	msg	'Hello World Nazar\n'
	jmp	mitak0

function:
    mul  a, a
	mul  a, b
	mul  a, c
    ret
	
dimas:
	msg 'I`m here!','\n'
	ret