============
tinyformat.h
============
------------------------------------------------------
A minimal type safe printf-replacement library for C++
------------------------------------------------------

This library aims to support 95% of casual C++ string formatting needs with a
single reasonably lightweight header file.  Anything you can do with this
library can also be done with the standard C++ streams, but probably with
considerably more typing :-)

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
interface.  As shown, it is possible to print a ``std::string`` using the
``"%s"`` conversion.) The same thing could be achieved using either of the two
convenience functions.  One returns a ``std::string``::

    std::string date = tfm::format("%s, %s %d, %.2d:%.2d\n",
                                   weekday, month, day, hour, min);
    std::cout << date;

The other prints to the ``std::cout`` stream::

    tfm::printf("%s, %s %d, %.2d:%.2d\n", weekday, month, day, hour, min);

Here's an example which further emphasizes the type safety; it will work with
any type which has the usual stream insertion operator ``<<`` defined.  If not,
it will fail at compile time::

    template<typename T>
    void myPrint(const T& value)
    {
        tfm::printf("My value is %s\n", value);
    }


Function reference
------------------

All user facing functions are defined in the namespace ``tinyformat``.  A
namespace alias ``tfm`` is provided to encourage brevity, but can easily be
disabled if desired.

Three main interface functions are available: an iostreams-based ``format()``,
a string-based ``format()`` and a ``printf()`` replacement.  These functions
can be thought of as C++ replacements for C's ``fprintf()``, ``sprintf()`` and
``printf()`` functions respectively.  All the interface functions can take an
unlimited number of input arguments if compiled with C++0x variadic templates
support.  For C++98 compatibility, an in-source python code generator (using
the excellent ``cog.py``; see http://nedbatchelder.com/code/cog/ ) has been
used to generate multiple versions of the functions with up to 10 values of
user-defined type.  This maximum can be customized by setting the ``maxParams``
parameter in the code generator and regenerating the code using the command
``cog.py -r tinyformat.h``.


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


Format strings and type safety
------------------------------

Tinyformat parses C99 format strings to guide the formatting process --- please
refer to any standard C99 printf documentation for format string syntax.  In
contrast to printf, tinyformat does not use the format string to decide on
the type to be formatted so this does not compromise the type safety: *you may
use any format specifier with any C++ type*.  The author suggests standardising
on the ``%s`` conversion unless formatting numeric types.

Let's look at what happens when you execute the function call::

    tfm::format(outStream, "%+6.4f", yourType);

First, the library parses the format string, and uses it to modify the state of
``outStream``:

1. The ``outStream`` formatting flags are cleared and the width, precision and
   fill reset to the default.
2. The flag ``'+'`` means to prefix positive numbers with a ``'+'``; tinyformat
   executes ``outStream.setf(std::ios::showpos)``
3. The number 6 gives the field width; execute ``outStream.width(6)``.
4. The number 4 gives the precision; execute ``outStream.precision(4)``.
5. The conversion specification character ``'f'`` means that floats should be
   formatted with a fixed number of digits; this corresponds to executing
   ``outStream.setf(std::ios::fixed, std::ios::floatfield);``

After all these steps, tinyformat executes::

    outStream << yourType;

and finally restores the stream flags, precision and fill.  What happens if
``yourType`` isn't actually a floating point type?  In this case the flags set
above are probably irrelevant and will be ignored by the underlying
``std::ostream`` implementation.  The field width of six may cause some padding
in the output of ``yourType``, but that's about it.


Special cases for "%p", "%c" and "%s"
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Tinyformat normally uses ``operator<<`` to convert types to strings.  However,
the "%p" and "%c" conversions require special rules for robustness.  Consider::

    uint8_t* pixels = get_pixels(/* ... */);
    tfm::printf("%p", pixels);

Clearly the intention here is to print a representation of the *pointer* to
``pixels``, but since ``uint8_t`` is a character type the compiler would
attempt to print it as a string if we blindly fed it into ``operator<<``.  To
counter this kind of madness, tinyformat tries to static_cast any type fed to
the "%p" conversion into a ``const void*`` before printing.  If this can't be
done at compile time the library falls back to using ``operator<<`` as usual.

The "%c" conversion has a similar problem: it signifies that the given integral
type should be converted into a ``char`` before printing.  The solution is
identical: attempt to convert the provided type into a char using
``static_cast`` if possible, and if not fall back to using ``operator<<``.

The "%s" conversion sets the boolalpha flag on the formatting stream.  This
means that a ``bool`` variable printed with "%s" will come out as ``true`` or
``false`` rather than the ``1`` or ``0`` that you would otherwise get.


Incompatibilities with C99 printf
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Not all features of printf can be simulated simply using standard iostreams.
Here's a list of known incompatibilities:

* The C99 ``"%a"`` and ``"%A"`` hexadecimal floating point conversions are not
  supported since the iostreams don't have the necessary flags.  These add no
  extra flags to the stream state but do trigger a conversion.
* The precision for integer conversions cannot be supported by the iostreams
  state independently of the field width.  (Note: **this is only a
  problem for certain obscure integer conversions**; float conversions like
  ``%6.4f`` work correctly.)  In tinyformat the field width takes precedence,
  so the 4 in ``%6.4d`` will be ignored.  However, if the field width is not
  specified, the width used internally is set equal to the precision and padded
  with zeros on the left.  That is, a conversion like ``%.4d`` effectively
  becomes ``%04d`` internally.  This isn't correct for every case (eg, negative
  numbers end up with one less digit than desired) but it's about the closest
  simple solution within the iostream model.
* The ``"%n"`` query specifier isn't supported to keep things simple and will
  result in a call to ``TINYFORMAT_ERROR``.
* Wide characters with the ``%ls`` conversion are not supported, though you
  could write your own ``std::ostream`` insertion operator for ``wchar_t*``.


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
specification and set the stream flags accordingly - see the source for details.


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
Here's what the usage looks like in the case above::

    #undef TINYFORMAT_WRAP_FORMAT_EXTRA_ARGS
    #define TINYFORMAT_WRAP_FORMAT_EXTRA_ARGS int code,
    TINYFORMAT_WRAP_FORMAT(
        void,                                        /* return type */
        error,                                       /* function name */
        /*empty*/,                                   /* function declaration suffix (eg, const) */
        std::cerr << "error (code " << code << ")";, /* stuff before format()*/
        std::cerr,                                   /* stream name */
        /*empty*/                                    /* stuff after format() */
    )
    #undef TINYFORMAT_WRAP_FORMAT_EXTRA_ARGS
    #define TINYFORMAT_WRAP_FORMAT_EXTRA_ARGS

This defines an overloaded set of ``error()`` functions which act like
the C++0x definition given above, at least up until ``maxPararms`` format
parameters.  Note that the content of ``TINYFORMAT_WRAP_FORMAT_EXTRA_ARGS``
is defined to be empty by default for convenience.  In this case we must
redefine it since we want an extra ``code`` argument.  It's important to note
that this macro *must contain a trailing comma for every extra argument* and
therefore can't be a normal macro parameter to ``TINYFORMAT_WRAP_FORMAT`` (the
commas would look like more than one macro argument to the preprocessor).


Benchmarks
----------

Compile time and code bloat
~~~~~~~~~~~~~~~~~~~~~~~~~~~

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

Speed tests
~~~~~~~~~~~

The following speed tests results were generated by building
``tinyformat_test.cpp`` with on linux ubuntu 10.04 with
``g++-4.4.3 -O3 -DSPEED_TEST``, and taking the best of three runs.  In the
test, the format string ``"%0.10f:%04d:%+g:%s:%p:%c:%%\n"`` is filled 2000000
times with output sent to ``/dev/null``; for further details see the source and
Makefile.

============== ========
test name      run time
============== ========
libc printf    1.18s
std::ostream   1.89s
tinyformat     2.10s
boost::format  9.10s
============== ========

It's likely that tinyformat has an advantage over boost.format because it tries
reasonably hard to avoid formatting into temporary strings, preferring instead
to send the results directly to the stream buffer.  Tinyformat cannot
be faster than the iostreams because it uses them internally, but it comes
acceptably close.


Rationale
---------

Or, why did I reinvent this particularly well studied wheel?

Nearly every program needs text formatting in some form but in most cases such
formatting is *incidental* to the main purpose of the program.  In these cases,
you really want a library which is simple to use but as lightweight as
possible.

The ultimate in lightweight dependencies are the solutions provided by the C++
and C libraries.  However, both the C++ iostreams and C's printf() have
well known usability problems: iostreams are hopelessly verbose for complicated
formatting and printf() lacks extensibility and type safety.  For example::

    // Verbose; hard to read, hard to type:
    std::cout << std::setprecision(2) << std::fixed << 1.23456 << "\n";
    // The alternative using a format string is much easier on the eyes
    tfm::printf("%.2f\n", 1.23456);

    // Type mismatch between "%s" and int: will cause a segfault at runtime!
    printf("%s", 1);
    // The following is perfectly fine, and will result in "1" being printed.
    tfm::printf("%s", 1);

On the other hand, there are plenty of excellent and complete libraries which
solve the formatting problem in great generality (boost.format and fastformat
come to mind, but there are many others).  Unfortunately these kind of
libraries tend to be rather heavy dependencies, far too heavy for projects
which need to do only a little formatting.  Problems include

1. Having many large source files.  This makes a heavy dependency unsuitable to
   bundle within other projects for convenience.
2. Slow build times for every file using any sort of formatting (this is very
   noticeable with boost/format.hpp. I'm not sure about the various other
   alternatives.)
3. Code bloat due to instantiating many templates

Tinyformat tries to solve these problems while providing formatting which is
sufficiently general for incidental day to day uses.


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
--- see http://www.generic-programming.org/~dgregor/cpp/variadic-templates.html ---
was also helpful, especially since it solves exactly the ``printf()`` problem
for the case of trivial format strings.

Bugs
----

Here's a list of known bugs which are probably cumbersome to fix:

* Field padding won't work correctly with complicated user defined types.  For
  general types, the only way to do this correctly seems to be format to a
  temporary string stream, check the length, and finally send to the output
  stream with padding if necessary.  Doing this for all types would be
  quite inelegant because it implies extra allocations to make the temporary
  stream.  A workaround is to add logic to operator<<() for composite user
  defined types so they are aware of the stream field width.
