# Note that this assumes to be called from the Makefile, you may want to adapt it.
{ echo "A very simple test"; } 2> /dev/null
./simple_test.sh
# Run the same test, but this time with valgrind
{ echo "A very simple test, with Valgrind"; } 2> /dev/null
VALGRIND=1 ./simple_test.sh
