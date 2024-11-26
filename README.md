CS471 Cloud workloads characterization by traceability

## Python Scripts
### processing.py

This script is responsible for transforming the data from the previous execlog into the correct format.

## C files
### main.c

Main file loading the files and starting the analysis of the execution log.

### parser.c

Contains functions to extract and give the data in a meaningful way to other script. This script should be the only one to really interact with the opened file (except checking address)

### trace-selection.c

Contains functions to build, check and process the traces using the execution log. This script should contain all functions with regards to traces (maybe except computing the metrics).