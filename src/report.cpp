#include "report.h"

#include "utils.h"

#include <fstream>
#include <iomanip>
#include <sstream>

namespace consensusx {

static std::string repeatChar(char c, size_t count) {
    return std::string(count, c);
}

std::string generateReport(const std::vector<SequenceRecord>& records,
                           const AnalysisSummary& summary,
                           const std::vector<std::vector<double>>& similarityMatrix) {
    std::ostringstream report;
    size_t alignmentLength = records.empty() ? 0 : records[0].sequence.size();

    report << repeatChar('=', 60) << "\n";
    report << "CONSENSUSX REPORT\n";
    report << repeatChar('=', 60) << "\n\n";

    report << "INPUT INFORMATION\n";
    report << repeatChar('-', 40) << "\n";
    report << "Number of sequences: " << records.size() << "\n";
    report << "Alignment length:    " << alignmentLength << "\n\n";

    for (size_t i = 0; i < records.size(); ++i) {
        report << "  " << records[i].name << " (" << records[i].sequence.size() << " bp)\n";
    }
    report << "\n";

    report << "CONSENSUS SEQUENCE\n";
    report << repeatChar('-', 40) << "\n";
    report << summary.consensus << "\n\n";

    report << "CONSENSUS STATISTICS\n";
    report << repeatChar('-', 40) << "\n";
    report << "Average conservation:       " << formatPercent(summary.averageConservation) << "\n";
    report << "Highly conserved positions: " << summary.highlyConservedCount << "\n";
    report << "Variable positions:         " << summary.variablePositionCount << "\n";
    report << "Average confidence:         " << formatPercent(summary.averageConfidence) << "\n\n";

    report << "GC ANALYSIS\n";
    report << repeatChar('-', 40) << "\n";
    report << "Average GC:    " << formatPercent(summary.averageGC) << "\n";
    report << "Consensus GC:  " << formatPercent(summary.consensusGC) << "\n";
    for (size_t i = 0; i < records.size(); ++i) {
        report << "  " << records[i].name << ": "
               << formatPercent(summary.sequenceGC[i]) << "\n";
    }
    report << "\n";

    report << "VARIATION SUMMARY\n";
    report << repeatChar('-', 40) << "\n";
    report << "Number of variable positions: " << summary.variablePositionCount << "\n";

    std::vector<PositionAnalysis> mostVariable = getMostVariablePositions(summary.positions, 5);
    report << "Most variable positions:\n";
    for (const auto& pos : mostVariable) {
        if (pos.isVariable) {
            report << "  Position " << pos.position << ": consensus=" << pos.consensus
                   << ", conservation=" << formatPercent(pos.conservation)
                   << ", alternatives=" << pos.alternatives << "\n";
        }
    }
    report << "\n";

    report << "SEQUENCE SIMILARITY\n";
    report << repeatChar('-', 40) << "\n";
    report << "Pairwise similarity matrix (%):\n\n";

    report << std::setw(12) << " ";
    for (size_t j = 0; j < records.size(); ++j) {
        report << std::setw(10) << records[j].name.substr(0, 8);
    }
    report << "\n";

    for (size_t i = 0; i < records.size(); ++i) {
        report << std::setw(12) << records[i].name.substr(0, 10);
        for (size_t j = 0; j < records.size(); ++j) {
            report << std::setw(9) << formatPercent(similarityMatrix[i][j]) << " ";
        }
        report << "\n";
    }
    report << "\n";

    report << "CONSERVATION ASCII PROFILE\n";
    report << repeatChar('-', 40) << "\n";
    report << buildConservationAscii(summary.positions) << "\n\n";

    report << "DETAILED POSITION ANALYSIS\n";
    report << repeatChar('-', 40) << "\n";
    for (const auto& pos : summary.positions) {
        report << "Position:     " << pos.position << "\n";
        report << "Consensus:    " << pos.consensus << "\n";
        report << "A:            " << pos.counts.a << "\n";
        report << "T:            " << pos.counts.t << "\n";
        report << "G:            " << pos.counts.g << "\n";
        report << "C:            " << pos.counts.c << "\n";
        report << "Gaps:         " << pos.counts.gap << "\n";
        report << "Conservation: " << formatPercent(pos.conservation)
               << " (" << pos.conservationCategory << ")\n";
        report << "Confidence:   " << formatPercent(pos.confidence)
               << " (" << pos.confidenceCategory << ")\n";
        report << "Variation:    " << (pos.isVariable ? "Yes" : "No");
        if (pos.isVariable) {
            report << " (Alternatives: " << pos.alternatives << ")";
        }
        report << "\n" << repeatChar('-', 30) << "\n";
    }

    report << "\n" << repeatChar('=', 60) << "\n";
    report << "End of ConsensusX Report\n";
    report << repeatChar('=', 60) << "\n";

    return report.str();
}

bool saveReportToFile(const std::string& report, const std::string& filepath) {
    std::ofstream out(filepath);
    if (!out.is_open()) {
        return false;
    }
    out << report;
    return true;
}

bool saveConsensusFasta(const std::string& consensus, const std::string& filepath) {
    std::ofstream out(filepath);
    if (!out.is_open()) {
        return false;
    }
    out << ">ConsensusX_consensus\n";
    out << consensus << "\n";
    return true;
}

}  // namespace consensusx
