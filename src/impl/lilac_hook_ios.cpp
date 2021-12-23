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

		const void* ret = reinterpret_cast<void*>(_ios_get_reg(context->uc_mcontext->__ss, lr));

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
}

bool iOS::initialize() {
	volatile int a = init;
	struct sigaction action;
	memset(&action, '\0', sizeof(action));
	action.sa_sigaction = &handler;
	action.sa_flags = SA_SIGINFO;

	int signal = SIGTRAP;

	return sigaction(signal, &action, NULL) < 0;
}
