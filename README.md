## SalernOS
SalernOS is a small proprietary Toy OS I'm working on.

## SalernOS EFI Bootloader (SEB)
The SalernOS EFI Bootloader is a free and open source GNU-EFI bootloader built on top of https://github.com/Absurdponcho/gnu-efi

## Naming Conventions
* ```_variablename``` and ```_variable_name``` are stack-allocated variables
* ```_VariableName``` is a struct member variable
* ```variablename``` and ```variable_name``` are global variables
* ```__variablename``` and ```__variable_name``` are function arguments

## Compiling SEB
Compiling SEB is quite eaasy, just follow these steps
* Make sure you're running a GNU/Linux system. If you're running Windows 10/11 or macOS, use WSL or Docker
* Install the build-essentials package via your package manager (apt-get/pacman etc)
* Open a terminal in the SEB directory
* If it's the first time you compile SEB, Type `make` and wait for a few seconds
* Type `make bootloader` and wait for a few seconds
* The resulting `main.efi` file will be in the `x86_64/bootloader` directory

## Editing SEB's source code
All of SEB's source code is in the `bootloader` directory.
NOTE: Remember to change something in `main.c` before compiling, otherwise it won't work!

## Using SEB as bootloader for an Operating System
This is a little trickier, but you should be able to do it, otherwise, open an issue on this Github page.

### The OpenBit Specification
The OpenBit Bootloader & Kernel Specification, developed by the OpenBit Discord Server, has not been finalised yet, SEB uses a temporary version of the Specification, called SalernOS OpenBit. Please note that this WILL change soon, as the server finalises the Official Specification.

#### Making a SalernOS-OpenBit-Bootloader Compatible File System
Currently the SalernOS OpenBit Bootloader Specification requires the following directories
* EFI
* EFI/BOOT
* openbit
* openbit/bin
* openbit/assets

The `EFI/BOOT`directory must contain a `bootx64.efi` file. This file is the SEB binary, you can get it by renaming the `main.efi` file.
The `openbit/assets` directory must contain a PSF1 Font named `kernelfont.psf`.
The `openbit/bin` directory must contain the kernel executable, named `kernel.elf`

#### Making a SalernOS-OpenBit-Kernel Compatible Kernel
Currently, the SalernOS OpenBit Kernel Specification only requires an entry point that must take one single parameter: a `BootInfo` struct.
You can find the definition of a `BootInfo` struct in `bootloader/main.c`.
