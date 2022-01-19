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


#ifndef SALERNOS_MEMORY_HEADER
#define SALERNOS_MEMORY_HEADER

    #include "bootimports.h"

    // Compares two memory buffers
    /*****************************
     * Returns
     * -------
     *  0 if equal
     *  1 if higher
     * -1 if lower
    *****************************/
    int bootloader_memcmp(const void* __aptr, const void* __bptr, size_t __size) {
        // Casts the two buffers to char pointers
        // This way the size of each entry is one byte
        const unsigned char *_a = __aptr, *_b = __bptr;

        for (size_t i = 0; i < __size; i++) {
            if (_a[i] < _b[i]) return -1;
            else if (_a[i] > _b[i]) return 1;
        }

        return 0;
    }

    void bootloader_memset(void* __buff, uint64 __buffsize, uint8 __val) {
        for (uint64 _i = 0; _i < __buffsize; _i++)
            *(unsigned char*)((uint64)__buff + _i) = __val;
    }

#endif
