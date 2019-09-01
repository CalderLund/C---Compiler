# C-- Compiler
C-- is a simplified C-based programming language. The goal of this project is to develop an general understanding of how compilers are built and how they can be optimized. This projects uses a 32-bit MIPS assembler, see https://www.student.cs.uwaterloo.ca/~cs241/mips/mipsref.pdf for commands.

So far, this compiler includes:
- Scanner (Reads code and determines if each token is valid. In addition, it outputs the tokens in order to std::out)

To add:
- Tests for scanner
- Code and tests for Parser
- Code and tests for Scoping & Type Checker
- Code and tests for Code Generator
- Code for Loader
- Code for Linker
- Code and tests for Heap
- Bash scripts for auto-compiling

Additional topics to explore:
- Building assembler
- Bootstrapping compiler
- Additional features in code
