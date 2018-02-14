


## Introduction

The analysis contains three parts:

 - The c++ files generate a binary `bin/souffle_disasm` which takes care
 of reading an elf file and generating several `.facts` files that
 represent the inital problem.
 
 - `src/souffle_rules.dl` is the specification of the analysis in
 datalog.  It takes the basic facts and computes likely EAs, chunks of
 code, etc. The results are stored in `.csv` files.
 
 - `src/disasm_driver.pl` is a prolog module that calls souffle_disasm
 first, then it calls souffle and finally reads the results from the
 analysis and prints the assembler code. (this script can be called by executing `./bin/disasm`)
 
## Dependencies

- The project is prepared to be built with GTScons and has to be located
in the grammatech trunk directory.

- The analysis depends on souffle being installed, 
in particular the 64 bits version at https://github.com/cfallin/souffle.git
Update (Feb. 9th): The 64 bits version is now in the master branch https://github.com/souffle-lang/souffle/pull/569

- The pretty printer is (for now) written in prolog. It requires some prolog environment
to be installed (preferably SWI-prolog).

## Building souffle_disasm



`/trunk/datalog_disasm/build`


## Running the analysis
Once souffle_disasm is built, we can run complete analysis on a file
by calling `/bin/disasm'`.
For example, we can run the analysis on one of the examples as
follows:

`cd bin` `./disasm ../examples/ex1/ex`

The script accepts the following parameters:

- `-hints` generates a file `hints` in a format that can be read as user hints from csurf

- `-debug` in addition to print what is considered to be code, it prints every instruction
  that has not been explicitly discarded and segments of assembler that have been discarded
  
- `-asm` generate assembler that can be given to an assembler directly.
   NOTE: this is not working just yet.
  
## Comparing to DVT

The script `./bin/test.sh`  runs the disassembler with hints generation on and gives those hints
to csurf using DVT. After csurf is done, the script calls `gtir_compare` and compares the hints generated by the disassembler (user hints) to the hints generated by DVT.

The script has the option to run the disassembler on the stripped binary

The script depends on having gtm (part of gtx) and gtir_compare.


 
## Current status

- The analysis can disassemble correctly `ex1`, `ex_virtualDispatch`
and `ex_switch`, even if these are stripped of their symbols (using
`strip --strip-unneeded`).

- The analysis can disassemble correcty gzip! if the binary is stripped there are
two functions that are missed 'make_simple_name' and 'warn' but these seem to not be
called at all (they seem to be dead code).

- The way clang and gcc deal with switch differs a lot. So far now it works with both.

- I have not tried to disassemble optimized code.


- I started generating information for symbolization (Literal reference disambiguation)
in a very naive way following [6].

### Issues/TODOs

- Compute additional seeds based on computed jumps/calls and exceptions.

- Resolve conflicts based on heuristics.
Update: The modified algorithm gets fewer conflics but I still get some when disassembling gcc (the binary in my computer, could it be optimized? )


## References
1. Souffle: "On fast large-scale program analysis in Datalog" CC2016
 - PDF: http://souffle-lang.org/pdf/cc.pdf
 - License: Universal Permissive License (UPL)
 - Web: https://github.com/souffle-lang/souffle
 
2. Porting Doop from LogicBlox to souffle
 - https://yanniss.github.io/doop2souffle-soap17.pdf

3. bddbddb
 - Web: http://bddbddb.sourceforge.net/
 - Papers:   https://suif.stanford.edu/papers/pldi04.pdf
             https://people.csail.mit.edu/mcarbin/papers/aplas05.pdf

4. Control Flow Integrity for COTS Binaries
   - PDF: http://stonecat/repos/reading/papers/12313-sec13-paper_zhang.pdf

5. Alias analysis for Assembly by Brumley at CMU:
  http://reports-archive.adm.cs.cmu.edu/anon/anon/usr/ftp/2006/CMU-CS-06-180R.pdf
  
6. Reassembleable Disassembling

7. Ramblr: Making Reassembly Great again