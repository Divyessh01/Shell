# Assignment 2


## Overview

* A user-defined interactive shell program using cpp that supports a semi-colon separated list of commands.

## Execution

* Compile c++ file using command: g++ [cppFilePath] -o [compiledFile]

    ```cpp
    g++ main.cpp -o main
    ```

* Execute compiled file : [compiledFilePath] [inputFilePath]
    ```cpp
    ./main
    ```

## Commands Implemented

### 1.Display

*  A shell prompt of the form : _username_@_system_name_:_current_directory_> appears and takes input from the user and responds as per the commands, until "exit" or "quit" is given as input by the user.

* The directory from which the shell is invoked will be the home directory(~).

### 2.cd, echo, pwd

* Similar execution as POSIX shell.

### 3.ls

* ls command implementation with flags '-a' and '-l', which can handle following command inputs:
    * ls
    * ls -a
    * ls -l
    * ls .
    * ls ..
    * ls ~
    * ls -a -l
    * ls -la/-al
    * ls [Directory/File_name]
    * ls -[Flags] [Directory/File_name]

### 4. System commands (bg / fg), with and without arguments

* Foreground Processes:
    * Shell will wait for this process to complete and regain control when this process exits.

* Background Processes:
    * Any command invoked with "&" is treated as a background command.
    * Shell will spawn that process and doesn't wait for the process to exit, instead it will keep taking other user commands after printing PID of the newly created background process.
    * Example:
        ```
        user@system:~>gedit &
        456
        ```

***