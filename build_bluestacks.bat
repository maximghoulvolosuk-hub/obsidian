@echo off
set NDK_PATH=C:\Users\%USERNAME%\AppData\Local\Android\Sdk\ndk\25.1.8937393
set TOOLCHAIN=%NDK_PATH%\build\cmake\android.toolchain.cmake

echo [+] Building Obsidian for BlueStacks (x86_64)...
if not exist build_bs mkdir build_bs
cd build_bs

cmake -G "MinGW Makefiles" ^
    -DCMAKE_TOOLCHAIN_FILE="%TOOLCHAIN%" ^
    -DANDROID_ABI="x86_64" ^
    -DANDROID_PLATFORM="android-29" ^
    -DCMAKE_BUILD_TYPE=Release ..

cmake --build . --config Release
echo [+] Build complete! libobsidian.x86_64.so is ready.
pause