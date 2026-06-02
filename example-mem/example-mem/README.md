# External Mem

Made by **Xander** , **OriGin**.

This project is a simple external memory base for BlueStacks guest memory work.
It finds the game task, resolves CR3, gets module base, then lets you read and write memory.

## flow

1. `setup(package)` connects to BlueStacks memory and resolves target game CR3.
2. `get_module_base(module, index)` scans VMAs and returns module start address.
3. `read<T>(address)` reads value from guest memory.
4. `write<T>(address, value)` writes value to guest memory.

## Basic Setup

- Build: `Release | x64`
- Toolset: VS2022
- Make sure BlueStacks is running.
- Default target process/module are in `workspace/app_config.hpp`.

## functions

- `setup`: initialize engine, find package task, resolve CR3, attach for user-space reads.
- `get_module_base`: walk process memory maps and return base of requested `.so`.
- `read`: translate guest VA to host and return value/bytes from that address.
- `write`: translate guest VA to host and push new value/bytes to that address.

## Use Case Example

```cpp
#include <impl/includes.hpp>

using namespace std;

int main() {
    if (!bluestacks->setup("com.dts.freefireth"))
        return 1;

    u32 xander_libil2cpp = bluestacks->get_module_base("libil2cpp.so", 1);
    if (!xander_libil2cpp)
        return 1;

    auto xander_base_game_facade   = memory->read<u32>(xander_libil2cpp + 0x9EC1C48);
    auto xander_game_facade        = memory->read<u32>(xander_base_game_facade);
    auto xander_static_game_facade = memory->read<u32>(xander_game_facade + 0x5C);
    auto xander_current_game       = memory->read<u32>(xander_static_game_facade);
    auto xander_current_match      = memory->read<u32>(xander_current_game + 0x50);
    auto xander_match_status       = memory->read<u32>(xander_current_match + 0x8C);
    auto xander_local_player       = memory->read<u32>(xander_current_match + 0x94);

    cout << "Xander module base: " << hex << xander_libil2cpp << "\n";
    cout << "Xander match status: " << dec << xander_match_status << "\n";

    u32 xander_new_value = 1;
    memory->write<u32>(xander_current_match + 0x8C, xander_new_value);

    return 0;
}
```

## Notes From Xander

- If setup fails, check package name first.
- If module base is `0`, try map index `2` or `3`.
- If values look wrong, re-check offsets.

---

Author copyright: **Xander**, **OriGin
