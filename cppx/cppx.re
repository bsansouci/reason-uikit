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

        static CAMLprim value isNil(value objc) {
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
  let processStructures = (moduleName, listOfStructures) =>
    List.map(
      s =>
        switch s {
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

                      ret = caml_alloc_small(5, Abstract_tag);
                      Field(ret, 0) = (value)[[%s alloc] init];
                      Field(ret, 1) = Val_none;
                      Field(ret, 2) = Val_none;
                      Field(ret, 3) = Val_none;
                      Field(ret, 4) = Val_none;

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

                      ret = caml_alloc_small(5, Abstract_tag);
                      Field(ret, 0) = (value)[[%s alloc] init%s:%s];
                      Field(ret, 1) = Val_none;
                      Field(ret, 2) = Val_none;
                      Field(ret, 3) = Val_none;
                      Field(ret, 4) = Val_none;

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
          appendToFile(pstr_loc, newFunction);
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
              CAMLreturn((value)[%s %s]);
            }
          |},
              fullFunctionName,
              moduleName,
              pvalNameString
            );
          log("static fullFunctionName: " ++ fullFunctionName);
          appendToFile(pstr_loc, newFunction);
          Ast_helper.Str.primitive({
            ...valDesc,
            pval_prim: [fullFunctionName]
          });
        | _ => s
        },
      listOfStructures
    );
  switch moduleDesc {
  | {
      pmb_attributes: [({txt: "c.class"}, _)],
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
    let newList = processStructures(moduleName, listOfStructures);
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
      pmb_attributes: [({txt: "c.class"}, _)],
      pmb_expr: {pmod_desc: Pmod_structure(listOfStructures)} as pmb_expr,
      pmb_name: {txt: moduleName}
    } =>
    let newList = processStructures(moduleName, listOfStructures);
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
      default_mapper.structure_item(
        mapper,
        {
          ...structure_item,
          pstr_desc: Pstr_recmodule(List.map(processModule, moduleList))
        }
      )
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
