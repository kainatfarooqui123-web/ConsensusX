# ConsensusX: DNA Alignment & Variant Insight Tool

**ConsensusX** is a C++17 command-line bioinformatics tool that analyzes pre-aligned DNA sequences. It goes beyond a basic classroom consensus calculator by combining consensus generation, conservation profiling, confidence scoring, variant detection, pairwise similarity, GC analysis, and professional report export in one cohesive program.

Built for a BS Bioinformatics portfolio — every algorithm is implemented manually using the C++ standard library, with no external bioinformatics dependencies.

---

## Project Motivation

Consensus sequences summarize shared nucleotide information across multiple aligned DNA reads or homologous sequences. In research and teaching, they help identify conserved regions, highlight variation, and summarize sequence collections.

This project transforms a simple "find the consensus" assignment into a **small but realistic sequence analysis pipeline** suitable for a GitHub portfolio and CV.

---

## Biological Background

### What is a Consensus Sequence?

A **consensus sequence** represents the most likely nucleotide at each position of a multiple sequence alignment (MSA). At each column, the program counts A, T, G, and C (ignoring gaps for majority voting) and selects the most frequent base. When two or more bases tie, **IUPAC ambiguity codes** (e.g., R for A/G, Y for C/T) are used.

### Why Are Consensus Sequences Useful?

- Summarize variation across homologous genes or viral genomes
- Identify conserved motifs in regulatory or coding regions
- Support primer design by highlighting stable regions
- Provide a single representative sequence for downstream visualization

### What Does Conservation Mean?

**Conservation** measures how frequently the consensus nucleotide appears at a given position among non-gap sequences. High conservation (≥90%) suggests functional or evolutionary constraint; low conservation indicates variability.

### What Do Variable Positions Represent?

A **variable position** is any alignment column where more than one distinct nucleotide appears (excluding gaps). These sites may reflect SNPs, sequencing errors, strain differences, or biological polymorphism — the tool identifies them for rapid inspection but does **not** classify them clinically.

### Confidence Score (Important Disclaimer)

The **confidence score** measures how strongly the majority nucleotide dominates at each position:

```
confidence = (max_count − second_max_count) / non_gap_total × 100
```

- Unanimous support (10/10 A) → 100% confidence
- Perfect tie (5 A, 5 G) → 0% confidence

This is an **analytical measure of positional agreement**, not a clinical, diagnostic, or mutation-prediction score.

---

## Features

| # | Feature | Description |
|---|---------|-------------|
| 1 | FASTA input | Standard (`>header`) and headerless name/sequence formats |
| 2 | DNA validation | Uppercase conversion, gap support, length/character checks |
| 3 | Consensus generation | Majority vote with IUPAC tie-breaking |
| 4 | Conservation profile | Per-position % with classification thresholds |
| 5 | Confidence scoring | Dominance-based analytical confidence |
| 6 | Variant detection | Lists variable positions with counts and alternatives |
| 7 | Pairwise similarity | Percent identity matrix, gaps ignored when both gap |
| 8 | GC content | Per-sequence, consensus, and average GC% |
| 9 | Sliding-window GC | Configurable window size GC profile |
| 10 | ASCII conservation map | Terminal visualization using block characters |
| 11 | Professional report | Full text analysis report |
| 12 | Export | `consensusx_report.txt` and `consensus.fasta` |
| 13 | CLI menu | Interactive 11-option menu |

---

## Analysis Pipeline

```
FASTA Input / Manual Entry
         ↓
    Validation
         ↓
  Sequence Analysis
         ↓
 Consensus Generation
         ↓
Conservation & Confidence
         ↓
 Variation Detection
         ↓
Similarity & GC Analysis
         ↓
       Report
```

---

## Algorithm Overview

### Consensus (per position)
1. Count A, T, G, C, and gaps at column *i*
2. Ignore gaps when selecting majority
3. If one base wins → use that base
4. If tied → IUPAC ambiguity code
5. If all gaps → output `-`

### Conservation
```
conservation = (consensus_base_count / non_gap_total) × 100
```

### Confidence
```
confidence = (max_count − second_max_count) / non_gap_total × 100
```

### Pairwise Similarity
```
similarity = (matching_positions / comparable_positions) × 100
```
Comparable positions exclude columns where **both** sequences have a gap.

### GC Content
```
GC% = (G + C) / valid_nucleotides × 100
```

---

## C++ Concepts Demonstrated

- **Modular design** — separate headers/sources per responsibility
- **STL containers** — `std::vector`, `std::string`, `std::map`
- **File I/O** — manual FASTA parsing with `std::ifstream`
- **Structs** — `SequenceRecord`, `PositionAnalysis`, `AnalysisSummary`
- **Namespaces** — `consensusx` for organization
- **Const correctness** — read-only parameters where appropriate
- **C++17** — structured, readable code without heavy template metaprogramming

---

## Project Architecture

```
ConsensusX/
├── src/
│   ├── main.cpp        # CLI menu and user interaction
│   ├── fasta.cpp/h     # FASTA parsing and manual input
│   ├── consensus.cpp/h # Consensus generation and IUPAC codes
│   ├── analysis.cpp/h  # Conservation, confidence, GC, variants
│   ├── similarity.cpp/h# Pairwise sequence similarity
│   ├── report.cpp/h    # Report generation and export
│   └── utils.cpp/h     # Validation and helpers
├── data/
│   └── example.fasta   # Sample input
├── tests/
│   └── test_cases.txt  # Manual test scenarios
├── CMakeLists.txt
├── README.md
└── LICENSE
```

---

## Compilation

### Requirements
- C++17-compatible compiler (g++, clang++, or MSVC)
- CMake 3.14+ (optional) or direct g++ invocation

### Option A: CMake
```bash
mkdir build && cd build
cmake ..
cmake --build .
```

The executable is `build/consensusx` (or `build\Release\consensusx.exe` on Windows with MSVC).

### Option B: g++ (direct)
```bash
g++ -std=c++17 -O2 -o consensusx \
    src/main.cpp src/utils.cpp src/fasta.cpp src/consensus.cpp \
    src/analysis.cpp src/similarity.cpp src/report.cpp
```

### Option C: MSVC (Windows)
```powershell
cl /EHsc /std:c++17 /Fe:consensusx.exe src\*.cpp
```

---

## Usage

```bash
./consensusx
```

### Example FASTA Input (`data/example.fasta`)
```
>Sequence_1
ATGCTAGCTAGC
>Sequence_2
ATGCTGGCTAGC
>Sequence_3
ATGCTAGATAGC
>Sequence_4
ATGCTAGCTAGT
```

### Example Menu Workflow
1. **Load FASTA file** → enter `data/example.fasta`
2. **Generate consensus sequence** → view consensus
3. **View conservation profile** → see per-position conservation and ASCII map
4. **View variable positions** → inspect variation sites
5. **Generate complete report** → full analysis in terminal
6. **Save results** → writes `consensusx_report.txt` and `consensus.fasta`

### Example Output (conservation snippet)
```
Position | Consensus | Conservation | Category
---------|-----------|--------------|----------
       1 |         A |       100.0% | Highly conserved
       7 |         G |        75.0% | Conserved
      12 |         T |        75.0% | Conserved
```

---

## Testing

See `tests/test_cases.txt` for 20 manual test scenarios covering:
- Identical and variable sequences
- Lowercase input, invalid characters, length mismatches
- Gaps, IUPAC ties, FASTA formats
- GC, conservation, confidence, similarity, sliding windows
- Export functionality

---

## Limitations

- **Pre-aligned input only** — ConsensusX does not perform sequence alignment (no Needleman-Wunsch, BLAST, etc.)
- **DNA only** — RNA (U) and protein alphabets are not supported
- **Single-line sequences** — manual entry expects one sequence per line
- **Terminal UI** — no graphical interface
- **Educational scope** — not validated for clinical or diagnostic use

---

## Future Improvements

- Support multi-line manual sequence entry
- RNA alphabet (U) option
- Export conservation data as CSV
- Simple command-line flags (non-interactive mode)
- Unit tests with an automated test runner
- Optional BED/GFF annotation overlay

---

## License

MIT License — see [LICENSE](LICENSE).

---

## Author

BS Bioinformatics student project — designed for academic portfolio and GitHub demonstration.
