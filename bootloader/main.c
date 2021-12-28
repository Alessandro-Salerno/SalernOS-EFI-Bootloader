/***********************************************************************
                            SalernOS EFI Bootloader
                        Copyright 2021 Alessandro Salerno

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


typedef struct BootInfo {
	Framebuffer* _Framebuffer;
} BootInfo;


EFI_STATUS efi_main(EFI_HANDLE ImageHandle, EFI_SYSTEM_TABLE* SystemTable) {
	BootInfo _bootinfo;
	
	InitializeLib(ImageHandle, SystemTable);
	Print(L"Entering SalernOS boot loader...\n\r");
	
	Print(L"About to load Graphics Output Protocol...\n\r");
	if ((_bootinfo._Framebuffer = bootloader_initialize_graphics()) == NULL)
		goto BOOT_END;
	
	Print(L"About to load system kernel (kernel.elf)\n\r");

	// Tries to get a pointer to the kernel ELF file
	// If it fails, it jumps to the end of the boot loader
	EFI_FILE* _kernel = bootloader_loadfile(NULL, L"kernel.elf", ImageHandle, SystemTable);
	if (_kernel == NULL) {
		Print(L"Unable to locate kernel.elf\n\r");
		goto BOOT_END;
	}

	// If the kernel binary is located
	// It goes on to read its ELF header
	Print(L"Reading kernel ELF header...\n\r");
	Elf64_Ehdr _elf_header;
		UINTN _file_info_size;
		EFI_FILE_INFO* _file_info;

		// Gets the size of the header and allocatres enough memory to hold it
		_kernel->GetInfo(_kernel, &gEfiFileInfoGuid, &_file_info_size, NULL);
		SystemTable->BootServices->AllocatePool(EfiLoaderData, _file_info_size, (void**)(&_file_info));

		// Gets the header, reads it and saves it into memory
		_kernel->GetInfo(_kernel, &gEfiFileInfoGuid, &_file_info_size, (void**)(&_file_info));
		UINTN _elf_header_size = sizeof(_elf_header);
		_kernel->Read(_kernel, &_elf_header_size, &_elf_header);

	// Checks if the ELF file is a valid executable
	if (bootloader_memcmp(&_elf_header.e_ident[EI_MAG0], ELFMAG, SELFMAG) != 0            ||
		_elf_header.e_ident[EI_CLASS]                                     != ELFCLASS64   ||
		_elf_header.e_ident[EI_DATA]                                      != ELFDATA2LSB  ||
		_elf_header.e_type                                                != ET_EXEC      ||
		_elf_header.e_machine                                             != EM_X86_64    ||
		_elf_header.e_version                                             != EV_CURRENT
	)
	{
		// If it isn't, it throws an error and jumps to the end
		Print(L"Invalid kernel header\n\r");
		goto BOOT_END;
	}

	// If it is, it continues
	Print(L"Kernel header verified!\n\r");

	Elf64_Phdr* _elf_program_headers;
		_kernel->SetPosition(_kernel, _elf_header.e_phoff);
		UINTN _elf_size = _elf_header.e_phnum * _elf_header.e_phentsize;
		SystemTable->BootServices->AllocatePool(EfiLoaderData, _elf_size, (void**)&_elf_program_headers);
		_kernel->Read(_kernel, &_elf_size, _elf_program_headers);
	
	for (Elf64_Phdr* _elf_program_header = _elf_program_headers;
		 (char*)(_elf_program_header) < (char*)(_elf_program_headers) + _elf_header.e_phnum * _elf_header.e_phentsize;
		 _elf_program_header = (Elf64_Phdr*)((char*)(_elf_program_header) + _elf_header.e_phentsize)
	)
	{
		if (_elf_program_header->p_type == PT_LOAD) {
			int _pages      = (_elf_program_header->p_memsz + 0x1000 - 1) / 0x1000;
			Elf64_Addr _seg = _elf_program_header->p_paddr;
			SystemTable->BootServices->AllocatePages(AllocateAddress, EfiLoaderData, _pages, &_seg);

			_kernel->SetPosition(_kernel, _elf_program_header->p_offset);
			UINTN _size = _elf_program_header->p_filesz;
			_kernel->Read(_kernel, &_size, (void*)(_seg));
		}
	}

	Print(L"Kernel loaded!\n\r");
	Print(L"Jumping to kernel entry...\n\r");

	void (*_kernel_entry)(BootInfo*) = ((__attribute__((sysv_abi)) void (*)(BootInfo*))(_elf_header.e_entry));
	_kernel_entry(&_bootinfo);

	BOOT_END:
	return EFI_SUCCESS;
}
