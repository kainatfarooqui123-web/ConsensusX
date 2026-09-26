#include "analysis.h"

#include "utils.h"

#include <algorithm>
#include <cmath>
#include <map>
#include <sstream>

namespace consensusx {

static int countForBase(const NucleotideCounts& counts, char base) {
    switch (base) {
        case 'A': return counts.a;
        case 'T': return counts.t;
        case 'G': return counts.g;
        case 'C': return counts.c;
        default: return 0;
    }
}

static std::vector<int> sortedNonGapCounts(const NucleotideCounts& counts) {
    std::vector<int> values;
    if (counts.a > 0) values.push_back(counts.a);
    if (counts.t > 0) values.push_back(counts.t);
    if (counts.g > 0) values.push_back(counts.g);
    if (counts.c > 0) values.push_back(counts.c);
    std::sort(values.begin(), values.end(), std::greater<int>());
    return values;
}

double calculateConservation(const NucleotideCounts& counts, char consensus) {
    int nonGap = counts.nonGapTotal();
    if (nonGap == 0) {
        return 0.0;
    }

    int consensusCount = 0;
    if (consensus == 'A' || consensus == 'T' || consensus == 'G' || consensus == 'C') {
        consensusCount = countForBase(counts, consensus);
    } else if (consensus != '-') {
        // IUPAC ambiguity: use the highest single-nucleotide count as reference.
        consensusCount = std::max({counts.a, counts.t, counts.g, counts.c});
    }

    return (static_cast<double>(consensusCount) / nonGap) * 100.0;
}

double calculateConfidence(const NucleotideCounts& counts) {
    int nonGap = counts.nonGapTotal();
    if (nonGap == 0) {
        return 0.0;
    }

    std::vector<int> sorted = sortedNonGapCounts(counts);
    int maxCount = sorted.empty() ? 0 : sorted[0];
    int secondMax = sorted.size() > 1 ? sorted[1] : 0;

    return (static_cast<double>(maxCount - secondMax) / nonGap) * 100.0;
}

int distinctNonGapNucleotides(const NucleotideCounts& counts) {
    int distinct = 0;
    if (counts.a > 0) ++distinct;
    if (counts.t > 0) ++distinct;
    if (counts.g > 0) ++distinct;
    if (counts.c > 0) ++distinct;
    return distinct;
}

static std::string buildAlternatives(const NucleotideCounts& counts, char consensus) {
    std::ostringstream oss;
    bool first = true;

    auto appendIfAlt = [&](char base, int count) {
        if (count > 0 && base != consensus) {
            if (!first) oss << ", ";
            oss << base;
            first = false;
        }
    };

    appendIfAlt('A', counts.a);
    appendIfAlt('T', counts.t);
    appendIfAlt('G', counts.g);
    appendIfAlt('C', counts.c);

    if (first) {
        return "None";
    }
    return oss.str();
}

std::vector<PositionAnalysis> analyzePositions(const std::vector<std::string>& sequences,
                                               const std::string& consensus) {
    std::vector<PositionAnalysis> results;
    if (sequences.empty()) {
        return results;
    }

    size_t length = sequences[0].size();
    results.reserve(length);

    for (size_t pos = 0; pos < length; ++pos) {
        PositionAnalysis analysis;
        analysis.position = pos + 1;
        analysis.counts = countAtPosition(sequences, pos);
        analysis.consensus = consensus[pos];
        analysis.conservation = calculateConservation(analysis.counts, analysis.consensus);
        analysis.confidence = calculateConfidence(analysis.counts);
        analysis.conservationCategory = classifyConservation(analysis.conservation);
        analysis.confidenceCategory = classifyConfidence(analysis.confidence);
        analysis.isVariable = distinctNonGapNucleotides(analysis.counts) > 1;
        analysis.alternatives = buildAlternatives(analysis.counts, analysis.consensus);
        results.push_back(analysis);
    }

    return results;
}

double calculateGCContent(const std::string& sequence) {
    int gc = 0;
    int valid = 0;
    for (char c : sequence) {
        if (c == 'G' || c == 'C') {
            ++gc;
            ++valid;
        } else if (c == 'A' || c == 'T') {
            ++valid;
        }
    }
    if (valid == 0) {
        return 0.0;
    }
    return (static_cast<double>(gc) / valid) * 100.0;
}

std::vector<GCWindow> slidingWindowGC(const std::string& sequence, size_t windowSize) {
    std::vector<GCWindow> windows;
    if (windowSize == 0 || sequence.size() < windowSize) {
        return windows;
    }

    for (size_t i = 0; i + windowSize <= sequence.size(); ++i) {
        std::string window = sequence.substr(i, windowSize);
        GCWindow entry;
        entry.start = i + 1;
        entry.end = i + windowSize;
        entry.gcPercent = calculateGCContent(window);
        windows.push_back(entry);
    }

    return windows;
}

std::string buildConservationAscii(const std::vector<PositionAnalysis>& positions) {
    std::ostringstream oss;
    oss << "Position:\n     ";
    for (const auto& pos : positions) {
        oss << pos.position;
        if (pos.position < 10) oss << "  ";
        else if (pos.position < 100) oss << " ";
    }
    oss << "\nConservation:\n     ";

    for (const auto& pos : positions) {
        char symbol = ' ';
        if (pos.conservation >= THRESHOLD_HIGHLY_CONSERVED) {
            symbol = 0xE2;  // Will use UTF-8 block chars below
        }
        (void)symbol;

        if (pos.conservation >= THRESHOLD_HIGHLY_CONSERVED) {
            oss << "\xE2\x96\x88 ";  // █
        } else if (pos.conservation >= THRESHOLD_CONSERVED) {
            oss << "\xE2\x96\x93 ";  // ▓
        } else if (pos.conservation >= THRESHOLD_MODERATE) {
            oss << "\xE2\x96\x92 ";  // ▒
        } else {
            oss << "\xE2\x96\x91 ";  // ░
        }
    }

    return oss.str();
}

std::vector<PositionAnalysis> getVariablePositions(const std::vector<PositionAnalysis>& positions) {
    std::vector<PositionAnalysis> variable;
    for (const auto& pos : positions) {
        if (pos.isVariable) {
            variable.push_back(pos);
        }
    }
    return variable;
}

std::vector<PositionAnalysis> getMostVariablePositions(const std::vector<PositionAnalysis>& positions,
                                                       int topN) {
    std::vector<PositionAnalysis> sorted = positions;
    std::sort(sorted.begin(), sorted.end(),
              [](const PositionAnalysis& a, const PositionAnalysis& b) {
                  if (a.conservation != b.conservation) {
                      return a.conservation < b.conservation;
                  }
                  return a.position < b.position;
              });

    if (static_cast<int>(sorted.size()) > topN) {
        sorted.resize(static_cast<size_t>(topN));
    }
    return sorted;
}

AnalysisSummary runFullAnalysis(const std::vector<std::string>& sequences) {
    AnalysisSummary summary;
    summary.consensus = generateConsensus(sequences);
    summary.positions = analyzePositions(sequences, summary.consensus);

    double totalConservation = 0.0;
    double totalConfidence = 0.0;
    int nonGapPositions = 0;

    for (const auto& pos : summary.positions) {
        if (pos.counts.nonGapTotal() > 0) {
            totalConservation += pos.conservation;
            totalConfidence += pos.confidence;
            ++nonGapPositions;
        }
        if (pos.conservation >= THRESHOLD_HIGHLY_CONSERVED) {
            ++summary.highlyConservedCount;
        }
        if (pos.isVariable) {
            ++summary.variablePositionCount;
        }
    }

    if (nonGapPositions > 0) {
        summary.averageConservation = totalConservation / nonGapPositions;
        summary.averageConfidence = totalConfidence / nonGapPositions;
    }

    double gcSum = 0.0;
    for (const std::string& seq : sequences) {
        double gc = calculateGCContent(seq);
        summary.sequenceGC.push_back(gc);
        gcSum += gc;
    }
    summary.consensusGC = calculateGCContent(summary.consensus);
    summary.averageGC = sequences.empty() ? 0.0 : gcSum / sequences.size();

    return summary;
}

}  // namespace consensusx
