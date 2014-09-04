user_id_mapping.txt
===
**format**
line_1: #users
line_i: original user ID(may be string type), mapped user ID(integer from 0 to #users-1)
**sample**
3
Ac213,0
u8J3A,2
98GBV,1

item_id_mapping.txt
===
**format**
Line_1: #items
Line_i: original item ID(may be string type), mapped item ID(integer from 0 to #items-1)
**sample**
4
vxzc1,0
DeSO9,1
Clk89,3
13c6U,2

movie.txt
===
**format**
Line_1: #movies
Line_i: item ID(mapped), year, title 
one movie each line, sort by year
**sample**
3
2,1993,Needful Things 
1,1993,My Life 
0,1993,Century 
 
rating_train.txt
===
**format**
Line_1: #ratings
Line_i: user ID(mapped), item ID(mapped), rating, timestamp
user ID is from 0 to #users-1, item ID is from 0 to #movies-1
**sample**
4
0,2,4,822873600
1,2,3,822873600
2,1,1,822873600
1,0,5,822873600

rating_test.txt
===
same as rating_train.txt
