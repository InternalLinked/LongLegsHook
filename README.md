# LongLegsHook
Simple ASM function hooking library written in C++ for x64/x32

Edit: okay i just found out that x64 is not working. The reason is probably that i forgot to push the r10 register onto the stack 
before writing it with the relative jump offset and then poping r10 afterwards to retrieve the original value.
This code here is extremely old. I might just completely rewrite it.



(also just for my application)
