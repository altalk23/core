#ifndef __LILAC_HOOK_IMPL_HPP__
#define __LILAC_HOOK_IMPL_HPP__

#include <cstddef>
#include <vector>
#include <unordered_map>
#include "lilac_hook_allocator.hpp"

#if defined(_WIN32)
	#include "lilac_hook_windows.hpp"
#elif defined(__arm__)
	#include "lilac_hook_android.hpp"
#elif defined(__APPLE__)
	#if defined(__arm64__)
		#include "lilac_hook_ios.hpp"
	#else
		#include "lilac_hook_macos.hpp"
	#endif
#else
	#error Currently unsupported platform.
#endif

namespace lilac::impl {
	class HookManager {
	private:
		struct HookChain;

		struct CallFrame {
			const void* address;
			HookChain* parent;
			char original_bytes[TargetPlatform::get_trap_size()] = { 0 };
			CallFrame() : address(nullptr), parent(nullptr) { }
		};

		struct HookChain {
			const void* address;
			std::vector<const void*> detours;
			std::vector<CallFrame*> frames;
			char original_bytes[TargetPlatform::get_trap_size()] = { 0 };
			HookChain() : address(nullptr), detours(), frames() {
				detours.reserve(16);
				frames.reserve(16);
			}
		};


	private:
		static inline auto& all_hooks() {
			static auto ret = std::unordered_map<const void*, HookChain>();
			return ret;
		}

		static inline auto& all_frames() {
			static auto ret = std::unordered_map<
				const void*, 
				CallFrame, 
				std::hash<const void*>,
				std::equal_to<const void*>,
				lilac_allocator<std::pair<const void* const, CallFrame> > 
			>();
			ret.rehash(frame_size);
			return ret;
		}

	private:
		/* these don't check char buffer bounds. it should have sizeof(trap) size.
		* pass nullptr to add_trap if you don't want to save the old bytes into a buffer.
		*/

		static void add_trap(const void* address, char buffer[]);
		static void remove_trap(const void* address, char buffer[]);

		static bool find_in_hooks(Exception& info);
		static bool find_in_frames(Exception& info);

	public:
		// returns true if handled, false if not.
		static bool handler(Exception& info);

		static HookHandle LILAC_CALL add_hook(const void* address, const void* detour);
		static bool LILAC_CALL remove_hook(HookHandle handle);
	};
}

#endif /* __LILAC_HOOK_IMPL_HPP__ */
