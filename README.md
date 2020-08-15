# qtbase

I'm working on this repo for my GSoC 2020 project. My commits can be found in the branch [cpdb](https://github.com/dryairship/qtbase/tree/cpdb).

# How to build

## CPDB enabled
Inside the shadow build directory, execute the following commands:
```
/path/to/qt5/configure -developer-build -opensource -confirm-license -nomake examples -nomake tests
make module-qtbase
```

## CPDB disabled
Inside the shadow build directory, execute the following commands:
```
/path/to/qt5/configure -developer-build -opensource -confirm-license -nomake examples -nomake tests -no-cpdb
make module-qtbase
```
<hr>

For faster builds, you can let `make` compile multiple files simultaneously by using the `-j` flag:
```
make -j$(nproc) module-qtbase
```
