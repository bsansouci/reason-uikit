cppx: cppx/cppx

cppx/cppx: cppx/cppx.re
	ocamlc -pp "refmt --print binary" -I +compiler-libs ocamlcommon.cma -o cppx/cppx -impl cppx/cppx.re
