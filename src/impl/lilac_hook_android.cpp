#include "lilac_hook_android.hpp"
#include "lilac_hook_impl.hpp"

#include <sys/mman.h>           /* mprotect             */

#include <signal.h>             /* sigaction            */
#include <sys/ucontext.h>       /* ucontext_t           */

#include <android/log.h>

using namespace lilac;
using namespace impl;

namespace {
	void handler(int signal, siginfo_t* signal_info, void* vcontext) {
		ucontext_t* context = reinterpret_cast<ucontext_t*>(vcontext);

		const void* ret = *reinterpret_cast<void**>(context->uc_mcontext.arm_sp);
		const void** current = reinterpret_cast<const void**>(&context->uc_mcontext.arm_pc);

		Exception exception = {
			signal_info->si_addr,
			ret,
			*current
		};

		HookManager::handler(exception);
	}

	template <typename T>
	T get_page(T ptr) {
		return (void*)((uintptr_t)ptr & PAGE_MASK);
	}

	template <typename T>
	T remove_bit0(T ptr) {
		return (void*)((uintptr_t)ptr & -2); 
	}
}


void Android::write_memory(void* to, const void* from, size_t size) {
	auto to_page = ::get_page(to);
	// auto from_page = ::get_page(from);
	__android_log_print(ANDROID_LOG_DEBUG, "[DEBUG] cacao -- ", "idk %p, %p, %d, %p, %p", to, from, size, *(void**)::remove_bit0(to), *(void**)::remove_bit0(from));
	// if (mprotect(to_page, PAGE_SIZE, PROT_WRITE) < 0) return;
	__android_log_print(ANDROID_LOG_DEBUG, "[DEBUG] cacao -- ", "prot1");
	// std::memcpy(::remove_bit0(to), ::remove_bit0(from), size);
	__android_log_print(ANDROID_LOG_DEBUG, "[DEBUG] cacao -- ", "memcpy");
	// if (mprotect(to_page, PAGE_SIZE, PROT_EXEC | PROT_READ) < 0) return;
	__android_log_print(ANDROID_LOG_DEBUG, "[DEBUG] cacao -- ", "prot2");
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
