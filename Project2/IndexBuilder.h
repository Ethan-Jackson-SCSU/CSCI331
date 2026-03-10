// IndexBuilder.h
#ifndef INDEXBUILDER_H
#define INDEXBUILDER_H

#include <string>

/**
 * @file IndexBuilder.h
 * @brief Builds a primary key index file for a .len data file.
 *
 * The index file stores:
 * - ZIP code (primary key)
 * - byte offset (file position) in the .len file where the record starts
 *
 * Example index line:
 * 56301 120
 *
 * Meaning:
 * - ZIP 56301 starts at byte 120 in the data file.
 *
 * Why we do this:
 * - During search, we load the index into RAM (allowed).
 * - Then we can jump directly to a ZIP record using seekg(offset).
 * - We read only ONE record at a time (RAM rule).
 */

/**
 * @brief Builds an index file from a .len file.
 *
 * This function:
 * 1) Opens the .len data file
 * 2) Reads and skips the LEN header record
 * 3) For each record:
 *    - saves the current file position (offset)
 *    - reads the record
 *    - extracts ZIP (first field before comma)
 *    - writes ZIP and offset into the index file
 *
 * @param lenFile Path to the length-indicated data file
 * @param indexFile Path to the output index file
 */
void buildIndex(const std::string& lenFile,
                const std::string& indexFile);

#endif