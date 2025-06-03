#include <fstream>
#include <iostream>
#include <filesystem>

#include "decryptor.hpp"
#include "encryptor.hpp"
#include "checker.hpp"

const std::string RED = "\033[1;31m";
const std::string GREEN = "\033[1;32m";
const std::string YELLOW = "\033[1;33m";
const std::string RESET = "\033[0m";

const std::string INVALID_ARGS = RED + "Invalid number of arguments!" + RESET;
const std::string CAN_BE_USED = GREEN + "This file can be used for steganography!" + RESET;

auto help() -> void {
    std::cout << "\nUsage: ./steganography [option] <file> [message] [key/length]\n"
            << "Writes or reads a secret message from an image file.\n"
            << RED << "Warning! " << RESET << "This program supports only " << GREEN << ".png " << RESET << "and " << GREEN
            << ".bmp/.dib " << RESET << "file formats.\n\n"
            << "You should use at least one of the following options:\n"
            << "-c, --check \"file\" \"message\"           Checks whether the file can store the given message\n"
            << "-d, --decrypt \"file\" \"key/length\"      Extracts a hidden message using the given key ("
            << GREEN << "png" << RESET << ") or message length (" << GREEN << "bmp/dib" << RESET << ")\n"
            << "-e, --encrypt \"file\" \"message\" \"key\"   Embeds the given message into the file using the provided key ("
            << GREEN << "key png only" << RESET << ")\n"
            << "-h, --help                             Displays this help message\n"
            << "-i, --info \"file\"                      Checks whether the file is readable and writable by the program\n"
            << std::endl;
}

auto check(const std::string &filepath, const std::string &message) -> bool {
    std::cout << "\nChecking if file can be used for steganography..." << std::endl;
    auto const extension = checker::extensions(filepath);
    auto const magic = checker::magics(filepath);
    if (magic == extension && extension == "bmp") {
        if (checker::bmp(filepath, message)) {
            std::cout << CAN_BE_USED << std::endl;
            return true;
        }
    } else if (magic == extension && extension == "png") {
        if (checker::png(filepath)) {
            std::cout << CAN_BE_USED << std::endl;
            return true;
        }
    }
    return false;
}

auto info(const std::string &filepath) -> bool {
    if (check(filepath, ""))
        return true;
    return false;
}

auto decrypt(const std::string &filepath, const std::string &key) -> bool {
    std::string message;
    std::cout << "Decrypting..." << std::endl;
    if (checker::magics(filepath) == "png")
        message = decryptor::png(filepath, key);
    else if (checker::magics(filepath) == "bmp") {
        int length;
        try {
            length = std::stoi(key);
        } catch (std::exception &e) {
            std::cerr << RED << "Invalid length! " << &e << RESET << std::endl;
        }
        message = decryptor::bmp(filepath, length);
    }

    std::cout << message << std::endl;
    return true;
}

auto encrypt(const std::string &filepath, const std::string &message, const std::string &key) -> bool {
    if (check(filepath, message)) {
        std::cout << "Checking if everything is correct..." << std::endl;
        if (checker::magics(filepath) == "png") {
            if (!key.empty()) {
                if (key.size() > 1) {
                    std::cerr << RED << "Key must not be more than one character!" << RESET << std::endl;
                    return false;
                }
            } else {
                std::cerr << RED << "The key value must be provided!" << RESET << std::endl;
                return false;
            }

            std::string new_filepath = filepath;
            new_filepath.resize(new_filepath.size() - 4);
            new_filepath = new_filepath + "_mod_.png";

            bool are_equal;

            do {
                std::cout << "Encrypting..." << std::endl;
                encryptor::png(filepath, message, key);
                std::cout << "Checking if encryption was correct..." << std::endl;
                std::string check_message = decryptor::png(new_filepath, key);
                are_equal = check_message == message;
                if (!are_equal)
                    std::cout << YELLOW << "Something went wrong!" << RESET << "\nRetrying" << std::endl;
            } while (!are_equal);
        } else if (checker::magics(filepath) == "bmp") {
            if (!key.empty())
                std::cout << YELLOW << "Key value is redundant here!" << RESET << std::endl;
            std::cout << "Encrypting..." << std::endl;
            encryptor::bmp(filepath, message);
        }
    }

    std::cout << GREEN << "The message was encrypted successfully!" << RESET << std::endl;
    return true;
}

int main(const int argc, char *argv[]) {
    const std::string option = argc > 1 ? argv[1] : "";
    const std::string key = argc > 4 ? argv[4] : "";
    const std::string length = argc > 3 ? argv[3] : "";
    if (argc < 2 || option == "-h" || option == "--help") {
        if (argc > 2) {
            std::cerr << INVALID_ARGS << std::endl;
            help();
            return 1;
        }
        help();
        return 0;
    }

    if (option == "-i" || option == "--info") {
        if (argc > 3) {
            std::cerr << INVALID_ARGS << std::endl;
            help();
            return 1;
        }
        return info(argv[2]) ? 0 : 1;
    }

    if (option == "-c" || option == "--check") {
        if (argc > 4) {
            std::cerr << INVALID_ARGS << std::endl;
            help();
            return 1;
        }
        return check(argv[2], argv[3]);
    }

    if (option == "-e" || option == "--encrypt") {
        if (argc > 5) {
            std::cerr << INVALID_ARGS << std::endl;
            help();
            return 0;
        }
        return encrypt(argv[2], argv[3], key);
    }

    if (option == "-d" || option == "--decrypt") {
        if (argc > 5) {
            std::cerr << INVALID_ARGS << std::endl;
            help();
            return 1;
        }
        return decrypt(argv[2], length);
    }
    return 1;
}
