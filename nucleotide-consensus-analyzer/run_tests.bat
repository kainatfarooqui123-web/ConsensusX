@echo off
REM Automated test runner for Nucleotide Consensus Sequence Analyzer
REM Each test sequence is piped to the analyzer (sequence + blank line).

set EXE=analyzer.exe
if not exist %EXE% (
    echo Building analyzer...
    g++ -std=c++17 main.cpp sequence_analyzer.cpp -o analyzer
    if errorlevel 1 exit /b 1
)

echo.
echo ============================================================
echo TEST: CAGCTG (should MATCH - 1 match)
echo ============================================================
(echo CAGCTG& echo.) | %EXE%

echo.
echo ============================================================
echo TEST: CTGCAG (should MATCH - 1 match)
echo ============================================================
(echo CTGCAG& echo.) | %EXE%

echo.
echo ============================================================
echo TEST: CCGCCG (should MATCH - 1 match)
echo ============================================================
(echo CCGCCG& echo.) | %EXE%

echo.
echo ============================================================
echo TEST: AAAAAAA (should NOT match)
echo ============================================================
(echo AAAAAAA& echo.) | %EXE%

echo.
echo ============================================================
echo TEST: CAGCTGCAGCTG (should MATCH - 2 overlapping)
echo ============================================================
(echo CAGCTGCAGCTG& echo.) | %EXE%

echo.
echo ============================================================
echo TEST: tggcag (should NOT match - becomes TGGCAG)
echo ============================================================
(echo tggcag& echo.) | %EXE%

echo.
echo ============================================================
echo TEST: CUGCAG (should MATCH - RNA, 1 match)
echo ============================================================
(echo CUGCAG& echo.) | %EXE%
