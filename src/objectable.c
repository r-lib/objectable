#include <R.h>
#include <Rinternals.h>
#include <stdlib.h> // for NULL
#include <R_ext/Rdynload.h>

/* .Call calls */
extern SEXP new_object_table(SEXP);

static const R_CallMethodDef CallEntries[] = {
    {"new_object_table", (DL_FUNC) &new_object_table, 1},
    {NULL, NULL, 0}
};

void R_init_objectable(DllInfo *dll)
{
    R_registerRoutines(dll, NULL, CallEntries, NULL, NULL);
    R_useDynamicSymbols(dll, FALSE);
}
