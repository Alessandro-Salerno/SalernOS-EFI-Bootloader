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


#ifndef SALERNOS_IMPORTS_HEADER
#define SALERNOS_IMPORTS_HEADER

    #include <efi.h>
    #include <efilib.h>
    #include "boottypes.h"

    #ifdef _WIN32
        #include "../lib/elf.h"
    #else
        #include <elf.h>
    #endif

    #define RETVOID
    #define RETIF(__cond, __ret) if (__cond) return __ret


    void bootloader_hardassert(uint8_t __cond, CHAR16* __msg) {
        RETIF(__cond, RETVOID);

        Print(__msg);
        while (1);
    }

    void bootloader_softassert(uint8_t __cond, CHAR16* __msg) {
        if (__cond) return;
        Print(__msg);
    }

#endif
