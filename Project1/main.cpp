/**
 * @file main.cpp
 * @brief Main application program for ZIP code geographic analysis
 * @author Teagen Lee (primary contributor)
 * @author Dristi Barnwal, Ethan Jackson, Marcus Julius, Natoli Mayu (reviewers)
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
 *
 * @note Some records can have identical latitude/longitude. To ensure output is
 * identical regardless of CSV row ordering, ties are broken deterministically
 * by choosing the smaller ZIP code.
 */

#include "ZipCodeBuffer.h"
#include <iostream>
#include <map>
#include <iomanip>
#include <limits>

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
 * @brief Tie-break helper: choose smaller ZIP if coordinate value ties
 * @param candidate Candidate ZIP code
 * @param current Current chosen ZIP code
 * @return true if candidate should replace current when tied on coordinate
 */
static bool smallerZipWins(int candidate, int current) {
    // If current is 0 (uninitialized), candidate should win
    if (current == 0) return true;
    return candidate < current;
}

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
 * If multiple records in the same state tie for an extreme coordinate value,
 * the record with smallest ZIP is chosen.
 */
map<string, StateExtremes> calculateStateExtremes(const vector<ZipCodeRecord>& records) {
    map<string, StateExtremes> stateMap;

    for (const auto& record : records) {
        const string& state = record.state;
        StateExtremes& extremes = stateMap[state]; // creates entry if not exist

        // EASTERNMOST (minimum longitude)
        if (record.longitude < extremes.minLongitude) {
            extremes.minLongitude = record.longitude;
            extremes.easternmost = record.zipCode;
        } else if (record.longitude == extremes.minLongitude) {
            // Tie on longitude -> choose smaller ZIP deterministically
            if (smallerZipWins(record.zipCode, extremes.easternmost)) {
                extremes.easternmost = record.zipCode;
            }
        }

        // WESTERNMOST (maximum longitude)
        if (record.longitude > extremes.maxLongitude) {
            extremes.maxLongitude = record.longitude;
            extremes.westernmost = record.zipCode;
        } else if (record.longitude == extremes.maxLongitude) {
            // Tie on longitude -> choose smaller ZIP deterministically
            if (smallerZipWins(record.zipCode, extremes.westernmost)) {
                extremes.westernmost = record.zipCode;
            }
        }

        // NORTHERNMOST (maximum latitude)
        if (record.latitude > extremes.maxLatitude) {
            extremes.maxLatitude = record.latitude;
            extremes.northernmost = record.zipCode;
        } else if (record.latitude == extremes.maxLatitude) {
            // Tie on latitude -> choose smaller ZIP deterministically
            if (smallerZipWins(record.zipCode, extremes.northernmost)) {
                extremes.northernmost = record.zipCode;
            }
        }

        // SOUTHERNMOST (minimum latitude)
        if (record.latitude < extremes.minLatitude) {
            extremes.minLatitude = record.latitude;
            extremes.southernmost = record.zipCode;
        } else if (record.latitude == extremes.minLatitude) {
            // Tie on latitude -> choose smaller ZIP deterministically
            if (smallerZipWins(record.zipCode, extremes.southernmost)) {
                extremes.southernmost = record.zipCode;
            }
        }
    }

    return stateMap;
}

/**
 * @brief Prints a formatted table of state extremes to stdout
 * @param stateMap Map containing StateExtremes for each state
 *
 * This function generates a formatted table with:
 * - A header row with column labels
 * - One row per state, alphabetically sorted
 * - ZIP codes formatted as 5-digit numbers (leading zeros preserved in output)
 */
void printStateExtremesTable(const map<string, StateExtremes>& stateMap) {
    cout << left;
    cout << setw(8)  << "State"
         << setw(15) << "Easternmost"
         << setw(15) << "Westernmost"
         << setw(15) << "Northernmost"
         << setw(15) << "Southernmost"
         << endl;

    cout << string(68, '-') << endl;

    for (const auto& entry : stateMap) {
        const string& state = entry.first;
        const StateExtremes& extremes = entry.second;

        cout << setw(8) << state;

        // Print ZIPs as 5 digits (leading zeros)
        cout << setfill('0') << setw(5) << extremes.easternmost
             << setfill(' ') << setw(10) << " ";

        cout << setfill('0') << setw(5) << extremes.westernmost
             << setfill(' ') << setw(10) << " ";

        cout << setfill('0') << setw(5) << extremes.northernmost
             << setfill(' ') << setw(10) << " ";

        cout << setfill('0') << setw(5) << extremes.southernmost
             << setfill(' ') << endl;
    }
}

/**
 * @brief Main program entry point
 */
int main(int argc, char* argv[]) {
    if (argc != 2) {
        cerr << "Usage: " << argv[0] << " <csv_filename>" << endl;
        cerr << "Example: " << argv[0] << " us_postal_codes.csv" << endl;
        return 1;
    }

    string filename = argv[1];

    ZipCodeBuffer buffer;
    if (!buffer.open(filename)) {
        cerr << "Error: Could not open file '" << filename << "'" << endl;
        cerr << "Please check that the file exists and is readable." << endl;
        return 2;
    }

    cout << "Reading ZIP code data from: " << filename << endl;
    cout << "Processing records..." << endl << endl;

    vector<ZipCodeRecord> allRecords = buffer.gatherAllRecords();

    if (allRecords.empty()) {
        cerr << "Error: No valid records found in file." << endl;
        buffer.close();
        return 3;
    }

    cout << "Total records read: " << allRecords.size() << endl << endl;

    map<string, StateExtremes> stateExtremes = calculateStateExtremes(allRecords);

    cout << "Analysis Results:" << endl;
    cout << "=================" << endl << endl;

    printStateExtremesTable(stateExtremes);

    cout << endl;
    cout << "Total states/territories: " << stateExtremes.size() << endl;

    buffer.close();
    return 0;
}