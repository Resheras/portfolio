#include <stdio.h>
#include <string>
#include <iostream>
#include <algorithm>
#include <iostream>
#include <cmath>
#include <list>
#include "./my_functions.h"

using namespace std;


list<string> split_doc(const char *doc_str) {
    list<string> results;
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


int edit_distance(string word1, string word2, int threshold, int current_dist) {
    int word1_len = word1.length();
    int word2_len = word2.length();
    if (current_dist > threshold) {
        //if existing distance exceed threshold - exit;
        return current_dist;
    }
    if (abs(int(word1_len - word2_len))>threshold) {
        //the words will never match under threshold, as they are too far apart - exit immediately with high number above threshold
        current_dist +=100;
        return current_dist;
    }
    if (word1 == word2) {
        //no need to adjust distance, words fully match, return as is
        return current_dist;
    }
    if (word1_len > 0 && word2_len > 0 && word1.at(0) == word2.at(0)) {
        //first letter matches, go into recursion of the remaining letters
        //or trim the word of the first letter with substr
        // no need to adjust distance in this step, but in recursive steps?
        return edit_distance(word1.substr(1),word2.substr(1), threshold, current_dist);
    } else if ( word1.length() == 0 && word2.length() > 0) {
        //we reached the end of the word 1 but not word 2
        //the remainder difference between word 1 and 2 should be added to distance as we will add all those letters
        //current_dist += word2.length();
        return current_dist+word2.length();
    } else if (word1.length() > 0 &&  word2.length() == 0) {
        //we reached the end of the word 2 but not word 1
        //the remainder difference between word 1 and 2 should be added to distance as we will add all those letters
        //current_dist += word1.length();
        return current_dist+word1.length();
    } else if (word1.length() == 0 && word2.length() == 0) {
        //we reached the end of both words, nothing more to do
        return current_dist;
    }
    else if (word1.at(0) != word2.at(0)) {
        //we current letter does not match - need to make adjustments
        //increase distance by 1, as we are going to manipulate strings
        //compare 3 options - add letter, remove letter, edit letter
        string added_letter = word2.at(0) + word1;
        string removed_letter = word1.substr(1);
        string replaced_letter = word2.at(0) + word1.substr(1);
        current_dist++;
        return min(min(edit_distance(added_letter, word2, threshold, current_dist),
                    edit_distance(removed_letter, word2, threshold, current_dist)),
                    edit_distance(replaced_letter,word2, threshold, current_dist)
        );
    }
}

bool edit_matching(list<string> query, list<string> doc, int edit_dist) {
    list<string>::iterator end_q = query.end();
    list<string>::iterator begin_q = query.begin();
    int size_q = query.size();
    bool count_matches[size_q];
    int i_c= 0;

    list<string>::iterator i = query.begin(); 
    for (i_c; i_c < size_q ; i_c++) {
        count_matches[i_c] = false;
    }
    i_c = 0;
    for (string word : query){
        bool match = false;
        for (string doc_word: doc) {
            if (edit_distance(word,doc_word,edit_dist,0) <= edit_dist) match = true;
        }
        count_matches[i_c] = match;
        i_c++;
    }

    int count_total_finds = 0;
    for (int i=0; i< size_q; i++) {
        if (count_matches[i]) count_total_finds++;
    }
    bool final_match = (count_total_finds == size_q);
    return final_match;   
}

int hamm_recursive_distance(string word1, string word2, int threshold, int current_dist) {
    if (word1.length() != word2.length()) return 100;
    int word1_len = word1.length();
    int word2_len = word2.length();
    
	if (current_dist > threshold) {
        //if existing distance exceed threshold - exit;
        return current_dist;
    }
    if (word1 == word2) {
        //no need to adjust distance, words fully match, return as is
        return current_dist;
    }


    if (word1_len > 0 && word2_len > 0 && word1.at(0) == word2.at(0)) {
        //first letter matches, go into recursion of the remaining letters
        //or trim the word of the first letter with substr
        // no need to adjust distance in this step, but in recursive steps?
        return hamm_recursive_distance(word1.substr(1),word2.substr(1), threshold, current_dist);
    } else if (word1_len == 0 && word2_len == 0) {
        //we reached the end of both words, nothing more to do
        return current_dist;
    }
    else if (word1.at(0) != word2.at(0)) {
        //we current letter does not match - need to make adjustments
        //increase distance by 1, as we are going to manipulate strings
        //1st word - edit letter
        current_dist++;
        string replaced_letter = word2.at(0) + word1.substr(1); //editing word1
        int replaced_letter_dist = hamm_recursive_distance(replaced_letter, word2, threshold, current_dist);
        return replaced_letter_dist;
    }
}

bool hamming_rec_matching(list<string> query, list<string> doc, int edit_dist) {
    list<string>::iterator end_q = query.end();
    list<string>::iterator begin_q = query.begin();
    int size_q = query.size();
    bool count_matches[size_q];
    int i_c= 0;

    list<string>::iterator i = query.begin(); 
    for (i_c; i_c < size_q ; i_c++) {
        count_matches[i_c] = false;
    }
    i_c = 0;
    for (string word : query){
        bool match = false;
        for (string doc_word: doc) {
            if (hamm_recursive_distance(word,doc_word,edit_dist,0) <= edit_dist) match = true;
        }
        count_matches[i_c] = match;
        i_c++;
    }

    int count_total_finds = 0;
    for (int i=0; i< size_q; i++) {
        if (count_matches[i]) count_total_finds++;
    }
    bool final_match = (count_total_finds == size_q);
    return final_match;   
}