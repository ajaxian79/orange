# Orange

A C++ app framework allowing for creation of clean Dear ImGUI Applications

## Credits

While this framework did not start as a fork of https://github.com/StudioCherno/Walnut an absolute metric ton of code 
was pulled from there, some was adapted and some remains the same.

## Intentions

I need a clean easy application framework for several commercial applications I will be writing, I considered starting
from Walnut but my aim is for my applications to be crossplatform and my current linux setup does not have Vulkan support
so for my personal use case a solution that uses OpenGL3 on Linux statically linked is better additionally I intend to
heavily modify this framework and add widgets to it directly to support my needs.

## Images/Icons Reources

In the `lib/images` directory there are two scripts, these accomplish unpacking and repacking resources, they are currently
written in bash and have several dependencies, sed, awk, hexdump, etc.  These utilities will eventually be rewritten so there
isn't a platform dependency on bash and these other various POSIX utilities.
