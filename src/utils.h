#ifndef CONSENSUSX_UTILS_H
#define CONSENSUSX_UTILS_H

#include <string>
#include <vector>

namespace consensusx {

// Conservation classification thresholds (percent). Easy to modify.
constexpr double THRESHOLD_HIGHLY_CONSERVED = 90.0;
constexpr double THRESHOLD_CONSERVED = 70.0;
constexpr double THRESHOLD_MODERATE = 50.0;

// Confidence classification thresholds (percent).
constexpr double CONFIDENCE_HIGH = 80.0;
constexpr double CONFIDENCE_MODERATE = 50.0;
constexpr double CONFIDENCE_LOW = 20.0;

struct ValidationResult {
    bool valid = false;
    std::string message;
};

// Convert lowercase DNA letters to uppercase; gaps stay as '-'.
std::string toUpperCase(const std::string& seq);

// Check if a character is a valid DNA nucleotide or gap.
bool isValidNucleotide(char c);

// Validate an entire alignment: length, characters, minimum sequence count.
ValidationResult validateAlignment(const std::vector<std::string>& sequences);

// Classify conservation percentage into a readable category.
std::string classifyConservation(double percent);

// Classify confidence score into a readable category.
std::string classifyConfidence(double percent);

// Trim whitespace from both ends of a string.
std::string trim(const std::string& str);

// Format a floating-point percentage for display.
std::string formatPercent(double value, int precision = 1);

}  // namespace consensusx

#endif  // CONSENSUSX_UTILS_H
