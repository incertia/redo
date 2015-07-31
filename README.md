# Redo
---

This is my attempt at implementing DJB's [redo](http://cr.yp.to/redo.html).
Semantics will be similar to Jekor's
[implementation](https://github.com/jekor/redo) in Haskell. However, this
project will be done in C in an effort to be slightly less resource intensive as
I can foresee no big speed improvements between the Haskell implementation and
this because as far as I can see, most of the time will be spent on `fork()` and
`exec()`. Maybe I'll get this packaged and send it to the AUR, but that's a
maybe.

## Do script syntax
---

* Do scripts must be executable with `sh`. An option will be added later to
change the program that executes the do script.
* `$1` is undecided at the moment
* `$2` contains the basename of the target. For example,
`/etc/rc.d/target.extension` would have basename `target`.
* `$3` is the output file the script should produce. For example, `gcc -o $3
src/$2.c`.
