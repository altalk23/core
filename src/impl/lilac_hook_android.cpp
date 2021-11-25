#include "lilac_hook_android.hpp"
#include "lilac_hook_impl.hpp"

#include <sys/mman.h>           /* mprotect             */

#include <signal.h>             /* sigaction            */
#include <sys/ucontext.h>       /* ucontext_t           */

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
}


void Android::write_memory(void* to, const void* from, size_t size) {
    if (mprotect((void*)((uintptr_t)to & PAGE_MASK), PAGE_SIZE, PROT_EXEC | PROT_WRITE | PROT_READ)) return;
    memcpy(to, from, size);
    if (mprotect((void*)((uintptr_t)to & PAGE_MASK), PAGE_SIZE, PROT_EXEC | PROT_READ)) return;
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
