#' @useDynLib objectable,.registration = TRUE

object_table <- function(
                         get = NULL,
                         set = NULL,
                         has = NULL,
                         unbind = NULL,
                         names = NULL,
                         attach = NULL,
                         detach = NULL,
                         parent_env = parent.frame()
                         ) {

  stopifnot(is.environment(parent_env))

  object_table <- list(
    get = get,
    set = set,
    has = has,
    unbind = unbind,
    names = names,
    attach = attach,
    detach = detach,
    parent_env = parent_env
  )

  .Call(new_object_table, object_table)
}

