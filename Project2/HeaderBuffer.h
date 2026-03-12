#ifndef HEADERBUFFER_H
#define HEADERBUFFER_H

#include <string>
#include <vector>
#include <fstream>

using namespace std;

/**
 * @struct FieldDescriptor
 * @brief Stores the name and type format for one field in the data file.
 */
struct FieldDescriptor {
    string name;    ///< e.g. "ZipCode", "PlaceName"
    string format;  ///< e.g. "int", "string", "double"
};

/**
 * @struct FileHeader
 * @brief All header record fields required by the assignment.
 */
struct FileHeader {
    string fileType;          ///< e.g. "ZipLenFile"
    int version;              ///< version number, start at 1
    int headerSizeBytes;      ///< size of header record in bytes
    int recordSizeByteCount;  ///< bytes used for each record size integer
    string sizeFormatType;    ///< "ASCII" or "binary"
    int sizeOfSizes;          ///< how many bytes represent the size
    bool sizeIncludesItself;  ///< true if size counts itself
    string indexFileName;     ///< name of the .idx file
    long long recordCount;    ///< total data records
    int fieldCount;           ///< number of fields per record
    vector<FieldDescriptor> fields; ///< one entry per field
    int primaryKeyFieldIndex; ///< 0-based index of the primary key field
    bool staleIndex;          ///< true if index may be out of date
};

/**
 * @class HeaderBuffer
 * @brief Reads and writes the header record for a length-indicated file.
 */
class HeaderBuffer {
public:
    HeaderBuffer();

    /// Build a default header for the ZIP code file
    void buildDefault(const string& indexFileName, long long recordCount);

    /// Write header to an open output stream
    bool write(ofstream& out);

    /// Read header from an open input stream
    bool read(ifstream& in);

    /// Get the loaded header data
    const FileHeader& getHeader() const;

    /// Print header contents to cout
    void print() const;

private:
    FileHeader header_;

    string serialize() const;
    bool deserialize(const string& s);
    bool writeLenLine(ofstream& out, const string& text);
    bool readLenLine(ifstream& in, string& text);
};

#endif