# Introduction
As you know, LLVM provides us an mechanism of passes which applies on IR during process of compilation. 
![How does LLVM work](https://user-images.githubusercontent.com/26509840/225095862-eb984adc-88e3-4e29-8a9a-6558ce3b9ecd.png)

So, we can make our own pass to reveal all LLVM secrets:
  + Collect static data from sources of [simple program](https://github.com/dainbow/LlvmPass/blob/master/targets/fibonacci.c) and make a graph of all llvm::instruction's users inside every single function.
  + Inject calls of log function before every call and ret instruction to collect some dynamic data and make a graph where we'll see what functions are called inside other functions, their arguments and what results they returned.

Then, combine static and dynamic data to make ultimate graph which will give us some kind of LLVM listing and program's profiling. 
