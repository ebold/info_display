# info_display

SW4STM32 project

Development host:
- Linux Mint 18.3 (Cinnamon 64-bit)

Development tools:
- GNU Arm Embedded Toolchain, Version 9-2020-q2-update Linux x86_64 Tarball (https://developer.arm.com/tools-and-software/open-source-software/developer-tools/gnu-toolchain/gnu-rm/downloads)

- System Workbench for STM32 (https://www.ac6-tools.com)
- STM32CubeMX (http://www.st.com/stm32cubemx)
- STLink v2-1 (https://github.com/texane/stlink.git)
- STSW-Link007 (https://www.st.com/en/development-tools/stsw-link007.html)

Target system:
- STM32 Nucleo-F103RB (https://www.st.com/en/evaluation-tools/nucleo-f103rb.html)
- TI Beaglebone Black with Arch Linux ARMv7 AM33x BeagleBone (https://archlinuxarm.org/about/downloads)

Installations

1. GNU Arm Embedded Toolchain

1.1. Download a Linux 64-bit tarball

wget -c https://developer.arm.com/-/media/Files/downloads/gnu-rm/9-2020q2/gcc-arm-none-eabi-9-2020-q2-update-x86_64-linux.tar.bz2?revision=05382cca-1721-44e1-ae19-1e7c3dc96118&la=en&hash=D7C9D18FCA2DD9F894FD9F3C3DC9228498FA281A -O -

1.2. Rename the downloaded tarball

mv gcc-arm-none-eabi-9-2020-q2-update-x86_64-linux.tar.bz2\?revision\=05382cca-1721-44e1-ae19-1e7c3dc96118 gcc-arm-none-eabi-9-2020-q2-update-x86_64-linux.tar.bz2

1.3. Check the MD5 checksum

md5sum -c <<<"2b9eeccc33470f9d3cda26983b9d2dc6 gcc-arm-none-eabi-9-2020-q2-update-x86_64-linux.tar.bz2"

1.4. Unpack the downloaded tarball into the home folder

tar -xvf gcc-arm-none-eabi-9-2020-q2-update-x86_64-linux.tar.bz2 -C ~/

1.5. Set the toolchain path (in ~/.bashrc)

export PATH=$PATH:~/gcc-arm-none-eabi-9-2020-q2-update/bin
