#include "LongLegs.h"

LongLegs::LongLegs(LPVOID targetFunction, LPVOID detourAddress, LPVOID trampolineFunction, int len) {

    std::cout << "Target function: " << targetFunction << std::endl;
    std::cout << "Detour function: " << detourAddress << std::endl;
    this->targetFunction = targetFunction;
    this->detourAddress = detourAddress;
    this->len = len;
    this->trampolineFunction = static_cast<LPVOID>(trampolineFunction);
    this->trampoline = this->trampolineHook();

}

LongLegs::~LongLegs() {
    if (!this->erased) {
        this->clear();
    }
}

LPVOID LongLegs::getTrampoline() {
    return this->trampoline;
}

bool LongLegs::clear() {
    std::cout << "Clearing..." << std::endl;
    std::cout << "1: Repatching to " << targetFunction << " from " << trampoline << std::endl;

    DWORD oldProtect;
    BOOL success = VirtualProtect(targetFunction, this->len, PAGE_EXECUTE_READWRITE, &oldProtect);

    memcpy_s(targetFunction, this->len, trampoline, this->len);;
    VirtualFree(trampoline, 0, MEM_RELEASE);

    VirtualProtect(targetFunction, this->len, oldProtect, &oldProtect);
    std::cout << "Done." << std::endl;
    this->erased = true;

    return true;
}

bool LongLegs::enable() {
    if (this->enabled) {
        return false;
    }
    else {
        this->enabled = true;
    }

#ifdef _WIN64

    DWORD oldProtect;
    BOOL success = VirtualProtect(targetFunction, this->len, PAGE_EXECUTE_READWRITE, &oldProtect);

    memset(targetFunction, 0x90, this->len);
    memcpy(LPVOID((std::uint64_t)targetFunction), detourJmpInstruction, sizeof(detourJmpInstruction));


    VirtualProtect(targetFunction, this->len, oldProtect, &oldProtect);

#else

    DWORD oldProtect;
    BOOL success = VirtualProtect(targetFunction, this->len, PAGE_EXECUTE_READWRITE, &oldProtect);

    memset(targetFunction, 0x90, this->len);
    memcpy(targetFunction, detourJmpInstruction, sizeof(detourJmpInstruction));

    VirtualProtect(targetFunction, this->len, oldProtect, &oldProtect);

#endif

    return true;

}

bool LongLegs::disable() {

    if (!this->enabled) {
        return false;
    }
    else {
        this->enabled = false;
    }

    DWORD oldProtect;
    BOOL success = VirtualProtect(targetFunction, this->len, PAGE_EXECUTE_READWRITE, &oldProtect);

    memcpy(targetFunction, trampoline, this->len);

    VirtualProtect(targetFunction, this->len, oldProtect, &oldProtect);

    return true;

}

bool LongLegs::hookFunc() {
#ifdef _WIN64
    if (!(this->len >= 13)) {
        return false;
    }

    memcpy(detourJmpInstruction + 2, &detourAddress, 8);

    std::cout << "JMP instruction addr: " << (LPVOID)&detourJmpInstruction << std::endl;

    std::cout << "Press VK_DOWN key to continue" << std::endl;

#else

    if (!(this->len >= 5)) {
        return false;
    }

    const std::uint32_t relayOffset = reinterpret_cast<std::uint32_t>(detourAddress) - reinterpret_cast<std::uint32_t>(targetFunction) - 5;

    std::cout << "Detour Relay Offset: " << relayOffset << std::endl;

    memcpy(detourJmpInstruction + 1, &relayOffset, 4);

    std::cout << "JMP instruction addr: " << &detourJmpInstruction << std::endl;


#endif

    return this->enable();

}

LPVOID LongLegs::trampolineHook() {

#ifdef _WIN64

    LPVOID trampoline = (char*)VirtualAlloc(0, this->len + 13, MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE);

    const std::uint64_t jumpAddress = reinterpret_cast<std::uint64_t>(targetFunction) + this->len;

    std::cout << "Trampoline: " << trampoline << std::endl;
    std::cout << "JumpAddress Offset: " << jumpAddress << std::endl;

    memcpy_s(trampoline, this->len, targetFunction, this->len);

	/*
    Im aware that this instructions can cause crashes in x64. But i have currently no time. The fix would be to push r10
	onto the stack and releasing it after the jump. But this would increase the size of the jmp payload (jmpInstruction) and also the 
	anmount of corrupted bytes that need to get saved when patching the payload. Therefore i might just completly rewrite this. All of this is really old
    */
    std::uint8_t jmpInstruction[13] = {
        0x49, 0xBA, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, //mov r10, addr
        0x41, 0xFF, 0xE2 //jmp r10
    };
    memcpy(jmpInstruction + 2, &jumpAddress, 8);
    memcpy((LPVOID)((std::uint64_t)trampoline + this->len), &jmpInstruction, sizeof(jmpInstruction));

    VirtualProtect(trampoline, this->len + 13, PAGE_EXECUTE_READ, NULL);
#else

    LPVOID trampoline = (char*)VirtualAlloc(0, this->len + 5, MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE);

    const std::uint32_t relayOffset = reinterpret_cast<std::uint32_t>(targetFunction) - reinterpret_cast<std::uint32_t>(trampoline) - this->len;

    std::cout << "Trampoline: " << trampoline << std::endl;
    std::cout << "Trampoline Relay Offset: " << relayOffset << std::endl;

    memcpy_s(trampoline, this->len, targetFunction, this->len);

    std::uint8_t jmpInstruction[5] = { 0xE9, 0x0, 0x0, 0x0, 0x0 };
    memcpy(jmpInstruction + 1, &relayOffset, 4);
    memcpy((LPVOID)((std::uint32_t)trampoline + this->len), &jmpInstruction, sizeof(jmpInstruction));

    VirtualProtect(trampoline, this->len + 5, PAGE_EXECUTE_READ, NULL);

#endif // 

    if (hookFunc()) {
        return trampoline;
    }

    return 0;
}