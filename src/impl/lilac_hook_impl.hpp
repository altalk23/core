#ifndef __LILAC_HOOK_IMPL_HPP__
#define __LILAC_HOOK_IMPL_HPP__

#include <vector>
#include <map>

#if defined(_WIN32)
	#include "lilac_hook_windows.hpp"
#elif defined(__APPLE__)
	#include "lilac_hook_macos.hpp"
#else
	// everything else is unsupported rn LOL
	#error Currently unsupported platform.
#endif

namespace lilac::impl {
	class HookManager {
	private:
		struct HookChain;

		struct CallFrame {
			const void* address = nullptr;
			HookChain* parent = nullptr;
			char original_bytes[TargetPlatform::get_trap_size()] = { 0 };
		};

		struct HookChain {
			const void* address = nullptr;
			std::vector<const void*> detours = {};
			std::vector<CallFrame*> frames = {};
			char original_bytes[TargetPlatform::get_trap_size()] = { 0 };
		};


	private:
		/**
		 * Cacao changes:
		 * Since camila doesn't like to dynamically link anything,
		 * this system uses weak symbols to emulate "sharedness"
		 * 
		 * Is it a good practice? No
		 * Does it work? Yes
		 * 
		 * So I don't see much problem
		 */
		WEAK static inline auto& all_hooks() {
			static auto ret = new std::map<const void*, HookChain>();
			return *ret;
		}

		WEAK static inline auto& all_frames() {
			static auto ret = new std::map<const void*, CallFrame>();
			return *ret;
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
