#pragma once
#include <iostream>
#include <stdio.h>
#include <Windows.h>


class LongLegs
{
    LPVOID targetFunction;
    LPVOID detourAddress;
    LPVOID trampolineFunction;
    LPVOID trampoline;
    bool enabled;
#ifdef _WIN64
    std::uint8_t detourJmpInstruction[13] = {
    0x49, 0xBA, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, //mov r10, addr
    0x41, 0xFF, 0xE2 //jmp r10
    };
#else
    std::uint8_t detourJmpInstruction[5] = { 0xE9, 0x0, 0x0, 0x0, 0x0 };
#endif // 


    int len;
    bool erased = false;

public:
    LongLegs(LPVOID targetFunction, LPVOID detourAddress, LPVOID trampolineFunction, int len);

    LPVOID getTrampoline();

    bool clear();

    ~LongLegs();

    bool enable();

    bool disable();

private:

    bool hookFunc();

    LPVOID trampolineHook();

};

