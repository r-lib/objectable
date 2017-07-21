
<!-- README.md is generated from README.Rmd. Please edit that file -->
objectable
==========

[![Travis-CI Build Status](https://travis-ci.org/r-lib/objectable.svg?branch=master)](https://travis-ci.org/r-lib/objectable) [![codecov](https://codecov.io/gh/r-lib/objectable/branch/master/graph/badge.svg)](https://codecov.io/gh/r-lib/objectable)

The goal of objectable is to make it easy to experiment with object tables using only R code. Object tables allow you to make "custom" environments where you can control the behaviour of `get()`, `assign()`, `rm()` and `ls()` using your own functions.

Installation
------------

You can install objectable from github with:

``` r
# install.packages("devtools")
devtools::install_github("r-lib/objectable")
```

Example
-------

``` r
library(objectable)

# Make a super weird enviornment
ot <- object_table(
  get = function(name) runif(nchar(name)),
  has = function(name) TRUE
)
ot$a
#> [1] 0.08075014
ot$a
#> [1] 0.834333
ot$abc
#> [1] 0.600760886 0.157208442 0.007399441
```

``` r
# Change behaviour of missing symbols
env <- new.env()
ot <- object_table(
  get = function(name) {
    if (!exists(name, envir = env)) {
      stop("`", name, "` does not exist", call. = FALSE)
    } else {
      env[[name]]
    }
  },
  parent_env = env
)

ot$a <- 10
ot$a
#> [1] 10
ot$b
#> Error: `b` does not exist
```
