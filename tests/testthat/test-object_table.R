context("object_table")

test_that("missing functions pass through to underlying env", {
  env <- new.env(parent = emptyenv())
  env$a <- 10

  ot <- object_table(parent_env = env)
  expect_equal(ls(envir = ot), "a")
  expect_true(exists("a", envir = ot))
  expect_equal(ot$a, 10)

  expect_false(exists("b", envir = ot))
  env$b <- 20
  expect_true(exists("b", envir = ot))
  rm(list = "b", envir = ot)
  expect_false(exists("b", envir = ot))
})
