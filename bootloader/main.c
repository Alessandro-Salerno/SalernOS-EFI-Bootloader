/***********************************************************************
                            SalernOS EFI Bootloader
                  Copyright 2021 - 2022 Alessandro Salerno

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.
***********************************************************************/


#include "bootimports.h"
#include "bootfile.h"
#include "bootmem.h"
#include "bootgraphics.h"
#include "bootfont.h"

#define SEB_FAILURE 1

#define COMPILATION_DATE_DAY 24
#define COMPILAIION_DATE_MONTH (uint16)('A' << 8)

#define SEB_MAJOR_VERSION 22
#define SEB_MINOR_VERSION (uint16)(COMPILAIION_DATE_MONTH | COMPILATION_DATE_DAY)


typedef struct MemoryInfo {
    EFI_MEMORY_DESCRIPTOR* _MemoryMap;
    UINTN                  _MemoryMapSize;
    UINTN                  _DescriptorSize;
} MemoryInfo;

typedef struct BootInfo {
    Framebuffer* _Framebuffer;
    BitmapFont*  _Font;
    MemoryInfo   _Memory;

    uint8        _SEBMajorVersion;
    uint16       _SEBMinorVersion;
} BootInfo;


EFI_STATUS efi_main(EFI_HANDLE __imagehandle, EFI_SYSTEM_TABLE* __systable) {
    BootInfo _bootinfo = (BootInfo) {
        ._SEBMajorVersion = SEB_MAJOR_VERSION,
        ._SEBMinorVersion = SEB_MINOR_VERSION
    };

    InitializeLib(__imagehandle, __systable);
    Print(L"Entering SalernOS EFI Bootloader (SEB)...\n\r\n\r");

    // Locate and use Graphics Output Protocol (UEFI GOP)
    Print(L"INFO: About to load Graphics Output Protocol...\n\r");
    _bootinfo._Framebuffer = bootloader_initialize_graphics();
    Print(L"INFO: About to load system kernel (openbit/bin/kernel.elf)\n\r");

    // Open openbit/bin directory
    EFI_FILE* _openbit_bin = bootloader_loadfile(bootloader_loadfile(NULL, L"openbit", __imagehandle, __systable), L"bin", __imagehandle, __systable);

    EFI_FILE* _kernel_file = bootloader_loadfile(_openbit_bin, L"kernel.elf", __imagehandle, __systable);
    ElfFile   _kernel      = bootloader_loadelf(_kernel_file, __systable);

    Print(L"SUCCESS: Kernel loaded!\n\r");

    _bootinfo._Font = bootloader_loadfont(bootloader_loadfile(bootloader_loadfile(NULL, L"openbit", __imagehandle, __systable), L"assets", __imagehandle, __systable), L"kernelfont.psf", __imagehandle, __systable);

    // Create Memory Map
    EFI_MEMORY_DESCRIPTOR* _mem_map = NULL;
        UINT32 _mem_descriptor_version;
        UINTN  _mem_map_size,
               _mem_map_key,
               _mem_descriptor_size;
        
        __systable->BootServices->GetMemoryMap(&_mem_map_size, _mem_map, &_mem_map_key, &_mem_descriptor_size, &_mem_descriptor_version);
        __systable->BootServices->AllocatePool(EfiLoaderData, _mem_map_size, (void**)(&_mem_map));
        __systable->BootServices->GetMemoryMap(&_mem_map_size, _mem_map, &_mem_map_key, &_mem_descriptor_size, &_mem_descriptor_version);

        // Send Memory Map to the kernel
        _bootinfo._Memory = (MemoryInfo) {
            ._MemoryMap      = _mem_map,
            ._MemoryMapSize  = _mem_map_size,
            ._DescriptorSize = _mem_descriptor_size
        };

    Print(L"INFO: Jumping to kernel entry...\n\r");
    void (*_kernel_entry)(BootInfo) = ((__attribute__((sysv_abi)) void (*)(BootInfo))(_kernel._Header.e_entry));

    // Exit EFI Boot Services and jump to the kernel
    __systable->BootServices->ExitBootServices(__imagehandle, _mem_map_key);
    _kernel_entry(_bootinfo);

    return EFI_SUCCESS;
}
