#include <windows.h>

#if 0 //May not needed
typedef int(__stdcall* func_ptr_t)();//generic function pointer type
#endif

int main()
{
   //Load DLL
   auto dllHandle = LoadLibrary("AcquisitionStubDll.dll");
   if (nullptr == dllHandle)
   {
      //Log : Cannot load "DllName" file
      return EXIT_FAILURE;
   }

   //Resolve function address
   GetProcAddress(dllHandle,"Exported_DLL_Func");

   FreeLibrary(dllHandle);
   return EXIT_SUCCESS;
}