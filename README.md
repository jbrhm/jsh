# jsh

##### TODO:
- Add gtest suit
- Add escaping for environment variables
  - Things of the form $VAR should be replaced by the variable contents


##### Schematics:

Inside of `jsh` there are two main components, `processes` and `jobs`. 
A `process` is as you would expect, it is one binary that will be invoked on the given command line arguments it is passed.
A `job` is a series of processes that will run in sequence with one another.
Processes are chained together into jobs through the use operators.

Operators:
- `|` (WIP): The `|` (pipe) operator chains together two commands such that the standard output of the first command becomes the standard input for the second command.
- `&&` (WIP): The `&&` (and) operator chains together two commands without altering either of them.
