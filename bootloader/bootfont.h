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


#ifndef SALERNOS_FONT_HEADER
#define SALERNOS_FONT_HEADER

    #include "bootimports.h"
    #include "bootfile.h"
    #include "bootmem.h"

    #define BITMAP_FONT_MAGIC0 0x36
    #define BITMAP_FONT_MAGIC1 0x04


    typedef struct BitmapFontHeader {
        uint8_t _Magic[2];
        uint8_t _Mode;
        uint8_t _CharSize;
    } BitmapFontHeader;

    typedef struct BitmapFont {
        BitmapFontHeader _Header;
        void*            _Buffer;
    } BitmapFont;


    BitmapFont bootloader_loadfont(EFI_FILE* __directory, CHAR16* __path, EFI_HANDLE __imagehandle, EFI_SYSTEM_TABLE* __systable) {
        Print(L"INFO: About to load bitmap font...\n\r");

        EFI_FILE* _font = bootloader_loadfile(__directory, __path, __imagehandle, __systable);
        bootloader_hardassert(_font != NULL, L"ERROR: Font file not found\n\r");

        BitmapFontHeader _font_header;
        UINTN _font_header_size = sizeof(BitmapFontHeader);
        _font->Read(_font, &_font_header_size, &_font_header);

        bootloader_hardassert(
            _font_header._Magic[0] == BITMAP_FONT_MAGIC0 && _font_header._Magic[1] == BITMAP_FONT_MAGIC1,
            L"ERROR: The specified file is not a PSF1 font!\n\n"
        );

        UINTN _buffer_size = _font_header._CharSize * ((_font_header._Mode == 1) ? 512 : 256); 
        void* _buffer;
        _font->SetPosition(_font, _font_header_size);
        __systable->BootServices->AllocatePool(EfiLoaderData, _buffer_size, (void**)(&_buffer));
        _font->Read(_font, &_buffer_size, _buffer);

        BitmapFont _final_font;
        _final_font._Header     = _font_header;
        _final_font._Buffer     = _buffer;

        Print(L"SUCCESS: PSF1 font loaded!\n\r");
        Print(L"Font mode: %d\n\rFont char height: %d pixels\n\rFont base address: 0x%x\n\r",
                _final_font._Header._Mode, _final_font._Header._CharSize, (uint64_t)(_final_font._Buffer));

        return _final_font;
    }

#endif
