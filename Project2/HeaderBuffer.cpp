#include "HeaderBuffer.h"
#include <sstream>
#include <iostream>
#include <iomanip>
#include <cctype>

using namespace std;

HeaderBuffer::HeaderBuffer() {}

void HeaderBuffer::buildDefault(const string& indexFileName,
                                long long recordCount) {
    header_.fileType            = "ZipLenFile";
    header_.version             = 1;
    header_.recordSizeByteCount = 10;
    header_.sizeFormatType      = "ASCII";
    header_.sizeOfSizes         = 10;
    header_.sizeIncludesItself  = false;
    header_.indexFileName       = indexFileName;
    header_.recordCount         = recordCount;
    header_.primaryKeyFieldIndex = 0;
    header_.staleIndex          = false;
    header_.fields = {
        {"ZipCode",   "int"},
        {"PlaceName", "string"},
        {"State",     "string"},
        {"County",    "string"},
        {"Latitude",  "double"},
        {"Longitude", "double"}
    };
    header_.fieldCount      = (int)header_.fields.size();
    header_.headerSizeBytes = (int)serialize().size();
}

const FileHeader& HeaderBuffer::getHeader() const {
    return header_;
}

bool HeaderBuffer::write(ofstream& out) {
    return writeLenLine(out, serialize());
}

bool HeaderBuffer::read(ifstream& in) {
    string s;
    if (!readLenLine(in, s)) return false;
    return deserialize(s);
}

void HeaderBuffer::print() const {
    cout << "File type:         " << header_.fileType << "\n";
    cout << "Version:           " << header_.version << "\n";
    cout << "Header size:       " << header_.headerSizeBytes << " bytes\n";
    cout << "Size format:       " << header_.sizeFormatType << "\n";
    cout << "Size width:        " << header_.sizeOfSizes << " bytes\n";
    cout << "Includes itself:   " << (header_.sizeIncludesItself ? "yes" : "no") << "\n";
    cout << "Index file:        " << header_.indexFileName << "\n";
    cout << "Record count:      " << header_.recordCount << "\n";
    cout << "Field count:       " << header_.fieldCount << "\n";
    cout << "Primary key field: " << header_.primaryKeyFieldIndex << "\n";
    cout << "Stale index:       " << (header_.staleIndex ? "yes" : "no") << "\n";
    for (int i = 0; i < (int)header_.fields.size(); i++) {
        cout << "  Field[" << i << "]: "
             << header_.fields[i].name
             << " (" << header_.fields[i].format << ")\n";
    }
}

string HeaderBuffer::serialize() const {
    ostringstream ss;
    ss << "HDR"
       << "," << header_.fileType
       << "," << header_.version
       << "," << header_.recordSizeByteCount
       << "," << header_.sizeFormatType
       << "," << header_.sizeOfSizes
       << "," << (header_.sizeIncludesItself ? 1 : 0)
       << "," << header_.indexFileName
       << "," << header_.recordCount
       << "," << header_.fieldCount
       << "," << header_.primaryKeyFieldIndex
       << "," << (header_.staleIndex ? 1 : 0);
    for (const auto& f : header_.fields)
        ss << "," << f.name << ":" << f.format;
    return ss.str();
}

bool HeaderBuffer::deserialize(const string& s) {
    istringstream ss(s);
    string tok;
    vector<string> parts;
    while (getline(ss, tok, ',')) parts.push_back(tok);
    if (parts.size() < 13) return false;
    if (parts[0] != "HDR") return false;
    header_.fileType             = parts[1];
    header_.version              = stoi(parts[2]);
    header_.recordSizeByteCount  = stoi(parts[3]);
    header_.sizeFormatType       = parts[4];
    header_.sizeOfSizes          = stoi(parts[5]);
    header_.sizeIncludesItself   = (parts[6] == "1");
    header_.indexFileName        = parts[7];
    header_.recordCount          = stoll(parts[8]);
    header_.fieldCount           = stoi(parts[9]);
    header_.primaryKeyFieldIndex = stoi(parts[10]);
    header_.staleIndex           = (parts[11] == "1");
    header_.fields.clear();
    for (int i = 12; i < (int)parts.size(); i++) {
        auto colon = parts[i].find(':');
        if (colon == string::npos) continue;
        FieldDescriptor fd;
        fd.name   = parts[i].substr(0, colon);
        fd.format = parts[i].substr(colon + 1);
        header_.fields.push_back(fd);
    }
    return true;
}

bool HeaderBuffer::writeLenLine(ofstream& out, const string& text) {
    out << setw(10) << setfill('0') << text.size()
        << ' ' << text << '\n';
    return static_cast<bool>(out);
}

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
    if (in.peek() == '\n') in.get();
    else if (in.peek() == '\r') { in.get(); if (in.peek() == '\n') in.get(); }
    return true;
}