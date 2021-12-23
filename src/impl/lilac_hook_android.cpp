#include "lilac_hook_android.hpp"
#include "lilac_hook_impl.hpp"

#include <sys/mman.h>           /* mprotect             */

#include <signal.h>             /* sigaction            */
#include <sys/ucontext.h>       /* ucontext_t           */

using namespace lilac;
using namespace impl;

namespace {
	template <typename T>
	T remove_bit0(T ptr) {
		return (T)((uintptr_t)ptr & -2); 
	}

	void handler(int signal, siginfo_t* signal_info, void* vcontext) {
		ucontext_t* context = reinterpret_cast<ucontext_t*>(vcontext);

		const void* ret = reinterpret_cast<void*>(context->uc_mcontext.arm_lr);
		const void** current = reinterpret_cast<const void**>(&context->uc_mcontext.arm_pc);

		Exception exception = {
			remove_bit0(signal_info->si_addr),
			remove_bit0(ret),
			*current
		};

		HookManager::handler(exception);
	}

	template <typename T>
	T get_page(T ptr) {
		return (void*)((uintptr_t)ptr & PAGE_MASK);
	}

	
}


void Android::write_memory(void* to, const void* from, size_t size) {
	auto to_page = ::get_page(to);

	mprotect((void *)to_page, PAGE_SIZE, PROT_READ | PROT_WRITE | PROT_EXEC);

	memcpy(::remove_bit0(to), from, size);

	// mprotect((void *)to_page, PAGE_SIZE, PROT_READ | PROT_EXEC);

	__builtin___clear_cache((char*)to, (char*)((size_t)to + size));
}

bool Android::initialize() {
	volatile int a = init; /* epic bugfixing */
	struct sigaction action;
	memset(&action, '\0', sizeof(action));
	action.sa_sigaction = &handler;
	action.sa_flags = SA_SIGINFO;

	int signal = SIGTRAP;

	return sigaction(signal, &action, NULL) < 0;
}

const void* Android::align_address(const void* address) {
	return ::remove_bit0(address);
}
