#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <filesystem>
#include <limits>
#include <unordered_map>

using namespace std;
namespace fs = std::filesystem;

#define DIRECTORY_SIZE 128
#define MAX_FILE_NAME_SIZE 64 // Increased to 64 bytes
#define BLOCK_SIZE 1024       // 1 KB (1024 bytes)
#define TOTAL_DISK_SIZE 64 * 1024 * 1024 // 64 MB
#define ENCRYPTION_KEY 0x5A

class FileSystem {
private:
    string baseDirectory;
    string currentDirectory;
    unordered_map<string, vector<int>> indexTable;

    string encryptDecrypt(const string& data) {
        string result = data;
        for (char& ch : result) {
            ch ^= ENCRYPTION_KEY; 
        }
        return result;
    }

    void initializeIndexTable() 
    {
        string indexFilePath = baseDirectory + "/index_table.dat";
        if (fs::exists(indexFilePath)) {
            ifstream indexFile(indexFilePath, ios::binary);
            if (indexFile.is_open()) {
                string fileName;
                int blockIndex;
                while (indexFile >> fileName >> blockIndex) {
                    indexTable[fileName].push_back(blockIndex);
                }
                indexFile.close();
            }
        }
         else 
        {
            ofstream indexFile(indexFilePath, ios::binary);
            indexFile.close();
        }
    }

    void saveIndexTable() 
    {
        string indexFilePath = baseDirectory + "/index_table.dat";
        ofstream indexFile(indexFilePath, ios::binary | ios::trunc);
        if (indexFile.is_open()) {
            for (const auto& entry : indexTable) {
                for (int blockIndex : entry.second) {
                    indexFile << entry.first << " " << blockIndex << "\n";
                }
            }
            indexFile.close();
        }
    }

public:
    FileSystem(const string& directory) 
    {
        baseDirectory = directory;
        currentDirectory = directory;

        if (!fs::exists(baseDirectory)) {
            fs::create_directory(baseDirectory);
        }

        initializeIndexTable();
    }

    ~FileSystem() 
    {
        saveIndexTable();
    }

    void createFile(const string& fileName) {
        if (fileName.size() > MAX_FILE_NAME_SIZE) {
            cout << "Error: File name exceeds the maximum allowed length (" << MAX_FILE_NAME_SIZE << " bytes).\n";
            return;
        }

        string filePath = currentDirectory + "/" + fileName;
        if (fs::exists(filePath)) {
            cout << "Error: File \"" << fileName << "\" already exists.\n";
            return;
        }

        ofstream file(filePath);
        if (file) {
            cout << "File \"" << fileName << "\" created successfully.\n";
            file.close();
        } else {
            cout << "Error: Unable to create file \"" << fileName << "\".\n";
        }
    }

    void readFile(const string& fileName) {
        string filePath = currentDirectory + "/" + fileName;
        if (!fs::exists(filePath)) {
            cout << "Error: File \"" << fileName << "\" does not exist.\n";
            return;
        }

        ifstream file(filePath);
        if (file) {
            cout << "Content of \"" << fileName << "\":\n";
            string line;
            while (getline(file, line)) {
                cout << encryptDecrypt(line) << endl; // Decrypt content before displaying
            }
            file.close();
        } else {
            cout << "Error: Unable to read file \"" << fileName << "\".\n";
        }
    }

    void writeFile(const string& fileName, const string& content) {
        if (content.size() > BLOCK_SIZE * 128) { // Maximum File Size is 128 blocks (128 KB)
            cout << "Error: File content exceeds the maximum allowed size (128 KB).\n";
            return;
        }

        string filePath = currentDirectory + "/" + fileName;
        if (!fs::exists(filePath)) {
            cout << "Error: File \"" << fileName << "\" does not exist.\n";
            return;
        }

        ofstream file(filePath, ios::app);
        if (file) {
            file << encryptDecrypt(content) << endl; // Encrypt content before writing
            cout << "File \"" << fileName << "\" updated successfully.\n";
            file.close();
        } else {
            cout << "Error: Unable to write to file \"" << fileName << "\".\n";
        }
    }

    void truncateFile(const string& fileName) {
        string filePath = currentDirectory + "/" + fileName;
        if (!fs::exists(filePath)) {
            cout << "Error: File \"" << fileName << "\" does not exist.\n";
            return;
        }

        ofstream file(filePath, ios::trunc);
        if (file) {
            cout << "File \"" << fileName << "\" truncated successfully.\n";
            file.close();
        } else {
            cout << "Error: Unable to truncate file \"" << fileName << "\".\n";
        }
    }

    void deleteFile(const string& fileName) {
        string filePath = currentDirectory + "/" + fileName;
        if (!fs::exists(filePath)) {
            cout << "Error: File \"" << fileName << "\" does not exist.\n";
            return;
        }

        if (fs::remove(filePath)) {
            indexTable.erase(fileName);
            cout << "File \"" << fileName << "\" deleted successfully.\n";
        } else {
            cout << "Error: Unable to delete file \"" << fileName << "\".\n";
        }
    }

    void createDirectory(const string& dirName) {
        if (dirName.size() > MAX_FILE_NAME_SIZE) {
            cout << "Error: Directory name exceeds the maximum allowed length (" << MAX_FILE_NAME_SIZE << " bytes).\n";
            return;
        }

        string dirPath = currentDirectory + "/" + dirName;
        if (fs::exists(dirPath)) {
            cout << "Error: Directory \"" << dirName << "\" already exists.\n";
            return;
        }

        if (fs::create_directory(dirPath)) {
            cout << "Directory \"" << dirName << "\" created successfully.\n";
        } else {
            cout << "Error: Unable to create directory \"" << dirName << "\".\n";
        }
    }

    void deleteDirectory(const string& dirName) {
        string dirPath = currentDirectory + "/" + dirName;
        if (!fs::exists(dirPath)) {
            cout << "Error: Directory \"" << dirName << "\" does not exist.\n";
            return;
        }

        if (!fs::is_empty(dirPath)) {
            cout << "Error: Directory \"" << dirName << "\" is not empty.\n";
            return;
        }

        if (fs::remove(dirPath)) {
            cout << "Directory \"" << dirName << "\" deleted successfully.\n";
        } else {
            cout << "Error: Unable to delete directory \"" << dirName << "\".\n";
        }
    }

    void changeDirectory(const string& dirName) {
        string dirPath = currentDirectory + "/" + dirName;
        if (!fs::exists(dirPath) || !fs::is_directory(dirPath)) {
            cout << "Error: Directory \"" << dirName << "\" does not exist.\n";
            return;
        }

        currentDirectory = dirPath;
        cout << "Changed to directory \"" << dirName << "\".\n";
    }

    void goBack() {
        if (currentDirectory == baseDirectory) {
            cout << "Already at the root directory.\n";
            return;
        }

        currentDirectory = fs::path(currentDirectory).parent_path().string();
        cout << "Moved back to directory: \"" << currentDirectory << "\".\n";
    }

    void listFilesAndDirectories() {
        if (fs::is_empty(currentDirectory)) {
            cout << "No files or directories exist in \"" << currentDirectory << "\".\n";
            return;
        }

        cout << "Contents of \"" << currentDirectory << "\":\n";
        for (const auto& entry : fs::directory_iterator(currentDirectory)) {
            cout << "- " << entry.path().filename().string();
            if (fs::is_directory(entry.path())) {
                cout << " [DIR]";
            }
            cout << endl;
        }
    }
};

int main() {
    string baseDir = "root";
    FileSystem fs(baseDir);

    cout << "Welcome to the enhanced indexed file system!\n";
    cout << "Commands: create_file, read_file, write_file, truncate_file, delete_file, create_dir, delete_dir, change_dir, list, go_back, exit\n";

    string command, name, content;

    while (true) {
        cout << "> ";
        cin >> command;

        if (command == "create_file") {
            cin >> name;
            fs.createFile(name);
        } else if (command == "read_file") {
            cin >> name;
            fs.readFile(name);
        } else if (command == "write_file") {
            cin >> name;
            cout << "Enter content (end with ';'): ";
            cin.ignore();
            getline(cin, content, ';');
            fs.writeFile(name, content);
        } else if (command == "truncate_file") {
            cin >> name;
            fs.truncateFile(name);
        } else if (command == "delete_file") {
            cin >> name;
            fs.deleteFile(name);
        } else if (command == "create_dir") {
            cin >> name;
            fs.createDirectory(name);
        } else if (command == "delete_dir") {
            cin >> name;
            fs.deleteDirectory(name);
        } else if (command == "change_dir") {
            cin >> name;
            fs.changeDirectory(name);
        } else if (command == "list") {
            fs.listFilesAndDirectories();
        } else if (command == "go_back") {
            fs.goBack();
        } else if (command == "exit") {
            cout << "Exiting file system. Goodbye!\n";
            break;
        } else {
            cout << "Invalid command.\n";
        }
    }

    return 0;
}
