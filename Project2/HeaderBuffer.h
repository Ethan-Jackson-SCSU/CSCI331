/**
 * @file HeaderBuffer.h
 * @brief defines the HeaderBuffer class and the FileHeader struct.
 * @author Dristi Barnwal (primary contributor)
 * @author Ethan Jackson (functional revisions and additional comments)
 * @author Marcus Julius, Teagen Lee, Natoli Mayu (reviewers)
 * @date March 2026 
 */
#ifndef HEADERBUFFER_H
#define HEADERBUFFER_H

#include <string>
#include <vector>
#include <fstream>

using namespace std;

/**
 * @struct FileHeader
 * @brief All header record fields required by the assignment.
 */
struct FileHeader {
    bool sizeIncludesItself;  ///< true if size counts itself
    bool staleIndex;          ///< true if index may be out of date
    char sizeFormatType;      ///< 'A' for ASCII, 'b' for binary
    int version;              ///< format version number, currently 2
    int sizeOfSizes;          ///< number of bytes used for record length
    int headerSizeBytes;      ///< size of header record in bytes
    int recordSizeByteCount;  ///< bytes used for each record size integer
    int fieldCount;           ///< number of fields in each record
    int primaryKeyFieldIndex; ///< 0-based index of the primary key field
    long recordCount;         ///< total number of data records
    string fileType;          ///< name of file type, e.g. "ZipLenFile"
    string indexFileName;     ///< name of the .idx file
    vector<string> fieldNames;///< the names of every field, e.g. "PlaceName"
    vector<string> fieldTypes;///< the data types of every field, e.g. "int"
};

/**
 * @class HeaderBuffer
 * @brief Reads and writes the header record for a length-indicated file.
 */
class HeaderBuffer {
public:
    HeaderBuffer();

    /// Build a default header for the ZIP code file
    void buildDefault(const string& indexFileName, long recordCount);

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