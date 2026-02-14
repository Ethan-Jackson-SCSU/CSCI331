/**
 * @file main.cpp
 * @brief Main application program for ZIP code geographic analysis
 * @author Teagen Lee, ADD NAMES
 * @date February 2026
 * 
 * This program analyzes ZIP code data from a CSV file and generates a report
 * showing the extreme geographic coordinates (Easternmost, Westernmost,
 * Northernmost, and Southernmost ZIP codes) for each state.
 * 
 * @section USAGE
 * Usage: ./zip_analysis <csv_filename>
 * 
 * @section OUTPUT
 * The program outputs a formatted table to stdout with the following columns:
 * - State: Two-letter state abbreviation
 * - Easternmost: ZIP code with least (most negative) longitude
 * - Westernmost: ZIP code with greatest (most positive) longitude  
 * - Northernmost: ZIP code with greatest (most positive) latitude
 * - Southernmost: ZIP code with least (most negative) latitude
 */

#include "ZipCodeBuffer.h"
#include <iostream>
#include <map>
#include <iomanip>
#include <algorithm>
#include <limits>

#include "ZipCodeBuffer.h"

using namespace std;

/**
 * @struct StateExtremes
 * @brief Holds the extreme ZIP codes for a single state
 * 
 * This structure stores the four extreme ZIP codes (by geographic coordinates)
 * for a particular state. It is used to aggregate data during the analysis.
 */
struct StateExtremes {
    int easternmost;     ///< ZIP code with minimum longitude (farthest east)
    int westernmost;     ///< ZIP code with maximum longitude (farthest west)
    int northernmost;    ///< ZIP code with maximum latitude (farthest north)
    int southernmost;    ///< ZIP code with minimum latitude (farthest south)
    
    double minLongitude; ///< Minimum longitude value (easternmost point)
    double maxLongitude; ///< Maximum longitude value (westernmost point)
    double maxLatitude;  ///< Maximum latitude value (northernmost point)
    double minLatitude;  ///< Minimum latitude value (southernmost point)
    
    /**
     * @brief Constructor initializes all values to sentinel values
     * 
     * Uses extreme values so that any real coordinate will replace them
     * during the first comparison.
     */
    StateExtremes() 
        : easternmost(0), westernmost(0), northernmost(0), southernmost(0),
          minLongitude(numeric_limits<double>::max()),
          maxLongitude(numeric_limits<double>::lowest()),
          maxLatitude(numeric_limits<double>::lowest()),
          minLatitude(numeric_limits<double>::max()) {
    }
};

/**
 * @brief Processes all ZIP code records and determines state extremes
 * @param records Vector of all ZIP code records
 * @return Map of state abbreviations to their StateExtremes data
 * 
 * This function iterates through all records and maintains running
 * extremes for each state. For each record:
 * - If the longitude is less than current minimum, update easternmost
 * - If the longitude is greater than current maximum, update westernmost
 * - If the latitude is greater than current maximum, update northernmost
 * - If the latitude is less than current minimum, update southernmost
 * 
 * @note Longitude in the US is negative (west of Prime Meridian)
 *       So MINIMUM longitude = EASTERNMOST point
 *       And MAXIMUM longitude = WESTERNMOST point
 * @note Latitude is positive in Northern Hemisphere
 *       So MAXIMUM latitude = NORTHERNMOST point
 *       And MINIMUM latitude = SOUTHERNMOST point
 */
map<string, StateExtremes> calculateStateExtremes(
    const vector<ZipCodeRecord>& records) {
    
    map<string, StateExtremes> stateMap;
    
    // Iterate through all records
    for (const auto& record : records) {
        const string& state = record.state;
        StateExtremes& extremes = stateMap[state]; // Creates entry if doesn't exist
        
        /*
         * Check and update EASTERNMOST (minimum longitude)
         * Longitude in US is negative, so more negative = farther east
         */
        if (record.longitude < extremes.minLongitude) {
            extremes.minLongitude = record.longitude;
            extremes.easternmost = record.zipCode;
        }
        
        /*
         * Check and update WESTERNMOST (maximum longitude)
         * Less negative (closer to 0) = farther west
         */
        if (record.longitude > extremes.maxLongitude) {
            extremes.maxLongitude = record.longitude;
            extremes.westernmost = record.zipCode;
        }
        
        /*
         * Check and update NORTHERNMOST (maximum latitude)
         * Higher latitude = farther north
         */
        if (record.latitude > extremes.maxLatitude) {
            extremes.maxLatitude = record.latitude;
            extremes.northernmost = record.zipCode;
        }
        
        /*
         * Check and update SOUTHERNMOST (minimum latitude)
         * Lower latitude = farther south
         */
        if (record.latitude < extremes.minLatitude) {
            extremes.minLatitude = record.latitude;
            extremes.southernmost = record.zipCode;
        }
    }
    
    return stateMap;
}

/**
 * @brief Prints a formatted table of state extremes to stdout
 * @param stateMap Map containing StateExtremes for each state
 * 
 * This function generates a nicely formatted table with:
 * - A header row with column labels
 * - One row per state, alphabetically sorted
 * - Aligned columns for readability
 * - ZIP codes formatted as 5-digit numbers
 * 
 * The output format is designed to be clear and professional,
 * suitable for reports or further processing.
 */
void printStateExtremesTable(const map<string, StateExtremes>& stateMap) {
    // Print header row with column labels
    cout << left;  // Left-align text
         cout << setw(8) << "State"
              << setw(15) << "Easternmost"
              << setw(15) << "Westernmost"
              << setw(15) << "Northernmost"
              << setw(15) << "Southernmost"
              << endl;
    
    // Print separator line for visual clarity
    cout << string(68, '-') << endl;
    
    /*
     * Print data rows
     * The map automatically keeps states in alphabetical order by key
     * because map maintains sorted order
     */
    for (const auto& entry : stateMap) {
        const string& state = entry.first;
        const StateExtremes& extremes = entry.second;
        
        // Print state abbreviation and ZIP codes
        // Format each ZIP code separately with proper padding
        cout << setw(8) << state;
        
        // Easternmost ZIP code
        cout << setfill('0') << setw(5) << extremes.easternmost 
                  << setfill(' ') << setw(10) << " ";
        
        // Westernmost ZIP code
        cout << setfill('0') << setw(5) << extremes.westernmost 
                  << setfill(' ') << setw(10) << " ";
        
        // Northernmost ZIP code
        cout << setfill('0') << setw(5) << extremes.northernmost 
                  << setfill(' ') << setw(10) << " ";
        
        // Southernmost ZIP code
        cout << setfill('0') << setw(5) << extremes.southernmost 
             << setfill(' ') << endl;
    }
}

/**
 * @brief Main program entry point
 * @param argc Number of command line arguments
 * @param argv Array of command line argument strings
 * @return 0 on success, non-zero on error
 * 
 * Program flow:
 * 1. Validate command line arguments
 * 2. Open CSV file using ZipCodeBuffer
 * 3. Read all records into memory
 * 4. Calculate extreme coordinates for each state
 * 5. Display formatted results
 * 6. Clean up and exit
 */
int main(int argc, char* argv[]) {
    // Check command line arguments
    if (argc != 2) {
        cerr << "Usage: " << argv[0] << " <csv_filename>" << endl;
        cerr << "Example: " << argv[0] << " us_postal_codes.csv" << endl;
        return 1;
    }
    
    // Get filename from command line
    string filename = argv[1];
    
    // Create ZipCodeBuffer object
    ZipCodeBuffer buffer;
    
    // Attempt to open the CSV file
    if (!buffer.open(filename)) {
        cerr << "Error: Could not open file '" << filename << "'" << endl;
        cerr << "Please check that the file exists and is readable." << endl;
        return 2;
    }
    
    cout << "Reading ZIP code data from: " << filename << endl;
    cout << "Processing records..." << endl << endl;
    
    // Read all records from the file
    vector<ZipCodeRecord> allRecords = buffer.gatherAllRecords();
    
    // Check if we got any records
    if (allRecords.empty()) {
        cerr << "Error: No valid records found in file." << endl;
        buffer.close();
        return 3;
    }
    
    cout << "Total records read: " << allRecords.size() << endl << endl;
    
    // Calculate state extremes
    map<string, StateExtremes> stateExtremes = calculateStateExtremes(allRecords);
    
    cout << "Analysis Results:" << endl;
    cout << "=================" << endl << endl;
    
    // Print the results table
    printStateExtremesTable(stateExtremes);
    
    cout << endl;
    cout << "Total states/territories: " << stateExtremes.size() << endl;
    
    // Close the file (destructor would do this automatically, but being explicit)
    buffer.close();
    
    return 0; // Success
}
