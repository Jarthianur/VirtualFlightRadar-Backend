# VFR-B Test Reports

This directory contains a C++11 unit-test framework as git submodule. In the reports folder, a test report and a HTML coverage report can be found. To view the coverage report open up the index.html in any browser.

## Build with coverage

Compile with flags `-fprofile-arcs -ftest-coverage`.
Link with `-coverage`.
Run program.

```bash
lcov --directory Debug/ --capture --output-file test/reports/all.info
cd test/reports
lcov -e all.info "$VFRB_ROOT/src/*" -o test/reports/vfrb.info
genhtml vfrb.info -o coverage/
```