# CSCI5010 Project - Incremental Delaunay Triangulation

### Gengjie Chen (SID: 1155067753)

### 2018 Spring

## Notes

### Compile:
```
g++ -std=c++11 main.cpp triangulation.cpp dcel.cpp -o triangulation -O3 -DNDBUG
```

### Run:
```
./triangulation -f <input_file>
```
(For input file `xxx.txt`, the output file will be `xxx_output.txt`.)

### Visualize:
```
./draw <output_file>
```