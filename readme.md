# RamHog

RamHog allocates a chunk of memory, fills it with random data, then
optionally repeatedly reads from that memory.

Usage:

    ramhog 8G --touch --random --sleep=500000000

this command tells ramhog to allocate 8G of memory, then repeatedly
cycle between reading random portions of that memory and sleeping for
a half second.



