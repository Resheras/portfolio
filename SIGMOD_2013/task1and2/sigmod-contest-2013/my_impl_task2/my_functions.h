#include <stdio.h>
#include <string>
#include <vector>
#include <iostream>
#include <algorithm>
#include <iostream>
#include <cmath>

using namespace std;


vector<string> split_doc(const char *doc_str);

bool exact_matching(vector<string> query, vector<string> doc);

bool hamming_matching(vector<string> query, vector<string> doc, int distance);

int edit_distance(string word1, string word2, int threshold, int current_dist);

bool edit_matching(vector<string> query, vector<string> doc, int edit_dist);


