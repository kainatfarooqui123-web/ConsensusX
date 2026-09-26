#include "sequence_analyzer.h"

#include <algorithm>
#include <cctype>
#include <iomanip>
#include <iostream>
#include <sstream>

// ---------------------------------------------------------------------------
// cleanSequence
// Strips whitespace so users can paste multi-line FASTA-style sequences.
// Also converts every character to uppercase for uniform processing.
// ---------------------------------------------------------------------------
std::string cleanSequence(const std::string& raw) {
    std::string cleaned;
    cleaned.reserve(raw.size());

    for (char ch : raw) {
        if (ch == ' ' || ch == '\t' || ch == '\n' || ch == '\r') {
            continue;
        }
        cleaned += static_cast<char>(std::toupper(static_cast<unsigned char>(ch)));
    }

    return cleaned;
}

// ---------------------------------------------------------------------------
// isNucleotide
// Valid symbols: A, C, G, T (DNA) and U (RNA).
// ---------------------------------------------------------------------------
bool isNucleotide(char c) {
    return c == 'A' || c == 'C' || c == 'G' || c == 'T' || c == 'U';
}

// ---------------------------------------------------------------------------
// validateSequence
// Ensures every character in the cleaned sequence is a valid nucleotide.
// ---------------------------------------------------------------------------
bool validateSequence(const std::string& sequence, std::string& errorMessage) {
    if (sequence.empty()) {
        errorMessage = "Error: Sequence is empty. Please enter at least one nucleotide.";
        return false;
    }

    for (size_t i = 0; i < sequence.size(); ++i) {
        if (!isNucleotide(sequence[i])) {
            std::ostringstream oss;
            oss << "Error: Invalid character '" << sequence[i]
                << "' at position " << (i + 1)
                << ". Only A, C, G, T, and U are allowed.";
            errorMessage = oss.str();
            return false;
        }
    }

    errorMessage.clear();
    return true;
}

// ---------------------------------------------------------------------------
// matchesConsensus
// Pattern: C - n - G - C - n - G
//   Position 0 -> C (fixed)
//   Position 1 -> any nucleotide
//   Position 2 -> G (fixed)
//   Position 3 -> C (fixed)
//   Position 4 -> any nucleotide
//   Position 5 -> G (fixed)
// ---------------------------------------------------------------------------
bool matchesConsensus(const std::string& sequence, size_t index) {
    if (index + 6 > sequence.size()) {
        return false;
    }

    return sequence[index] == 'C'
        && isNucleotide(sequence[index + 1])
        && sequence[index + 2] == 'G'
        && sequence[index + 3] == 'C'
        && isNucleotide(sequence[index + 4])
        && sequence[index + 5] == 'G';
}

// ---------------------------------------------------------------------------
// findConsensus
// Slides a 6-nucleotide window across the sequence one base at a time.
// Overlapping matches are reported separately (e.g., CAGCTGCAGCTG -> 2 matches).
// ---------------------------------------------------------------------------
std::vector<ConsensusMatch> findConsensus(const std::string& sequence) {
    std::vector<ConsensusMatch> matches;

    if (sequence.size() < 6) {
        return matches;
    }

    for (size_t i = 0; i + 6 <= sequence.size(); ++i) {
        if (matchesConsensus(sequence, i)) {
            ConsensusMatch match;
            match.sequence = sequence.substr(i, 6);
            match.startPos = static_cast<int>(i + 1);      // 1-based biology
            match.endPos = static_cast<int>(i + 6);        // inclusive end
            matches.push_back(match);
        }
    }

    return matches;
}

// ---------------------------------------------------------------------------
// calculateGCContent
// GC% uses G and C counts divided by total sequence length.
// U and T are counted separately but do not enter the GC formula.
// ---------------------------------------------------------------------------
SequenceStats calculateGCContent(const std::string& sequence) {
    SequenceStats stats;
    stats.length = static_cast<int>(sequence.size());

    for (char ch : sequence) {
        switch (ch) {
            case 'A': ++stats.countA; break;
            case 'C': ++stats.countC; break;
            case 'G': ++stats.countG; break;
            case 'T': ++stats.countT; break;
            case 'U': ++stats.countU; break;
            default: break;
        }
    }

    if (stats.length > 0) {
        stats.gcPercent = (static_cast<double>(stats.countG + stats.countC)
                           / static_cast<double>(stats.length)) * 100.0;
    }

    return stats;
}

// ---------------------------------------------------------------------------
// displayResults
// Prints motif search results and sequence statistics in a readable format.
// ---------------------------------------------------------------------------
void displayResults(const std::string& /*sequence*/,
                    const std::vector<ConsensusMatch>& matches,
                    const SequenceStats& stats) {
    std::cout << "\n";
    std::cout << "============================================================\n";
    std::cout << "                    ANALYSIS RESULTS\n";
    std::cout << "============================================================\n\n";

    std::cout << "Consensus: CnGCnG\n";
    std::cout << "Note: In this motif, 'n' means ANY valid nucleotide (A, C, G, T, or U).\n\n";

    if (matches.empty()) {
        std::cout << "RESULT: NOT FOUND\n\n";
        std::cout << "Consensus sequence CnGCnG was not found.\n";
    } else {
        std::cout << "RESULT: FOUND\n\n";
        std::cout << "Number of matches: " << matches.size() << "\n\n";

        for (size_t i = 0; i < matches.size(); ++i) {
            std::cout << "Match " << (i + 1) << ":\n";
            std::cout << "  Sequence: " << matches[i].sequence << "\n";
            std::cout << "  Position: " << matches[i].startPos
                      << "-" << matches[i].endPos << "\n\n";
        }
    }

    std::cout << "------------------------------------------------------------\n";
    std::cout << "                 SEQUENCE STATISTICS\n";
    std::cout << "------------------------------------------------------------\n\n";

    std::cout << "Total sequence length: " << stats.length << "\n";
    std::cout << "A nucleotides: " << stats.countA << "\n";
    std::cout << "C nucleotides: " << stats.countC << "\n";
    std::cout << "G nucleotides: " << stats.countG << "\n";
    std::cout << "T nucleotides: " << stats.countT << "\n";
    std::cout << "U nucleotides: " << stats.countU << "\n";
    std::cout << std::fixed << std::setprecision(2);
    std::cout << "GC percentage: " << stats.gcPercent << "%\n";
    std::cout << "\n============================================================\n";
}

// ---------------------------------------------------------------------------
// findSequence
// Searches fullSequence for querySequence independently.
// Case-insensitive, ignores spaces/linebreaks, returns 1-based start positions.
// ---------------------------------------------------------------------------
SpecificSearchResult findSequence(const std::string& fullSequence, const std::string& querySequence) {
    SpecificSearchResult result;
    std::string full = cleanSequence(fullSequence);
    std::string query = cleanSequence(querySequence);

    if (full.empty() || query.empty() || query.size() > full.size()) {
        return result;
    }

    size_t pos = full.find(query, 0);
    while (pos != std::string::npos) {
        result.positions.push_back(static_cast<int>(pos + 1));
        pos = full.find(query, pos + 1);
    }

    result.present = !result.positions.empty();
    return result;
}

