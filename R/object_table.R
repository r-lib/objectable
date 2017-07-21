#' @useDynLib objectable,.registration = TRUE
NULL

#' Create an object table
#'
#' An object table is a custom environment that allows you to redefine the
#' usual behaviour of R's environments.
#'
#' @param get `function(name) {}`. Can return any type of object. Affects
#'   the operation of `$`, `[[`, `get()`.
#' @param set `function(name, value) {}`. Should return logical indicating
#'    success or failure. Affects the operation of `$<-`, `[[<-` and `assign`.
#' @param has `function(name) {}`. Should return logical. Affects the
#'   operation of [exists()].
#' @param names `function() {}`. Should return character vector.
#' @param unbind `function(name) {}`. Should return logical indicating success
#'    or failure. Affects the operation of [rm()].
#' @param parent_env This environment is used to implement default operations
#'   for all other arguents.
#' @export
#' @examples
#' ot <- object_table(get = function(name) nchar(name))
#' ot$a
#' ot$this_is_a_long_name
object_table <- function(
                         get = NULL,
                         set = NULL,
                         has = NULL,
                         unbind = NULL,
                         names = NULL,
                         attach = NULL,
                         detach = NULL,
                         parent_env = emptyenv()
                         ) {

  stopifnot(is.environment(parent_env))

  object_table <- list(
    get = get,
    set = set,
    has = has,
    unbind = unbind,
    names = names,
    attach = NULL,
    detach = NULL,
    parent_env = parent_env
  )

  .Call(new_object_table, object_table)
}

