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
