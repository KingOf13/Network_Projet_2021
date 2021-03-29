# Note that this assumes to be called from the Makefile, you may want to adapt it.
{ echo "A very simple test"; } 2> /dev/null
./simple_test.sh
{ echo "A very simple test, with short delay"; } 2> /dev/null
./short_delay_test.sh
{ echo "A very simple test, with short jitter"; } 2> /dev/null
./short_jitter_test.sh
{ echo "A very simple test, with small loss"; } 2> /dev/null
./small_loss_test.sh
{ echo "A very simple test, with small cut factor"; } 2> /dev/null
./small_cut_test.sh
{ echo "A very simple test, with small corruption"; } 2> /dev/null
./small_error_test.sh
{ echo "A very simple test, with short delay and small loss"; } 2> /dev/null
./short_delay_small_loss_test.sh
{ echo "A very simple test with binary file"; } 2> /dev/null
./simple_binary_test.sh
{ echo "A very simple test with binary file, with short delay"; } 2> /dev/null
./short_delay_binary_test.sh
{ echo "A very simple test with binary file, with short jitter"; } 2> /dev/null
./short_jitter_binary_test.sh
{ echo "A very simple test with binary file, with small loss"; } 2> /dev/null
./small_loss_binary_test.sh
{ echo "A very simple test with binary file, with small cut factor"; } 2> /dev/null
./small_cut_binary_test.sh
{ echo "A very simple test with binary file, with small corruption"; } 2> /dev/null
./small_error_binary_test.sh
{ echo "A very simple test with binary file, with short delay and small loss"; } 2> /dev/null
./short_delay_small_loss_binary_test.sh
{ echo "A very simple test, with all sort of little imperfection"; } 2> /dev/null
./short_complete_test.sh
{ echo "A very simple test with binary file, with all sort of little imperfection"; } 2> /dev/null
./short_complete_binary_test.sh
#{ echo "A very simple image test, with short delay and small loss"; } 2> /dev/null
#./transfert_images_test.sh
# Run the same test, but this time with valgrind
# { echo "A very simple test, with Valgrind"; } 2> /dev/null
#VALGRIND=1 ./simple_test.sh
