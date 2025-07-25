# jsh

## Usage:

To build `jsh` from source use the `build.sh` script.

To setup the environment install all dependencies in `scripts/setup.sh` and then from the `jsh` folder run `scripts/build_google_test`.

To begin execute `build/jsh`.

## Basics:

Inside of `jsh` there are two main components, `processes` and `jobs`. 
A `process` is as you would expect, it is one binary that will be invoked on the given command line arguments it is passed.
A `job` is a series of processes that will run in sequence with one another.
Processes are chained together into jobs through the use of operators.
The `exit` keyword can be used to exit the `jsh` shell.

## Environment Variables:

`jsh` supports setting environment variables through the keyword `export`.
Export commands must be of the form `$export[whitespace][variable name]=[value]`

> [!IMPORTANT]  
> Variable names cannot contain the `=` character
> 
> Variable names cannot contain the `{` or `}` character
>
> Variables and values cannot contain any whitespace

`jsh` supports using environment variables through substitution using the `$` character.
To achieve substitution, the substitution must be of the form `$[command part 1]${[environment variable name]}[command part 2]`.
In this case the characters from the `$` to the `}` will be replaced by the value of the environment variable name.
In the case where the environment variable does not exist, it will be substituted for an empty string.
The environment variable `$?` will return the previous process' exit value.

> [!IMPORTANT]  
> `jsh` does not support nested variable substitutions
> 
> If there is a `$` character but the `{` and `}` are not properly placed (ie. the `{` is not immediately after the `$`), the substitution will fail and `jsh` will continue on to the next substitution in the input after the current substitutions `$`.
> 
> `jsh` will use the next `}` after the `{` to use as closing brace for the substitution
>
> `jsh` supports 1000000 substitutions for one prompt
> 
> `jsh` does not allow the name of a environment variable to be `?`

## Operators:

- `|`: The `|` (pipe) operator chains together two commands such that the standard output of the first command becomes the standard input for the second command.
- `&&`: The `&&` (and) operator chains together two commands.

> [!IMPORTANT]  
> Operator chaining must be used in the form `$[first command and args][whitespace][operator][whitespace][second command and args]`

## Redirection:

In `jsh` there are two types of indirection, input and output.

Input Redirection:

Input redirection can be achieved by appending the `<` character followed by the relative or absolute filepath to the desired input file at any point in the command.
This files contents will then populate standard in for the first program in the job.

Output Redirection:

Output redirection can be achieved by appending the `>` character followed by the relative or absolute filepath to the desired output file at any point in the command.
This file will then be populated with contents of standard out from the last process in the job.

> [!IMPORTANT]  
> Input and output redirection must be of the form `$[arg1][whitespace][arg2][whitespace]...[< or >][whitespace][filepath][whitespace]...[whitespace][argn-1][whitespace][argn]`
>
> `jsh` does not support redirecting for standard error currently
>
> In `jsh` pipes have precedence for IO redirection over redirection, such that if both redirection and pipes are utilized, the pipe will be written to instead of the file.

## Contributing:

To contribute, please make github issues. These github issues will then be solved by the maintainer.
