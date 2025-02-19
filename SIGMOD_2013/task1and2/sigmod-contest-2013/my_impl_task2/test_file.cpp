#include <stdio.h>
#include <string>
#include <vector>
#include <iostream>
#include <algorithm>
#include <iostream>
#include <cmath>
#include "./my_functions.h"

using namespace std;


vector<string> split_doc(const char *doc_str) {
    vector<string> results;
    string new_word;
    char * cur_ltr_ptr = (char *)doc_str;

    while (*cur_ltr_ptr) {
        char cur_ltr_val = *cur_ltr_ptr;
        if (cur_ltr_val !=' '){
            new_word += cur_ltr_val;
        } else {
            if (new_word.length() > 0) {
                results.push_back(new_word);
                new_word = "";
            }
        }
        
        cur_ltr_ptr++;
    }
    //for the last read, when we find NULL-string, get the last word recorded
    if (new_word.length() > 0) {
        results.push_back(new_word);
    }
    return results;
}

bool exact_matching(vector<string> query, vector<string> doc) {
    return all_of(query.begin(), query.end(), [&](string value) {
        return find(doc.begin(), doc.end(), value) != doc.end(); //if nothing found, returns end
        //return lower_bound(doc.begin(), doc.end(), value) != doc.end() ;
    });
}

bool hamming_matching(vector<string> query, vector<string> doc, int distance) {
    
    return all_of(query.begin(), query.end(), [&](string query_term) {
        for (string word : doc) {
            //if the word in text is different length to query term - skip comparison
            if (query_term.length() != word.length()) {
                continue;
            }  
            //int length_term = query_term.length();
            int i = 0;
            int ham_dist = 0;
            //count the ham_distance as long as the word is not over, and distance is not exceeding threshold
            while (i < query_term.length() && ham_dist <= distance) {
                ham_dist += query_term[i] != word[i];
                i++;
            }
            //after comparison is over, check if ham distance is within threshold
            if (ham_dist <= distance) {
                return true;
            }
        }
        return false; 
    });
}

int edit_distance(string word1, string word2, int threshold, int current_dist) {
    
    if (current_dist > threshold) {
        //if existing distance exceed threshold - exit;
        return current_dist;
    }
    if (abs(int(word1.size() - word2.size()))>threshold) {
        //the words will never match under threshold, as they are too far apart - exit immediately with high number above threshold
        current_dist +=100;
        return current_dist;
    }
    if (word1 == word2) {
        //no need to adjust distance, words fully match, return as is
        return current_dist;
    }
    if (word1.length() > 0 && word2.length() > 0 && word1[0] == word2[0]) {
        //first letter matches, go into recursion of the remaining letters
        //or trim the word of the first letter with substr
        // no need to adjust distance in this step, but in recursive steps?
        return edit_distance(word1.substr(1),word2.substr(1), threshold, current_dist);
    } else if ( word1.length() == 0 && word2.length() > 0) {
        //we reached the end of the word 1 but not word 2
        //the remainder difference between word 1 and 2 should be added to distance as we will add all those letters
        current_dist += word2.length();
        return current_dist;
    } else if (word1.length() > 0 &&  word2.length() == 0) {
        //we reached the end of the word 2 but not word 1
        //the remainder difference between word 1 and 2 should be added to distance as we will add all those letters
        current_dist += word1.length();
        return current_dist;
    } else if (word1.length() == 0 && word2.length() == 0) {
        //we reached the end of both words, nothing more to do
        return current_dist;
    }
    else if (word1[0] != word2[0]) {
        //we current letter does not match - need to make adjustments
        //increase distance by 1, as we are going to manipulate strings
        //compare 3 options - add letter, remove letter, edit letter
        current_dist++;
        return min(min(edit_distance(word2[0] + word1, word2, threshold, current_dist),
                    edit_distance(word1.substr(1), word2, threshold, current_dist)),
                    edit_distance(word1.substr(1),word2.substr(1), threshold, current_dist)
        );
    }
}

bool edit_matching(vector<string> query, vector<string> doc, int edit_dist) {
    return all_of(query.begin(), query.end(), [&](string query_value) {

        return any_of(doc.begin(), doc.end(), [&](string doc_value) {
            return edit_distance(query_value,doc_value,edit_dist,0) <= edit_dist;
        }); 
    });
}

