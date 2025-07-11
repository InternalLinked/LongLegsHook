# LongLegsHook
Simple ASM function hooking library written in C++ for x64/x32

Edit: okay i just found out that x64 is not working. The reason is probably that i forgot to push the r15 register onto the stack 
before writing it with the relative jump offset and then poping r15 afterwards to retrieve the old value.
 This code here is extremely old. I might just completely rewrite it.



(also just for my application)
