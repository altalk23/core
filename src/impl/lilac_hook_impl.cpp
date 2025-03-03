#include "lilac_hook_impl.hpp"

using namespace lilac;
using namespace impl;

namespace {
	struct Handle {
		const void* address;
		const void* detour;
	};
}

void HookManager::add_trap(const void* address, char buffer[]) {
	void* addr = const_cast<void*>(TargetPlatform::align_address(address));
	if (buffer != nullptr) {
		TargetPlatform::write_memory(buffer, addr, TargetPlatform::get_trap_size());
	}
	TargetPlatform::write_memory(addr, TargetPlatform::get_trap(), TargetPlatform::get_trap_size());
}

void HookManager::remove_trap(const void* address, char buffer[]) {
	void* addr = const_cast<void*>(TargetPlatform::align_address(address));
	TargetPlatform::write_memory(addr, buffer, TargetPlatform::get_trap_size());
}

bool HookManager::find_in_hooks(Exception& info) {

	auto pair = all_hooks().find(info.address);
	if (pair != all_hooks().end()) {
		auto& hook = pair->second;

		// add a frame hook for cleanup
		auto frame_pair = all_frames().find(info.return_address);
		if (frame_pair != all_frames().end()) {
			if (frame_pair->second.parent != &hook) {
				// tail call in another HookChain, we need to clean up
			}
		}
		else {
			auto result = all_frames().insert({ info.return_address, CallFrame() });
			if (!result.second) {
				// insertion failed
				return false;
			}
			else {
				frame_pair = result.first;
				/* we only want to add if it's not a tail call. 
				* if we add in a tail call, the original bytes will be
				* the trap instruction and it'll mess up.
				*/
				add_trap(info.return_address, frame_pair->second.original_bytes);
			}
		}

		auto& frame = frame_pair->second;
		frame.parent = &hook;
		frame.address = info.return_address;


		hook.frames.push_back(&frame);

		// redirect to next detour
		info.instruction_pointer = hook.detours[hook.frames.size() - 1];
		
		// specialization for last detour: original calls don't call next detour
		if (hook.frames.size() == hook.detours.size()) {
			remove_trap(info.address, hook.original_bytes);
		}
		return true;
	}
	else {
		return false;
	}
}

bool HookManager::find_in_frames(Exception& info) {
	auto pair = all_frames().find(info.address);
	if (pair != all_frames().end()) {
		HookChain& hook = *pair->second.parent;

		// specialization for last frame: need to replace trap
		if (hook.frames.size() == hook.detours.size()) {
			add_trap(hook.address, nullptr);
		}
		
		// pop each tail call
		size_t count = 1;
		if (hook.frames.size() > 1) {
			for (auto i = hook.frames.rbegin() + 1;
				i != hook.frames.rend();
				++i) {
				if ((*i)->address != info.address) break;
				++count;
			}
		}

		hook.frames.resize(hook.frames.size() - count);

		remove_trap(info.address, pair->second.original_bytes);
		all_frames().erase(pair);

		return true;
	}
	else {

		return false;
	}
}

bool HookManager::handler(Exception& info) {
	return (find_in_hooks(info) || find_in_frames(info));
}


HookHandle HookManager::add_hook(const void* address, const void* detour) {
	auto& dummy = all_frames();
	auto& hook = all_hooks()[address];
	
	auto& detours = hook.detours;
	auto i = std::find(detours.begin(), detours.end(), detour);
	if (i != detours.end()) {
		/* we're not allowing one detour to be assigned
		* in the hook chain multiple times.
		*/
		return nullptr;
	}
	else {
		if (detours.empty()) {
			// add trap instruction if this is the first detour to be added.
			add_trap(address, hook.original_bytes);
			hook.address = address;
		}

		detours.push_back(detour);

		return new Handle{ address, detour };
	}
}

bool HookManager::remove_hook(HookHandle handle) {
	auto& real = *static_cast<const Handle*>(handle);
	auto pair = all_hooks().find(real.address);
	if (pair != all_hooks().end()) {
		auto& detours = pair->second.detours;
		auto detour = std::find(detours.begin(), detours.end(), real.detour);
		if (detour != detours.end()) {
			detours.erase(detour);
			if (detours.empty()) {
				remove_trap(real.address, pair->second.original_bytes);
			}
			delete &real;
			return true;
		}
	}

	return false;
}
