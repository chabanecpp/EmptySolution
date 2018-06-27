#include <windows.h>

#if 0 //May not needed
typedef int(__stdcall* func_ptr_t)();//generic function pointer type
#endif

extern "C" int __cdecl samplefunc();
extern "C" int __cdecl samplefunc2(int);

int main()
{
   //https://www.tutorialspoint.com/dll/dll_quick_guide.htm


   //DLL runtime dynamic linking
   {
      typedef int(__cdecl *SAMPLEFUNC)();
      typedef int(__cdecl *SAMPLEFUNC2)(int);

	   //Load DLL
	   auto dllHandle = LoadLibrary("DllName.dll");
	   if (nullptr == dllHandle)
	   {
	      //Log : Cannot load "DllName" file
	      return EXIT_FAILURE;
	   }
	
	   //Resolve function address
	   SAMPLEFUNC samplefunc = (SAMPLEFUNC)GetProcAddress(dllHandle, "samplefunc");
	   SAMPLEFUNC2 samplefunc2 = (SAMPLEFUNC2)GetProcAddress(dllHandle, "samplefunc2");
	
	   if (nullptr != samplefunc)
	      auto ret1 = (samplefunc)();
	
	   if (nullptr != samplefunc2)
	      auto ret2 = (samplefunc2)(5);
	
	   FreeLibrary(dllHandle);
   }

   //DLL load time dynamic linking
   {
      auto ret1 = samplefunc();
      auto ret2 = samplefunc2(5);
   }
   return EXIT_SUCCESS;
}