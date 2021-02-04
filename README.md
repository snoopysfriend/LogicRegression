# LogicRegression

ICCAD 2019 CAD Contest
Problem A: Logic Regression on High Dimensional
Boolean Space
## prerequest
```
install abc
```
## installation
```
> git clone https://github.com/snoopysfriend/LogicRegression.git
> make
> mv <path of abc> .

```
## how to run
```
> ./lrg-spflip <path-of-io_info.txt> <path-of-iogen> <path-of-circuit.blif>
## this would write the circuit to circuit.blif
> ./test <path-of-io_info.txt> <path-of-iogen> <path-of-circuit.blif>
## this would test the accuracy of the circuit.blif

> ./abv-opt <path-of-io_info.txt> <path-of-iogen> <path-of-circuit.blif> <path-of-curcuit.v>
## this would test the accuracy and generate the .v file for circuit.blif

python src/cal_size.py <path-of-curcuit.v>
## this calculate the size of curcuit.v

```

## script
``` 
> time.sh 
## this would record the time of gernerate .blif file, and test the accuracy of .blif file

> opt.sh
## this would use the .blif file to generate .v file and also test the accuracy of .v file and print the circuit size