#ifndef __LILAC_HOOK_ANDROID_HPP__
#define __LILAC_HOOK_ANDROID_HPP__

#include "lilac_hook_platform.hpp"

namespace lilac::impl {
	class Android : public Platform<Android> {
	public:
		/* Arm Thumb Breakpoint */
		static constexpr char trap[] = { '\x01', '\xDE'};
	public:
		static void write_memory(void* to, const void* from, size_t size);
		static bool initialize();
		static const void* align_address(const void* address);
	};

	using TargetPlatform = Platform<Android>;


}

#endif /* __LILAC_HOOK_ANDROID_HPP__ */
