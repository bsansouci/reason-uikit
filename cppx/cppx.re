/* ppx_test.re */
open Asttypes;

open Parsetree;

open Ast_mapper;

let sp = Printf.sprintf;

let (+/) = Filename.concat;

let log = str => {
  let oc =
    open_out_gen([Open_creat, Open_text, Open_append], 0o640, "logs.txt");
  output_string(oc, str ++ "\n");
  close_out(oc);
};

let printLineNumber = ({Location.loc_start: {Lexing.pos_fname, pos_lnum}}) =>
  sp("#line %d \"%s\"\n", pos_lnum, "src" +/ Filename.basename(pos_fname));

let oc = ref(None);

let appendToFile = (loc, str) => {
  let filename = loc.Location.loc_start.Lexing.pos_fname;
  let bindingsFileName =
    "bindings"
    +/ Filename.chop_extension(Filename.basename(filename))
    ++ "_bindings.m";
  /*let shouldAddHeader = ! Sys.file_exists(bindingsFileName);*/
  let oc =
    switch oc^ {
    | None =>
      let newOc =
        open_out_gen(
          [Open_creat, Open_text, Open_trunc, Open_wronly],
          0o640,
          bindingsFileName
        );
      let header =
        sp("#line %d \"%s\"\n", 0, bindingsFileName)
        ++ {|
        #include <caml/alloc.h>
        #include <caml/custom.h>
        #include <caml/memory.h>
        #include <caml/mlvalues.h>
        #include <caml/callback.h>

        #include <Foundation/Foundation.h>
        #include <UIKit/UIkit.h>
        #import <stdint.h>

        #define Val_none Val_int(0)

        static value Val_some(value v) {
          CAMLparam1(v);
          CAMLlocal1(some);
          some = caml_alloc_small(1, 0);
          Field(some, 0) = v;
          CAMLreturn(some);
        }

        CAMLprim value isNil(value objc) {
          CAMLparam1(objc);
          CAMLreturn(Int_val((id)(objc) == nil));
        }
        |};
      output_string(newOc, header);
      oc := Some(newOc);
      newOc;
    | Some(oc) => oc
    };
  output_string(oc, printLineNumber(loc) ++ str);
};

let splitFirst = (str, c) => {
  let j = ref(-1);
  for (i in 0 to String.length(str) - 1) {
    if (j^ == (-1) && str.[i] == c) {
      j := i;
    };
  };
  if (j^ == (-1)) {
    (str, "");
  } else {
    (String.sub(str, 0, j^), String.sub(str, j^, String.length(str) - j^));
  };
};

let processModule = moduleDesc => {
  let methodsToAdd = ref([]);
  let functionsToAdd = ref([]);
  let processStructures = (moduleName, loc, superclassName, listOfStructures) => {
    let structures =
      List.map(
        s =>
          switch s {
          | {
              pstr_loc,
              pstr_desc:
                Pstr_value(
                  Nonrecursive,
                  [
                    {
                      pvb_pat: {
                        ppat_desc: Ppat_var({txt: pvalNameString, loc})
                      },
                      pvb_attributes: [({txt: "c.method"}, _)],
                      pvb_expr: actualFunc
                    } as valDesc
                  ]
                )
            } =>
            let (arg1Type, arg2Name, arg2Type) =
              switch actualFunc {
              | {
                  pexp_desc:
                    Pexp_fun(
                      "",
                      None,
                      {ppat_desc: Ppat_var({txt: "view"})},
                      {
                        pexp_desc:
                          Pexp_fun(
                            "",
                            None,
                            {
                              ppat_desc:
                                Ppat_constraint(
                                  {ppat_desc: Ppat_var({txt: firstArgName})},
                                  {
                                    ptyp_desc:
                                      Ptyp_constr(
                                        {
                                          txt:
                                            Longident.Ldot(
                                              Longident.Lident(firstArgType),
                                              "t"
                                            )
                                        },
                                        _
                                      )
                                  }
                                )
                            },
                            secondExpr
                          )}

                                          )
                } =>
                let (secondArgName, secondArgType) =
                  switch secondExpr {
                  | {
                      pexp_desc:
                        Pexp_fun(
                          "",
                          None,
                          {
                            ppat_desc:
                              Ppat_constraint(
                                {ppat_desc: Ppat_var({txt: secondArgName})},
                                {
                                  ptyp_desc:
                                    Ptyp_constr(
                                      {
                                        txt:
                                          Longident.Ldot(
                                            Longident.Lident(secondArgType),
                                            "t"
                                          )
                                      },
                                      _
                                    )
                                }
                              )
                          },
                          _
                        )
                    } => (
                      Some(secondArgName),
                      Some(secondArgType)
                    )
                  | _ => (None, None)
                  };
                (Some(firstArgType), secondArgName, secondArgType);
              | _ => (None, None, None)
              };
            let registeredName = moduleName ++ "_" ++ pvalNameString;
            /* caml_register_global_root / caml_remove_global_root */
            /* For now we only support 1 arg method overriding */
            let newMethod =
              switch (arg1Type, arg2Name, arg2Type) {
              | (Some(arg1Type), Some(arg2Name), Some(arg2Type)) =>
                sp(
                  {|
                  - (void)%s:(%s *)arg1 %s:(%s *)arg2 {
                    static value * closure_f = NULL;
                    if (closure_f == NULL) {
                        /* First time around, look up by name */
                        closure_f = caml_named_value("%s");
                    }
                    CAMLlocal3(view, arg1_wrap, arg2_wrap);
                    view = caml_alloc_small(1, Abstract_tag);
                    Field(view, 0) = (value)self;

                    arg1_wrap = caml_alloc_small(1, Abstract_tag);
                    Field(arg1_wrap, 0) = (value)arg1;

                    arg2_wrap = caml_alloc_small(1, Abstract_tag);
                    Field(arg2_wrap, 0) = (value)arg2;
                    caml_callback3(*closure_f, view, arg1_wrap, arg2_wrap);
                  }
                |},
                  pvalNameString,
                  arg1Type,
                  arg2Name,
                  arg2Type,
                  registeredName
                )
              | (Some(arg1Type), None, None) =>
                sp(
                  {|
                  - (void)%s:(%s *)arg1 {
                    static value * closure_f = NULL;
                    if (closure_f == NULL) {
                        /* First time around, look up by name */
                        closure_f = caml_named_value("%s");
                    }
                    CAMLlocal2(view, arg1_wrap);
                    view = caml_alloc_small(1, Abstract_tag);
                    Field(view, 0) = (value)self;

                    arg1_wrap = caml_alloc_small(1, Abstract_tag);
                    Field(arg1_wrap, 0) = (value)arg1;
                    caml_callback2(*closure_f, view, arg1_wrap);
                  }
                |},
                  pvalNameString,
                  arg1Type,
                  registeredName
                )
              | (None, None, None) =>
                sp(
                  {|
                  - (void)%s {
                    static value * closure_f = NULL;
                    if (closure_f == NULL) {
                        /* First time around, look up by name */
                        closure_f = caml_named_value("%s");
                    }
                    CAMLlocal1(view);
                    view = caml_alloc_small(1, Abstract_tag);
                    Field(view, 0) = (value)self;
                    caml_callback(*closure_f, view);
                  }
                |},
                  pvalNameString,
                  registeredName
                )
              | _ => assert false
              };
            methodsToAdd := [(pstr_loc, newMethod), ...methodsToAdd^];
            Ast_helper.(
              Str.value(
                Nonrecursive,
                [
                  {
                    ...valDesc,
                    pvb_expr:
                      Exp.let_(
                        Nonrecursive,
                        [Vb.mk(Pat.var({txt: "ret", loc}), actualFunc)],
                        Exp.sequence(
                          Exp.apply(
                            Exp.ident({
                              txt:
                                Longident.(
                                  Ldot(Lident("Callback"), "register")
                                ),
                              loc
                            }),
                            [
                              (
                                "",
                                Exp.constant(
                                  Const_string(registeredName, None)
                                )
                              ),
                              (
                                "",
                                Exp.ident({txt: Longident.Lident("ret"), loc})
                              )
                            ]
                          ),
                          Exp.ident({txt: Longident.Lident("ret"), loc})
                        )
                      )
                  }
                ]
              )
            );
          | {
              pstr_loc,
              pstr_desc:
                Pstr_primitive(
                  {
                    pval_name: {txt: pvalNameString},
                    pval_attributes: [({txt: "c.new"}, _)],
                    pval_prim,
                    pval_type: {
                      ptyp_desc:
                        Ptyp_arrow(
                          _,
                          {ptyp_desc: Ptyp_constr({txt: inputType}, [])},
                          _
                        )
                    }
                  } as valDesc
                )
            } =>
            let (fullFunctionName, newFunction) =
              switch pvalNameString {
              | "_new" => (
                  sp("%s_new", moduleName),
                  sp(
                    {|
                    CAMLprim value %s_new() {
                      CAMLparam0();
                      CAMLlocal1(ret);

                      ret = caml_alloc_small(1, Abstract_tag);
                      Field(ret, 0) = (value)[[%s alloc] init];
                      /*Field(ret, 1) = Val_none;
                      Field(ret, 2) = Val_none;
                      Field(ret, 3) = Val_none;
                      Field(ret, 4) = Val_none;*/

                      CAMLreturn(ret);
                    }
                  |},
                    moduleName,
                    moduleName
                  )
                )
              | _ =>
                log(
                  "pvalNameString big enough: "
                  ++ string_of_bool(String.length(pvalNameString) > 3)
                );
                let truncatedName =
                  String.sub(
                    pvalNameString,
                    3,
                    String.length(pvalNameString) - 3
                  );
                let typeCast = typeName => sp("(%s *)arg", typeName);
                let inputConvertion =
                  Longident.(
                    switch inputType {
                    | Lident("string") => "String_val(arg)"
                    | Lident("float") => "Double_val(arg)"
                    | Lident("int") => "Int_val(arg)"
                    | Lident("bool") => "Bool_val(arg)"
                    | Lident(inputType) =>
                      log("it was a long ident: " ++ inputType);
                      typeCast(inputType);
                    | Ldot(Lident(inputType), "t") =>
                      log("it was an Ldot : " ++ inputType);
                      typeCast(inputType);
                    | _ =>
                      log("waht the heck");
                      "**BUG-HERE**";
                    }
                  );
                let fullFunctionName = sp("%s_%s", moduleName, pvalNameString);
                (
                  fullFunctionName,
                  sp(
                    {|
                    CAMLprim value %s(value arg) {
                      CAMLparam1(arg);
                      CAMLlocal1(ret);

                      ret = caml_alloc_small(1, Abstract_tag);
                      Field(ret, 0) = (value)[[%s alloc] init%s:%s];
                      /*Field(ret, 1) = Val_none;
                      Field(ret, 2) = Val_none;
                      Field(ret, 3) = Val_none;
                      Field(ret, 4) = Val_none;*/

                      CAMLreturn(ret);
                    }
                  |},
                    fullFunctionName,
                    moduleName,
                    truncatedName,
                    inputConvertion
                  )
                );
              };
            log("new fullFunctionName: " ++ fullFunctionName);
            functionsToAdd := [(pstr_loc, newFunction), ...functionsToAdd^];
            /*appendToFile(pstr_loc, newFunction);*/
            Ast_helper.Str.primitive({
              ...valDesc,
              pval_prim: [fullFunctionName]
            });
          | {
              pstr_loc,
              pstr_desc:
                Pstr_primitive(
                  {
                    pval_name: {txt: pvalNameString},
                    pval_attributes: [({txt: "c.static"}, _)],
                    pval_prim,
                    pval_type: {
                      ptyp_desc:
                        Ptyp_arrow(
                          _,
                          {
                            ptyp_desc:
                              Ptyp_constr({txt: Longident.Lident("unit")}, [])
                          },
                          {
                            ptyp_desc:
                              Ptyp_constr({txt: Longident.Lident("t")}, [])
                          }
                        )
                    }
                  } as valDesc
                )
            } =>
            let fullFunctionName = sp("%s_%s", moduleName, pvalNameString);
            let newFunction =
              sp(
                {|
            CAMLprim value %s() {
              CAMLparam0();
              CAMLlocal1(ret);
              ret = caml_alloc_small(1, Abstract_tag);
              Field(ret, 0) = (value)[%s %s];
              CAMLreturn(ret);
            }
          |},
                fullFunctionName,
                moduleName,
                pvalNameString
              );
            log("static fullFunctionName: " ++ fullFunctionName);
            functionsToAdd := [(pstr_loc, newFunction), ...functionsToAdd^];
            /*appendToFile(pstr_loc, newFunction);*/
            Ast_helper.Str.primitive({
              ...valDesc,
              pval_prim: [fullFunctionName]
            });
          | _ => s
          },
        listOfStructures
      );
    /* Look over methodsToAdd */
    if (methodsToAdd^ != []) {
      /* TODO: figure out location */
      let maybeSuperclass =
        switch superclassName {
        | None => ""
        | Some(superclass) => " : " ++ superclass
        };
      appendToFile(loc, sp("@interface %s%s\n", moduleName, maybeSuperclass));
      appendToFile(loc, "@end\n");
      appendToFile(loc, sp("@implementation %s\n", moduleName));
      List.iter(((loc, str)) => appendToFile(loc, str), methodsToAdd^);
      appendToFile(loc, "@end\n");
    };
    methodsToAdd := [];
    List.iter(((loc, str)) => appendToFile(loc, str), functionsToAdd^);
    structures;
  };
  switch moduleDesc {
  | {
      pmb_attributes: [({txt: "c.class", loc}, PStr(potentialSubclassing))],
      pmb_expr:
        {
          pmod_desc:
            Pmod_constraint(
              {pmod_desc: Pmod_structure(listOfStructures)} as inner_pmod_desc,
              other
            )
        } as pmb_expr,
      pmb_name: {txt: moduleName}
    } =>
    let superclassName =
      switch potentialSubclassing {
      | [
          {
            pstr_desc:
              Pstr_eval(
                {pexp_desc: Pexp_constant(Const_string(superclassName, _))},
                _
              )
          }
        ] =>
        Some(superclassName)
      | _ => None
      };
    let newList =
      processStructures(moduleName, loc, superclassName, listOfStructures);
    {
      ...moduleDesc,
      pmb_expr: {
        ...pmb_expr,
        pmod_desc:
          Pmod_constraint(
            {...inner_pmod_desc, pmod_desc: Pmod_structure(newList)},
            other
          )
      }
    };
  | {
      pmb_attributes: [({txt: "c.class", loc}, PStr(potentialSubclassing))],
      pmb_expr: {pmod_desc: Pmod_structure(listOfStructures)} as pmb_expr,
      pmb_name: {txt: moduleName}
    } =>
    let superclassName =
      switch potentialSubclassing {
      | [
          {
            pstr_desc:
              Pstr_eval(
                {pexp_desc: Pexp_constant(Const_string(superclassName, _))},
                _
              )
          }
        ] =>
        Some(superclassName)
      | _ => None
      };
    let newList =
      processStructures(moduleName, loc, superclassName, listOfStructures);
    {
      ...moduleDesc,
      pmb_expr: {
        ...pmb_expr,
        pmod_desc: Pmod_structure(newList)
      }
    };
  | _ =>
    log("Moduledesc wasn't touched");
    moduleDesc;
  };
};

let test_mapper = _argv => {
  ...default_mapper, /* we extend the default_mapper */
  /* And override the 'expr' property */
  expr: (mapper, expr) =>
    switch expr {
    | {pexp_desc: Pexp_extension(({txt: "c.raw", loc}, rest))} =>
      switch rest {
      | PStr([
          {
            pstr_desc:
              Pstr_eval(
                {pexp_desc: Pexp_constant(Const_string(str, _)), pexp_loc},
                _
              )
          }
        ]) =>
        appendToFile(pexp_loc, str);
        Ast_helper.Exp.constant(Const_int(0));
      | _ => Ast_helper.Exp.constant(Const_int(0))
      }
    | other => default_mapper.expr(mapper, other)
    },
  structure_item: (mapper, structure_item) =>
    switch structure_item {
    | {pstr_desc: Pstr_recmodule(moduleList)} =>
      let ret =
        default_mapper.structure_item(
          mapper,
          {
            ...structure_item,
            pstr_desc: Pstr_recmodule(List.map(processModule, moduleList))
          }
        );
      ret;
    | {pstr_desc: Pstr_module(moduleDesc)} =>
      default_mapper.structure_item(
        mapper,
        Ast_helper.(Str.module_(processModule(moduleDesc)))
      )
    | _ => default_mapper.structure_item(mapper, structure_item)
    }
};

register("c.raw", test_mapper);

switch oc^ {
| None => ()
| Some(oc) => close_out(oc)
};
