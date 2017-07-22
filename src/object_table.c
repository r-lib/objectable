#define R_NO_REMAP

#include <R.h>
#include <Rinternals.h>
#include <R_ext/Callbacks.h>
#include <stdbool.h>

typedef enum {
  OTL_GET,
  OTL_SET,
  OTL_HAS,
  OTL_UNBIND,
  OTL_NAMES,
  OTL_ATTACH,
  OTL_DETACH,
  OTL_PARENT_ENV
} Component;

SEXP ot_fun_get(R_ObjectTable *ot, Component action) {
  SEXP obj = (SEXP) ot->privateData;
  return VECTOR_ELT(obj, (int) action);
}

bool ot_fun_has(R_ObjectTable *ot, Component action) {
  return !Rf_isNull(ot_fun_get(ot, action));
}

bool as_flag(SEXP x, const char* name) {
  if (TYPEOF(x) != LGLSXP || Rf_length(x) != 1) {
    Rf_errorcall(
      R_NilValue,
      "%s must return logical vector of length 1",
      name
    );
  }
  return LOGICAL(x)[0];
}

SEXP ot_fun_call_empty(R_ObjectTable *ot, Component action) {
  SEXP fun = ot_fun_get(ot, action);

  SEXP fun_call = PROTECT(Rf_lang1(fun));
  SEXP val = Rf_eval(fun_call, R_GlobalEnv);

  UNPROTECT(1);
  return val;
}


SEXP ot_fun_call_name(R_ObjectTable *ot, Component action, const char* const name) {
  SEXP fun = ot_fun_get(ot, action);

  SEXP name_ = PROTECT(Rf_allocVector(STRSXP, 1));
  SET_STRING_ELT(name_, 0, Rf_mkCharCE(name, CE_UTF8));

  SEXP fun_call = PROTECT(Rf_lang2(fun, name_));
  SEXP val = Rf_eval(fun_call, R_GlobalEnv);

  UNPROTECT(2);
  return val;
}

SEXP ot_parent(R_ObjectTable *ot) {
  return VECTOR_ELT(ot->privateData, (int) OTL_PARENT_ENV);
};

// Returns an environment without frame or hashtab
SEXP bare_environment(SEXP parent) {
  SEXP env = Rf_cons(R_NilValue, parent);
  SET_TYPEOF(env, ENVSXP);
  return env;
}

// Wrapper R functions in C functions ------------------------------------------

SEXP ot_names(R_ObjectTable *ot) {
  if (!ot_fun_has(ot, OTL_NAMES)) {
    return R_lsInternal(ot_parent(ot), true);
  } else {
    return ot_fun_call_empty(ot, OTL_NAMES);
  }
}

void ot_attach(R_ObjectTable *ot) {
  if (ot_fun_has(ot, OTL_ATTACH)) {
    ot_fun_call_empty(ot, OTL_ATTACH);
  }
}

void ot_detach(R_ObjectTable *ot) {
  if (ot_fun_has(ot, OTL_DETACH)) {
    ot_fun_call_empty(ot, OTL_DETACH);
  }
}

Rboolean ot_has(const char* const name, Rboolean *canCache, R_ObjectTable *ot) {
  if (!ot_fun_has(ot, OTL_HAS)) {
    SEXP val = Rf_findVarInFrame3(ot_parent(ot), Rf_install(name), false);
    return val != R_UnboundValue;
  } else {
    SEXP val = ot_fun_call_name(ot, OTL_HAS, name);
    return as_flag(val, "has()");
  }
}

SEXP ot_get(const char* const name, Rboolean *canCache, R_ObjectTable *ot) {
  if (!ot_fun_has(ot, OTL_GET)) {
    return Rf_findVar(Rf_install(name), ot_parent(ot));
  } else {
    return ot_fun_call_name(ot, OTL_GET, name);
  }
}

int ot_unbind(const char* const name, R_ObjectTable *ot) {
  if (!ot_fun_has(ot, OTL_UNBIND)) {
    Rf_defineVar(Rf_install(name), R_UnboundValue, ot_parent(ot));
    return true;
  } else {
    SEXP val = ot_fun_call_name(ot, OTL_UNBIND, name);
    return as_flag(val, "unbind()");
  }
}

SEXP ot_set(const char* const name, SEXP value, R_ObjectTable *ot) {
  if (!ot_fun_has(ot, OTL_SET)) {
    Rf_defineVar(Rf_install(name), value, ot_parent(ot));
    return value;
  }

  SEXP fun = ot_fun_get(ot, OTL_SET);

  SEXP name_ = PROTECT(Rf_allocVector(STRSXP, 1));
  SET_STRING_ELT(name_, 0, Rf_mkCharCE(name, CE_UTF8));

  SEXP fun_call = PROTECT(Rf_lang3(fun, name_, value));
  Rf_eval(fun_call, R_GlobalEnv);

  UNPROTECT(2);
  return value;
}

// Create object table -------------------------------------------------

SEXP new_object_table(SEXP x) {
  if (!Rf_isNewList(x) || Rf_length(x) != 8) {
    Rf_errorcall(R_NilValue, "Invalid input");
  }

  R_ObjectTable* ot = (R_ObjectTable*) malloc(sizeof(R_ObjectTable));
  ot->exists =   &ot_has;
  ot->get =      &ot_get;
  ot->remove =   &ot_unbind;
  ot->assign =   &ot_set;
  ot->objects =  &ot_names;
  ot->onAttach = &ot_attach;
  ot->onDetach = &ot_detach;

  ot->canCache = false;
  ot->active = true;
  ot->privateData = x;

  SEXP env_ptr = PROTECT(R_MakeExternalPtr(
    ot,
    Rf_install("UserDefinedDatabase"),
    x
  ));

  SEXP env = PROTECT(bare_environment(R_EmptyEnv));
  SET_HASHTAB(env, env_ptr);

  SEXP classes = PROTECT(Rf_allocVector(STRSXP, 2));
  SET_STRING_ELT(classes, 0, Rf_mkChar("hook_env"));
  SET_STRING_ELT(classes, 1, Rf_mkChar("UserDefinedDatabase"));
  Rf_setAttrib(env, Rf_install("class"), classes);

  UNPROTECT(3);
  return env;
}
