context("object_table")

test_that("missing functions pass through to underlying env", {
  env <- new.env(parent = emptyenv())
  env$a <- 10

  ot <- object_table(parent_env = env)
  expect_equal(ls(envir = ot), "a")
  expect_true(exists("a", envir = ot))
  expect_equal(ot$a, 10)

  expect_false(exists("b", envir = ot))
  ot$b <- 20
  expect_true(exists("b", envir = ot))
  rm(list = "b", envir = ot)
  expect_false(exists("b", envir = ot))
})

test_that("proxied get uses inheritance", {
  env1 <- new.env(parent = emptyenv())
  env1$a <- 10

  env2 <- new.env(parent = env1)
  ot <- object_table(parent_env = env2)

  expect_equal(get("a", envir = ot), 10)
})

test_that("can override get", {
  ot <- object_table(get = function(name) nchar(name))
  expect_equal(ot$a, 1)
  expect_equal(ot$abcd, 4)
})

test_that("can override set", {
  val <- NULL
  ot <- object_table(set = function(name, value) val <<- value)

  ot$x <- 1234
  expect_equal(val, 1234)
  expect_false(exists("x", envir = ot))
})

test_that("can override names", {
  ot <- object_table(names = function() letters)
  expect_equal(ls(envir = ot), letters)
})

test_that("can overide has", {
  ot <- object_table(has = function(name) TRUE)
  expect_true(exists("qwertyuiop", envir = ot))
})

test_that("errors propagate", {
  ot <- object_table(get = function(name) {
    if (name == "a") {
      1
    } else {
      stop("!")
    }
  })

  expect_equal(ot$a, 1)
  expect_error(ot$b, "!")
})
