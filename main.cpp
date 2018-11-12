#include <windows.h>
unsigned int base;

unsigned int Skip_JMP_back;
unsigned int Normal_JMP_back;
void __declspec(naked) ASMNullCheck(){
    asm("push ecx"); //original code
    asm("mov ecx,[eax+esi*4]"); //original code

    asm("test ecx, ecx"); //Is ECX null?
    asm("jnz 0f"); //jump if not null

    asm("pop ecx"); //Take that argument back off the stack
    asm("jmp [_Skip_JMP_back]");

    asm("0:");
    asm("add ecx, 0x90"); //original code
    asm("jmp [_Normal_JMP_back]");
}

void WriteJMP(BYTE* location, BYTE* newFunction){
	DWORD dwOldProtection;
	VirtualProtect(location, 5, PAGE_EXECUTE_READWRITE, &dwOldProtection);
    location[0] = 0xE9; //jmp
    *((DWORD*)(location + 1)) = (DWORD)(( (unsigned INT32)newFunction - (unsigned INT32)location ) - 5);
	VirtualProtect(location, 5, dwOldProtection, &dwOldProtection);
}

extern "C" __declspec(dllexport) bool APIENTRY DllMain(HINSTANCE hinstDLL, DWORD fdwReason, LPVOID lpvReserved)
{
    switch (fdwReason)
    {

        case DLL_PROCESS_ATTACH:
            base = (unsigned int)GetModuleHandle(NULL);

            Skip_JMP_back = base + 0x2642A;
            Normal_JMP_back = base + 0x26425;
            WriteJMP((BYTE*)(base + 0x2641B), (BYTE*)&ASMNullCheck);
            break;

    }
    return true;
}
