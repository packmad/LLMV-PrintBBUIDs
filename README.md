# LLMV - Print Basic Block Unique IDentifiers

This LLVM pass instruments each basic block (BB) of a C program by injecting as its first instruction a call to the `print` function, so that each time a BB is executed, it prints a unique identifier.


## What is the purpose of this?

I have a class on fuzzing in my [Introduction to Cybersecurity](https://simoneaonzo.it/introsec/) course. 
In this class I have some limitations: 

1) I only have three hours to introduce them to software testing;
 
2) The students only know how to program in Python;

3) They have to do an assignment, but I want them to use something other than real fuzzers: get their hands dirty with code!

4) I want them to fuzz a real binary, maybe one they wrote, so I need a feedback mechanism.

So my solution is [a fuzzer written in Python](https://github.com/packmad/IntrosecFuzzing): minimal code, easy to read and modify.
On the other hand, this project allows feedback-based fuzzing using basic block coverage.


## Usage
LLMV-PrintBBUIDs only works on Linux (tested on Ubuntu 24.04) with Docker installed. 

Install Docker:
```
curl -fsSL https://get.docker.com -o get-docker.sh
sh get-docker.sh
```

Then, clone this repo and run [llvm_printBBUIDs.sh](https://github.com/packmad/LLMV-PrintBBUIDs/blob/main/llvm_printBBUIDs.sh):
```
git clone https://github.com/packmad/LLMV-PrintBBUIDs.git
cd LLMV-PrintBBUIDs/
./llvm_printBBUIDs.sh MAIN_C_FILE
```

You just have to pass an argument, the `MAIN_C_FILE`, namely, the C source code file with the main function. 
The bash script will mount the parent folder of the main C file under Docker and generate an instrumented executable in the same folder with `.exe` extension.


## Disclaimer & Limitations
I know full well that implementing a fuzzer in Python is a very bad idea, as is a feedback mechanism involving syscalls. 
But this is all for educational purposes, and the simplifications are there to introduce newbies to fuzzing.

This is a toy project, probably full of bugs. 
It does not currently support build options or build automation. 
Also, in my use case, it only works if all the source files are in the same folder.

Finally, the `llvm_printBBUIDs.sh` [always try to build the container](https://github.com/packmad/LLMV-PrintBBUIDs/blob/main/llvm_printBBUIDs.sh#L6), wasting a bit of time even though no changes have been made. 
I prefer to waste time but always be sure that I am working with the latest source version, you can remove the line if you prefer.


## Acknowledgments
My heartfelt thanks go to the authors of [llvm-tutor](https://github.com/banach-space/llvm-tutor), a tutorial for learning how LLVM passes work. 
With their project, I was able to develop the supporting code in a reasonable time!
