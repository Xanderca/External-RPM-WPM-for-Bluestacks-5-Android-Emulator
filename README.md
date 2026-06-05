## The project was created by me and with help of my friend origin

- my discord @xander.best
- my friend dc @opgamer_ff
- my friend dc @addr_t
### @opgamer_ff aslo known as paidfire helped me in understanding the virtual box layout and implementation of overall mem_engine and cache sub_system.
### @addr_t also known as zxsrxt early who help me in understanding kernel layout and finded kernel structs which is crucial part thanks to him.

## thanks to all the contributors and friends without you it was never possible


# External Read/Write Process Memory  

Simple external process memory read/write base for BlueStacks 5 emulator, focused on fast setup, module-base resolve, and clean memory access flow.

## Overview

This project initializes a memory engine on `HD-Player.exe`, resolves guest CR3 from target package task, finds module base from VMAs, then provides read/write APIs for guest memory.

## Core Functions

- `setup(package)`: initializes engine, finds target task, resolves CR3, and prepares memory access.
- `get_module_base(module, index)`: scans mapped VMAs and returns the selected module base address.
- `read<T>(address)`: reads typed data from guest memory at the given address.
- `write<T>(address, value)`: writes typed data into guest memory at the given address.

## Basic Setup

1. Build with `Release | x64` in Visual Studio 2022.
2. Run BlueStacks before launching the Executable.
3. Set default process/module in `workspace/app_config.hpp` if needed.

## Example

```cpp
if (!bluestacks->setup("com.dts.freefireth"))
    return 1;

u32 libil2cpp = bluestacks->get_module_base("libil2cpp.so", 1);
if (!libil2cpp)
    return 1;

auto value = memory->read<u32>(libil2cpp + 0x9EC1C48);
memory->write<u32>(libil2cpp + 0x9EC1C48, value);
```

## Notes

- Detailed project notes are available in [example-mem README](example-mem/example-mem/README.md).
- Created by Xander.  
