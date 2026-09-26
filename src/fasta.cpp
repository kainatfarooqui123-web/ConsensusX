#include "fasta.h"

#include "utils.h"

#include <cctype>
#include <fstream>
#include <iostream>

namespace consensusx {

namespace {

bool isSequenceLine(const std::string& line) {
    if (line.empty()) {
        return false;
    }
    for (char c : line) {
        char upper = c;
        if (upper >= 'a' && upper <= 'z') {
            upper = static_cast<char>(std::toupper(static_cast<unsigned char>(upper)));
        }
        if (!isValidNucleotide(upper)) {
            return false;
        }
    }
    return true;
}

}  // namespace

bool parseFastaFile(const std::string& filepath, std::vector<SequenceRecord>& records,
                    std::string& errorMessage) {
    records.clear();

    std::ifstream file(filepath);
    if (!file.is_open()) {
        errorMessage = "Error: Could not open FASTA file '" + filepath + "'.";
        return false;
    }

    SequenceRecord current;
    bool hasCurrent = false;
    std::string line;

    auto finalizeCurrent = [&]() {
        if (hasCurrent && !current.sequence.empty()) {
            current.sequence = toUpperCase(current.sequence);
            records.push_back(current);
            current = SequenceRecord();
            hasCurrent = false;
        }
    };

    while (std::getline(file, line)) {
        line = trim(line);
        if (line.empty()) {
            continue;
        }

        if (line[0] == '>') {
            finalizeCurrent();
            current.name = trim(line.substr(1));
            if (current.name.empty()) {
                current.name = "Unnamed_sequence_" + std::to_string(records.size() + 1);
            }
            hasCurrent = true;
        } else if (isSequenceLine(line)) {
            if (!hasCurrent) {
                current.name = "Sequence_" + std::to_string(records.size() + 1);
                hasCurrent = true;
            }
            current.sequence += line;
        } else {
            finalizeCurrent();
            current.name = line;
            hasCurrent = true;
        }
    }

    finalizeCurrent();

    if (records.empty()) {
        errorMessage = "Error: FASTA file contains no sequences.";
        return false;
    }

    errorMessage = "Loaded " + std::to_string(records.size()) + " sequence(s) from FASTA file.";
    return true;
}

bool readManualSequences(std::vector<SequenceRecord>& records, std::string& errorMessage) {
    records.clear();

    std::cout << "\nEnter the number of sequences: ";
    int count = 0;
    if (!(std::cin >> count) || count < 1) {
        errorMessage = "Error: Invalid sequence count.";
        std::cin.clear();
        std::cin.ignore(10000, '\n');
        return false;
    }
    std::cin.ignore(10000, '\n');

    for (int i = 0; i < count; ++i) {
        SequenceRecord record;
        std::cout << "Enter name for sequence " << (i + 1) << ": ";
        std::getline(std::cin, record.name);
        record.name = trim(record.name);
        if (record.name.empty()) {
            record.name = "Sequence_" + std::to_string(i + 1);
        }

        std::cout << "Enter sequence " << (i + 1) << " (single line): ";
        std::getline(std::cin, record.sequence);
        record.sequence = toUpperCase(trim(record.sequence));
        records.push_back(record);
    }

    errorMessage = "Entered " + std::to_string(records.size()) + " sequence(s) manually.";
    return true;
}

}  // namespace consensusx
