/**
 * @file ZipCodeBuffer.h
 * @brief Header file for the ZipCodeBuffer class and ZipCodeRecord structure
 * @author Teagen Lee (primary contributor)
 * @author Ethan Jackson (additional comments and formatting)
 * @author Dristi Barnwal, Marcus Julius, Natoli Mayu (reviewers)
 * @date February 2026
 *
 * This file contains:
 * - ZipCodeRecord: a structure that stores one ZIP code record
 * - ZipCodeBuffer: a class that reads ZIP code data from a CSV file
 *
 * Project 2 improvement:
 * This version supports CSV files whose columns may appear in different orders.
 * It does this by reading the header row first and finding which column contains:
 * - ZIP code
 * - place name
 * - state
 * - county
 * - latitude
 * - longitude
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
 * This structure stores one full data row from the ZIP code file.
 */
struct ZipCodeRecord {
    int zipCode;           ///< The 5-digit ZIP code
    string placeName;      ///< Name of the place/city
    string state;          ///< Two-letter state abbreviation
    string county;         ///< County name
    double latitude;       ///< Latitude coordinate
    double longitude;      ///< Longitude coordinate

    /**
     * @brief Default constructor
     *
     * Initializes all values to safe defaults.
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
 * This class opens a CSV file, reads one record at a time, and converts each
 * line into a ZipCodeRecord object.
 *
 * Project 2 improvement:
 * The class does not assume that columns are always in the same order.
 * Instead, it reads the header row and finds where each needed field is.
 */
class ZipCodeBuffer {
private:
    ifstream fileStream;       ///< Input file stream for CSV data
    string filename;           ///< Name of the current file
    bool headerSkipped;        ///< True after header row is processed
    long recordCount;          ///< Number of records successfully read

    /**
     * Column positions found from the CSV header row.
     *
     * These values tell the parser where each needed field is located.
     * Example:
     * - colZip = 0 means ZIP code is in column 0
     * - colState = 2 means state is in column 2
     *
     * If the columns are re-ordered, these values will change, and parsing
     * will still work correctly.
     */
    int colZip;
    int colPlace;
    int colState;
    int colCounty;
    int colLat;
    int colLong;

    /**
     * @brief Parses a CSV line into a ZipCodeRecord
     * @param line The CSV line to parse
     * @param record Reference to record to fill
     * @return true if parsing succeeds, false otherwise
     */
    bool parseLine(const string& line, ZipCodeRecord& record);

    /**
     * @brief Splits a CSV line into separate fields
     * @param line One CSV line
     * @return Vector of field strings
     */
    vector<string> splitCSV(const string& line);

    /**
     * @brief Removes whitespace from both ends of a string
     * @param str Input string
     * @return Trimmed string
     */
    string trim(const string& str);

    /**
     * @brief Reads the header row and finds the column position of each field
     * @param headerLine The first line of the CSV file
     * @return true if all required columns were found, false otherwise
     *
     * This function makes the class work even when the CSV columns are
     * re-ordered.
     */
    bool parseHeader(const string& headerLine);

public:
    /**
     * @brief Default constructor
     */
    ZipCodeBuffer();

    /**
     * @brief Constructor that immediately opens a CSV file
     * @param csvFilename Path to CSV file
     */
    explicit ZipCodeBuffer(const string& csvFilename);

    /**
     * @brief Destructor
     */
    ~ZipCodeBuffer();

    /**
     * @brief Opens a CSV file for reading
     * @param csvFilename Path to CSV file
     * @return true if file opened successfully, false otherwise
     */
    bool open(const string& csvFilename);

    /**
     * @brief Closes the file and resets internal state
     */
    void close();

    /**
     * @brief Checks whether a file is currently open
     * @return true if file is open
     */
    bool isOpen() const;

    /**
     * @brief Reads the next ZIP code record from the file
     * @param record Output record
     * @return true if record was read successfully, false on EOF or error
     */
    bool readRecord(ZipCodeRecord& record);

    /**
     * @brief Reads all records into a vector
     * @return Vector of all records
     *
     * Note:
     * This is kept mainly for compatibility with older code, but Project 2
     * should prefer reading one record at a time.
     */
    vector<ZipCodeRecord> gatherAllRecords();

    /**
     * @brief Resets the file position to the beginning, then processes header
     * @return true if reset succeeds, false otherwise
     */
    bool reset();

    /**
     * @brief Gets the number of records read so far
     * @return Record count
     */
    long getRecordCount() const;

    /**
     * @brief Gets the current filename
     * @return Filename
     */
    string getFilename() const;
};

#endif