#ifndef __LILAC_HOOK_WINDOWS_HPP__
#define __LILAC_HOOK_WINDOWS_HPP__

#include "lilac_hook_platform.hpp"

namespace lilac::impl {
	class WindowsX86 : public Platform<WindowsX86> {
	public:
	#if defined(NDEBUG)
		static constexpr char trap[] = { '\xCC' };
	#else
		static constexpr char trap[] = { '\x0F', '\x0B' };
	#endif

	public:
		static void write_memory(void* to, const void* from, size_t size);
		static bool initialize();
		static const void* align_address(const void* address) {
			return address;
		}
	};

	using TargetPlatform = Platform<WindowsX86>;
}

#endif /* __LILAC_HOOK_WINDOWS_HPP__ */
