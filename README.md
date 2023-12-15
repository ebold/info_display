# Info display

This project is targeted to a custom embedded system, which is used to display short information (like date, time, outside temperature etc). Basic building blocks are:
- LED matrix display: 6 pieces of 5x8 matrix
- display controller: STM32 Nucleo-F103RB
- host computer: Raspberry Pi or Beaglebone

At the moment it displays the actual time, which is synchronized to the system time of the host computer.

## Project setup

### Development host:
- Linux Mint 21.2 (Cinnamon 64-bit)

### Development tools

#### latest (recommended):

Use [STM32CubeIDE](https://www.st.com/en/development-tools/stm32cubeide.html), an all-in-one cross-platform IDE.

- Main features of STM32CubeIDE are:
    - based on Eclipse CDT
    - GCC for ARM toolchain and GDB debugger
    - supports ST-LINK and J-Link (Segger)
    - integrates [STM32CubeMX](https://www.st.com/en/development-tools/stm32cubemx.html)
    - imports project from AC6 System Workbench for STM32 (SW4STM32) and Atollic TrueSTUDIO

Alternatively an universal IDE [Visual Studio Code](https://code.visualstudio.com/), which is capable of editing and building embedded software for different microcontroller architectures, can be used.

- Visual Studio Code v18.4.2 (with extensions listed below)
   - PlatformIO v3.3.1
   - C/C++ v1.18.5
   - [STM32 VS Code Extension](https://learn.arm.com/install-guides/stm32_vs/) v1.0.0 requires:
      - [STM32CubeCLT](https://www.st.com/en/development-tools/stm32cubeclt.html) that includes:
         - GCC for ARM toolchain
         - GDB debugger
         - STM32Cube programmer
         - System view descriptor files (SVD) for the entire STM32 MCU portfolio
         - Map files associating STM32 MCUs and devel boards
      - [STM32CubeMX](https://www.st.com/en/development-tools/stm32cubemx.html)
      - [STMCUFinder](https://www.st.com/en/development-tools/st-mcu-finder-pc.html)

#### obsolte (as of 29.09.2020):

- GNU Arm Embedded Toolchain, Version 9-2020-q2-update [Linux x86_64 Tarball](https://developer.arm.com/tools-and-software/open-source-software/developer-tools/gnu-toolchain/gnu-rm/downloads)
- STM32 ST-LINK utility v1.6.1, [link](https://github.com/stlink-org/stlink)
- System Workbench for STM32, [link](https://www.openstm32.org)
- STM32CubeMX, [link](http://www.st.com/stm32cubemx)

### Target boards:
- STM32 Nucleo-F103RB, [link](https://www.st.com/en/evaluation-tools/nucleo-f103rb.html)
- TI Beaglebone Black with Arch Linux ARMv7 AM33x BeagleBone, [image](https://archlinuxarm.org/about/downloads)

## Installations

### 1. GNU Arm Embedded Toolchain

  1.1. Download a Linux 64-bit tarball
```
wget -c https://developer.arm.com/-/media/Files/downloads/gnu-rm/9-2020q2/gcc-arm-none-eabi-9-2020-q2-update-x86_64-linux.tar.bz2?revision=05382cca-1721-44e1-ae19-1e7c3dc96118&la=en&hash=D7C9D18FCA2DD9F894FD9F3C3DC9228498FA281A -O -
```
  1.2. Rename the downloaded tarball
```
mv gcc-arm-none-eabi-9-2020-q2-update-x86_64-linux.tar.bz2\?revision\=05382cca-1721-44e1-ae19-1e7c3dc96118 gcc-arm-none-eabi-9-2020-q2-update-x86_64-linux.tar.bz2
```
  1.3. Check the MD5 checksum
```
md5sum -c <<<"2b9eeccc33470f9d3cda26983b9d2dc6 gcc-arm-none-eabi-9-2020-q2-update-x86_64-linux.tar.bz2"
```
  1.4. Unpack the downloaded tarball into the home folder
```
tar -xvf gcc-arm-none-eabi-9-2020-q2-update-x86_64-linux.tar.bz2 -C ~/
```
  1.5. Set the toolchain path (in ~/.bashrc)
```
export PATH=$PATH:~/gcc-arm-none-eabi-9-2020-q2-update/bin
```

### 2. STM32 ST-LINK/V2-1 utility

  2.1. Download an external Debian package into your desired directory

```
wget -c https://github.com/stlink-org/stlink/releases/download/v1.6.1/stlink-1.6.1-1_amd64.deb
```

  2.2. Install the downloaded package

```
sudo apt install ./stlink-1.6.1-1_amd64.deb

```

  2.3. ([Optional](https://github.com/stlink-org/stlink/blob/develop/doc/tutorial.md)) Verify if udev rules are set properly

### 3. System Workbench for STM32

*Requirements:*
- Java RE (7 or upper version) and gksudo must be installed.
- User registration

  3.1. Download an installer for the 64-bit Linux and MD5 sum of the installer (might require registration)

```
wget -c http://www.ac6-tools.com/downloads/SW4STM32/install_sw4stm32_linux_64bits-v2.9.run --no-check-certificate
wget -c http://www.ac6-tools.com/downloads/SW4STM32/install_sw4stm32_linux_64bits-v2.9.run.md5 --no-check-certificate
md5sum -c install_sw4stm32_linux_64bits-v2.9.run.md5
```

  3.2. Launch the installer and follow the installation steps

```
chmod a+x install_sw4stm32_linux_64bits-v2.9.run
./install_sw4stm32_linux_64bits-v2.9.run
```

The installer contains the ST-LINK/V2 driver and ST-LINK server, if these components are already installed, it will check them.

  3.3. Install 32-bit version of the C shared libraries

```
sudo apt install libc6:i386 lib32ncurses5
```

### 4. STM32CubeMX

*Requirements:*
- The Java RE minimal version is 1.8_45. The version 1.8_251 must not be used (known Java issue).
- User registration

  4.1. Download the installation package (eg, en.stm32cubemx_v6-0-1.zip)

  4.2. Extract the downloaded package into an installation directory

```
mkdir -p /tmp/stm32cubemx
unzip en.stm32cubemx_v6-0-1.zip -d /tmp/stm32cubemx/
```

  4.3. Launch an installer and follow the installation steps

```
cd /tmp/stm32cubemx
chmod a+x SetupSTM32CubeMX-6.0.1.linux
./SetupSTM32CubeMX-6.0.1.linux
```

### 5. STM32CubeIDE [3]

**Important**: Only 64-bit OS versions are supported.

The installation is done through a product installer, which comes in different bundles to suit the various Linux distros. Choose a suitable bundle by inspecting the bundle name: *st-stm32cubeide_VERSION_ARCH.PACKAGE*, where:
- *ARCH*: architecture of the development host computer to run STM32CubeCLT, ie., amd64
- *PACKAGE*: Linux package type:
   - .sh: generic Linux
   - deb_bundle.sh: Debian distros (Mint, Ubuntu)
   - rpm_bundle.sh: RPM distros (CentOS, Fedora)

Just proceed as follows:

5.1. Navigate to the location of the installer.

5.2. Invoke the following command:

```
sudo sh ./st-stm32cubeide_VERSION_ARCH.PACKAGE
```
in our case:
```
sudo sh ./st-stm32cubeide_1.14.0_19471_20231121_1200_amd64.deb_bundle.sh
```

5.3. Follow the instructions: IDE is installed in **'/opt/st/'**, by default.

5.4. To update the PATH variable, either restart the computer or log out and log in back into the session.

Uninstallation depends on the distribution:

- any distro: sudo /opt/st/stm32cubeide_x.y.z/uninstall.sh
- Debian based distro: sudo apt remove st-stm32cubeide-x.y.z st-stlink-udev-rules st-stlink-server

in our case:
```
sudo apt remove st-stm32cubeide-1.14.0 st-stlink-udev-rules st-stlink-server
```

### 6. STM32CubeCLT [5]

In general the installation steps are same as for STM32CubeIDE.

Just proceed as follows:

6.1. Navigate to the location of the installer.

6.2. Invoke the following command:

```
sudo sh ./st-stm32cubeclt_VERSION_ARCH.PACKAGE
```
in our case:
```
sudo sh ./st-stm32cubeclt_1.14.0_19471_20231121_1200_amd64.deb_bundle.sh
```

6.3. Follow the instructions: may be you do not need to install Segger J-Link udev rules.

6.4. To update the PATH variable, either restart the computer or log out and log in back into the session.

Uninstallation depends on the distribution:

- any distro: sudo /opt/st/stm32cubeclt_x.y.z/uninstall.sh
- Debian based distro: sudo dpkg -r st-stm32cubeclt-x.y.z st-stlink-udev-rules st-stlink-server

in our case:
```
sudo dpkg -r st-stm32cubeclt-1.14.0 st-stlink-udev-rules st-stlink-server
```

### 7. STM32CubeMX

Download the installation package from the ST website.

7.1. Extract (unzip) the downloaded package.

7.2. (optional) change the permission of extracted SetupSTM32CubeMX-VERSION tool, so that the file is executable.

7.3. Lauch it from the console or double-click on it. By default it will be installed in the '$HOME/STM32CubeMX' folder, where $HOME is the user home folder.

For uninstallation launch '$HOME/STM32CubeMX/Uninstaller/uninstall.sh' script

### 8. STMCUFinder

The installation procedure is similar as for STM32CubeMX.

The default locations for:
- installation: '$HOME/STMCUFinder'
- uninstallation: '$HOME/STMCUFinder/Uninstaller'

## How-to

### 1. System Workbench for STM32

  1.1. Import an existing project (info_display) [1]

- File >> Import...
- Choose "Existing Projects into Workspace" from the "General" group
- Click on “Browse” for the root directory. It should select your workspace by default, if not, enter the workspace location.
- The projects list is refreshed and should display your project (info_display), select it.
- Ensure the option “Copy projects into workspace” is **unchecked**
- Click on the "Finish" button.

  1.2. Change the Eclipse default theme to 'Dark'

- Window >> Preference
- Choose "Appearance" from the "General" group
- Select "Dark" from "Theme:" and click on the "OK" button.

  1.3. Import 'Solarized Dark' theme [2]

- File >> Import >> General >> Preference
- Press "Next" and then browse for the .epf file.
- Check the "Import all" box and click on the "Finish" button.

  1.4. Revert to the Eclipse default settings

```
rm -r $WORKSPACE_DIR/.metadata/.plugins/org.eclipse.core.runtime/.settings/

```

### 2. STM32 VS Code Extension

  2.1. Import an existing System Workbench project

**Prerequisites**: any System Workbench project must be migrated into STM32Cube before importing into Visual Studio Code [4]. After migration if build fails with an error complaining about "*multiple definitions of 'variable'* ", add the '**-fcommon**' compiler option: right-click on project -> Properties -> C/C++ Build -> Settings -> MCU GCC Compiler -> Miscellaneous'.

**Prerequisites**: installation of STM32CubeCLT, STM32CubeMX and STMCUFinder.

**Prerequisites**: 'Embedded Tools' extension is needed to import STM32Cube projects.

Switch to 'STM32 VS Code Extension' (Activity Bar) and use the 'Import a local project' command to open a project file.

If the project file (**.project**) is not visible, then open the project directory: File -> Open Folder... This will create new workspace. Afterwards, open 'language.settings.xml' and back to 'STM32 VS Code Extension' and try to open the ST project.

  2.2. Project importing fails. The selected project file is not STM32Cube project.

TODO: not yet solved

## Links

[1] Importing a STM32CubeMX project into SW4STM32, [link](https://www.openstm32.org/Importing%2Ba%2BSTCubeMX%2Bgenerated%2Bproject)

[2] Eclipse Preferences [EPF](http://www.eclipsecolorthemes.org/?view=theme&id=1115) - for Eclipse Import

[3] STM32CubeIDE, [link](https://www.st.com/en/development-tools/stm32cubeide.html)

[4] Migration guide from System Workbench to STM32CubeIDE, [pdf](docs/um2579-migration-guide-from-system-workbench-to-stm32cubeide-stmicroelectronics.pdf)

[5] STM32CubeCLT installation guide, [pdf](docs/um3089-stm32cubeclt-installation-guide-stmicroelectronics.pdf)