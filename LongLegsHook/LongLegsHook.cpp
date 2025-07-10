#include <iostream>
#include <stdio.h>
#include "LongLegs.h"
#include <Windows.h>

#define LOG(msg) std::cout << msg << std::endl;

class TestClass {

public:
    void myfunc(int one, int two) {
        int three = 0;
        while (three < two) {
            three += 1;
        }

        std::cout << "One: " << one << " Two: " << two << " Three: " << three << std::endl;
    }

};

typedef void(__thiscall* OriginalFunction)(void* This, int one, int two);
OriginalFunction fpOriginal = NULL;

void __fastcall HookedFunction(void* This, void* EDX_, int one, int two) {

    std::cout << "Hooked" << std::endl;
    std::cout << "HEHEHEdd ddd d d" << one << " " << two << std::endl;

    fpOriginal(This, one, two);
}


void disable(HMODULE instance, FILE* f) {
    std::cout << "Aborting!!!" << std::endl;
    Sleep(4000);

    std::cout << "End." << std::endl;

    FreeConsole();
    fclose(f);

    Sleep(1000);
    FreeLibraryAndExitThread(instance, 0);
}

int main()
{
    TestClass testClass;

    float test = 0.44;
    float* ptr = &test;

    void (TestClass:: * funcPtr)(int, int) = &TestClass::myfunc;
    void* funcAddr = *(void**)&funcPtr;
    std::cout << funcAddr << std::endl;
    LongLegs hook(funcAddr, &HookedFunction, &fpOriginal, 6);
    fpOriginal = (OriginalFunction)hook.getTrampoline();

    testClass.myfunc(2, 34);
    testClass.myfunc(2, 12);
    testClass.myfunc(2, 5);

    //MessageBoxA(NULL, "Test", "test", MB_OK);
   //MessageBoxA(NULL, "Test", "test", MB_OK);


}