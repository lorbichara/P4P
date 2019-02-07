Lorraine Bichara
lb34995
CS 377 P

This program multiplies two matrices made up of random double values. The dimension is provided by the user.
The program also includes several measurements used to complete assignment 1. Since not all values can be measured at the same time, I commented some of them. At this moment, the user can choose between measuring execution time using PAPI or clock_gettime, but more PAPI measurements are available in comments. To use them, only uncomment those specific lines of code.

The output is displayed as a label indicating the measurement, and the value.

To compile the code, uncompress/untar the .tar.gz file, and inside that directory use the "make" command. This will create an executable called "executable" linking to the required libraries and using the options. To execute, run the following command: ./executable