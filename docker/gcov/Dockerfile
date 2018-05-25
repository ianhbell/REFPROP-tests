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
