# TODO / Roadmap document

## Completed releases

v0.1 (minimally useful to me personally):

-   connect to ssh server @done
-   display emoji mapping @done

## Upcoming releases

Note: this is just a rough map, not a contract. Features in this list may be 
moved from one release to another or axed entirely.

v0.2 (basic feature completeness):

-   display pgp mapping @done
-   add '-a' flag to specify action @done
-   connect to TLS services (https, imaps, etc) and display cert fingerprints
-   take input buffer on the command line, via a switch or a pipe
    -   take hex input as 0x1234 or ab239f or 12:34:56
    -   take decimal input as 1234
    -   take in whole strings, and find any of the above via PCRE library, 
        replace with the requested byterep, and output the entire string with 
        replacements. Useful particularly for piping or taking input from a file

v1.0 (completeness and correctness):

-   write tests
-   audit memory management
-   audit exposed objects (which functions are internal use only vs which one 
    should be exposed to other modules)
-   audit style and consistency (don't use _t b/c it's reserved by POSIX, etc...
    will require more research and possibly help from a more experienced C 
    programmer)
-   build a library that can be used from any program
    -   step one: implement generic buf2map() function @done
    -   step other ones: ???
-   build on Windows with cl.exe and nmake.exe
-   build on Unix with (at least) clang and make
-   get some sort of build system that generates makefiles or whatever

v1.1 (distribution):

-   create a NuGet package and submit it to Chocolatey 
-   submit a debian package for consideration

v2.0 (and beyond):

-   C# bindings & powershell cmdlets
-   New mode: implement openssh's VisualHostKey (particularly useful on Windows)
-   New mode: sentences. First byte from a list of nouns, second from list of 
    verbs, third from list of objects. (Or something like that.) 

    Consider implementing this in a separate experimental module after I finish 
    library support in 0.9 - this feature would not be stable until I had a 
    really *good* selection of words in each list, and I will have to do 
    significant experimentation to discover such a selection. 

Maybes & ideas: 

-   Remove requirement for libssh - implement just the part of the protocol I 
    need myself
