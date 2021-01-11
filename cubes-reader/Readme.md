#       Cubes-Reader
This is a software to read the binary files output from the 
Cubes / Proto-Cubes Arduino boards and store the information in ROOT 
files. [CUBES][1] is a payload on the KTH student satellite mission 
[MIST][2]. [ROOT][3] is a C++ based modular scientific analysis toolkit 
released by CERN.

## Principle
The binary file obtained from Cubes consists of data formatted as per 
the [Cubes Interface document][4]. The __Cubes-Reader__ takes this file 
and parses the formatted information. It stores the parsed information in 
ROOT based _TTree_ and _TH1F_ structures saved in a ROOT file. The 
stored information can be viewed / processed later using ROOT based tools.

## Pre-requisites
Following libraries are required in order to compile and run this code

* CERN's ROOT ( made for version 6.14, should work with other versions)
* C++ STL (made for C++11. Tested with gnu compiler g++)
* CMake utility (for enabling compilation)

## Installation and execution
On a Linux system, with CMake, ROOT and gnu compilers available, 
the following commands should compile and build an executable

```
$ cd `/path/to/cubes-reader ` (Folder with "CMakeLists.txt" )
$ mkdir build
$ cd build
$ cmake ../
$ make
```

In order to run this, do
```
$ ./build/Cubes-Reader <binary_filename>
```

This will create a file called `Outhist.root` with the stored _TTree_ and _TH1F_
values.

[1]: https://www.kth.se/sci/centra/rymdcenter/studentsatellit/cubes
[2]: https://www.kth.se/en/sci/centra/rymdcenter/studentsatellit/studentsatelliten-mist
[3]: https://root.cern.ch/
[4]: https://www.dropbox.com/s/bgmuprx38zf2seh/citiroc_intf_icd_v4.pdf?dl=0 
