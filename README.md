
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

Thanks to: the [gcov-fortran-example](https://github.com/sebastianbeyer/gcov-fortran-example) repo of [sebastianbeyer](https://github.com/sebastianbeyer)

On linux-based OS (probably OSX too), ``gcov`` can be used to get a report of the code coverage of the tests.  Combined with the tests in this repository, comprehensive tests can be written that exercise the full code of REFPROP.  To enable:

1. Build the shared library with [REFPROP-cmake](https://github.com/usnistgov/REFPROP-cmake) with GCOV support.  A call like this should do it:

```
(from root of REFPROP-cmake repo)
mkdir gcov && cd gcov
cmake .. -DREFPROP_GCOV=ON -DREFPROP_FORTRAN_PATH=/path/to/REFPROP/FORTRAN -DCMAKE_BUILD_TYPE=Debug
cmake --build .
```

2. Run the exe and use gcov to generate the HTML documentation; zip it all up
```
(from gcov folder of REFPROP-cmake)
RPPREFIX=`pwd` /path/to/test/exe 
gcov CMakeFiles/refprop.dir/media/R/FORTRAN/*.gcno   (path will need to be updated)
lcov --gcov-tool `which gcov` --capture --directory . --output-file coverage.info && genhtml --output-directory html coverage.info
zip -r html.zip html
```

3. Open the index.html that was generated in the ``html`` folder to check out the code coverage

## Or running gcov tests as a Dockerfile

Notes:
1. You need to make an output path called ``output`` in your working directory, drop the Dockerfile contents from below into a file called ``Dockerfile``.
2. The ``REFPROP.zip`` file should contain the folders ``FLUIDS``, ``FORTRAN``, and ``MIXTURES`` inside a ``REFPROP`` main folder.
3. Upon successful run, a zip file called html.zip will be generated in the output folder

```
##
## Build me as: docker image build -t gcov . 
## Run me as: docker container run -it --mount type=bind,src="$(pwd)"/output,dst=/output gcov
##

FROM ubuntu:17.10

RUN apt-get -y -m update && apt-get install -y cmake python3-numpy g++ gfortran git zip python3-six lcov

# This ADD block forces a build (invalidates the cache) if the git repo contents have changed, otherwise leaves it untouched.
# See https://stackoverflow.com/a/39278224
ADD https://api.github.com/repos/usnistgov/REFPROP-cmake/git/refs/heads/master RPcmake-version.json
ADD https://api.github.com/repos/ianhbell/REFPROP-tests/git/refs/heads/master RPtests-version.json

RUN git clone --recursive https://github.com/usnistgov/REFPROP-cmake && \
    git clone --recursive https://github.com/ianhbell/REFPROP-tests 

ADD REFPROP.zip . 

RUN unzip REFPROP.zip -d REFPROP

# Build the special shared library & copy fluid files
WORKDIR /REFPROP-cmake
RUN mkdir build && cd build && cmake .. -DREFPROP_FORTRAN_PATH=/REFPROP/FORTRAN -DCMAKE_BUILD_TYPE=Debug -DREFPROP_GCOV=ON && cmake --build .
WORKDIR /REFPROP-cmake/build
RUN cp -rfv /REFPROP/FLUIDS/ . && cp -rfv /REFPROP/MIXTURES/ . && echo `pwd` && echo `ls -l`

# Build the testing EXE
WORKDIR /REFPROP-tests
RUN mkdir build && cd build && cmake .. -DCMAKE_BUILD_TYPE=Release && cmake --build .

# Set the environment variable
ENV RPPREFIX /REFPROP-cmake/build
 
# Run the exe, generating gcov output, running from the REFPROP-cmake build
WORKDIR /REFPROP-cmake/build
CMD (/REFPROP-tests/build/main ~[Torture] || echo "Tests done") && \
    gcov CMakeFiles/refprop.dir/REFPROP/FORTRAN/*.gcno && \
    lcov --gcov-tool `which gcov` --capture --directory . --output-file coverage.info && \
    genhtml --output-directory html coverage.info && \
    zip -r /output/html.zip html
```