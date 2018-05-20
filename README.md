
# Running valgrind tests

In order to look for uninitialized variables caused by the use of an implicitly defined variable that is subsequently used, the ``memcheck`` program from the ``valgrind`` suite can be used.  Here is a snippet that runs each tag that can be found in the Catch exe, one at a time, in order to try to identify the presence of any memory-related bugs. 

Note: Make sure you build REFPROP with debug symbols.  A ``RelWithDebInfo`` CMake build target also works perfectly.

``` python
import subprocess, os

# Find all the tags by asking the exe for all the tags that it knows about
output = subprocess.run('build/main -t', shell = True, stdout = subprocess.PIPE).stdout.decode('utf-8')

# Set the path to the REFPROP shared library
os.environ['RPPREFIX'] = '/path/to/REFPROP-cmake/build/'

# Loop over the lines that were found (tags)
for il, line in enumerate(output.split('\n')[1::]):

    if not line or '[' not in line: continue
    tag = '[' + line.split('[')[1]
    # if 'SETMOD' not in tag: continue
    root =  tag.replace('[', '').replace(']','') + '.txt'
    print(tag, ' --> ', root)
    # The call to valgrind
    cmd = 'valgrind --tool=memcheck --error-limit=no --track-origins=yes build/main ' + tag
    # Set up the files for the output of stderr (from valgrind) and stdout (from Catch)
    with open('log_'+root,'w') as fp_stderr:
        with open('err_'+root,'w') as fp_stdout:
            # Run the check
            subprocess.run(cmd, shell = True, stdout = fp_stdout, stderr = fp_stderr)

# Print the last line from each test
subprocess.run('for f in log_*.txt; do echo $f && tail -n 1 $f; done', shell=True)
```

# Running gcov

On linux-based OS (probably OSX too), ``gcov`` can be used to get a report of the code coverage of the tests.  Combined with the tests in this repository, comprehensive tests can be written that exercise the full code of REFPROP.  To enable:

1. Build the shared library with [REFPROP-cmake](https://github.com/usnistgov/REFPROP-cmake) with GCOV support.  A call like this should do it:

```
(from root of REFPROP-cmake repo)
mkdir build && cd build
cmake .. -DREFPROP_GCOV=ON -DREFPROP_FORTRAN_PATH=/path/to/REFPROP/FORTRAN -DCMAKE_BUILD_TYPE=Debug
cmake --build .
```

2. Run the exe and use gcov to generate the HTML documentation; zip it all up
```
(from build folder of REFPROP-cmake)
RPPREFIX=`pwd` /path/to/test/exe 
gcov CMakeFiles/refprop.dir/media/R/FORTRAN/*.gcno   (path will need to be updated)
lcov --gcov-tool `which gcov` --capture --directory . --output-file coverage.info && genhtml --output-directory html coverage.info
zip -r html.zip html
```

3. Open the index.html that was generated in the ``html`` folder to check out the code coverage