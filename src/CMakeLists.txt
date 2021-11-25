cmake_minimum_required(VERSION 3.8)

if ("${CMAKE_SYSTEM_NAME}" STREQUAL "iOS")
	add_library(LilacHook STATIC
		"lilac_hook.cpp"
		"impl/lilac_hook_impl.cpp" 
		"impl/lilac_hook_ios.cpp")
elseif ("${CMAKE_SYSTEM_NAME}" STREQUAL "Android")
    add_library(LilacHook STATIC
        "lilac_hook.cpp"
        "impl/lilac_hook_impl.cpp" 
        "impl/lilac_hook_android.cpp")
elseif(APPLE)
	add_library(LilacHook STATIC
		"lilac_hook.cpp"
		"impl/lilac_hook_impl.cpp" 
		"impl/lilac_hook_macos.cpp")
elseif(WIN32)
	add_library(LilacHook STATIC
		"lilac_hook.cpp"
		"impl/lilac_hook_impl.cpp" 
		"impl/lilac_hook_windows.cpp")
endif()
set_property(TARGET LilacHook PROPERTY CXX_STANDARD 17)
target_include_directories(LilacHook PRIVATE ${CMAKE_CURRENT_LIST_DIR}/../include)
