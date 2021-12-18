#include "lilac_hook_ios.hpp"
#include "lilac_hook_impl.hpp"

#include <mach/vm_map.h>       /* mach_vm_*            */
#include <mach/mach_init.h>     /* mach_task_self()     */

#include <signal.h>             /* sigaction            */
#include <sys/ucontext.h>       /* ucontext_t           */

extern "C" {
	extern void MSHookMemory(void* dst, const void* src, uintptr_t size);
}

using namespace lilac;
using namespace impl;

#if __DARWIN_OPAQUE_ARM_THREAD_STATE64
	#define _ios_get_reg(f, r) f.__opaque_##r
#else
	#define _ios_get_reg(f, r) f.__##r
#endif

namespace {
	void handler(int signal, siginfo_t* signal_info, void* vcontext) {
		ucontext_t* context = reinterpret_cast<ucontext_t*>(vcontext);

		const void* ret = *reinterpret_cast<void**>(_ios_get_reg(context->uc_mcontext->__ss, sp));
		const void** current = const_cast<const void**>(reinterpret_cast<void**>(&_ios_get_reg(context->uc_mcontext->__ss, pc)));

		Exception exception = {
			signal_info->si_addr,
			ret,
			*current
		};
		HookManager::handler(exception);
	}
}

void iOS::write_memory(void* dst, const void* data, size_t size) {
	MSHookMemory(dst, data, size);
    //return true;

	/*kern_return_t kret;
	uintptr_t to_ui = reinterpret_cast<uintptr_t>(to);
	uintptr_t to_shifted = to_ui & ~PAGE_MASK;
	int psize = ((to_ui + size - 1) & ~PAGE_MASK) - to_shifted + PAGE_SIZE;

	void* copy = mmap(NULL, psize, PROT_WRITE | PROT_READ, MAP_ANON | MAP_SHARED, -1, 0);
	if (copy == MAP_FAILED) {
		caclog("mmap failed!");
		return;
	} else caclog("mmap succeeded!");

	if (vm_copy(mach_task_self(), to_shifted, psize, (vm_address_t)copy)) {
		caclog("vm_copy failed!");
		return;
	} else caclog("vm_copy succeeded!");

	//mmap((void*)to_shifted, psize, PROT_NONE, MAP_SHARED | MAP_ANON | MAP_FIXED, -1, 0);
	//caclog("mmap (no protection) succeeded!");

	memcpy((void*)(reinterpret_cast<uintptr_t>(copy)+(to_ui - to_shifted)), from, size);
	caclog("memcpy succeeded!");

	mprotect(copy, psize, PROT_READ | PROT_EXEC);
	caclog("mprotect succeeded!");
	vm_prot_t a, b;
	kret = mach_vm_remap(
		mach_task_self(),
		(mach_vm_address_t*)&to_shifted,
		psize,
		0,
        VM_FLAGS_OVERWRITE,
        mach_task_self(),
        (mach_vm_address_t)copy,
        TRUE,
        &a,
        &b,
        VM_INHERIT_SHARE
    );
    if (kret) {
    	caclog("remap failed!");
    	return;
    } else caclog("remap succeeded!");
    munmap(copy, psize);
    caclog("munmap succeeded!");*/
}

bool iOS::initialize() {
	volatile int a = init;
	struct sigaction action;
	memset(&action, '\0', sizeof(action));
	action.sa_sigaction = &handler;
	action.sa_flags = SA_SIGINFO;

	int signal = SIGTRAP;

	return sigaction(signal, &action, NULL) < 0;
	//return true; // im just testing
}
