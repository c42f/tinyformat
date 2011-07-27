============
tinyformat.h
============
------------------------------------------------------
A minimal type safe printf-replacement library for C++
------------------------------------------------------

This library aims to support 95% of casual C++ string formatting needs with a
single lightweight header file.  Anything you can do with this library can
also be done with the standard C++ streams, but probably with considerably
more typing :)

Design goals:

* Simplicity and minimalism.  A single header file to include and distribute
  with your own projects.
* Type safety and extensibility for user defined types.
* Parse standard C99 format strings, and support most features.
* Support as many commonly used ``printf()`` features as practical without
  compromising on simplicity.


Example usage
-------------

To print the date, we might use::

    std::string weekday = "Wednesday";
    const char* month = "July";
    long day = 27;
    int hour = 14;
    int min = 44;

    tfm::format(std::cout, "%s, %s %d, %.2d:%.2d\n",
                weekday, month, day, hour, min);

(The types here are intentionally odd to emphasize the type safety of the
interface.)  The same thing could be achieved using either of the two
convenience functions.  One returns a ``std::string``::

    std::string date = tfm::format("%s, %s %d, %.2d:%.2d\n",
                                weekday, month, day, hour, min);
    std::cout << date;

The other prints to the ``std::cout`` stream::

    tfm::printf("%s, %s %d, %.2d:%.2d\n", weekday, month, day, hour, min);


Function reference
------------------

All user-facing functions are defined in the namespace ``tinyformat``.  A
namespace alias ``tfm`` is provided to encourage brevity, but can easily be
disabled if desired.

Three main interface functions are available: an iostreams-based ``format()``,
a string-based ``format()`` and a ``printf()`` replacement.  All these
functions can take an unlimited number of input arguments if compiled with
C++0x varadic templates support.  For C++98 compatibility, an in-source python
code generator (via the excellent cog.py, see
http://nedbatchelder.com/code/cog/ ) has been used to generate multiple
versions of the functions with up to 10 user-defined types.  This maximum can
be customized by setting the maxParams parameter in the code generator.


The ``format()`` function taking a stream as the first argument is the main
part of the tinyformat interface.  stream is the output stream, fmt is a
format string in C99 ``printf()`` format, and the values to be formatted are a
list of types ``T1, T2, ..., TN``, taken by const reference::

    template<typename T1, typename T2, ...>
    void format(std::ostream& stream, const char* formatString,
                const T1& value1, const T2& value1, ...)


The second version of ``format()`` is a convenience function which returns a
``std::string`` rather than printing onto a stream.  This function simply
calls the main version of ``format()`` using a ``std::ostringstream``, and
returns the resulting string::

    template<typename T1, typename T2, ...>
    std::string format(const char* formatString,
                       const T1& value1, const T2& value1, ...)


``printf()`` is a convenience function which calls ``format()`` with
``std::cout`` as the first argument::

    template<typename T1, typename T2, ...>
    void printf(const char* formatString,
                const T1& value1, const T2& value1, ...)


Error handling
--------------

By default, tinyformat calls ``assert()`` if it encounters an error in the
format string or number of arguments.  This behaviour can be changed by
defining the ``TINYFORMAT_ERROR`` macro before including tinyformat.h, or
editing the config section of the header.


Formatting user defined types
-----------------------------

User defined types with a stream insertion operator will be formatted using
``operator<<(std::ostream&, T)`` by default.

For further customization, the user can override the ``formatValue()``
function to use the standard format specifier strings to modify the stream
state.  If this isn't general enough, it's also possible to overload the
``formatValueBasic()`` function to allow the user defined formatter to do its
own parsing of the format specification string and setting of stream flags.


Rationale
---------

Or, why did I reinvent this particularly well studied wheel?

It's true that there are lots of other excellent and complete solutions to the
formatting problem (``boost::format`` and fastformat come to mind, but there
are many others).  Unfortunately, these tend to be very heavy dependencies for
the purposes of the average "casual" formatting usage.  This heaviness
manifests in two ways:

1. Large build time dependencies with many source files.  This means the
   alternatives aren't suitable to bundle within other projects.
2. Slow build times for every file using the formatting headers (this is very
   noticeable with boost/format.hpp. I'm not sure about the various other
   alternatives.)

Tinyformat tries to solve these problems while providing formatting which is
sufficiently general for most incidental day to day uses.  If you need a very
general or very performant library, tinyformat is probably not for you.


License
-------

For minimum license-related fuss, tinyformat.h is distributed under the boost
software license, version 1.0.  (Summary: you must keep the license text on
all source copies, but don't have to mention tinyformat when distributing
binaries.)


Author and acknowledgments
--------------------------

Tinyformat was written by Chris Foster [chris42f (at) gmail (d0t) com].  The
implementation owes much to ``boost::format`` for showing that it's fairly
easy to use stream based formatting to simulate most of the ``printf()``
syntax.

Bugs
----

* Negative signs are not extended correctly when padding integer fields with
  zeros.
* Field padding is unlikely to work correctly with complicated user defined
  types.
