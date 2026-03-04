/**
 * @file ZipCodeBuffer.cpp
 * @brief Implementation of the ZipCodeBuffer class
 * @author Teagen Lee (primary contributor)
 * @author Dristi Barnwal, Ethan Jackson, Marcus Julius, Natoli Mayu (reviewers)
 * @date February 2026
 * 
 * This file contains the implementation of all methods declared in
 * ZipCodeBuffer.h. The class provides robust CSV parsing with proper
 * error handling and memory management.
 */

#include "ZipCodeBuffer.h"
#include <algorithm>
#include <cctype>

using namespace std;

/*
 * ============================================================================
 * ZipCodeRecord Implementation
 * ============================================================================
 */

/**
 * Default constructor implementation
 * Initializes all fields to default values
 */
ZipCodeRecord::ZipCodeRecord() 
    : zipCode(0), placeName(""), state(""), county(""), latitude(0.0), longitude(0.0) {
}

/**
 * Parameterized constructor implementation
 * Initializes all fields with provided values
 */
ZipCodeRecord::ZipCodeRecord(int zip, const string& place, const string& st,
                             const string& cnty, double lat, double lon)
    : zipCode(zip), placeName(place), state(st), county(cnty), latitude(lat), longitude(lon) {
}

/*
 * ============================================================================
 * ZipCodeBuffer Implementation
 * ============================================================================
 */

/**
 * Default constructor implementation
 * Initializes member variables to safe default values
 */
ZipCodeBuffer::ZipCodeBuffer() 
    : filename(""), headerSkipped(false), recordCount(0) {
}

/**
 * Parameterized constructor implementation
 * Opens the specified file and prepares it for reading
 */
ZipCodeBuffer::ZipCodeBuffer(const string& csvFilename) 
    : filename(""), headerSkipped(false), recordCount(0) {
    open(csvFilename);
}

/**
 * Destructor implementation
 * Ensures file is properly closed to prevent resource leaks
 */
ZipCodeBuffer::~ZipCodeBuffer() {
    close();
}

/**
 * Opens a CSV file for reading
 * 
 * This method:
 * 1. Closes any previously open file
 * 2. Opens the new file in input mode
 * 3. Skips the header row
 * 4. Resets the record counter
 * 
 * @param csvFilename Path to the CSV file to open
 * @return true if successful, false if file cannot be opened
 */
bool ZipCodeBuffer::open(const string& csvFilename) {
    // Close any currently open file
    close();
    
    // Store the filename
    filename = csvFilename;
    
    // Open the file
    fileStream.open(filename);
    
    // Check if file opened successfully
    if (!fileStream.is_open()) {
        return false;
    }
    
    // Skip the header row
    string headerLine;
    if (getline(fileStream, headerLine)) {
        headerSkipped = true;
    } else {
        // File is empty or unreadable
        close();
        return false;
    }
    
    // Reset record counter
    recordCount = 0;
    
    return true;
}

/**
 * Closes the currently open file
 * 
 * Resets all internal state variables and closes the file stream.
 * Safe to call multiple times or when no file is open.
 */
void ZipCodeBuffer::close() {
    if (fileStream.is_open()) {
        fileStream.close();
    }
    filename = "";
    headerSkipped = false;
    recordCount = 0;
}

/**
 * Checks if file is open and ready for reading
 * 
 * @return true if file stream is open, false otherwise
 */
bool ZipCodeBuffer::isOpen() const {
    return fileStream.is_open();
}

/**
 * Reads a single record from the CSV file
 * 
 * This method reads one line from the file, parses it into fields,
 * and populates the provided ZipCodeRecord structure.
 * 
 * @param record Reference to ZipCodeRecord to be populated
 * @return true if a record was read successfully, false on EOF or error
 */
bool ZipCodeBuffer::readRecord(ZipCodeRecord& record) {
    // Check if file is open
    if (!fileStream.is_open()) {
        return false;
    }
    
    // Read a line from the file
    string line;
    if (getline(fileStream, line)) {
        // Skip empty lines
        if (line.empty() || trim(line).empty()) {
            return readRecord(record); // Recursively read next non-empty line
        }
        
        // Parse the line into a record
        if (parseLine(line, record)) {
            recordCount++;
            return true;
        } else {
            return false; // Parse error
        }
    }
    
    // End of file reached
    return false;
}

/**
 * Reads all records from the file into a vector
 * 
 * This method reads the entire file and returns all valid records.
 * The file position is reset to the beginning after reading.
 * 
 * @return Vector containing all ZipCodeRecord objects from the file
 */
vector<ZipCodeRecord> ZipCodeBuffer::gatherAllRecords() {
    vector<ZipCodeRecord> records;
    
    // Reset to beginning of file (after header)
    reset();
    
    // Read all records
    ZipCodeRecord record;
    while (readRecord(record)) {
        records.push_back(record);
    }
    
    // Reset again so file can be re-read if needed
    reset();
    
    return records;
}

/**
 * Resets the file position to the beginning (after header)
 * 
 * This allows the file to be re-read without closing and reopening.
 * 
 * @return true if reset was successful, false otherwise
 */
bool ZipCodeBuffer::reset() {
    if (!fileStream.is_open()) {
        return false;
    }
    
    // Clear any error flags
    fileStream.clear();
    
    // Seek to beginning of file
    fileStream.seekg(0, ios::beg);
    
    // Skip header row again
    string headerLine;
    if (!getline(fileStream, headerLine)) {
        return false;
    }
    
    // Reset record counter
    recordCount = 0;
    
    return true;
}

/**
 * Gets the count of records read so far
 * 
 * @return Number of records successfully read
 */
long ZipCodeBuffer::getRecordCount() const {
    return recordCount;
}

/**
 * Gets the filename of the currently open file
 * 
 * @return String containing the filename
 */
string ZipCodeBuffer::getFilename() const {
    return filename;
}

/**
 * Parses a CSV line into a ZipCodeRecord
 * 
 * This private helper method handles the conversion of string fields
 * to appropriate data types and performs basic validation.
 * 
 * Expected CSV format: ZipCode,PlaceName,State,County,Lat,Long
 * 
 * @param line The CSV line to parse
 * @param record Reference to ZipCodeRecord to populate
 * @return true if parsing succeeded, false if format is invalid
 */
bool ZipCodeBuffer::parseLine(const string& line, ZipCodeRecord& record) {
    // Split the line into fields
    vector<string> fields = splitCSV(line);
    
    // Verify we have the correct number of fields
    if (fields.size() != 6) {
        return false;
    }
    
    try {
        // Parse each field with appropriate type conversion
        record.zipCode = stoi(trim(fields[0]));
        record.placeName = trim(fields[1]);
        record.state = trim(fields[2]);
        record.county = trim(fields[3]);
        record.latitude = stod(trim(fields[4]));
        record.longitude = stod(trim(fields[5]));
        
        return true;
    } catch (const exception& e) {
        // Conversion failed - invalid data format
        return false;
    }
}

/**
 * Splits a CSV line into individual fields
 * 
 * This method correctly handles:
 * - Regular comma-separated fields
 * - Quoted fields containing commas
 * - Embedded quotes (escaped as "")
 * 
 * Algorithm:
 * 1. Iterate through each character
 * 2. Track whether we're inside quotes
 * 3. Split on commas that are not inside quotes
 * 
 * @param line The CSV line to split
 * @return Vector of field strings
 */
vector<string> ZipCodeBuffer::splitCSV(const string& line) {
    vector<string> fields;
    string currentField;
    bool inQuotes = false;
    
    for (size_t i = 0; i < line.length(); i++) {
        char c = line[i];
        
        if (c == '"') {
            // Toggle quote state
            inQuotes = !inQuotes;
        } else if (c == ',' && !inQuotes) {
            // Field separator found (not inside quotes)
            fields.push_back(currentField);
            currentField.clear();
        } else {
            // Regular character - add to current field
            currentField += c;
        }
    }
    
    // Add the last field
    fields.push_back(currentField);
    
    return fields;
}

/**
 * Trims leading and trailing whitespace from a string
 * 
 * This utility function removes spaces, tabs, newlines, and other
 * whitespace characters from both ends of the string.
 * 
 * @param str The string to trim
 * @return Trimmed string
 */
string ZipCodeBuffer::trim(const string& str) {
    // Find first non-whitespace character
    size_t start = str.find_first_not_of(" \t\r\n");
    if (start == string::npos) {
        return ""; // String is all whitespace
    }
    
    // Find last non-whitespace character
    size_t end = str.find_last_not_of(" \t\r\n");
    
    // Extract substring
    return str.substr(start, end - start + 1);
}
