# redo

This is my attempt at implementing DJB's [redo](http://cr.yp.to/redo.html).
Semantics will be similar to Jekor's
[implementation](https://github.com/jekor/redo) in Haskell. However, this
project will be done in C in an effort to be slightly less resource intensive as
I can foresee no big speed improvements between the Haskell implementation and
this because as far as I can see, most of the time will be spent on `fork()` and
`exec()`. Maybe I'll get this packaged and send it to the AUR, but that's a
maybe.
