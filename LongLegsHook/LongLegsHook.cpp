#include <iostream>
#include <stdio.h>
#include "LongLegs.h"
#include <Windows.h>

#define LOG(msg) std::cout << msg << std::endl;

typedef int(__stdcall* OriginalFunction)(HWND hwnd, LPCSTR text, LPCSTR caption, UINT type);
OriginalFunction fpOriginal = NULL;

int __stdcall HookedFunction(HWND hwnd, LPCSTR text, LPCSTR caption, UINT type) {

    std::cout << "Hooked" << std::endl;
    
    return fpOriginal(hwnd, "Modified", "Modified", type);
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

    void* funcAddr = &MessageBoxA;
    std::cout << funcAddr << std::endl;
    LongLegs hook(funcAddr, &HookedFunction, &fpOriginal, 5);
    fpOriginal = (OriginalFunction)hook.getTrampoline();

    MessageBoxA(NULL, "Not modified", "Not modified", MB_OK);
    MessageBoxA(NULL, "Not modified", "Not modified", MB_OK);
    //hook.clear();

    //MessageBoxA(NULL, "Test", "test", MB_OK);
   //MessageBoxA(NULL, "Test", "test", MB_OK);


}