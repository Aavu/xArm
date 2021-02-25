### Compiling and Linking
```
git clone https://github.com/Aavu/xArm.git
cd xArm
mkdir build && cd build
cmake ..
make
```

### Run
`./xArm`

Note: If you want to run it on the real xArm, 
uncomment line #5 in CMakeLists.txt (```add_compile_definitions(USE_XARM)```) 
and repeat the config/compilation process