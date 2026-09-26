#ifndef SEQUENCE_ANALYZER_H
#define SEQUENCE_ANALYZER_H

#include <string>
#include <vector>

// Represents one occurrence of the CnGCnG consensus motif in a sequence.
struct ConsensusMatch {
    std::string sequence;  // The 6-nucleotide match (e.g., "CAGCTG")
    int startPos;          // Biological start position (1-based)
    int endPos;            // Biological end position (1-based)
};

// Holds nucleotide counts and GC percentage for a validated sequence.
struct SequenceStats {
    int length = 0;
    int countA = 0;
    int countC = 0;
    int countG = 0;
    int countT = 0;
    int countU = 0;
    double gcPercent = 0.0;
};

// Removes spaces, tabs, and line breaks from raw pasted input.
std::string cleanSequence(const std::string& raw);

// Returns true if the character is a valid DNA/RNA nucleotide (A, C, G, T, U).
bool isNucleotide(char c);

// Returns true if the entire sequence contains only valid nucleotides.
bool validateSequence(const std::string& sequence, std::string& errorMessage);

// Returns true if the 6-character substring at index i matches C-any-G-C-any-G.
bool matchesConsensus(const std::string& sequence, size_t index);

// Scans the full sequence (including overlapping windows) for all CnGCnG matches.
std::vector<ConsensusMatch> findConsensus(const std::string& sequence);

// Counts each nucleotide and computes GC% = (G + C) / length * 100.
SequenceStats calculateGCContent(const std::string& sequence);

// Holds result of specific sequence search.
struct SpecificSearchResult {
    bool present = false;
    std::vector<int> positions; // 1-based biological starting positions
};

// Searches fullSequence for querySequence independently (case-insensitive, unaligned).
SpecificSearchResult findSequence(const std::string& fullSequence, const std::string& querySequence);

// Prints a formatted analysis report to the console.
void displayResults(const std::string& sequence,
                    const std::vector<ConsensusMatch>& matches,
                    const SequenceStats& stats);

#endif // SEQUENCE_ANALYZER_H
