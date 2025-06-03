# Stegano

`Stegano` is a simple C++ project for **steganography** — the technique of hiding information inside other data, such as images, audio, or plain text. The project demonstrates how to encode and decode hidden messages while keeping the modified file as close to the original as possible.


## Features
- Encode text into `.png` and .`bmp`/`.dib` files.
- Decode messages hidden inside files.
- Simple, modular structure for easy extension and customization.


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
   ./steganography
   ```

## Usage
steganography \[option\] \<file\> \[message\] \[key/length\]
This program supports only `.png` and `.bmp`/`.dib` file formats.

-c, --check "file" "message"                Checks whether the file can store the given message
-d, --decrypt "file" "key/length"         Extracts a hidden message using the given key (`.png`) or message length (`.bmp`/`.dib`)
-e, --encrypt "file" "message" "key"   Embeds the given message into the file using the provided key (.png only)
-h, --help                                            Displays the help message
-i, --info "file"                                     Checks whether the file is readable and writable by the program

## Examples
```Shell
./steganography -c "./picture.bmp" "some message"
```
Checks if this message can be used for steganography in the file.

```Shell
./steganography -e "./picture.png" "some message" "1"
```
Hides the message into the file and encrypts the message with key `1`.

```Shell
./steganography -d "./secret.png" "1"
```
Extracts the message from the file and decrypts it with key `1`.

```Shell
./steganography -d "./secret.bmp" 20
```
Extracts 20 bytes of possible message from the file.


## License
This project is licensed under the MIT License. See the [LICENSE](./LICENSE) file for details.