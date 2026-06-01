#ifndef INCLUDES_HPP
#define INCLUDES_HPP

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif

#ifndef NOMINMAX
#define NOMINMAX
#endif

#include <Windows.h>

#include <algorithm>
#include <array>
#include <chrono>
#include <cerrno>
#include <cstddef>
#include <cstdint>
#include <cstdlib>
#include <cstring>
#include <cwchar>
#include <cwctype>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <limits>
#include <memory>
#include <mutex>
#include <optional>
#include <shared_mutex>
#include <sstream>
#include <string>
#include <string_view>
#include <type_traits>
#include <unordered_map>
#include <utility>
#include <vector>

#include <imgui.h>
#include <backends/imgui_impl_dx11.h>
#include <backends/imgui_impl_win32.h>

#include <workspace/app_config.hpp>
#include <workspace/bluestacks.hpp>
#include <workspace/gva_memory_bridge.hpp>
#include <workspace/kernel_structs.hpp>
#include <workspace/memory.hpp>
#include <workspace/memory_engine.hpp>
#include <workspace/render.hpp>
#include <workspace/types.hpp>

#endif
