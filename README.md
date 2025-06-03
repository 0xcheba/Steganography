# Stegano

`Stegano` is a simple C++ project for **steganography** — the technique of hiding information inside other data, such as images, audio, or plain text. The project demonstrates how to encode and decode hidden messages while keeping the modified file as close to the original as possible.


## Features
- Encode text into carrier files (e.g., image or text files).
- Decode messages hidden inside files.
- Simple, modular structure for easy extension and customization.
- CMake-based build system for cross-platform compatibility.


## Build Instructions
### Prerequisites:
1. Install a **C++20-compatible compiler** (e.g., GCC, Clang).
2. Install **CMake** (version 3.29 or newer).

### Steps:
1. Clone the repository:
   ```bash
   git clone https://github.com/0xcheba/Steganography.git
   cd Steganography
   ```
2. Create a build directory and navigate to it:
   ```bash
   mkdir build
   cd build
   ```
3. Configure the project using CMake:
   ```bash
   cmake ..
   ```
4. Build the project:
   ```bash
   cmake --build .
   ```
5. Run the program:
   ```bash
   ./stegano
   ```


## License
This project is licensed under the MIT License. See the [LICENSE](./LICENSE) file for details.