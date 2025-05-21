# jsh

##### TODO:
- Add gtest suit
- Add escaping for environment variables
  - Things of the form $VAR should be replaced by the variable contents


## Basics:

Inside of `jsh` there are two main components, `processes` and `jobs`. 
A `process` is as you would expect, it is one binary that will be invoked on the given command line arguments it is passed.
A `job` is a series of processes that will run in sequence with one another.
Processes are chained together into jobs through the use operators.

## Operators:

- `|` (WIP): The `|` (pipe) operator chains together two commands such that the standard output of the first command becomes the standard input for the second command.
- `&&` (WIP): The `&&` (and) operator chains together two commands without altering either of them.

> [!IMPORTANT]  
> Operator chaining must be used in the form `$<first command and args><whitespace><operator><whitespace><second command and args>`

## Redirection:

In `jsh` there are two types of indirection, input and output.

Input Redirection:

Input redirection can be achieved by appending the `<` character followed by the relative or absolute filepath to the desired input file.
This files contents will then populate standard in for the first program in the job.

Output Redirection:

Output redirection can be achieved by appending the `>` character followed by the relative or absolute filepath to the desired output file.
This file will then be populated with contents of standard out from the last process in the job.

