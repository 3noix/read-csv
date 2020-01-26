# ReadCsv

This code allows to perform a robust reading of csv files in C++. It is adapted from the code of this article:
https://julien-blanc.developpez.com/tutoriels/c++/lecture-csv-c++/

The function readCsvFile reads the whole data from the file before processing it, but we can do way more interesting things with the code provided.

Source files that can be used with standard C++:
- CsvData.h
- CsvDataWriter.h
- CsvError.h
- CsvParser.h

Source files that need Qt:
- main.cpp
- MyCsv.h
- MyCsvTraits.h
