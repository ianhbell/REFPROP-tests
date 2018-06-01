
# About

Tests written by Ian Bell, NIST, 2018, with the [Catch](https://github.com/catchorg/Catch2) testing framework.  This framework allows for only a subset of the tests to be run, for instance you can list the tags that are found:

``` bash
>main -t
All available tags:
   1  [100comps]
   1  [100Reloads]
   2  [911]
   1  [acentric]
   2  [AGA8]
   1  [AGA8stayon]
   2  [alphar]
   2  [ancillary]
   2  [BIP]
   1  [CAS]
   2  [crit]
   1  [D2O]
   2  [enum]
   1  [EOS]
   2  [ETA]
   1  [Excel]
   1  [FDIR]
   3  [file_loading]
   3  [flags]
   8  [flash]
   1  [GERG2008]
   3  [GERGy]
   1  [GPA]
   1  [hUnits]
   1  [InChI]
   1  [MLT]
   1  [nbp]
   1  [NH3]
   1  [oldfiles]
   1  [OneN2]
   2  [PR]
   2  [predef_mixes]
   1  [props]
   2  [PX0]
   1  [PX0mix]
   1  [R134AMM]
   1  [REDX]
   5  [SETMOD]
  15  [setup]
   1  [Stateless]
   1  [STP]
   1  [sublimation]
   2  [surten]
   1  [TCX]
   1  [TholLNG]
   1  [Torture]
   3  [transport]
   1  [units]
   1  [water]
49 tags

```
and then run only a subset of them:

```
main [REDX] [TCX]
```
for instance.  More information can be obtained by running ``main -h`` at the command line.

# License

The tests in this repository are all public domain, while NIST REFPROP is not open source.

# Running tests

All the test runners are contained in docker containers. While it is possible to run the tests standalone without invoking the docker and docker-compose tools, it is not recommended.

Running the tests requires:
* ``docker``
* ``docker-compose``

Zip up the ``FLUIDS``, ``FORTRAN``, and ``MIXTURES`` folders into a zip file called ``REFPROP.zip``, maybe with something like:
``` bash
cp -rv /media/R/FLUIDS/ .
cp -rv /media/R/FORTRAN/ .
cp -rv /media/R/MIXTURES/ .
zip -r REFPROP.zip FLUIDS FORTRAN MIXTURES
rm -rf FLUIDS FORTRAN MIXTURES
```

Then copy the ``REFPROP.zip`` file into the folder of the test you want to run (e.g. ``docker/gcov`` for the ``gcov`` coverage tests).  At the command line, first make sure you have the ``REFPROP.zip`` file, then away we go:
``` bash
ls REFPROP.zip
docker-compose up
```

It's the same procedure to run the normal Catch tests (``docker/test``) or to run the valgrind memory checks for each tag (``docker/valgrind``).