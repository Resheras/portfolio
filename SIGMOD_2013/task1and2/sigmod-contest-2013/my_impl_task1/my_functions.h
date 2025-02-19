#include <stdio.h>
#include <string>
#include <iostream>
#include <algorithm>
#include <iostream>
#include <cmath>
#include <list>
using namespace std;


list<string> split_doc(const char *doc_str);

int edit_distance(string word1, string word2, int threshold, int current_dist);

bool edit_matching(list<string> query, list<string> doc, int edit_dist);

int hamm_recursive_distance(string word1, string word2, int threshold, int current_dist); 

bool hamming_rec_matching(list<string> query, list<string> doc, int edit_dist);


