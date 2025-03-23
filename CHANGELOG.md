# Changelog

All notable changes to this project will be documented in this file.

The format is based on [Keep a Changelog](https://keepachangelog.com/en/1.1.0/),
and this project adheres to [Semantic Versioning](https://semver.org/spec/v2.0.0.html).

Given a version number MAJOR.MINOR.PATCH, increment the:

- MAJOR version when you make incompatible API changes
- MINOR version when you add functionality in a backward compatible manner
- PATCH version when you make backward compatible bug fixes


## [Unreleased]

- Start thinking about deep reinforcement learning: exploring AlphaGo and AlphaGoZero ideas
- Currently improving C++ code for better runtime performances


## [3.0.0] - 2025-02-20

### Complete C++ engine

- Folder cpp contains the engine written entirely in C++
- Implementation of negamax, with initial moves encoded inside the engine


## [2.0.0] - 2025-02-07

### C++ engine implementation

- Folder cpp_and_py contains the code for the engine that is run in python but delegates all the board computations to C++


## [1.1.0] - 2024-12-25

### Utilites

- File exe_tester.py and ours_tester.py to create insights on what Strategy wins more and in how many turns
- Started alpha-beta pruning strategy, with very easy implementation of the heuristic

## [1.0.0] - 2024-11-27

### Project start

- File README.md, src/Engine.md and CHANGELOG.md to explain what we're doing.
- Completed Hivemind's engine with options (folder pyth), possibility to change strategy and to find the best move.
