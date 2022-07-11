# HOML
## HOML-translator

### build and run：
```
cd translator
make
./translator
```

```
Usage: translator [options] infile outfile
Options:
  -n <num> -- Set the number of processes to <num>.
  -a       -- Generate a formula to verify Agreement.
  -u       -- Generate a formula to verify Univalence.
  -i       -- Generate a formula to verify Invariant.
  -s       -- Generate smt2 code to verify.(default)
  -b       -- Generate smt2 bv code to verify.
  -y       -- Generate yices code to verify.
```

yices-smt2 is used to translate SMT for CNF, refer to https://yices.csl.sri.com/


### eg:
generate SMT file：

```
translator -n 3 -a -s lastvoting.ho a3.smt2
```

generate CNF file：

```
translator -n 3 -a -b lastvoting.ho a3_bv.smt2
yices-smt2 --dimacs==a3.cnf a3_bv.smt2
```
## HOML-HOME

Eclipse IDE plugin

You need to install Eclipse PDE and Eclipse Xtext in the Eclipse Marketplace.

dependencies:
```
Eclipse IDE 2022-03
JavaSE-13
org.eclipse.xtext 2.26.0
org.eclipse.ui 3.201.0
org.eclipse.swt 3.119.0
com.google.gson 2.8.9
```

export plugin：
```
File -> Export -> Plug-in Development -> Deployable plug-ins and fragments
choose org.consensus.config，org.consensus.homodel，org.consensus.homodel.ide，org.consensus.homodel.ui
Select the directory to export to
Finish
```

install plugin：

Copy the generated JAR package to the Plugins directory in the Eclipse installation directory and restart the Eclipse IDE


## solvers

z3：https://github.com/Z3Prover/

yices-smt2：https://yices.csl.sri.com/

cvc4：https://cvc4.github.io/downloads.html

Alt-ergo：https://alt-ergo.ocamlpro.com/

Glucose and Glucose-syrup：https://www.labri.fr/perso/lsimon/glucose/

minisat：http://minisat.se/






