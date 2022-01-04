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


#ifndef SALERNOS_FILE_HEADER
#define SALERNOS_FILE_HEADER

    #include "bootimports.h"
    #include "bootmem.h"

    typedef struct ElfFile {
        Elf64_Ehdr  _Header;
        Elf64_Phdr* _PrgramHeaders;

        uint8_t     _Valid;
    } ElfFile;


    // Returns a pointer to a file in the boot drive
    EFI_FILE* bootloader_loadfile(EFI_FILE* __directory, CHAR16* __path, EFI_HANDLE __imagehandle, EFI_SYSTEM_TABLE* __systable) {
        EFI_FILE* _loaded_file;
        EFI_LOADED_IMAGE_PROTOCOL* _loaded_image;

        // Allocatres enough memory to make room for the loaded image
        __systable->BootServices->HandleProtocol(__imagehandle, &gEfiLoadedImageProtocolGuid, (void**)(&_loaded_image));

        // Creates a simple filesystem
        EFI_SIMPLE_FILE_SYSTEM_PROTOCOL* _filesys;
        __systable->BootServices->HandleProtocol(_loaded_image->DeviceHandle, &gEfiSimpleFileSystemProtocolGuid, (void**)(&_filesys));

        // Sets the directory to the root folder
        if (__directory == NULL)
            _filesys->OpenVolume(_filesys, &__directory);

        // Opens the file and stores the status
        EFI_STATUS _status = __directory->Open(__directory, &_loaded_file, __path, EFI_FILE_MODE_READ, EFI_FILE_READ_ONLY);
        
        // If the operation failed, NULL (void ptr) is returned
        if (_status != EFI_SUCCESS)
            return NULL;

        // Otherwise, the pointer is returned
        return _loaded_file;
    }

    // Loads an ELF binary executable
    ElfFile bootloader_loadelf(EFI_FILE* __file, EFI_SYSTEM_TABLE* __systable) {
        // Tries to get a pointer to the ELF file
        // If it fails, it returns an invalid ElfFIle struct
        if (__file == NULL) {
            Print(L"ERROR: Unable to locate ELF file\n\r");
            goto LOAD_FAILED;
        }

        // If the binary is located
        // It goes on to read its ELF header
        Print(L"INFO: Reading ELF header...\n\r");
        Elf64_Ehdr _elf_header;
            UINTN _file_info_size;
            EFI_FILE_INFO* _file_info;

            // Gets the size of the header and allocatres enough memory to hold it
            __file->GetInfo(__file, &gEfiFileInfoGuid, &_file_info_size, NULL);
            __systable->BootServices->AllocatePool(EfiLoaderData, _file_info_size, (void**)(&_file_info));

            // Gets the header, reads it and saves it into memory
            __file->GetInfo(__file, &gEfiFileInfoGuid, &_file_info_size, (void**)(&_file_info));
            UINTN _elf_header_size = sizeof(_elf_header);
            __file->Read(__file, &_elf_header_size, &_elf_header);

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
            Print(L"ERROR: Invalid ELF header\n\r");
            goto LOAD_FAILED;
        }

        // If it is, it continues
        Print(L"SUCCESS: ELF header verified!\n\r");

        Elf64_Phdr* _elf_program_headers;
            __file->SetPosition(__file, _elf_header.e_phoff);
            UINTN _elf_size = _elf_header.e_phnum * _elf_header.e_phentsize;
            __systable->BootServices->AllocatePool(EfiLoaderData, _elf_size, (void**)(&_elf_program_headers));
            __file->Read(__file, &_elf_size, _elf_program_headers);
        
        for (Elf64_Phdr* _elf_program_header = _elf_program_headers;
            (char*)(_elf_program_header) < (char*)(_elf_program_headers) + _elf_size;
            _elf_program_header = (Elf64_Phdr*)((char*)(_elf_program_header) + _elf_header.e_phentsize)
        )
        {
            if (_elf_program_header->p_type == PT_LOAD) {
                int _pages      = (_elf_program_header->p_memsz + 0x1000 - 1) / 0x1000;
                Elf64_Addr _seg = _elf_program_header->p_paddr;
                __systable->BootServices->AllocatePages(AllocateAddress, EfiLoaderData, _pages, &_seg);

                __file->SetPosition(__file, _elf_program_header->p_offset);
                UINTN _size = _elf_program_header->p_filesz;
                __file->Read(__file, &_size, (void*)(_seg));
            }
        }

        return (ElfFile) {
            ._Header        = _elf_header,
            ._PrgramHeaders = _elf_program_headers,
            ._Valid         = 1
        };

        LOAD_FAILED:
        return (ElfFile) {
            ._Valid         = 0
        };
    }

#endif
