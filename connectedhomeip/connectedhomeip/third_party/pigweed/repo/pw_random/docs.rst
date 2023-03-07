.. _module-pw_random:

---------
pw_random
---------
Pigweed's ``pw_random`` module provides a generic interface for random number
generators, as well as some practical embedded-friendly implementations. While
this module does not provide drivers for hardware random number generators, it
acts as a user-friendly layer that can be used to abstract away such hardware.

Embedded systems have the propensity to be more deterministic than your typical
PC. Sometimes this is a good thing. Other times, it's valuable to have some
random numbers that aren't predictable. In security contexts or areas where
things must be marked with a unique ID, this is especially important. Depending
on the project, true hardware random number generation peripherals may or may
not be available. Even if RNG hardware is present, it might not always be active
or accessible. ``pw_random`` provides libraries that make these situations
easier to manage.

Using RandomGenerator
=====================
There's two sides to a RandomGenerator; the input, and the output. The outputs
are relatively straightforward; ``GetInt()`` randomizes the passed integer
reference, and ``Get()`` dumps random values into a the passed span. The inputs
are in the form of the ``InjectEntropy*()`` functions. These functions are used
to "seed" the random generator. In some implementations, this can simply be
resetting the seed of a PRNG, while in others it might directly populate a
limited buffer of random data. In all cases, entropy injection is used to
improve the randomness of calls to ``Get*()``.

It might not be easy to find sources of entropy in a system, but in general a
few bits of noise from ADCs or other highly variable inputs can be accumulated
in a RandomGenerator over time to improve randomness. Such an approach might
not be sufficient for security, but it could help for less strict uses.

Algorithms
==========
xorshift*
---------
The ``xorshift*`` algorithm is a pseudo-random number generation algorithm. It's
very simple in principle; the state is represented as an integer that, with each
generation, performs exclusive OR operations on different left/right bit shifts
of itself. The "*" refers to a final multiplication that is applied to the
output value.

Pigweed's implementation augments this with an ability to inject entropy to
reseed the generator throughout its lifetime. When entropy is injected, the
results of the generator are no longer completely deterministic based on the
original seed.

Note that this generator is NOT cryptographically secure.

For more information, see:

 * https://en.wikipedia.org/wiki/Xorshift
 * https://www.jstatsoft.org/article/view/v008i14
 * http://vigna.di.unimi.it/ftp/papers/xorshift.pdf

Future Work
===========
A simple "entropy pool" implementation could buffer incoming entropy later use
instead of requiring an application to directly poll the hardware RNG peripheral
when the random data is needed. This would let a device collect entropy when
idling, improving the latency of potentially performance-sensitive areas where
random numbers are needed.
