### Compiling and Linking
```
git clone --recurse-submodules -j4 https://github.com/Aavu/xArm
cd xArm
```
Follow [xArm-CPP-SDK](https://github.com/xArm-Developer/xArm-CPLUS-SDK) to install the library. No need to clone the repo. The above git clone command has already cloned the repo inside the xArm Folder. After installing change to xArm folder before proceeding.
```
mkdir build && cd build
cmake ..
make
```

### Run
`./xArm`

Note: If you want to run it on the real xArm, 
uncomment line #5 in CMakeLists.txt (```add_compile_definitions(USE_XARM)```) 
and repeat the config/compilation process
