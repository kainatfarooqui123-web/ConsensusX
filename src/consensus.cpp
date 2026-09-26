#include "consensus.h"

#include <algorithm>
#include <map>
#include <set>

namespace consensusx {

NucleotideCounts countAtPosition(const std::vector<std::string>& sequences, size_t position) {
    NucleotideCounts counts;
    for (const std::string& seq : sequences) {
        char c = seq[position];
        switch (c) {
            case 'A': ++counts.a; break;
            case 'T': ++counts.t; break;
            case 'G': ++counts.g; break;
            case 'C': ++counts.c; break;
            case '-': ++counts.gap; break;
            default: break;
        }
    }
    return counts;
}

char iupacCodeForTie(const std::vector<char>& tiedBases) {
    std::set<char> bases(tiedBases.begin(), tiedBases.end());

    if (bases.count('A') && bases.count('C')) return 'M';
    if (bases.count('A') && bases.count('G')) return 'R';
    if (bases.count('A') && bases.count('T')) return 'W';
    if (bases.count('C') && bases.count('G')) return 'S';
    if (bases.count('C') && bases.count('T')) return 'Y';
    if (bases.count('G') && bases.count('T')) return 'K';

    if (bases.count('A') && bases.count('C') && bases.count('T')) return 'H';
    if (bases.count('A') && bases.count('C') && bases.count('G')) return 'V';
    if (bases.count('A') && bases.count('G') && bases.count('T')) return 'D';
    if (bases.count('C') && bases.count('G') && bases.count('T')) return 'B';

    return 'N';  // All four or unknown combination
}

char determineConsensusNucleotide(const NucleotideCounts& counts) {
    if (counts.nonGapTotal() == 0) {
        return '-';  // Entire column is gaps
    }

    // Build frequency map for non-gap nucleotides only.
    std::map<char, int> freq;
    if (counts.a > 0) freq['A'] = counts.a;
    if (counts.t > 0) freq['T'] = counts.t;
    if (counts.g > 0) freq['G'] = counts.g;
    if (counts.c > 0) freq['C'] = counts.c;

    int maxCount = 0;
    for (const auto& pair : freq) {
        maxCount = std::max(maxCount, pair.second);
    }

    std::vector<char> topBases;
    for (const auto& pair : freq) {
        if (pair.second == maxCount) {
            topBases.push_back(pair.first);
        }
    }

    if (topBases.size() == 1) {
        return topBases[0];
    }

    return iupacCodeForTie(topBases);
}

std::string generateConsensus(const std::vector<std::string>& sequences) {
    if (sequences.empty()) {
        return "";
    }

    size_t alignmentLength = sequences[0].size();
    std::string consensus;
    consensus.reserve(alignmentLength);

    for (size_t pos = 0; pos < alignmentLength; ++pos) {
        NucleotideCounts counts = countAtPosition(sequences, pos);
        consensus += determineConsensusNucleotide(counts);
    }

    return consensus;
}

}  // namespace consensusx
