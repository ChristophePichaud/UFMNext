#pragma once

#include "Document.h"
#include <string>
#include <iostream>

namespace ufmnext::core {

class Serializer {
public:
    static std::string SerializeDocumentToRawData(const Document& doc);
    static bool DeserializeDocumentFromRawData(const std::string& data, Document& doc);

    static bool SaveToFile(const Document& doc, const std::string& filePath);
    static bool LoadFromFile(const std::string& filePath, Document& doc);
};

} // namespace ufmnext::core
