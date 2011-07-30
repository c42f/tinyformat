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
* Parse standard C99 format strings
* Support as many commonly used C99 ``printf()`` features as practical without
  compromising on simplicity.
* Use variadic templates with C++0x but provide good C++98 support for backward
  compatibility


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

All user facing functions are defined in the namespace ``tinyformat``.  A
namespace alias ``tfm`` is provided to encourage brevity, but can easily be
disabled if desired.

Three main interface functions are available: an iostreams-based ``format()``,
a string-based ``format()`` and a ``printf()`` replacement.  All these
functions can take an unlimited number of input arguments if compiled with
C++0x variadic templates support.  For C++98 compatibility, an in-source python
code generator (using the excellent ``cog.py``; see
http://nedbatchelder.com/code/cog/ ) has been used to generate multiple
versions of the functions with up to 10 values of user-defined type.  This
maximum can be customized by setting the ``maxParams`` parameter in the code
generator and regenerating the code using ``cog.py``.


The ``format()`` function which takes a stream as the first argument is the
main part of the tinyformat interface.  ``stream`` is the output stream,
``formatString`` is a format string in C99 ``printf()`` format, and the values
to be formatted are a list of types ``T1, T2, ..., TN``, taken by const
reference::

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


Finally, ``printf()`` is a convenience function which calls ``format()`` with
``std::cout`` as the first argument::

    template<typename T1, typename T2, ...>
    void printf(const char* formatString,
                const T1& value1, const T2& value1, ...)


Error handling
--------------

By default, tinyformat calls ``assert()`` if it encounters an error in the
format string or number of arguments.  This behaviour can be changed (for
example, to throw an exception) by defining the ``TINYFORMAT_ERROR`` macro
before including tinyformat.h, or editing the config section of the header.


Formatting user defined types
-----------------------------

User defined types with a stream insertion operator will be formatted using
``operator<<(std::ostream&, T)`` by default.  The ``"%s"`` format specifier is
suggested for user defined types, unless the type is inherently numeric.

For further customization, the user can override the ``formatValue()``
function to specify formatting independently of the stream insertion operator.
If you override this function, the library will have already parsed the format
specification and set the stream flags accordingly.  If ``formatValue()`` isn't
general enough, it's also possible to overload the ``formatValueBasic()``
function to allow the user defined formatter to do its own parsing of the
format specification string and setting of stream flags.  See the source for
details on these functions.


Wrapping tfm::format() inside a user defined format function
------------------------------------------------------------

Suppose you wanted to define your own function which wraps ``tfm::format``.
For example, consider an error function taking an error code, which in C++0x
might be written simply as::

    template<typename... Args>
    void error(int code, const char* fmt, const Args&... args)
    {
        std::cerr << "error (code " << code << ")";
        tfm::format(std::cerr, fmt, args...);
    }

Unfortunately it's rather painful to do this with C++98, because you must
write a version of ``error()`` for every number of arguments you want to
support.  However, tinyformat provides a macro ``TINYFORMAT_WRAP_FORMAT`` to
do this for you in a handy range of cases.  (In fact, this is the way that the
convenience functions ``format()`` and ``printf()`` are defined internally.)
Here's what the usage looks like::

    #define TINYFORMAT_WRAP_FORMAT_EXTRA_ARGS int code,
    TINYFORMAT_WRAP_FORMAT(
        void,                                        /* return type */
        error,                                       /* function name */
        std::cerr << "error (code " << code << ")";, /* stuff before format()*/
        std::cerr,                                   /* stream name */
        /*empty*/                                    /* stuff after format() */
    )
    #undef TINYFORMAT_WRAP_FORMAT_EXTRA_ARGS

This defines an overloaded set of ``error()`` functions which act like
the C++0x definition given above, at least up until ``maxPararms`` format
parameters.  Note that the content of ``TINYFORMAT_WRAP_FORMAT_EXTRA_ARGS``
*must contain a trailing comma for every extra argument* and therefore can't be
a normal macro parameter to ``TINYFORMAT_WRAP_FORMAT`` (the commas would look
like more than one macro argument to the preprocessor).


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
3. Code bloat due to instantiating a lot of templates

Tinyformat tries to solve these problems while providing formatting which is
sufficiently general for most incidental day to day uses.  If you need a very
general or very performant library, tinyformat is probably not for you.

The script ``bloat_test.sh`` included in the repository tests whether
tinyformat succeeds in avoiding compile time and code bloat for nontrivial
projects.  The idea is to include ``tinyformat.h`` into 100 translation units
and use ``printf()`` five times in each to simulate a medium sized project.
The resulting executable size and compile time (g++-4.4.3, linux ubuntu 10.04,
best of three) is shown in the following tables.

**Non-optimized build**

====================== ================== ==========================
test name              total compile time executable size (stripped)
====================== ================== ==========================
libc printf            1.2s               44K  (36K)
std::ostream           8.5s               84K  (64K)
tinyformat, no inlines 12.0s              128K (100K)
tinyformat             12.9s              172K (140K)
tinyformat, c++0x mode 14.8s              172K (140K)
boost::format          51.6s              772K (676K)
====================== ================== ==========================

**Optimized build (-O3)**

====================== ================== ==========================
test name              total compile time executable size (stripped)
====================== ================== ==========================
libc printf            1.6s               44K  (32K)
std::ostream           9.5s               80K  (60K)
tinyformat, no inlines 21.0s              168K (144K)
tinyformat             33.6s              340K (308K)
tinyformat, c++0x mode 36.2s              340K (308K)
boost::format          101.1s             1.2M (1.1M)
====================== ================== ==========================

We can see that with each level of convenience/generality you pay a penalty,
with the worst being the jump from ``printf()`` to ``std::ostream`` (the best
performing but least convenient typesafe alternative).  For large projects it's
arguably worthwhile to do separate compilation of the non-templated parts of
tinyformat, as shown in the rows labelled *tinyformat, no inlines*.  These were
generated by taking the contents of ``namespace detail`` along with the
zero-argument version of ``format()`` and putting them into a separate file,
tinyformat.cpp.


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
syntax.  Douglas Gregor's introduction to variadic templates
-- see http://www.generic-programming.org/~dgregor/cpp/variadic-templates.html --
was also helpful, especially since it solves exactly the ``printf()`` problem
for the case of trivial format strings.

Bugs
----

Here's a list of known bugs which are probably cumbersome to fix:

* Field padding is unlikely to work correctly with complicated user defined
  types.
