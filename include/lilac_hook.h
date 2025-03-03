#ifndef __LILAC_HOOK_H__
#define __LILAC_HOOK_H__

#ifdef __cplusplus
extern "C" {
#else
typedef _Bool bool;
#endif

#ifdef LILAC_CALL
	#undef LILAC_CALL
#endif

#if defined(_WIN32)
	// idk maybe clean platform stuff more
	#define LILAC_CALL __stdcall
	#define VISIBLE __declspec(dllexport)
#else
	#define LILAC_CALL 
	#define VISIBLE __attribute__((visibility("default")))
// #else
// 	// everything else is unsupported rn LOL
// 	#error Currently unsupported platform.
#endif



typedef const void* LilacHookHandle;

/* 
* params:
* address - a pointer to the virtual memory to be hooked.
* detour - a pointer to the function to redirect the instruction pointer to.
* 
* returns:
* null if failed.
* a valid, constant pointer to a hook handle if succeeded.
* 
* notes:
* the hooked function should have the same calling convention and parameters
* as the detour. otherwise, crashing is almost certain to occur.
*/
LilacHookHandle LILAC_CALL lilac_add_hook(const void* address, const void* detour);

/*
* params:
* handle - a pointer to the hook handle to be removed.
* 
* returns:
* true if the hook was successfully removed.
* false if removal failed.
*/
bool LILAC_CALL lilac_remove_hook(LilacHookHandle handle);

#ifdef __cplusplus
}
#endif

#endif /* __LILAC_HOOK_H__ */
