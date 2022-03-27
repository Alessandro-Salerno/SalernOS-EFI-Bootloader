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


#ifndef SALERNOS_GRAPHICS_HEADER
#define SALERNOS_GRAPHICS_HEADER

    #include "bootimports.h"
    #include "bootmem.h"
    #include "sbs.h"


    struct SimpleBootFramebuffer bootloader_initialize_graphics() {
        EFI_GRAPHICS_OUTPUT_PROTOCOL* _graphics_output_protocol;
        EFI_GUID                      _gop_guid = EFI_GRAPHICS_OUTPUT_PROTOCOL_GUID;
        EFI_STATUS                    _status   = uefi_call_wrapper(BS->LocateProtocol, 3, &_gop_guid, NULL, (void**)(&_graphics_output_protocol));

        struct SimpleBootFramebuffer _framebuffer;

        bootloader_hardassert(_status == EFI_SUCCESS, L"ERROR: Unable to locate Graphics Output Protocol.\n\r");

        #ifdef SEB_DEBUG
            Print(L"SUCCESS: Graphics Output Protocol located!\n\r");
        #endif

        _framebuffer._BaseAddress       = (void*)(_graphics_output_protocol->Mode->FrameBufferBase);
        _framebuffer._BufferSize        = _graphics_output_protocol->Mode->FrameBufferSize;
        _framebuffer._Width             = _graphics_output_protocol->Mode->Info->PixelsPerScanLine;
        _framebuffer._Height            = _graphics_output_protocol->Mode->Info->VerticalResolution;

        #ifdef SEB_DEBUG
            Print(L"SUCCESS: Framebuffer set up!\n\r");
            Print(L"Base Address: 0x%x\n\rSize: %d bytes\n\rWidth: %d (Real: %d) pixels\n\rHeight: %d pixels\n\rBPP: %d\n\r",
                    framebuffer._BaseAddress, framebuffer._BufferSize, framebuffer._Width,
                    framebuffer._PixelsPerScanLine, framebuffer._Height, framebuffer._BytesPerPixel);
        #endif

        return _framebuffer;
    }

#endif
