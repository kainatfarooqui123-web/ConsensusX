#include "analysis.h"
#include "consensus.h"
#include "fasta.h"
#include "report.h"
#include "similarity.h"
#include "utils.h"

#include <iostream>
#include <iomanip>
#include <limits>
#include <string>
#include <vector>

using namespace consensusx;

static std::vector<std::string> extractSequences(const std::vector<SequenceRecord>& records) {
    std::vector<std::string> sequences;
    sequences.reserve(records.size());
    for (const auto& record : records) {
        sequences.push_back(record.sequence);
    }
    return sequences;
}

static void pause() {
    std::cout << "\nPress Enter to continue...";
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
}

static void printHeader() {
    std::cout << "\n";
    std::cout << "========================================\n";
    std::cout << "       ConsensusX v1.0\n";
    std::cout << "  DNA Alignment & Variant Insight Tool\n";
    std::cout << "========================================\n";
}

static void printMenu() {
    std::cout << "\n--- Main Menu ---\n";
    std::cout << " 1. Load FASTA file\n";
    std::cout << " 2. Enter sequences manually\n";
    std::cout << " 3. View sequences\n";
    std::cout << " 4. Generate consensus sequence\n";
    std::cout << " 5. View conservation profile\n";
    std::cout << " 6. View variable positions\n";
    std::cout << " 7. View pairwise similarity\n";
    std::cout << " 8. Analyze GC content\n";
    std::cout << " 9. Generate complete report\n";
    std::cout << "10. Save results\n";
    std::cout << "11. Exit\n";
    std::cout << "Select an option: ";
}

static bool ensureLoaded(const std::vector<SequenceRecord>& records) {
    if (records.empty()) {
        std::cout << "\nNo sequences loaded. Use option 1 or 2 first.\n";
        return false;
    }
    return true;
}

static bool ensureValidated(const std::vector<SequenceRecord>& records, bool validated) {
    if (!ensureLoaded(records)) {
        return false;
    }
    if (!validated) {
        std::cout << "\nSequences are not validated. Reload or re-enter sequences.\n";
        return false;
    }
    return true;
}

int main() {
    std::vector<SequenceRecord> records;
    bool validated = false;
    AnalysisSummary lastAnalysis;
    std::vector<std::vector<double>> lastSimilarity;
    bool analysisReady = false;

    printHeader();
    std::cout << "Welcome! Load aligned DNA sequences to begin analysis.\n";

    int choice = 0;
    while (true) {
        printMenu();
        if (!(std::cin >> choice)) {
            std::cin.clear();
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            std::cout << "Invalid input. Please enter a number.\n";
            continue;
        }

        if (choice == 11) {
            std::cout << "\nThank you for using ConsensusX. Goodbye!\n";
            break;
        }

        switch (choice) {
            case 1: {
                std::cout << "\nEnter FASTA file path: ";
                std::string path;
                std::cin >> path;
                std::string message;
                if (parseFastaFile(path, records, message)) {
                    std::vector<std::string> sequences = extractSequences(records);
                    ValidationResult vr = validateAlignment(sequences);
                    std::cout << message << "\n";
                    std::cout << vr.message << "\n";
                    validated = vr.valid;
                    analysisReady = false;
                } else {
                    std::cout << message << "\n";
                    records.clear();
                    validated = false;
                    analysisReady = false;
                }
                break;
            }
            case 2: {
                std::string message;
                if (readManualSequences(records, message)) {
                    std::vector<std::string> sequences = extractSequences(records);
                    ValidationResult vr = validateAlignment(sequences);
                    std::cout << message << "\n";
                    std::cout << vr.message << "\n";
                    validated = vr.valid;
                    analysisReady = false;
                } else {
                    std::cout << message << "\n";
                    records.clear();
                    validated = false;
                    analysisReady = false;
                }
                break;
            }
            case 3: {
                if (!ensureLoaded(records)) break;
                std::cout << "\n--- Loaded Sequences ---\n";
                for (size_t i = 0; i < records.size(); ++i) {
                    std::cout << ">" << records[i].name << "\n";
                    std::cout << records[i].sequence << "\n";
                }
                std::cout << "Total: " << records.size() << " sequences, length "
                          << records[0].sequence.size() << " bp\n";
                break;
            }
            case 4: {
                if (!ensureValidated(records, validated)) break;
                lastAnalysis = runFullAnalysis(extractSequences(records));
                lastSimilarity = calculatePairwiseSimilarity(extractSequences(records));
                analysisReady = true;
                std::cout << "\n--- Consensus Sequence ---\n";
                std::cout << lastAnalysis.consensus << "\n";
                std::cout << "Length: " << lastAnalysis.consensus.size() << " bp\n";
                break;
            }
            case 5: {
                if (!ensureValidated(records, validated)) break;
                if (!analysisReady) {
                    lastAnalysis = runFullAnalysis(extractSequences(records));
                    analysisReady = true;
                }
                std::cout << "\n--- Conservation Profile ---\n";
                std::cout << "Position | Consensus | Conservation | Category\n";
                std::cout << "---------|-----------|--------------|----------\n";
                for (const auto& pos : lastAnalysis.positions) {
                    if (pos.counts.nonGapTotal() == 0) continue;
                    std::cout << std::setw(8) << pos.position << " | "
                              << "        " << pos.consensus << " | "
                              << std::setw(11) << formatPercent(pos.conservation) << " | "
                              << pos.conservationCategory << "\n";
                }
                std::cout << "\n--- ASCII Conservation Profile ---\n";
                std::cout << buildConservationAscii(lastAnalysis.positions) << "\n";

                std::cout << "\n--- Confidence Scores ---\n";
                std::cout << "Position | Consensus | Confidence | Category\n";
                std::cout << "---------|-----------|------------|----------\n";
                for (const auto& pos : lastAnalysis.positions) {
                    if (pos.counts.nonGapTotal() == 0) continue;
                    std::cout << std::setw(8) << pos.position << " | "
                              << "        " << pos.consensus << " | "
                              << std::setw(10) << formatPercent(pos.confidence) << " | "
                              << pos.confidenceCategory << "\n";
                }
                break;
            }
            case 6: {
                if (!ensureValidated(records, validated)) break;
                if (!analysisReady) {
                    lastAnalysis = runFullAnalysis(extractSequences(records));
                    analysisReady = true;
                }
                std::vector<PositionAnalysis> variable = getVariablePositions(lastAnalysis.positions);
                std::cout << "\n--- Variable Positions ---\n";
                if (variable.empty()) {
                    std::cout << "No variable positions detected. All positions are identical.\n";
                    break;
                }
                for (const auto& pos : variable) {
                    std::cout << "\nPosition:      " << pos.position << "\n";
                    std::cout << "Consensus:     " << pos.consensus << "\n";
                    std::cout << "A: " << pos.counts.a << "  T: " << pos.counts.t
                              << "  G: " << pos.counts.g << "  C: " << pos.counts.c << "\n";
                    std::cout << "Conservation:  " << formatPercent(pos.conservation) << "\n";
                    std::cout << "Confidence:    " << formatPercent(pos.confidence)
                              << " (" << pos.confidenceCategory << ")\n";
                    std::cout << "Alternative:   " << pos.alternatives << "\n";
                }
                std::cout << "\nTotal variable positions: " << variable.size() << "\n";
                break;
            }
            case 7: {
                if (!ensureValidated(records, validated)) break;
                lastSimilarity = calculatePairwiseSimilarity(extractSequences(records));
                std::cout << "\n--- Pairwise Sequence Similarity (%) ---\n\n";
                std::cout << std::setw(14) << " ";
                for (const auto& rec : records) {
                    std::cout << std::setw(12) << rec.name.substr(0, 10);
                }
                std::cout << "\n";
                for (size_t i = 0; i < records.size(); ++i) {
                    std::cout << std::setw(14) << records[i].name.substr(0, 12);
                    for (size_t j = 0; j < records.size(); ++j) {
                        std::cout << std::setw(11) << formatPercent(lastSimilarity[i][j]) << " ";
                    }
                    std::cout << "\n";
                }
                break;
            }
            case 8: {
                if (!ensureValidated(records, validated)) break;
                if (!analysisReady) {
                    lastAnalysis = runFullAnalysis(extractSequences(records));
                    analysisReady = true;
                }
                std::cout << "\n--- GC Content Analysis ---\n";
                for (size_t i = 0; i < records.size(); ++i) {
                    std::cout << records[i].name << ": "
                              << formatPercent(lastAnalysis.sequenceGC[i]) << "\n";
                }
                std::cout << "Consensus:  " << formatPercent(lastAnalysis.consensusGC) << "\n";
                std::cout << "Average:    " << formatPercent(lastAnalysis.averageGC) << "\n";

                std::cout << "\nEnter sliding window size (0 to skip): ";
                size_t windowSize = 0;
                std::cin >> windowSize;
                if (windowSize > 0) {
                    std::cout << "\n--- Sliding-Window GC Profile (Consensus) ---\n";
                    std::vector<GCWindow> windows =
                        slidingWindowGC(lastAnalysis.consensus, windowSize);
                    if (windows.empty()) {
                        std::cout << "Window size too large for consensus length.\n";
                    } else {
                        for (const auto& w : windows) {
                            std::cout << "Window " << w.start << "-" << w.end << ": "
                                      << formatPercent(w.gcPercent) << "\n";
                        }
                    }
                }
                break;
            }
            case 9: {
                if (!ensureValidated(records, validated)) break;
                lastAnalysis = runFullAnalysis(extractSequences(records));
                lastSimilarity = calculatePairwiseSimilarity(extractSequences(records));
                analysisReady = true;
                std::string report = generateReport(records, lastAnalysis, lastSimilarity);
                std::cout << report;
                break;
            }
            case 10: {
                if (!ensureValidated(records, validated)) break;
                if (!analysisReady) {
                    lastAnalysis = runFullAnalysis(extractSequences(records));
                    lastSimilarity = calculatePairwiseSimilarity(extractSequences(records));
                    analysisReady = true;
                }
                std::string report = generateReport(records, lastAnalysis, lastSimilarity);
                if (saveReportToFile(report, "consensusx_report.txt")) {
                    std::cout << "\nReport saved to consensusx_report.txt\n";
                } else {
                    std::cout << "\nError: Could not save report.\n";
                }
                if (saveConsensusFasta(lastAnalysis.consensus, "consensus.fasta")) {
                    std::cout << "Consensus saved to consensus.fasta\n";
                } else {
                    std::cout << "Error: Could not save consensus FASTA.\n";
                }
                break;
            }
            default:
                std::cout << "Invalid option. Please select 1-11.\n";
                break;
        }
    }

    return 0;
}
