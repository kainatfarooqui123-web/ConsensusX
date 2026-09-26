#ifndef CONSENSUSX_FASTA_H
#define CONSENSUSX_FASTA_H

#include <string>
#include <vector>

namespace consensusx {

struct SequenceRecord {
    std::string name;
    std::string sequence;
};

// Parse a FASTA file manually. Lines starting with '>' are headers.
// Sequence lines are concatenated until the next header or EOF.
bool parseFastaFile(const std::string& filepath, std::vector<SequenceRecord>& records,
                    std::string& errorMessage);

// Read sequences interactively from terminal input.
bool readManualSequences(std::vector<SequenceRecord>& records, std::string& errorMessage);

}  // namespace consensusx

#endif  // CONSENSUSX_FASTA_H
