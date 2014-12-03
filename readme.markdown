# remembyte - experiments in human-memorable byte arrays

DISCLAIMER: Much of this is aspirational, all code is alpha at the moment, and until there is a real release, the code might not even build.

The problem: it's really hard to remember `16:27:ac:a5:76:28:2d:36:63:1b:56:4d:eb:df:a6:48` (which happens to represent GitHub's RSA SSH host key fingerprint, at the time of this writing). 

`remembyte` is a set of experients in ways to make this easier. For instance, you might map each byte to [emoji](http://www.windytan.com/2014/10/visualizing-hex-bytes-with-unicode-emoji.html), or to an [alternating pair of word lists](https://en.wikipedia.org/wiki/PGP_word_list). 

Essentially, this project was born out of a frustration when setting up a new computer for the first time, and a desire to learn some C. For now, it's just for fun. 

## Features

`remembyte` is designed to be extensible. Currently it supports connecting to an SSH server and displaying host key fingerprints. Eventually, it will also support connecting to a TLS server (HTTPS, etc) and displaying TLS certificate fingerprints, as well as being passed hex or decimal values on the command line. 

Its display system is also designed to be extensible. Currently it can display byte array representations of the emoji and PGP wordlist mappings listed above, but eventually I would like to experiment with combinations (noun list, verb list, object list - remember sentences! etc) and other mapping types. 

See the [todo document](./todo.markdown) for the current status, and a roadmap of planned features.

