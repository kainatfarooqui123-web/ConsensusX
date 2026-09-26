#ifndef CONSENSUSX_ANALYSIS_H
#define CONSENSUSX_ANALYSIS_H

#include "consensus.h"

#include <string>
#include <vector>

namespace consensusx {

struct PositionAnalysis {
    size_t position = 0;  // 1-based for display
    char consensus = '-';
    NucleotideCounts counts;
    double conservation = 0.0;
    double confidence = 0.0;
    std::string conservationCategory;
    std::string confidenceCategory;
    bool isVariable = false;
    std::string alternatives;
};

struct GCWindow {
    size_t start = 0;  // 1-based
    size_t end = 0;
    double gcPercent = 0.0;
};

struct AnalysisSummary {
    std::string consensus;
    std::vector<PositionAnalysis> positions;
    double averageConservation = 0.0;
    int highlyConservedCount = 0;
    int variablePositionCount = 0;
    double averageConfidence = 0.0;
    std::vector<double> sequenceGC;
    double consensusGC = 0.0;
    double averageGC = 0.0;
};

// Conservation = (consensus base count among non-gaps) / non-gap total * 100.
// For IUPAC consensus, use the highest single-nucleotide count at that position.
double calculateConservation(const NucleotideCounts& counts, char consensus);

// Confidence = (max_count - second_max_count) / non_gap_total * 100.
double calculateConfidence(const NucleotideCounts& counts);

// Count how many distinct non-gap nucleotides appear at a position.
int distinctNonGapNucleotides(const NucleotideCounts& counts);

// Build per-position analysis for the entire alignment.
std::vector<PositionAnalysis> analyzePositions(const std::vector<std::string>& sequences,
                                               const std::string& consensus);

// Summarize alignment statistics.
AnalysisSummary runFullAnalysis(const std::vector<std::string>& sequences);

// GC% = (G + C) / valid nucleotides * 100, gaps ignored.
double calculateGCContent(const std::string& sequence);

// Sliding-window GC profile for a sequence.
std::vector<GCWindow> slidingWindowGC(const std::string& sequence, size_t windowSize);

// ASCII conservation visualization for terminal display.
std::string buildConservationAscii(const std::vector<PositionAnalysis>& positions);

// Get variable positions only.
std::vector<PositionAnalysis> getVariablePositions(const std::vector<PositionAnalysis>& positions);

// Find positions with lowest conservation (most variable).
std::vector<PositionAnalysis> getMostVariablePositions(const std::vector<PositionAnalysis>& positions,
                                                       int topN = 5);

}  // namespace consensusx

#endif  // CONSENSUSX_ANALYSIS_H
