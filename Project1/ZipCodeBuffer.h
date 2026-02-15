/**
 * @file ZipCodeBuffer.h
 * @brief Header file for the ZipCodeBuffer class
 * @author Teagen Lee, ADD NAMES
 * @date February 2026
 * 
 * This file contains the declaration of the ZipCodeBuffer class which provides
 * functionality to read and parse ZIP code records from a CSV file. The class
 * implements buffered reading for efficient file I/O operations.
 */

#ifndef ZIPCODEBUFFER_H
#define ZIPCODEBUFFER_H

#include <string>
#include <fstream>
#include <sstream>
#include <vector>

using namespace std;

/**
 * @struct ZipCodeRecord
 * @brief Structure to hold a single ZIP code record
 * 
 * This structure represents one row from the ZIP code CSV file,
 * containing all relevant geographic and administrative information
 * for a specific ZIP code.
 */
struct ZipCodeRecord {
    int zipCode;           ///< The 5-digit ZIP code
    string placeName;      ///< Name of the place/city
    string state;          ///< Two-letter state abbreviation
    string county;         ///< County name
    double latitude;       ///< Latitude coordinate (decimal degrees)
    double longitude;      ///< Longitude coordinate (decimal degrees)
    
    /**
     * @brief Default constructor
     * 
     * Initializes all numeric fields to zero and strings to empty.
     */
    ZipCodeRecord();
    
    /**
     * @brief Parameterized constructor
     * @param zip ZIP code number
     * @param place Place name
     * @param st State abbreviation
     * @param cnty County name
     * @param lat Latitude
     * @param lon Longitude
     */
    ZipCodeRecord(int zip, const string& place, const string& st,
                  const string& cnty, double lat, double lon);
};

/**
 * @class ZipCodeBuffer
 * @brief A buffer class for reading ZIP code records from CSV files
 * 
 * This class provides an abstraction layer for reading ZIP code data
 * from a comma-separated values (CSV) file. It handles file I/O,
 * parsing, and error checking while maintaining a clean interface
 * for client code.
 * 
 * The class uses internal buffering to efficiently read data from
 * the file and parse it into structured ZipCodeRecord objects.
 * 
 * @note The CSV file must have a header row which is automatically skipped
 * @note Expected CSV format: ZipCode,PlaceName,State,County,Lat,Long
 */
class ZipCodeBuffer {
private:
    ifstream fileStream;       ///< Input file stream for reading CSV data
    string filename;           ///< Name of the CSV file being read
    bool headerSkipped;        ///< Flag to track if header row has been skipped
    long recordCount;          ///< Counter for total records read
    
    /**
     * @brief Parses a CSV line into a ZipCodeRecord
     * @param line The CSV line to parse
     * @param record Reference to ZipCodeRecord to populate
     * @return true if parsing was successful, false otherwise
     * 
     * This private helper method takes a raw CSV line and extracts
     * the individual fields, converting them to appropriate data types
     * and storing them in the provided ZipCodeRecord structure.
     * 
     * Handles quoted fields and embedded commas correctly.
     */
    bool parseLine(const string& line, ZipCodeRecord& record);
    
    /**
     * @brief Splits a CSV line into individual fields
     * @param line The CSV line to split
     * @return Vector of strings containing individual fields
     * 
     * This utility function correctly handles CSV formatting including:
     * - Quoted fields
     * - Embedded commas within quotes
     * - Leading/trailing whitespace
     */
    vector<string> splitCSV(const string& line);
    
    /**
     * @brief Trims whitespace from both ends of a string
     * @param str The string to trim
     * @return Trimmed string
     */
    string trim(const string& str);

public:
    /**
     * @brief Default constructor
     * 
     * Creates an uninitialized ZipCodeBuffer. The open() method
     * must be called before reading any records.
     */
    ZipCodeBuffer();
    
    /**
     * @brief Parameterized constructor
     * @param csvFilename Path to the CSV file to open
     * 
     * Creates a ZipCodeBuffer and automatically opens the specified file.
     * The header row is skipped during initialization.
     */
    explicit ZipCodeBuffer(const string& csvFilename);
    
    /**
     * @brief Destructor
     * 
     * Ensures the file stream is properly closed when the object
     * is destroyed, preventing resource leaks.
     */
    ~ZipCodeBuffer();
    
    /**
     * @brief Opens a CSV file for reading
     * @param csvFilename Path to the CSV file
     * @return true if file opened successfully, false otherwise
     * 
     * Opens the specified CSV file and skips the header row.
     * If a file is already open, it is closed first.
     */
    bool open(const string& csvFilename);
    
    /**
     * @brief Closes the currently open file
     * 
     * Closes the file stream and resets internal state.
     * Safe to call even if no file is open.
     */
    void close();
    
    /**
     * @brief Checks if a file is currently open
     * @return true if file is open and ready for reading, false otherwise
     */
    bool isOpen() const;
    
    /**
     * @brief Reads the next ZIP code record from the file
     * @param record Reference to ZipCodeRecord to populate
     * @return true if a record was successfully read, false on EOF or error
     * 
     * Reads one line from the CSV file, parses it, and populates the
     * provided ZipCodeRecord structure. Returns false when end of file
     * is reached or if a parsing error occurs.
     * 
     * @note Automatically skips the header row on first read
     */
    bool readRecord(ZipCodeRecord& record);
    
    /**
     * @brief Reads and gathers all records from the file
     * @return Vector containing all ZIP code records from the file
     * 
     * Reads the entire CSV file and returns all valid records as a vector.
     * The file position is reset to the beginning (after header) when complete.
     * 
     * @note This method loads all data into memory - use with caution for very large files
     */
    vector<ZipCodeRecord> gatherAllRecords();
    
    /**
     * @brief Resets the file position to the beginning (after header)
     * @return true if reset was successful, false otherwise
     * 
     * Seeks back to the start of the file and skips the header row again,
     * allowing the file to be re-read without closing and reopening.
     */
    bool reset();
    
    /**
     * @brief Gets the total number of records read so far
     * @return Count of records read
     */
    long getRecordCount() const;
    
    /**
     * @brief Gets the name of the currently open file
     * @return Filename string
     */
    string getFilename() const;
};

#include "ZipCodeBuffer.cpp"

#endif // ZIPCODEBUFFER_H
