# CMake generated Testfile for 
# Source directory: /home/konrad/Desktop/SO/cw02/task1
# Build directory: /home/konrad/Desktop/SO/cw02/task1
# 
# This file includes the relevant testing commands required for 
# testing this directory and lists subdirectories to be tested as well.
add_test(testSystem "sh" "-c" "rm -f /home/konrad/Desktop/SO/cw02/task1/tests/systemResults.txt           && /home/konrad/Desktop/SO/cw02/task1/bin/system 10 12                       generate /home/konrad/Desktop/SO/cw02/task1/tests/testRandomFileSystem                       copy /home/konrad/Desktop/SO/cw02/task1/tests/testRandomFileSystem /home/konrad/Desktop/SO/cw02/task1/tests/testRandomFileSystem2                       sort /home/konrad/Desktop/SO/cw02/task1/tests/testRandomFileSystem2           >> /home/konrad/Desktop/SO/cw02/task1/tests/systemResults.txt")
add_test(testLib "sh" "-c" "rm -f /home/konrad/Desktop/SO/cw02/task1/tests/libResults.txt           && /home/konrad/Desktop/SO/cw02/task1/bin/lib 10 12                       generate /home/konrad/Desktop/SO/cw02/task1/tests/testRandomFileLib                       copy /home/konrad/Desktop/SO/cw02/task1/tests/testRandomFileLib /home/konrad/Desktop/SO/cw02/task1/tests/testRandomFileLib2                       sort /home/konrad/Desktop/SO/cw02/task1/tests/testRandomFileLib2           >> /home/konrad/Desktop/SO/cw02/task1/tests/libResults.txt")
