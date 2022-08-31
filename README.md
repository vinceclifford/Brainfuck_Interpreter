
# Brainfuck_Interpreter

This C-Program will interpret Brainfuck code. It's highly optimized for minimizing the amount of processor cycles needed to interpret the code. In a competition with more than 850 submissions, my interpreter was the 5th fastest. The array-size of the Interpreter is set at 30000. The Brainfuck-command ',' was not implemented because the tests of the competition only performed mathematical operations and checked the output of the program on the terminal.


In order to run the interpreter the project has to be cloned
```
git clone https://github.com/vinceclifford/Brainfuck_Interpreter.git
```

I willingly decided to implement all the code in one single C-file to make the compilation as easy as possible. Compile the program with gcc or clang. Run the command to compile

````
gcc main.c
````

With that you're all set. You can now execute the interpreter. Run the executable and pass the Brainfuck code as the only argument as a String. Example to run the interpreter for the Brainfuck code ">++.<+.": 
````
./a.out ">++.<+."
````
