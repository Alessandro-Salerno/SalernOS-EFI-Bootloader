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

    // Returns a pointer to a file in the boot drive
    EFI_FILE* bootloader_loadfile(EFI_FILE* __directory, CHAR16* __path, EFI_HANDLE __imagehandle, EFI_SYSTEM_TABLE* __systable) {
        EFI_FILE* _loaded_file;
        EFI_LOADED_IMAGE_PROTOCOL* _loaded_image;

        // Allocatres enough memory to make roo for the loaded image
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

#endif
