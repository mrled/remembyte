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
-   move from statically defined mappings to externally defined mappings
    -   put map files somewhere. have config file that associates map(s), a 
        terminator, & a seperator w/ a name. get_display_hash() looks these up
        by a name the user will pass on the cli.
-   take input buffer on the cli
    -   accept it via a switch @done
    -   accept it via a pipe
    -   take hex input as 0x1234 or ab239f or 12:34:56 @done
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

-   submit a NuGet package to Chocolatey for consideration
-   submit an apt package to debian for consideration
-   create a homebrew tap

v2.0 (and beyond):

-   C# bindings & powershell cmdlets
-   New mode: implement openssh's VisualHostKey (particularly useful on Windows)
-   New mode: sentences. First byte from a list of nouns, second from list of 
    verbs, third from list of objects. (Or something like that.) 

    Consider implementing this in a separate experimental module after I finish 
    library support in 0.9 - this feature would not be stable until I had a 
    really *good* selection of words in each list, and I will have to do 
    significant experimentation to discover such a selection. 

## Maybes & ideas

-   Remove requirement for libssh - implement just the part of the protocol I 
    need myself

## Misc Tasks

This stuff is basically uncategorized at this point

-   Validate configuration. For example, what if the config doesn't have a 
    default, or has more than one default, or what if a composedmap specifies
    a rawmap that doesn't exist?
-   Use libssh's ssh_keytypes_e enum to iterate through SSH key types

Cross platform concerns

-   Abstract away some method for normalizing file paths. Fucking christ.
-   Had to disable SDL checks under VS for ini.c (`/sdl-`) because of its use 
    of strdup(). 
-   Ended up just disabling SDL checks under VS (`/sdl-`) altogether. These 
    should be reenabled and all warnings fixed. `inih` library triggers one of
    these, so it will need to be modified too. 

