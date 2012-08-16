# Build and run the unit tests (or speed tests) on linux
#
# Should work with recent versions of both gcc and clang.  (To compile with
# clang use "make CXX=clang++".)

CXXFLAGS?=-Wall

test: tinyformat_test_cxx98 tinyformat_test_cxx0x
	@echo running tests...
	@./tinyformat_test_cxx98 && ./tinyformat_test_cxx0x && echo "No errors"

doc: tinyformat.html

speed_test: tinyformat_speed_test
	@echo running speed tests...
	@echo printf timings:
	@time -p ./tinyformat_speed_test printf > /dev/null
	@echo iostreams timings:
	@time -p ./tinyformat_speed_test iostreams > /dev/null
	@echo tinyformat timings:
	@time -p ./tinyformat_speed_test tinyformat > /dev/null
	@echo boost timings:
	@time -p ./tinyformat_speed_test boost > /dev/null

tinyformat_test_cxx98: tinyformat.h tinyformat_test.cpp
	$(CXX) $(CXXFLAGS) --std=c++98 -DTINYFORMAT_NO_VARIADIC_TEMPLATES tinyformat_test.cpp -o tinyformat_test_cxx98

tinyformat_test_cxx0x: tinyformat.h tinyformat_test.cpp
	$(CXX) $(CXXFLAGS) --std=c++0x -DTINYFORMAT_USE_VARIADIC_TEMPLATES tinyformat_test.cpp -o tinyformat_test_cxx0x

tinyformat.html: README.rst
	@echo building docs...
	rst2html README.rst > tinyformat.html

tinyformat_speed_test: tinyformat.h tinyformat_test.cpp
	$(CXX) $(CXXFLAGS) -O3 -DSPEED_TEST tinyformat_test.cpp -o tinyformat_speed_test

clean:
	rm -f tinyformat_test_cxx98 tinyformat_test_cxx0x tinyformat_speed_test
	rm -f tinyformat.html
	rm -f _bloat_test_tmp_*
