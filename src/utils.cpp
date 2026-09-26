#include "utils.h"

#include <algorithm>
#include <cctype>
#include <sstream>

namespace consensusx {

std::string toUpperCase(const std::string& seq) {
    std::string result = seq;
    for (char& c : result) {
        if (c >= 'a' && c <= 'z') {
            c = static_cast<char>(std::toupper(static_cast<unsigned char>(c)));
        }
    }
    return result;
}

bool isValidNucleotide(char c) {
    return c == 'A' || c == 'T' || c == 'G' || c == 'C' || c == '-';
}

ValidationResult validateAlignment(const std::vector<std::string>& sequences) {
    ValidationResult result;

    if (sequences.size() < 2) {
        result.message = "Error: At least two sequences are required for alignment analysis.";
        return result;
    }

    size_t expectedLength = 0;
    bool lengthSet = false;

    for (size_t i = 0; i < sequences.size(); ++i) {
        if (sequences[i].empty()) {
            result.message = "Error: Sequence " + std::to_string(i + 1) + " is empty.";
            return result;
        }

        std::string upper = toUpperCase(sequences[i]);
        for (size_t j = 0; j < upper.size(); ++j) {
            if (!isValidNucleotide(upper[j])) {
                result.message = "Error: Invalid character '" + std::string(1, upper[j]) +
                                 "' in sequence " + std::to_string(i + 1) +
                                 " at position " + std::to_string(j + 1) +
                                 ". Valid characters: A, T, G, C, -";
                return result;
            }
        }

        if (!lengthSet) {
            expectedLength = upper.size();
            lengthSet = true;
        } else if (upper.size() != expectedLength) {
            result.message = "Error: Sequence " + std::to_string(i + 1) +
                             " has length " + std::to_string(upper.size()) +
                             ", but expected " + std::to_string(expectedLength) +
                             " (all aligned sequences must be equal length).";
            return result;
        }
    }

    result.valid = true;
    result.message = "Validation successful: " + std::to_string(sequences.size()) +
                     " sequences, alignment length " + std::to_string(expectedLength) + ".";
    return result;
}

std::string classifyConservation(double percent) {
    if (percent >= THRESHOLD_HIGHLY_CONSERVED) {
        return "Highly conserved";
    }
    if (percent >= THRESHOLD_CONSERVED) {
        return "Conserved";
    }
    if (percent >= THRESHOLD_MODERATE) {
        return "Moderately variable";
    }
    return "Highly variable";
}

std::string classifyConfidence(double percent) {
    if (percent >= CONFIDENCE_HIGH) {
        return "High";
    }
    if (percent >= CONFIDENCE_MODERATE) {
        return "Moderate";
    }
    if (percent >= CONFIDENCE_LOW) {
        return "Low";
    }
    return "Very low";
}

std::string trim(const std::string& str) {
    size_t start = 0;
    while (start < str.size() && std::isspace(static_cast<unsigned char>(str[start]))) {
        ++start;
    }
    size_t end = str.size();
    while (end > start && std::isspace(static_cast<unsigned char>(str[end - 1]))) {
        --end;
    }
    return str.substr(start, end - start);
}

std::string formatPercent(double value, int precision) {
    std::ostringstream oss;
    oss.setf(std::ios::fixed);
    oss.precision(precision);
    oss << value;
    return oss.str() + "%";
}

}  // namespace consensusx
