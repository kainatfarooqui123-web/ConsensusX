#ifndef CONSENSUSX_CONSENSUS_H
#define CONSENSUSX_CONSENSUS_H

#include <string>
#include <vector>

namespace consensusx {

struct NucleotideCounts {
    int a = 0;
    int t = 0;
    int g = 0;
    int c = 0;
    int gap = 0;

    int nonGapTotal() const { return a + t + g + c; }
    int total() const { return a + t + g + c + gap; }
};

// Count nucleotides at a single alignment column, ignoring nothing during count.
NucleotideCounts countAtPosition(const std::vector<std::string>& sequences, size_t position);

// Select consensus nucleotide: majority among non-gap bases; IUPAC code on ties.
char determineConsensusNucleotide(const NucleotideCounts& counts);

// Generate full consensus sequence across the alignment.
std::string generateConsensus(const std::vector<std::string>& sequences);

// Map tied nucleotide sets to IUPAC ambiguity codes.
char iupacCodeForTie(const std::vector<char>& tiedBases);

}  // namespace consensusx

#endif  // CONSENSUSX_CONSENSUS_H
