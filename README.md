Radix sorting vs std::sort results:  

32768 elements-----------------------  
std::sort:                  1994000ns  
LSD radix sort:             2991000ns  
65536 elements-----------------------  
std::sort:                  6986000ns  
LSD radix sort:             1992000ns  
131072 elements----------------------  
std::sort:                 13963000ns  
LSD radix sort:             9990000ns  
262144 elements----------------------  
std::sort:                 30919000ns  
LSD radix sort:            12958000ns  
524288 elements----------------------  
std::sort:                 60837000ns  
LSD radix sort:            23909000ns  
1048576 elements---------------------  
std::sort:                120703000ns  
LSD radix sort:            43849000ns  
2097152 elements---------------------  
std::sort:                257311000ns  
LSD radix sort:            88728000ns  
4194304 elements---------------------  
std::sort:                524604000ns  
LSD radix sort:           181480000ns  
8388608 elements---------------------  
std::sort:               1091117000ns  
LSD radix sort:           374009000ns  
16777216 elements--------------------  
std::sort:               2265910000ns  
LSD radix sort:           683173000ns  
33554432 elements--------------------  
std::sort:               4651533000ns  
LSD radix sort:          1348422000ns  

  
Custom hash set vs std::unordered_map vs __gnu_pbds::gp_hash_table vs __gnu__pbds::cc_hash_table:

Custom hash set: 269ms  
gp_hash_table: 522ms  
cc_hash_table: 1709ms  
unordered_set: 2009ms  
