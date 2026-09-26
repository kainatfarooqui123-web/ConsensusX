#include "similarity.h"

namespace consensusx {

double sequenceSimilarity(const std::string& seqA, const std::string& seqB) {
    if (seqA.size() != seqB.size() || seqA.empty()) {
        return 0.0;
    }

    int matches = 0;
    int comparable = 0;

    for (size_t i = 0; i < seqA.size(); ++i) {
        char a = seqA[i];
        char b = seqB[i];

        // Ignore positions where both sequences contain gaps.
        if (a == '-' && b == '-') {
            continue;
        }

        ++comparable;
        if (a == b) {
            ++matches;
        }
    }

    if (comparable == 0) {
        return 0.0;
    }

    return (static_cast<double>(matches) / comparable) * 100.0;
}

std::vector<std::vector<double>> calculatePairwiseSimilarity(
    const std::vector<std::string>& sequences) {
    size_t n = sequences.size();
    std::vector<std::vector<double>> matrix(n, std::vector<double>(n, 0.0));

    for (size_t i = 0; i < n; ++i) {
        for (size_t j = 0; j < n; ++j) {
            if (i == j) {
                matrix[i][j] = 100.0;
            } else {
                matrix[i][j] = sequenceSimilarity(sequences[i], sequences[j]);
            }
        }
    }

    return matrix;
}

}  // namespace consensusx
