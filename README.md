STEPS:


conan install . -of build-debug -s build_type=Debug -b missing


cmake -S .. -DCMAKE_TOOLCHAIN_FILE=build-debug/conan_toolchain.cmake -DCMAKE_BUILD_TYPE=Debug