#ifndef CONSENSUSX_SIMILARITY_H
#define CONSENSUSX_SIMILARITY_H

#include <string>
#include <vector>

namespace consensusx {

// Pairwise similarity matrix (%). Diagonal is 100%.
// Positions where BOTH sequences have gaps are ignored.
std::vector<std::vector<double>> calculatePairwiseSimilarity(
    const std::vector<std::string>& sequences);

// Similarity between two aligned sequences.
double sequenceSimilarity(const std::string& seqA, const std::string& seqB);

}  // namespace consensusx

#endif  // CONSENSUSX_SIMILARITY_H
