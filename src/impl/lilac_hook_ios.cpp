#include "lilac_hook_ios.hpp"
#include "lilac_hook_impl.hpp"

#include <mach/vm_map.h>       /* mach_vm_*            */
#include <mach/mach_init.h>     /* mach_task_self()     */

#include <signal.h>             /* sigaction            */
#include <sys/ucontext.h>       /* ucontext_t           */

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

void iOS::write_memory(void* to, const void* from, size_t size) {
	kern_return_t ret;
	
	ret = vm_protect(mach_task_self(), (mach_vm_address_t)to, size, FALSE, VM_PROT_COPY | VM_PROT_EXECUTE | VM_PROT_WRITE | VM_PROT_READ);
	if (ret != KERN_SUCCESS) return;

	ret = vm_write(mach_task_self(), (mach_vm_address_t)to, (vm_offset_t)from, (mach_msg_type_number_t)size);
}

bool iOS::initialize() {
	volatile int a = init; /* epic bugfixing */
	struct sigaction action;
	memset(&action, '\0', sizeof(action));
	action.sa_sigaction = &handler;
	action.sa_flags = SA_SIGINFO;

	int signal = SIGTRAP;

	return sigaction(signal, &action, NULL) < 0;
}
