/**
 * @file HeaderBuffer.cpp
 * @brief implementation of the HeaderBuffer class
 * @author Dristi Barnwal (original author)
 * @author Ethan Jackson (refactoring and additional comments)
 * @author Marcus Julius, Teagen Lee, Natoli Mayu (reviewers)
 * @date March 2026
 */
#include "HeaderBuffer.h"
#include <sstream>
#include <iostream>
#include <iomanip>
#include <cctype>

using namespace std;

/**
 * @brief Default constructor for the HeaderBuffer class.
 */

HeaderBuffer::HeaderBuffer() {}

/**
 * @brief Builds a default header structure.
 *
 * Initializes the file header metadata including file type,
 * version number, record size formatting, index file name,
 * record count, and field descriptions for Zip Code records.
 *
 * @param indexFileName Name of the primary key index file.
 * @param recordCount Number of records stored in the data file.
 */

void HeaderBuffer::buildDefault(const string& indexFileName, long recordCount) {
    header_.fileType = "ZipLenFile";
    header_.version = 2;
    header_.recordSizeByteCount = 22; //minimum record size (State is 2 chars)
    header_.sizeFormatType = 'A';
    header_.sizeOfSizes = 10; //10 digits in ASCII
    header_.sizeIncludesItself = true; //based on how header size is calculated
    header_.indexFileName = indexFileName;
    header_.recordCount = recordCount;
    header_.primaryKeyFieldIndex = 0;
    header_.staleIndex = false;
    header_.fieldNames = {"ZipCode", "PlaceName", "State", "County", //continued
                          "Longitude", "Latitude"};
    header_.fieldTypes = {"int","string","string","string","double","double"}
    header_.fieldCount = (int)header_.fieldNames.size();
    header_.headerSizeBytes = (int)serialize().size();
}
/**
 * @brief Retrieves the current file header.
 *
 * @return Constant reference to the FileHeader structure.
 */
const FileHeader& HeaderBuffer::getHeader() const {
    return header_;
}
/**
 * @brief Writes the header record to the output file.
 *
 * The header is serialized and written using a length indicated
 * record format.
 *
 * @param out Output file stream.
 * @return True if the header was written successfully.
 */
bool HeaderBuffer::write(ofstream& out) {
    return writeLenLine(out, serialize());
}

/**
 * @brief Reads the header record from an input file.
 *
 * @param in Input file stream.
 * @return True if the header was successfully read and parsed.
 */
bool HeaderBuffer::read(ifstream& in) {
    string s;
    if (!readLenLine(in, s)) return false;
    return deserialize(s);
}

/**
 * @brief Prints the header metadata to the console.
 *
 * This function is for debugging and verifying
 * the contents of the file header.
 */
void HeaderBuffer::print() const {
    cout << "File type:\t    " << header_.fileType << "\n";
    cout << "Version:\t    " << header_.version << "\n";
    cout << "Header size:\t    " << header_.headerSizeBytes << " bytes\n";
    cout << "Size format:\t    " << header_.sizeFormatType << "\n";
    cout << "Size of sizes:\t    " << header_.sizeOfSizes << " bytes\n";
    cout<<"Size counts itself: "<<(header_.sizeIncludesItself?"yes":"no")<<"\n";
    cout << "Index file:\t    " << header_.indexFileName << "\n";
    cout << "Record count:\t    " << header_.recordCount << "\n";
    cout << "Field count:\t    " << header_.fieldCount << "\n";
    cout << "Primary key field:  " << header_.primaryKeyFieldIndex << "\n";
    cout << "Stale index:\t    " << (header_.staleIndex ? "yes" : "no") << "\n";
    for (int i = 0; i < (int)header_.fieldCount; i++) {
        cout << "Field[" << i << "]:\t    " << header_.fieldNames[i];
             << " (" << header_.fieldTypes[i] << ")\n";
    }
}
/**
 * @brief Serializes the header structure into a comma separated string.
 *
 * The serialized string begins with the identifier "HDR" followed
 * by all metadata values and field definitions.
 * 
 * @return Serialized header string.
 */
string HeaderBuffer::serialize() const {
    ostringstream ss;
    ss << "HDR"
       << "," << header_.fileType
       << "," << header_.version
       << "," << header_.recordSizeByteCount
       << "," << header_.sizeFormatType
       << "," << header_.sizeOfSizes
       << "," << (header_.sizeIncludesItself ? "1" : "0")
       << "," << header_.indexFileName
       << "," << header_.recordCount
       << "," << header_.fieldCount
       << "," << header_.primaryKeyFieldIndex
       << "," << (header_.staleIndex ? "1" : "0");
    for (const string& f : header_.fieldNames)
        ss << "," << f;
    for (const string& f : header_.fieldTypes)
        ss << "," << f;
    return ss.str();
}
/**
 * @brief Converts a serialized header string into a header structure.
 *
 * Parses the comma separated metadata and reconstructs the
 * FileHeader structure including field descriptors.
 *
 * @param s Serialized header string.
 * @return True if the header was successfully parsed.
 */
bool HeaderBuffer::deserialize(const string& s) {
    istringstream ss(s);
    string tok;
    vector<string> parts;
    while (getline(ss, tok, ',')) parts.push_back(tok);
    if (parts.size() < 14 || (parts.size() % 2 == 1)) return false;
    if (parts[0] != "HDR") return false;
    header_.fileType             = parts[1];
    header_.version              = stoi(parts[2]);
    header_.recordSizeByteCount  = stoi(parts[3]);
    header_.sizeFormatType       = parts[4].at(0);
    header_.sizeOfSizes          = stoi(parts[5]);
    header_.sizeIncludesItself   = (parts[6] == "1");
    header_.indexFileName        = parts[7];
    header_.recordCount          = stol(parts[8]);
    header_.fieldCount           = stoi(parts[9]);
    header_.primaryKeyFieldIndex = stoi(parts[10]);
    header_.staleIndex           = (parts[11] == "1");
    header_.fieldNames.clear();
    header_.fieldTypes.clear();
    if (header_.fieldCount * 2 + 12 != parts.size()) return false;
    int typesStart = header_.fieldCount + 12;
    for (int i = 12; i < typesStart; i++)
        header_.fieldNames.push_back(parts[i]);
    for (int i = typesStart; i < (int)parts.size(); i++)
        header_.fieldTypes.push_back(parts[i]);
    return true;
}
/**
 * @brief Writes a length indicated record to the output file.
 * @param out Output file stream.
 * @param text Text content to write.
 * @return True if the operation succeeded.
 */

bool HeaderBuffer::writeLenLine(ofstream& out, const string& text) {
    out << setw(10) << setfill('0') << text.size()
        << ' ' << text << '\n';
    return static_cast<bool>(out);
}
/**
 * @brief Reads a length indicated record from the input file.
 *
 * The function first reads a fixed-width numeric field indicating
 * the size of the following record, then reads the record text.
 *
 * @param in Input file stream.
 * @param text Output variable storing the record text.
 * @return True if the record was successfully read.
 */
bool HeaderBuffer::readLenLine(ifstream& in, string& text) {
    char buf[10];
    if (!in.read(buf, 10)) return false;
    for (int i = 0; i < 10; i++)
        if (!isdigit((unsigned char)buf[i])) return false;
    char sp;
    if (!in.get(sp) || sp != ' ') return false;
    int len = stoi(string(buf, 10));
    text.resize(len);
    if (!in.read(&text[0], len)) return false;
    if (in.peek() == '\r') in.get();
    if (in.peek() == '\n') in.get();
    return true;
}
