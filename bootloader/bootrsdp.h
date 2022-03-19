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


#ifndef SALERNOS_RSDP_HEADER
#define SALERNOS_RSDP_HEADER

    #include "bootimports.h"
    #include "bootmem.h"


    void* bootloader_rsdp(EFI_SYSTEM_TABLE* __systable) {
        EFI_CONFIGURATION_TABLE* _config = __systable->ConfigurationTable;
        void* _rsdp_ptr                  = NULL;

        EFI_GUID _acpi2_table_id = ACPI_20_TABLE_GUID;

        for (UINTN _idx = 0; _idx < __systable->NumberOfTableEntries; _idx++) {
            if (CompareGuid(&_config[_idx].VendorGuid, &_acpi2_table_id))
                if (bootloader_strcmp((CHAR8*)"RSD PTR ", (CHAR8*)(_config->VendorTable), 8))
                    _rsdp_ptr = (void*)(_config->VendorTable);

            _config++;
        }

        return _rsdp_ptr;
    }

#endif
