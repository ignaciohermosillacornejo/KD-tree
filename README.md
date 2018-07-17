# Assignment N1
Course IIC2133 @ PUC (2018-1) - Data structures and algorithms

Implementation of an image filter using a voronoi diagram, implemented using a 2 dimensional KD-Tree

To run, first execute move into "Programa" folder

```bash
cd Programa
```

and then execute the Makefile

```bash
make
```

If you are on a windows machine or ubuntu, you may need to change line 10 of the Makefile to target your instalation of GCC.
\
To run the program, execute the following code. In this example we are running the code on lena.png with 100 random nuclei and a random seed of 1

```bash
./filter imgs/lenga.png 100 1