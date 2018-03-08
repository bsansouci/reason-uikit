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
    | {
        pstr_desc:
          Pstr_module(
            {
              pmb_attributes: [({txt: "c.class"}, _)],
              pmb_loc,
              pmb_expr: {pmod_desc: Pmod_structure(listOfStructures)},
              pmb_name: {txt: moduleName}
            }
          )
      } =>
      log("found module: " ++ moduleName);
      List.iter(
        s =>
          switch s {
          | {
              pstr_loc,
              pstr_desc:
                Pstr_primitive({
                  pval_name: {txt: pvalNameString},
                  pval_attributes: [({txt: "c.new"}, _)]
                })
            } =>
            let newFunction =
              sp(
                {|
                  CAMLprim value %s_new() {
                    CAMLparam0();
                    CAMLlocal1(ret);

                    ret = caml_alloc_small(5, Abstract_tag);
                    Field(ret, 0) = (value)[%s new];
                    Field(ret, 1) = Val_none;
                    Field(ret, 2) = Val_none;
                    Field(ret, 3) = Val_none;
                    Field(ret, 4) = Val_none;

                    CAMLreturn(ret);
                  }
                |},
                moduleName,
                moduleName
              );
            appendToFile(pstr_loc, newFunction);
          | _ => ()
          },
        listOfStructures
      );
      default_mapper.structure_item(mapper, structure_item);
    | _ => default_mapper.structure_item(mapper, structure_item)
    }
};

register("c.raw", test_mapper);

switch oc^ {
| None => ()
| Some(oc) => close_out(oc)
};
