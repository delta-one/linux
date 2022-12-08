============
mISDN Driver
============

mISDN is a new modular ISDN driver, in the long term it should replace
<<<<<<< HEAD
the old I4L driver architecture for passive ISDN cards.
=======
the old I4L driver architecture for passiv ISDN cards.
>>>>>>> b7ba80a49124 (Commit)
It was designed to allow a broad range of applications and interfaces
but only have the basic function in kernel, the interface to the user
space is based on sockets with a own address family AF_ISDN.
