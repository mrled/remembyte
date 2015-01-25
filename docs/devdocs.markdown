# Dev Docs

This is just a notebook of shit I'm reading / learning about when I am working on this

SSH libraries: 

- libssh appears to support ED25519, even though it's not listed on the front page: 
  http://git.libssh.org/projects/libssh.git/tree/doc/curve25519-sha256@libssh.org.txt
- libssh docs are incomplete; you'll want to get the source and be able to reference it
- was looking on GitHub, but apparently this is wrong: https://github.com/nviennot/libssh/
- official repos are here: http://git.libssh.org/

Regex Libraries: 

- pcre appears to be the industry standard
- not clear how well pcre will work on Windows w/ MSVC
- NONE OF THESE SUPPORT REPLACEMENTS???? what the fuck
- pcre has Contrib/pcrs which can do substitutions, but it worries me to rely on it tbh
- [list of other regex libs](http://blog.brush.co.nz/2009/02/regex-libs/), a few very lightweight
- [pcre api documentation](http://www.gsp.com/cgi-bin/man.cgi?topic=pcreapi)
- [pcredemo.c program](http://www.opensource.apple.com/source/pcre/pcre-4.1/pcre/pcredemo.c) - useful reference

Config file libraries: 

- Not even sure I'll need these, but possibly for post-compile definitions of mappings
- [inih](http://blog.brush.co.nz/2009/02/inih/) looks interesting, also that page has a list of alternatives
- [tcfp](http://tcfp.sourceforge.net/)
- [more options](http://stackoverflow.com/questions/3695591/parsing-ini-like-configuration-files)
- [libconfig](http://www.hyperrealm.com/libconfig/)
- [libconfuse](https://github.com/martinh/libconfuse)

## C and Unicode

God damn it's a pain in the ass to figure some of this shit out. 

### wchar_t is mostly of historical interest

<https://www.mail-archive.com/linux-utf8@nl.linux.org/msg05612.html>

> > By the way, another question, if all those stdio.h and
> > string.hfunctions, work well with UTF-8 strings, as they actually do,
> > what would be
> > the reason to use wchar_t and wchar_t-aware functions?
> 
> There are a mix of reasons, but most stem from the fact that the
> Japanese designed some really bad encodings for their language prior
> to UTF-8, which are almost impossible to use in a standard C
> environment. At the time, the ANSI/ISO C committee thought that it
> would be necessary to avoid using char strings directly for
> multilingual text purposes, and was setting up to transition to
> wchar_t strings; however, this was very incomplete. Note that C has no
> support for using wchar_t strings as filenames, and likewise POSIX has
> no support for using them for anything having to do with interfacing
> with the system or library in places where strings are needed. Thus
> there was going to be a dichotomy where multilingual text would be a
> special case only available in some places, while system stuff,
> filenames, etc. would have to be ASCII. UTF-8 does away with that
> dichotomy.
> 
> ...

He does go on to explain some cases where you could use wchar_t, but they are not normal and I don't anticipate that they will affect me.  

<https://www.mail-archive.com/linux-utf8@nl.linux.org/msg05604.html>

> > And what about UTF-8 strings? Do you mean that these strings should be
> > stored in common char*
> 
> Yes.
> 
> > variables? So, what about the character size defference (Unicode and ASCII)?
> > And also, string functions? (like, strtok())
> 
> strtok, strsep, strchr, strrchr, strpbrk, strspn, and strcspn will all
> work just fine on UTF-8 strings as long as the separator characters
> you're looking for are ASCII.


## Remembyte's build system

This is still a work in progress, I haven't decided anything for certain at this point.

My plan is to create a Makefile that works on OS X, Linux, and BSD `make` programs out of the box. In addition, I will maintain an XCode project and a Visual Studio solution. 

This lets me use the debugging and correctness-checking methods that are standard for each platform that I use. As a new C programmer, I've found XCode's Project -> Analyze and Visual Studio's SDL warnings and debugger to be *extremely* helpful in finding errors I don't yet know enough to look for. Furthermore, it means that a developer familiar with the normal practices on any one environment can download my project and view it with tools they are accustomed to. 

### A universal Makefile?

That's not to say that the idea of a single, platform-agnostic Makefile doesn't appeal to me. I do almost all my editing outside of the IDEs, and being able to compile it on the command line, no matter the platform, is appealing to me. 

My original plan was to use make in this way. And I still could. But honestly, dealing with three make variants (GNU on Linux and OS X, BSD on the BSDs, and nmake on Windows) plus two IDE build systems (XCode and Visual Studio) sounds like an even bigger pain in the ass. 

Plus, relying on nmake on Windows seems a little weird. From my reading, it looks like nmake is just there for legacy projects; Microsoft intends for you to use MSBuild for new ones. And then there's the pain in the ass that is the "Visual Studio Command Prompt", which is apparently the best thing Microsoft could come up with.

### What about tests? 

This might change immediately because how I do tests impacts how difficult it is to keep those IDE projects in sync with my Makefile. If the tests are all in separate .c files, I'll have to add them to the IDE projects every time I create a new one, which sounds suuuuper fun. 

I may keep the IDE projects around, just for their analysis and debugging capabilities, and rely on a Makefile to run my tests. 

### Other options

- `autoconf`: fuck no, even if it wasn't limited to MinGW or Cygwin on Windows
- GNU `make` everywhere: impossible if I want to avoid MinGW 
- `CMake`: maybe. I don't like the idea of another tool, but it is *one* syntax for all platforms

### I think I'm going to need to do this now

I want to start writing tests like this: <http://c.learncodethehardway.org/book/ex30.html>. To do that, I need a build system that can support building lots of tiny executables. 