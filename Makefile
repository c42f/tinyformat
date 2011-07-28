# Build the test programs using g++

test: tinyformat_test_cxx98 tinyformat_test_cxx0x
	@echo running tests...
	@./tinyformat_test_cxx98 && ./tinyformat_test_cxx0x && echo "No errors"

tinyformat_test_cxx98: tinyformat.h tinyformat_test.cpp
	g++ -Wall --std=c++0x -DTINYFORMAT_NO_VARIADIC_TEMPLATES tinyformat_test.cpp -o tinyformat_test_cxx98

tinyformat_test_cxx0x: tinyformat.h tinyformat_test.cpp
	g++ -Wall --std=c++0x tinyformat_test.cpp -o tinyformat_test_cxx0x

clean:
	rm -f tinyformat_test_cxx98 tinyformat_test_cxx0x
	rm -f _bloat_test_tmp_*
