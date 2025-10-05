#pragma once

#include <string>
#include <vector>
#include <fstream>
#include <sstream>

class SimpleEmbeddedDataManager
{
public:
    struct DataSection {
        std::string name;
        size_t offset;
        size_t size;
        bool compressed;
    };
    
    static const std::string MAGIC_HEADER;
    static const std::string SECTION_DELIMITER;
    
private:
    std::vector<DataSection> sections;
    std::string filePath;
    
    bool findSections(std::ifstream& file);
    bool parseSections(std::ifstream& file, size_t startPos);
    
public:
    SimpleEmbeddedDataManager(const std::string& path);
    bool initialize();
    bool readSection(const std::string& sectionName, MemoryBlock& output);
    bool writeSection(const std::string& sectionName, const MemoryBlock& data);
};