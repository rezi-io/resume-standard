#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <map>
#include <sstream>
#include <iomanip>
#include <cctype>
#include <algorithm>
#include <cstring>  // Added this include for strlen()

// For Windows binary mode stdin
#ifdef _WIN32
#include <io.h>
#include <fcntl.h>
#endif

// Simple JSON string escaping
std::string escapeJSON(const std::string& input) {
    std::ostringstream ss;
    for (auto c : input) {
        switch (c) {
            case '"': ss << "\\\""; break;
            case '\\': ss << "\\\\"; break;
            case '\b': ss << "\\b"; break;
            case '\f': ss << "\\f"; break;
            case '\n': ss << "\\n"; break;
            case '\r': ss << "\\r"; break;
            case '\t': ss << "\\t"; break;
            default:
                if ('\x00' <= c && c <= '\x1f') {
                    ss << "\\u" << std::hex << std::setw(4) << std::setfill('0') << static_cast<int>(c);
                } else {
                    ss << c;
                }
        }
    }
    return ss.str();
}

// Trim whitespace from string
std::string trim(const std::string& str) {
    size_t first = str.find_first_not_of(" \t\n\r");
    if (first == std::string::npos) return "";
    size_t last = str.find_last_not_of(" \t\n\r");
    return str.substr(first, last - first + 1);
}

// Function to find substring between two markers
std::string extractBetween(const std::string& source, const std::string& startMarker, const std::string& endMarker, size_t startPos = 0) {
    size_t begin = source.find(startMarker, startPos);
    if (begin == std::string::npos) return "";

    begin += startMarker.length();
    size_t end = source.find(endMarker, begin);
    if (end == std::string::npos) return "";

    return source.substr(begin, end - begin);
}

// Finds all occurrences of a pattern in a string and returns start positions
std::vector<size_t> findAll(const std::string& source, const std::string& pattern, size_t startPos = 0) {
    std::vector<size_t> positions;
    size_t pos = startPos;
    while ((pos = source.find(pattern, pos)) != std::string::npos) {
        positions.push_back(pos);
        pos += pattern.length();
    }
    return positions;
}

// Clean up key by removing namespace prefix
std::string cleanKey(const std::string& key) {
    size_t colonPos = key.find(':');
    if (colonPos != std::string::npos) {
        return key.substr(colonPos + 1);
    }
    return key;
}

// Class to handle PDF XMP metadata extraction
class PDFMetadataExtractor {
private:
    std::string content;
    std::map<std::string, std::string> metadata;
    std::map<std::string, std::string> cleanedMetadata;
    bool hasRMPProducer;
    std::vector<std::string> namespaces;

    // Extract XMP packet from PDF content using basic string operations
    std::string extractXMPPacket() {
        // Try to find the XMP packet
        size_t startPos = content.find("<?xmp-packet");
        if (startPos != std::string::npos) {
            size_t endPos = content.find("</x:xmpmeta>", startPos);
            if (endPos != std::string::npos) {
                endPos += 12; // Length of "</x:xmpmeta>"
                return content.substr(startPos, endPos - startPos);
            }
        }

        // Try alternate format
        startPos = content.find("<x:xmpmeta");
        if (startPos != std::string::npos) {
            size_t endPos = content.find("</x:xmpmeta>", startPos);
            if (endPos != std::string::npos) {
                endPos += 12; // Length of "</x:xmpmeta>"
                return content.substr(startPos, endPos - startPos);
            }
        }

        // Try without namespace
        startPos = content.find("<xmpmeta");
        if (startPos != std::string::npos) {
            size_t endPos = content.find("</xmpmeta>", startPos);
            if (endPos != std::string::npos) {
                endPos += 10; // Length of "</xmpmeta>"
                return content.substr(startPos, endPos - startPos);
            }
        }

        // Look for PDF/A XMP
        startPos = content.find("<pdfaid:part");
        if (startPos != std::string::npos) {
            // Try to find a containing XMP structure
            size_t xmpStart = content.rfind("<x:xmpmeta", startPos);
            if (xmpStart != std::string::npos) {
                size_t xmpEnd = content.find("</x:xmpmeta>", startPos);
                if (xmpEnd != std::string::npos) {
                    xmpEnd += 12;
                    return content.substr(xmpStart, xmpEnd - xmpStart);
                }
            }

            // Just extract the PDF/A info if no XMP container found
            size_t endPos = content.find(">", startPos);
            if (endPos != std::string::npos) {
                return content.substr(startPos, endPos - startPos + 1);
            }
        }

        return "";
    }

    // Extract namespace declarations from XMP
    void extractNamespaces(const std::string& xmpData) {
        // Find all xmlns: declarations
        size_t pos = 0;
        while (true) {
            size_t xmlnsPos = xmpData.find("xmlns:", pos);
            if (xmlnsPos == std::string::npos) break;

            // Find the equals sign
            size_t equalsPos = xmpData.find('=', xmlnsPos);
            if (equalsPos == std::string::npos) break;

            // Extract namespace prefix
            std::string prefix = xmpData.substr(xmlnsPos + 6, equalsPos - xmlnsPos - 6);
            prefix = trim(prefix);
            namespaces.push_back(prefix);

            pos = equalsPos + 1;
        }

        // Add standard namespaces that might not be explicitly declared
        namespaces.push_back("pdf");
        namespaces.push_back("dc");
        namespaces.push_back("xmp");
        namespaces.push_back("pdfaid");
        namespaces.push_back("rdf");

        // Remove duplicates
        std::sort(namespaces.begin(), namespaces.end());
        namespaces.erase(std::unique(namespaces.begin(), namespaces.end()), namespaces.end());
    }

    // Extract attributes from XML tags
    void extractTagAttributes(const std::string& tag, std::map<std::string, std::string>& attributes) {
        size_t pos = 0;
        while (true) {
            // Find attribute name
            size_t nameStart = tag.find_first_of("abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ:", pos);
            if (nameStart == std::string::npos || nameStart >= tag.length() - 1) break;

            // Find equals sign
            size_t equalsPos = tag.find('=', nameStart);
            if (equalsPos == std::string::npos) break;

            // Extract name
            std::string name = tag.substr(nameStart, equalsPos - nameStart);
            name = trim(name);

            // Find opening quote
            size_t quotePos = tag.find('"', equalsPos);
            if (quotePos == std::string::npos) {
                // Try single quote
                quotePos = tag.find('\'', equalsPos);
                if (quotePos == std::string::npos) break;

                // Find closing single quote
                size_t endQuotePos = tag.find('\'', quotePos + 1);
                if (endQuotePos == std::string::npos) break;

                // Extract value
                std::string value = tag.substr(quotePos + 1, endQuotePos - quotePos - 1);

                // Store attribute
                attributes[name] = value;

                // Move position
                pos = endQuotePos + 1;
            } else {
                // Find closing double quote
                size_t endQuotePos = tag.find('"', quotePos + 1);
                if (endQuotePos == std::string::npos) break;

                // Extract value
                std::string value = tag.substr(quotePos + 1, endQuotePos - quotePos - 1);

                // Store attribute
                attributes[name] = value;

                // Move position
                pos = endQuotePos + 1;
            }
        }
    }

    // Extract all metadata based on identified namespaces
    void extractAllNamespacedMetadata(const std::string& xmpData) {
        for (const auto& ns : namespaces) {
            // Skip namespace prefixes that might cause issues
            if (ns == "rdf" || ns == "xmlns" || ns.empty()) continue;

            // Look for tags with this namespace
            std::string nsPrefix = "<" + ns + ":";
            std::vector<size_t> positions = findAll(xmpData, nsPrefix);

            for (size_t pos : positions) {
                // Find the tag name
                size_t tagStart = pos + nsPrefix.length();
                size_t tagEnd = xmpData.find_first_of(" \t\n\r>", tagStart);
                if (tagEnd == std::string::npos) continue;

                std::string tagName = xmpData.substr(tagStart, tagEnd - tagStart);

                // Find the full tag
                size_t closePos = xmpData.find(">", tagEnd);
                if (closePos == std::string::npos) continue;

                // Check if it's a self-closing tag
                bool selfClosing = (xmpData[closePos - 1] == '/');

                if (selfClosing) {
                    // Extract attributes from self-closing tag
                    std::string fullTag = xmpData.substr(pos, closePos - pos);
                    std::map<std::string, std::string> attrs;
                    extractTagAttributes(fullTag, attrs);

                    for (const auto& attr : attrs) {
                        metadata[ns + ":" + tagName + ":" + attr.first] = attr.second;
                    }
                } else {
                    // Find the corresponding end tag
                    std::string endTag = "</" + ns + ":" + tagName + ">";
                    size_t endPos = xmpData.find(endTag, closePos);
                    if (endPos == std::string::npos) continue;

                    // Extract content between tags
                    std::string content = xmpData.substr(closePos + 1, endPos - closePos - 1);
                    content = trim(content);

                    // Store the value if it's not empty and doesn't contain child tags
                    if (!content.empty() && content.find('<') == std::string::npos) {
                        metadata[ns + ":" + tagName] = content;
                    }

                    // Also check for rdf:Alt/rdf:li pattern
                    if (content.find("<rdf:Alt>") != std::string::npos) {
                        size_t liStart = content.find("<rdf:li");
                        if (liStart != std::string::npos) {
                            size_t liClose = content.find(">", liStart);
                            if (liClose != std::string::npos) {
                                size_t liEnd = content.find("</rdf:li>", liClose);
                                if (liEnd != std::string::npos) {
                                    std::string liContent = content.substr(liClose + 1, liEnd - liClose - 1);
                                    liContent = trim(liContent);
                                    metadata[ns + ":" + tagName] = liContent;
                                }
                            }
                        }
                    }

                    // Extract attributes from the opening tag
                    std::string openingTag = xmpData.substr(pos, closePos - pos);
                    std::map<std::string, std::string> attrs;
                    extractTagAttributes(openingTag, attrs);

                    for (const auto& attr : attrs) {
                        metadata[ns + ":" + tagName + ":" + attr.first] = attr.second;
                    }
                }
            }
        }
    }

    // Extract PDF-specific metadata including Producer
    void extractPDFMetadata(const std::string& xmpData) {
        std::vector<std::string> pdfElements = {"Producer", "CreationDate", "ModDate", "Creator", "Title", "Author"};

        for (const auto& element : pdfElements) {
            std::string startTag = "<pdf:" + element + ">";
            std::string endTag = "</pdf:" + element + ">";

            std::string value = extractBetween(xmpData, startTag, endTag);
            if (!value.empty()) {
                metadata["pdf:" + element] = trim(value);

                // Check for rmp-v2.*.* in Producer
                if (element == "Producer") {
                    size_t pos = value.find("rmp-v2.");
                    if (pos != std::string::npos) {
                        hasRMPProducer = true;
                    }

                    // Also check for rz-v* pattern (like in your example)
                    pos = value.find("rz-v");
                    if (pos != std::string::npos) {
                        hasRMPProducer = true;
                    }
                }
            }
        }
    }

    // Extract Dublin Core elements
    void extractDublinCoreElements(const std::string& xmpData) {
        std::vector<std::string> dcElements = {"title", "creator", "description", "subject", "publisher",
                                            "contributor", "date", "type", "format", "identifier",
                                            "source", "language", "relation", "coverage", "rights"};

        for (const auto& element : dcElements) {
            // Try to find the simple format first
            std::string startTag = "<dc:" + element + ">";
            std::string endTag = "</dc:" + element + ">";

            std::string value = extractBetween(xmpData, startTag, endTag);
            if (!value.empty()) {
                metadata["dc:" + element] = trim(value);
                continue;
            }

            // Try alt format
            startTag = "<dc:" + element + "><rdf:Alt><rdf:li";
            size_t startPos = xmpData.find(startTag);
            if (startPos != std::string::npos) {
                // Find the first > after <rdf:li
                size_t tagEndPos = xmpData.find(">", startPos + startTag.length());
                if (tagEndPos != std::string::npos) {
                    size_t valueStart = tagEndPos + 1;
                    size_t valueEnd = xmpData.find("</rdf:li>", valueStart);
                    if (valueEnd != std::string::npos) {
                        value = xmpData.substr(valueStart, valueEnd - valueStart);
                        metadata["dc:" + element] = trim(value);
                    }
                }
            }
        }
    }

    // Parse RDF metadata from XMP
    void parseRDFMetadata(const std::string& xmpData) {
        // Extract namespaces first
        extractNamespaces(xmpData);

        // Extract RDF Description blocks
        std::string startTag = "<rdf:Description";
        std::string endTag = "</rdf:Description>";

        size_t pos = 0;
        while (true) {
            size_t startPos = xmpData.find(startTag, pos);
            if (startPos == std::string::npos) break;

            size_t tagEndPos = xmpData.find(">", startPos);
            if (tagEndPos == std::string::npos) break;

            // Check if it's a self-closing tag
            if (xmpData[tagEndPos - 1] == '/') {
                // Extract attributes from self-closing tag
                std::string openingTag = xmpData.substr(startPos, tagEndPos - startPos);
                extractTagAttributes(openingTag, metadata);

                // Move to next potential description
                pos = tagEndPos + 1;
                continue;
            }

            size_t endPos = xmpData.find(endTag, tagEndPos);
            if (endPos == std::string::npos) break;

            // Extract the opening tag to get attributes
            std::string openingTag = xmpData.substr(startPos, tagEndPos - startPos);
            extractTagAttributes(openingTag, metadata);

            // Extract the content
            std::string content = xmpData.substr(tagEndPos + 1, endPos - tagEndPos - 1);

            // Move to next description
            pos = endPos + endTag.length();
        }

        // Extract metadata for all namespaces
        extractAllNamespacedMetadata(xmpData);

        // Special handling for Dublin Core elements and PDF metadata
        extractDublinCoreElements(xmpData);
        extractPDFMetadata(xmpData);

        // Create cleaned metadata map
        for (const auto& pair : metadata) {
            cleanedMetadata[cleanKey(pair.first)] = pair.second;
        }
    }

    // Scan for XMP in byte by byte, handling arbitrary binary data
    bool scanForXMP() {
        const char* xmpStart1 = "<?xmp-packet";
        const char* xmpStart2 = "<x:xmpmeta";
        const char* xmpEnd = "</x:xmpmeta>";
        const size_t len1 = strlen(xmpStart1);
        const size_t len2 = strlen(xmpStart2);
        const size_t endLen = strlen(xmpEnd);

        // Scan for start marker
        for (size_t i = 0; i < content.size() - len1; ++i) {
            if ((i + len1 <= content.size() && content.compare(i, len1, xmpStart1) == 0) ||
                (i + len2 <= content.size() && content.compare(i, len2, xmpStart2) == 0)) {

                // Found a start marker, now look for the end
                for (size_t j = i; j < content.size() - endLen; ++j) {
                    if (content.compare(j, endLen, xmpEnd) == 0) {
                        // Found complete XMP block
                        std::string xmpBlock = content.substr(i, j + endLen - i);
                        parseRDFMetadata(xmpBlock);
                        return true;
                    }
                }
            }
        }

        return false;
    }

public:
    PDFMetadataExtractor() : hasRMPProducer(false) {}

    // Load and parse PDF file
    bool loadPDF(const std::string& filename) {
        std::ifstream file(filename, std::ios::binary);
        if (!file) {
            std::cerr << "Error: Could not open file: " << filename << std::endl;
            return false;
        }

        // Read file content
        content.assign((std::istreambuf_iterator<char>(file)),
                      std::istreambuf_iterator<char>());
        file.close();

        return parseContent();
    }

    // Load and parse PDF from buffer in memory
    bool loadFromBuffer(const char* buffer, size_t size) {
        if (!buffer || size == 0) {
            std::cerr << "Error: Invalid buffer or size" << std::endl;
            return false;
        }

        // Copy buffer content to the content string
        content.assign(buffer, size);

        return parseContent();
    }

    // Common parsing logic used by both loading methods
    bool parseContent() {
        // Extract and parse XMP data
        std::string xmpData = extractXMPPacket();
        if (xmpData.empty()) {
            std::cerr << "No XMP metadata found with standard methods, trying deeper scan..." << std::endl;
            if (!scanForXMP()) {
                std::cerr << "No XMP metadata found in the file." << std::endl;
                return false;
            }
            return true;
        }

        parseRDFMetadata(xmpData);
        return true;
    }

    // Smart load function that detects whether input is a file path or a buffer
    bool load(const void* input, size_t size = 0) {
        // If size is 0 and input is a null-terminated string, assume it's a file path
        if (size == 0 && input != nullptr) {
            const char* path = static_cast<const char*>(input);
            return loadPDF(path);
        }
        // Otherwise, treat it as a buffer
        else if (input != nullptr && size > 0) {
            const char* buffer = static_cast<const char*>(input);
            return loadFromBuffer(buffer, size);
        }

        std::cerr << "Error: Invalid input" << std::endl;
        return false;
    }

    // Check if the PDF was produced by rmp-v2.*.*
    bool isRMPProducer() const {
        return hasRMPProducer;
    }

    // Get all extracted metadata (raw with namespaces)
    const std::map<std::string, std::string>& getMetadata() const {
        return metadata;
    }

    // Output metadata as JSON with clean keys (no namespaces)
    std::string toCleanJSON() const {
        std::ostringstream json;
        json << "{\n  \"status\": \"success\",\n  \"data\": {\n";

        bool first = true;
        for (const auto& pair : cleanedMetadata) {
            if (!first) {
                json << ",\n";
            }
            json << "    \"" << escapeJSON(pair.first) << "\": \"" << escapeJSON(pair.second) << "\"";
            first = false;
        }

        json << "\n  }\n}";
        return json.str();
    }

    // Output empty result JSON
    static std::string emptyJSON() {
        return "{\n  \"status\": \"empty\"\n}";
    }

    // Print summary of metadata found
    void printSummary() const {
        std::cout << "Total metadata entries found: " << metadata.size() << std::endl;

        // Group by namespace
        std::map<std::string, int> nsCounts;
        for (const auto& pair : metadata) {
            size_t colonPos = pair.first.find(':');
            if (colonPos != std::string::npos) {
                std::string ns = pair.first.substr(0, colonPos);
                nsCounts[ns]++;
            }
        }

        // Print namespace counts
        std::cout << "Entries by namespace:" << std::endl;
        for (const auto& ns : nsCounts) {
            std::cout << "  " << ns.first << ": " << ns.second << std::endl;
        }
    }
};

int main(int argc, char* argv[]) {
    PDFMetadataExtractor extractor;
    bool success = false;

    // Check if we should read from stdin
    if (argc == 1 || (argc == 2 && std::string(argv[1]) == "-")) {
        // Read from stdin (binary mode)
        std::string buffer;
        char chunk[4096];

        while (std::cin.read(chunk, sizeof(chunk))) {
            buffer.append(chunk, std::cin.gcount());
        }

        if (std::cin.gcount() > 0) {
            buffer.append(chunk, std::cin.gcount());
        }

        if (!buffer.empty()) {
            success = extractor.loadFromBuffer(buffer.data(), buffer.size());
        } else {
            std::cerr << "Error: No data received on stdin" << std::endl;
            std::cout << PDFMetadataExtractor::emptyJSON() << std::endl;
            return 0;
        }
    }
    // Otherwise, use the file path from command line
    else if (argc == 2) {
        success = extractor.loadPDF(argv[1]);
    }
    else {
        std::cerr << "Usage: " << argv[0] << " [pdf_file|-]" << std::endl;
        std::cerr << "  If no argument or '-' is provided, reads from stdin" << std::endl;
        return 1;
    }

    // Output the result
    if (!success) {
        std::cout << PDFMetadataExtractor::emptyJSON() << std::endl;
        return 0;
    }

    // Print metadata in clean JSON format
    std::cout << extractor.toCleanJSON() << std::endl;

    return 0;
}