// for making sure function declarations match easier
#include "lilac_hook.hpp"
#include "impl/lilac_hook_impl.hpp"

using namespace lilac;

void LILAC_CALL Memory::write(void* to, const void* from, size_t size) {
	return lilac::impl::TargetPlatform::write_memory(to, from, size);
}

HookHandle LILAC_CALL Hooks::add(const void* address, const void* detour) {
	auto align = lilac::impl::TargetPlatform::align_address(address);
	auto align2 = lilac::impl::TargetPlatform::align_address(detour);
	return impl::HookManager::add_hook(align, align2);
}

bool LILAC_CALL Hooks::remove(HookHandle handle) {
	return impl::HookManager::remove_hook(handle);
}

extern "C" LilacHookHandle LILAC_CALL lilac_add_hook(const void* address, const void* detour) {
	auto align = lilac::impl::TargetPlatform::align_address(address);
	auto align2 = lilac::impl::TargetPlatform::align_address(detour);
	return impl::HookManager::add_hook(align, align2);
}

extern "C" bool LILAC_CALL lilac_remove_hook(LilacHookHandle handle) {
	return impl::HookManager::remove_hook(handle);
}
