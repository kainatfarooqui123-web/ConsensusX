# Nucleotide Consensus Sequence Analyzer

A beginner-friendly **C++17** command-line tool for bioinformatics students. Paste a DNA or RNA nucleotide sequence and the program searches for the **CnGCnG** consensus motif, reports every match (including overlapping ones), and summarizes sequence statistics.

---

## Project Purpose

This program helps you practice sequence analysis in C++ without external bioinformatics libraries. You enter a nucleotide string, the tool validates it, scans for a biologically meaningful pattern, and prints clear results suitable for lab reports or coursework.

---

## What Is a Consensus Sequence?

In molecular biology, a **consensus sequence** is a short pattern that describes a recurring motif found in related DNA or RNA sequences. Instead of matching one exact string, a consensus uses fixed letters and **wildcard** positions so that several similar sequences can all be recognized.

---

## Meaning of CnGCnG

The motif searched by this program is:

```
C  n  G  C  n  G
```

| Position | Rule | Meaning |
|----------|------|---------|
| 1 | **C** | Must be cytosine |
| 2 | **n** | Any valid nucleotide (A, C, G, T, or U) |
| 3 | **G** | Must be guanine |
| 4 | **C** | Must be cytosine |
| 5 | **n** | Any valid nucleotide |
| 6 | **G** | Must be guanine |

The program does **not** search for the literal text `"CnGCnG"`. The letter **n** is a wildcard placeholder.

### Examples that match

| Sequence | Why it matches |
|----------|----------------|
| `CAGCTG` | C-A-G-C-T-G |
| `CTGCAG` | C-T-G-C-A-G |
| `CCGCCG` | C-C-G-C-C-G |
| `CUGCAG` | C-U-G-C-A-G (RNA) |

### Example that does not match

| Sequence | Why it fails |
|----------|--------------|
| `AAAAAAA` | No C-G-C-G pattern at fixed positions |

---

## Meaning of `n`

**n = any nucleotide**

At positions marked `n`, the program accepts **A**, **C**, **G**, **T**, or **U**. This mirrors common bioinformatics notation where `n` represents an unspecified base.

---

## How the Algorithm Works

1. **Read input** — The user pastes a sequence (multi-line allowed).
2. **Clean** — `cleanSequence()` removes spaces and line breaks, then converts to uppercase.
3. **Validate** — `validateSequence()` checks that every character is A, C, G, T, or U.
4. **Search** — `findConsensus()` slides a 6-nucleotide window across the full sequence. At each index, `matchesConsensus()` tests the pattern C–any–G–C–any–G.
5. **Overlap** — The window moves one base at a time, so overlapping matches (e.g. `CAGCTGCAGCTG`) are all reported.
6. **Statistics** — `calculateGCContent()` counts each nucleotide and computes GC%.
7. **Display** — `displayResults()` prints matches with **1-based** start/end positions (biology convention).

---

## Project Files

```
nucleotide-consensus-analyzer/
├── main.cpp                 # Program entry point and user interface
├── sequence_analyzer.h      # Function declarations and data structures
├── sequence_analyzer.cpp    # Core analysis logic
├── README.md                # This file
└── web/
    └── index.html           # Browser version (shareable, no install)
```

---

## How to Compile

**Requirements:** A C++17 compiler (g++, clang++, or MinGW on Windows)

```bash
g++ -std=c++17 main.cpp sequence_analyzer.cpp -o analyzer
```

On Windows (PowerShell), the executable will be `analyzer.exe`.

---

## How to Run

```bash
./analyzer
```

On Windows:

```powershell
.\analyzer.exe
```

Paste your sequence when prompted, then press **Enter** on a blank line to finish.

---

## Example Input

```
CAGCTGCAGCTG
```

Or a longer paste with line breaks:

```
cag ctg
c tgcag
```

(Lowercase and spaces are accepted and normalized automatically.)

---

## Example Output

```
============================================================
                    ANALYSIS RESULTS
============================================================

Consensus: CnGCnG
Note: In this motif, 'n' means ANY valid nucleotide (A, C, G, T, or U).

RESULT: FOUND

Number of matches: 2

Match 1:
  Sequence: CAGCTG
  Position: 1-6

Match 2:
  Sequence: CAGCTG
  Position: 7-12

------------------------------------------------------------
                 SEQUENCE STATISTICS
------------------------------------------------------------

Total sequence length: 12
A nucleotides: 2
C nucleotides: 4
G nucleotides: 4
T nucleotides: 2
U nucleotides: 0
GC percentage: 66.67%

============================================================
```

If no motif is found:

```
Consensus sequence CnGCnG was not found.
```

---

## Web Version (Shareable Tool)

Open `web/index.html` in any modern browser — no compilation or server required. You can host the `web` folder on [GitHub Pages](https://pages.github.com/) or any static file host and share the URL with classmates.

---

## Test Cases

| Input | Should match CnGCnG? | Notes |
|-------|----------------------|-------|
| `CAGCTG` | Yes | 1 match at 1–6 |
| `CTGCAG` | Yes | 1 match at 1–6 |
| `CCGCCG` | Yes | 1 match at 1–6 |
| `AAAAAAA` | No | No motif |
| `CAGCTGCAGCTG` | Yes | 2 overlapping matches |
| `tggcag` | No | Lowercase → `TGGCAG`; position 1 is T, not C |
| `CUGCAG` | Yes | RNA sequence; 1 match at 1–6 |

---

## C++ Concepts Used

- Header/source file separation
- `std::string` manipulation
- `std::vector` for storing matches
- Structs for grouped data
- Functions with clear single responsibilities
- Input validation and formatted console output

---

## License

Educational use — suitable for university bioinformatics coursework.
