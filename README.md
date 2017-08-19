# C assembler
C assembler for final project - Course 20465 Open University

## Introduction
The assembler is a two pass assembler and it is written in the C90 standard.
It encodes assembly (.as) files into output files encoded in strange 4 base.
The results are kept in and .ob, .ext and .ent files.

The object file (.ob) will contain encoding of the code part followed by the data part.
The first like in the object file will describe the IC (Instruction Counter) and DC (Data Counter) sizes.

The external file (.ext) will contain encoding of variables and their respective location in memory,
that are external to the assembly files provided as input.

Finally, the entry file (.ent) will contain encoding of variables and their respective location in memory,
which should be referenced later by external programs.


### How to run
To compile just run make, then run the my_assembler binary file with command line arguments.
Command line arguments should be assembly file names separated by spaces, for example:
```
./my_assembler examples/positive_file1/positive_file1.as examples/positive_file2/positive_file2.as
```

## System components and flow
The assembler is comprised of three major parts, which are the assembler's first pass (first_pass.c),
second pass (second_pass.c) and the line parser (line_parser.c).
In addition, the assembler makes use of structs defined in the data structures file (data_structures.c) for its operation.

The assembler.c contains some functions related to releasing dynamic memory allocated structs which are linked into
linked lists. Those linked lists are in fact the data table, symbol table, code table and sentence table.

For each input assembly file, the main program will execute the first pass and if no errors are found it will execute
the second pass as well. In any case, all errors in an assembly file will be printed into standard error.
For example of some of the errors run:

```
./my_assembler examples/negative_examples/negative_examples.as
```

First pass uses extensively functions of line parser in order to:
* Check for errors in each line of the input file.
* Parse each line of input into a "sentence" data structure, populating it's fields with useful data for both
first and second pass.

After the first pass is complete, the data, symbol and sentence tables are populated and ready for use by the
second pass assembler. The second pass assembler will encode the sentence table followed by the data table into the
code table. Finally, it will write the code table into the output object file, encoded in 4 strange base.
The external and entry files will be created while encoding the sentence table during the second pass run.

The second pass makes extensive use of the helper conversion functions provided in number_conversions.c. First pass
makes some use of it as well, however not to the same extent.

## Authors

* **Dafna Groisman (A.K.A TheBestDafna)**
* **Dafna Assaf (A.K.A La Magnifica)**

## License

This project is licensed under the MIT License - see the [LICENSE.md](LICENSE.md) file for details