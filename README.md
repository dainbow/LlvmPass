# Introduction
I hope you have some basic knowledges about LLVM to use this little project.

As you know, LLVM provides us an mechanism of passes which applies on IR during process of compilation. 
![How does LLVM work](https://user-images.githubusercontent.com/26509840/225095862-eb984adc-88e3-4e29-8a9a-6558ce3b9ecd.png)

So, we can make our own pass to reveal all LLVM secrets:
  + Collect static data from sources of [simple program](https://github.com/dainbow/LlvmPass/blob/master/targets/INSPECTED_fibonacci.cpp) and make a graph of all llvm::instruction's users inside every single function.
  + Inject calls of log function before every call and ret instruction to collect some dynamic data and make a graph where we'll see what functions are called inside other functions, their arguments and what results they returned.

Then, combine static and dynamic data to make ultimate graph which will give us some kind of LLVM listing and program's profiling. 

# Installation
## Dependences 
  + ``` llvm ``` (tested on ``` 15.0.7-2 ```) 
  + ``` llvm-libs ``` (tested on ``` 15.0.7-2 ```)
  + ``` clang ``` (tested on ``` 15.0.7-2 ```)
  + ``` make ``` (tested on ``` 4.4.1-2 ```)
  + ``` cmake ``` (tested on ``` 3.26.0-1 ``` )
## Process of install
  + ``` $ git clone https://github.com/dainbow/LlvmPass.git ```
  + ``` $ cd LlvmPass ```
  + ``` $ make init ```
  
Now you can use this project!

# Usage
  1. Put .cpp file with your program into /targets/ folder. (There is already simple program which calculates fibonacci numbers)
      - Note that inspectation pass applies only on files contains INSPECTED in the name
  3. Now you should compile it. 
      - ``` $ make *your program name without extension*```
  4. Then you should execute your program to collect dynamic data.
      - ``` $ ./*your program name without extension*.out ```
  5. Both static and dynamic data are collected so we can make graphs
      - ``` $ make dumps ```
  6. Finally, you should combine static and dynamic data. It can be done with [combiner program](https://github.com/dainbow/LlvmPass/blob/master/targets/combiner.cpp)
      - ``` $ make combiner ```
      - ``` $ ./combiner.out ```
  7. You can see three images in /dumps/ folder
      - ``` static.png ``` shows listing of LLVM instructions, these instructions placed into its function's subgraphs
      - ``` dynamic.png ``` shows dependences between functions during execution. You can see callers and callees, attrubites to call function and what it returned. Also, the most frequent functions' become angrier
      - ``` combined.png ``` shows combination of last two graphs

# Notes
  + This simple program assumes that your function has only one ``` uint64_t ``` argument and return this type also.
  + If you see strange name of your functions in graph, just put ``` extern "C" ``` just before function's declaration.

# Graph examples

![static graph](https://user-images.githubusercontent.com/26509840/226178025-d5f8c44e-1c84-423c-b88b-60a4f8e5b1c8.png)
![dynamic graph](https://user-images.githubusercontent.com/26509840/226178044-532d6df0-5201-491d-ba13-d85108530520.png)
![combined graph](https://user-images.githubusercontent.com/26509840/226178054-9e7d59ba-ac7c-4523-be12-7f054b3ea2a5.png)


