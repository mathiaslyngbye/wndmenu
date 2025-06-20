<!-- Title -->

wndmenu provides improved application launching on MS Windows.

## Configuration

Modify ```src/config.hpp``` as desired and rebuild.

## Build from source

This project supports building from source on Windows using **CMake** and **Visual Studio Build Tools**.

### Prerequisites

- [Visual Studio Build Tools 2022](https://visualstudio.microsoft.com/downloads/)

### Build instructions

1. Open the terminal:
   > **Start Menu → x64 Native Tools Command Prompt for VS 2022**

2. Build the project:

    ```cmd
    mkdir build
    cd build
    cmake .. -G "NMake Makefiles"
    nmake
    ```

5. The resulting executable (`wndwalk.exe`) will be located in the `build/` directory.

### Troubleshooting

- Make sure you're using the **x64 Native Tools Command Prompt** — not the x86 or generic command prompt.
