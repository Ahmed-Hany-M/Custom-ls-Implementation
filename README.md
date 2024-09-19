# Custom-ls-Implementation

### Compilation and setup:

#### Download:
+ auxiliary_functions.h
+ auxiliary_functions.c
+ custom_ls.c

Then, compile using the following command in a bash shell: 

`gcc -o custom_ls auxiliary_functions.c custom_ls.c`

To run this custom ls command simply type `./custom_ls`


-------------------

### Options that are supported.

+ -l: long format.
+ -i: shows inode number of each entry.
+ -1: prints each entry on a separate line.
+ -d: shows the directory.
+ -a: shows hidden files.
+ -t: sorts the entries based on modification time.
+ -u: sorts the entries based on access time.
+ -c: sorts the entries based on change time.
+ -f: No sorting. {By default, ls sorts alphabetically; however, this option ignores any sorting algorithm.}

Not only that, but any combination of the above options are supported as well.

However, there are some rules to be considered:

+ If -t, -u, and -c are combined (for example -tu, -tc, or -uc) then the priority is for -t then -c and lastly -u.

That is, -t is the most dominant, and -c is more dominant than -u. 

In the case that they are alone, the sorting is done by their corresponding functionalities, as discussed above.

+ If -f comes the first option and is followed with -l then the output will be long format and unsorted. 

However, if -f comes in the middle of the combination (e.g. -lfa) then -l is disabled. 

--------------------------------------------

