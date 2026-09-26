#ifndef CONSENSUSX_REPORT_H
#define CONSENSUSX_REPORT_H

#include "analysis.h"
#include "fasta.h"

#include <string>
#include <vector>

namespace consensusx {

// Build the full text report as a single string.
std::string generateReport(const std::vector<SequenceRecord>& records,
                           const AnalysisSummary& summary,
                           const std::vector<std::vector<double>>& similarityMatrix);

// Write report to consensusx_report.txt
bool saveReportToFile(const std::string& report, const std::string& filepath);

// Write consensus sequence to consensus.fasta
bool saveConsensusFasta(const std::string& consensus, const std::string& filepath);

}  // namespace consensusx

#endif  // CONSENSUSX_REPORT_H
