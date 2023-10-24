#define NOMINMAX
#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <stack>
#include "Dll1.h"
#include <windows.h>



class Dll1 {
public:
    typedef std::string(*encrypt_ptr_t)(std::string, int);
    typedef std::string(*decrypt_ptr_t)(std::string, int);
    HINSTANCE handle;
    decrypt_ptr_t decrypt_ptr;
    encrypt_ptr_t encrypt_ptr;
    Dll1() : handle(nullptr), decrypt_ptr(nullptr), encrypt_ptr(nullptr) {
        handle = LoadLibrary(TEXT("Dll1.dll"));
        if (handle != nullptr || handle != INVALID_HANDLE_VALUE) {
            decrypt_ptr = (decrypt_ptr_t)GetProcAddress(handle, "decrypt");
            encrypt_ptr = (encrypt_ptr_t)GetProcAddress(handle, "encrypt");
        }
    }
    ~Dll1() {
        FreeLibrary(handle);
    }

    std::string decrypt(std::string text, int key) {
        if (decrypt_ptr == nullptr)
        {
            return "Function not found";
        }
        return decrypt_ptr(text, key);
    }

    std::string encrypt(std::string text, int key) {

        if (encrypt_ptr == nullptr)
        {
            return "Function not found";
        }
        return encrypt_ptr(text, key);
    }
};

class FileRW {
public:
    Dll1 cipher;
    std::vector<std::string> bufferD;
    std::vector<std::string> bufferE;
    void Read(std::string command, std::string name, int key) {
        std::string newLine;
        std::ifstream file(name);
        if (!file.is_open()) {
            std::cout << "File not found" << std::endl;
        }
        if (command == "15") {
            while (getline(file, newLine)) {
                newLine = cipher.decrypt(newLine, key);
                bufferD.push_back(newLine);
            }
        }
        if (command == "16") {
            while (getline(file, newLine)) {
                newLine = cipher.encrypt(newLine, key);
                bufferE.push_back(newLine);
            }
        }
        file.close();

    }

    void Write(std::string command, std::string name) {
        std::ofstream file(name);
        if (!file.is_open()) {
            std::cout << "File not found" << std::endl;
        }
        if (command == "15") {
            for (size_t i = 0; i < bufferD.size(); i++)
            {
                file << bufferD[i] << std::endl;
            }
            bufferD.clear();
        }
        if (command == "16") {
            for (size_t i = 0; i < bufferE.size(); i++)
            {
                file << bufferE[i] << std::endl;
            }
            bufferE.clear();
        }
        file.close();
    }


};

class FileClass {
public:
    FileClass(const std::string& fileName) : file_name(fileName) {}

    std::string file_name;
};

class FileActions {
public:
    FileActions() {
    } 

    void read_from_file(const FileClass& fileN) {
        std::cout << "You chose read from file option." << std::endl;
        std::ifstream file(fileN.file_name);
        if (!file.is_open()) {
            std::cout << "Error opening file." << std::endl;
        }
        else {
            std::string line;
            while (std::getline(file, line)) {
                std::cout << line << std::endl;
            }
            file.close();
        }
    }

    void save_in_file(const FileClass& fileN, const std::vector<std::string>& buffer) {
        std::cout << "You chose write in file option." << std::endl;
        std::ofstream file(fileN.file_name);
        if (file.is_open()) {
            for (const auto& line : buffer) {
                if (!line.empty()) {
                    file << line;
                }
            }
            file.close();
        }
    }

    void search_in_file(const FileClass& fileN) {
        printf("You chose find text in file option.\n");
        std::ifstream file(fileN.file_name);
        std::string line;
        std::string substring;

        int row = 0;
        printf("Enter the substring to search for: ");
        std::cin >> substring;

        while (std::getline(file, line)) {
            int col = 0;
            size_t index = 0;

            while ((index = line.find(substring, index)) != std::string::npos) {
                std::cout << "Row: " << row << ", Column: " << col + index << std::endl;
                index += substring.length();
            }

            row++;
        }

        file.close();
    }
};

class BufferClass {
public:
    BufferClass() {
        undo_stack.push(buffer);
    }
    void undo() {
        std::cout << "You chose undo option." << std::endl;
        if (undo_stack.size() < 2) {
            std::cout << "Undo is not available." << std::endl;
            return;
        }

        redo_stack.push(buffer);
        buffer = undo_stack.top();
        undo_stack.pop();
    }

    void redo() {
        std::cout << "You chose redo option." << std::endl;
        if (redo_stack.empty()) {
            std::cout << "Redo is not available." << std::endl;
            return;
        }


        undo_stack.push(buffer);
        buffer = redo_stack.top();
        redo_stack.pop();
    }

    const std::vector<std::string>& getBuffer() const {
        return buffer;
    }

    void append() {
        std::cout << "You chose append option." << std::endl;
        std::string text_to_append;
        std::getline(std::cin, text_to_append);
        undo_stack.push(buffer);
        buffer.push_back(text_to_append);
    }

    void output() {
        for (int i = 0; i < buffer.size(); i++)
        {
            std::cout << buffer[i] << std::endl;;
        }
    }

    void deleteFunc() {
        std::cout << "You chose delete option." << std::endl;
        int row;
        int index;
        int num_of_symb;

        std::cout << "Enter the row number: ";
        std::cin >> row;

        std::cout << "Enter the column number: ";
        std::cin >> index;

        std::cout << "Enter the number of symbols to delete: ";
        std::cin >> num_of_symb;

        undo_stack.push(buffer);

        buffer[row].erase(index, num_of_symb);

    }

    void copy() {
        std::cout << "You chose copy option." << std::endl;
        int row;
        int index;
        int num_of_symb;

        std::cout << "Enter the row number: ";
        std::cin >> row;

        std::cout << "Enter the column number: ";
        std::cin >> index;

        std::cout << "Enter the number of symbols to copy: ";
        std::cin >> num_of_symb;

        clipboard = buffer[row].substr(index, num_of_symb);
        
    }

    void cut() {
        std::cout << "You chose cut option." << std::endl;
        int row;
        int index;
        int num_of_symb;

        std::cout << "Enter the row number: ";
        std::cin >> row;

        std::cout << "Enter the column number: ";
        std::cin >> index;

        std::cout << "Enter the number of symbols to cut: ";
        std::cin >> num_of_symb;

        clipboard = buffer[row].substr(index, num_of_symb);

        undo_stack.push(buffer);
        
        buffer[row].erase(index, num_of_symb);
        
    }

    void paste() {
        std::cout << "You chose paste option." << std::endl;
        int row;
        int index;
        int num_of_symb;

        std::cout << "Enter the row number: ";
        std::cin >> row;

        std::cout << "Enter the column number: ";
        std::cin >> index;

        undo_stack.push(buffer);


        buffer[row].insert(index, clipboard);
        
    }

    void replace() {
        std::cout << "You chose replace option." << std::endl;
        int row;
        int index;
        int num_of_symb;

        std::cout << "Enter the text to add: ";

        std::string text_to_replace;
        std::getline(std::cin, text_to_replace);

        std::cout << "Enter the row number: ";
        std::cin >> row;

        std::cout << "Enter the column number: ";
        std::cin >> index;

        undo_stack.push(buffer);

        buffer[row].erase(index, text_to_replace.size());
        buffer[row].insert(index, text_to_replace);

    }

private:
    int initial_lines = 20;
    int initial_symbols = 50;
    int current_line = 0;
    std::vector<std::string> buffer;
    std::stack<std::vector<std::string>> undo_stack;
    std::stack<std::vector<std::string>> redo_stack; 
    std::string clipboard;
};

void clear_console() {
    system("cls");
    std::cout << "You chose clear console option." << std::endl;
}

int main() {
    BufferClass buffer;
    FileClass file("myfile.txt");
    FileActions fileActions;
    FileRW cipher;
    int loop = 1;
    int choice;
    int key;
    while (loop) {
        std::cout << "\nEnter a number (1-14): ";
        std::cin >> choice;
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

        switch (choice) {
        case 1:
            buffer.append();
            break;
        case 2:
            std::cout << "No." << std::endl;
            break;
        case 3:
            buffer.output();
            break;
        case 4:
            fileActions.save_in_file(file, buffer.getBuffer());
            break;
        case 5:
            fileActions.read_from_file(file);
            break;
        case 6:
            fileActions.search_in_file(file); 
            break;
        case 7:
            std::cout << "You chose nonexistent insert function." << std::endl;
            break;
        case 8:
            buffer.deleteFunc();
            break;
        case 9:
            buffer.undo();
            break;
        case 10:
            buffer.redo();
            break;
        case 11:
            buffer.copy();
            break;
        case 12:
            buffer.cut();
            break;
        case 13:
            buffer.paste();
            break;
        case 14:
            buffer.replace();
            break;
        case 15:
            std::cin >> key;
            cipher.Read("15", "C:/Labs_Kse/Paradigms of Programming/Text_EditorV2/myfile.txt", key);
            break;
        case 16:
            std::cin >> key;
            cipher.Read("16", "C:/Labs_Kse/Paradigms of Programming/Text_EditorV2/myfile.txt", key);
            break;
        case 17:
            cipher.Write("15", "C:/Labs_Kse/Paradigms of Programming/Text_EditorV2/myfile.txt");
            break;
        case 18:
            cipher.Write("16", "C:/Labs_Kse/Paradigms of Programming/Text_EditorV2/myfile.txt");
            break;
        case 19:
            clear_console();
            break;
        default:
            std::cout << "Invalid choice." << std::endl;
            break;
        }
    }
    return 0;
}
