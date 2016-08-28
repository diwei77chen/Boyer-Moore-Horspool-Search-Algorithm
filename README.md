// Date Created: 10 MAY 2016
// Date Revised: 18 MAY 2016
// Author: Diwei Chen

This program mainly implements Boyer-Moore-Horspool algorithm for finding strings. 

Initially, there is one preprocessing function named LastOcc() to build a matrix with each dimension of size 128 to record last occurrence of the characters in a given query. For example, if the given query is “aaba”, the matrix built will be [-1,…,1,2,…,-1](using ascii value and the last character in the query is ignored). With this matrix, we can decide the number of characters we can shift each time when Compare() between text and query.

Furthermore, In the BoyerMooreHorspool() function, it will find matches between a given query and a given text and return the frequency of the query in the text. With taking advantage of the last occurrence matrix, this function will obtain an average-case complexity of O(N) and will perform O(MN) in the worst case, where N is the length of the text and M is the length of the query. Because the worst case usually would not exist in a common English documents, it could be way fast in searching for a matching pattern in the text.

Last but not least, this program is not built on multithread environment, instead it will calculate the frequency of the given queries one by one by BoyerMooreHorspool() in an open file and finally add them up to give the result.