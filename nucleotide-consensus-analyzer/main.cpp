#include "sequence_analyzer.h"

#include <iostream>
#include <string>
#include <vector>

// Reads a pasted sequence from stdin (may span multiple lines).
// An empty line after input tells the program the paste is complete.
static std::string readUserSequence() {
    std::string line;
    std::string sequence;

    std::cout << "Paste your nucleotide sequence below.\n";
    std::cout << "Press Enter on an empty line when finished:\n\n";

    while (std::getline(std::cin, line)) {
        if (line.empty() && !sequence.empty()) {
            break;
        }
        if (!sequence.empty()) {
            sequence += '\n';
        }
        sequence += line;
    }

    return sequence;
}

int main() {
    std::cout << "============================================================\n";
    std::cout << "       NUCLEOTIDE CONSENSUS SEQUENCE ANALYZER\n";
    std::cout << "============================================================\n\n";

    std::cout << "This program searches for the consensus motif: CnGCnG\n";
    std::cout << "  C  = cytosine (fixed)\n";
    std::cout << "  n  = any nucleotide (A, C, G, T, or U)\n";
    std::cout << "  G  = guanine (fixed)\n\n";

    std::cout << "Valid nucleotides: A, C, G, T, U\n";
    std::cout << "Spaces and line breaks in pasted input are ignored.\n\n";

    std::string rawInput = readUserSequence();

    // Step 1: Clean whitespace and convert to uppercase.
    std::string sequence = cleanSequence(rawInput);

    // Step 2: Validate characters.
    std::string errorMessage;
    if (!validateSequence(sequence, errorMessage)) {
        std::cout << "\n" << errorMessage << "\n";
        return 1;
    }

    // Step 3: Search for CnGCnG matches (including overlaps).
    std::vector<ConsensusMatch> matches = findConsensus(sequence);

    // Step 4: Compute nucleotide statistics.
    SequenceStats stats = calculateGCContent(sequence);

    // Step 5: Display formatted results.
    displayResults(sequence, matches, stats);

    return 0;
}
