## INTRODUCTION:
- Code supplement for the paper "Stable Container-Route Matching in Multimodal Transport: A Case of Yangtze River Economic Belt".
- If you need help using the code, please send an email to shanwenxuan[at]buaa[dot]edu[dot]cn.
- The code and data sets are also available from https://github.com/TransportShan/Stable_match_nTo1.git.

## DATASETS:
- The instance data are provided in the folder "data". 
- Each instance is named by the format "Na-Rb-Sc", in which Na means a nodes in the network, Rb means b routes and Sc means c shipments.
- In the file "shipment.csv", the column "Classification" is redundant and filled with "-1".
- In the file "route.csv", the first column is route index. The second column is redundant and filled with "-1". The third column is the number of nodes in this route. The fourth to (3+a)th columns are node index in this route, and unpassed nodes are filled with "-1". The (3+a+1)th to (3+2*a)th columns are freight from corresponding node to destination. The (3+2*a+1)th to (3+3*a)th columns are travel time from corresponding node to destination. The last column is the capacity of this route.


## RESULTS:
- The detailed results obtained by each solution method for all instances are provided in the folder "data", name by "stable matching.csv". 

## CODE:
- Code of all algorithms used in our computational experiments is provided in the folder "Stable_match_nTo1". 

## USAGE:
- To run an algorithm for solving an instance:
  1. Copy the files "cooperation parameter.csv", "network.csv", "route.csv", and "shipment.csv" from an "Na-Rb-Sc" folder to the folder "./data./". 
  2. Build and run the code;
  3. Obtain the results from the file "stable matching.csv" in the folder "./result./".
