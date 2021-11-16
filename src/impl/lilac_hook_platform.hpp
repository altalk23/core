#ifndef __LILAC_HOOK_PLATFORM_HPP__
#define __LILAC_HOOK_PLATFORM_HPP__

/* platform template for platforms, lol
* in a different header to avoid cyclic dependencies
*/
#include "lilac_hook.hpp"

#include <cstddef>

namespace lilac::impl {
	template<typename T>
	class Platform {
	public:
		static constexpr const char* get_trap() {
			return T::trap;
		}

		static constexpr size_t get_trap_size() {
			return sizeof(T::trap);
		}

		static void write_memory(void* to, const void* from, size_t size) {
			return T::write_memory(to, from, size);
		}

		VISIBLE static inline auto& initialized() {
			static auto ret = new bool(false);
			return *ret;
		}

		static bool initialize() {
			if (initialized()) return true;
			return initialized() = T::initialize();
		}

		static const inline bool init = initialize();
	};

	struct Exception {
		const void* address = nullptr;
		const void* return_address = nullptr;
		const void*& instruction_pointer;
	};
}

#endif /* __LILAC_HOOK_PLATFORM_HPP__ */
