DigitalSignalRenderer

A simple Qt application that loads and renders digital signal data from a text file.

Build Instructions

1. Create and enter a build directory:

mkdir build
cd build

2. Generate build files with CMake:

cmake ..

3. Compile the project:

make

---

Running the Application

From the build directory, run:

./SignalRenderer

Ensure the input file data.txt is located in the directory from which you run the executable, or adjust the path in the source code accordingly.

---

Running Unit Tests

Google Test is integrated for unit testing.

From the build directory, run:

./SignalRendererTests

This will execute all available tests and print the results.

Tests cover:

- Widget construction and destruction
- File parsing (valid and invalid files)
- Repainting stability
- Zoom event handling using Ctrl + scroll

Tests are executed manually by running the test binary directly.

(Optional) To integrate tests with CTest, uncomment the following lines in CMakeLists.txt:

include(GoogleTest)
gtest_discover_tests(SignalRendererTests)

Then, from the build directory, run:

ctest

This will execute the tests automatically through CTest.

---

Input File Format

The input file should follow this format:

TIMESTEP;100;
UNITS;ns;
...
DATA;
1;
1;
0;
...

- TIMESTEP specifies the time interval between signal points.
- UNITS specifies the time unit used for TIMESTEP (e.g., ns = nanoseconds).
- DATA; marks the start of signal values.
- Each signal value line contains either 0; or 1;.