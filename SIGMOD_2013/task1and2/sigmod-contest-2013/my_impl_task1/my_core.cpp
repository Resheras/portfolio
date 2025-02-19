/*
 * core.cpp version 1.0
 * Copyright (c) 2013 KAUST - InfoCloud Group (All Rights Reserved)
 * Author: Amin Allam
 *
 * Permission is hereby granted, free of charge, to any person
 * obtaining a copy of this software and associated documentation
 * files (the "Software"), to deal in the Software without
 * restriction, including without limitation the rights to use,
 * copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following
 * conditions:
 *
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
 * OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
 * HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
 * WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
 * OTHER DEALINGS IN THE SOFTWARE.
 */

#include "../include/core.h"
#include "./my_functions.h"
#include <cstring>
#include <cstdlib>
#include <cstdio>
#include <stdio.h>
#include <string>
#include <algorithm>
#include <iostream>
#include <cmath>
#include <list>
using namespace std;

// Keeps all information related to an active query
struct Query
{
	QueryID query_id;
	char str[MAX_QUERY_LENGTH];
	MatchType match_type;
	unsigned int match_dist;
};

///////////////////////////////////////////////////////////////////////////////////////////////

// Keeps all query ID results associated with a dcoument
struct Document
{
	DocID doc_id;
	unsigned int num_res;
	QueryID* query_ids;
};

///////////////////////////////////////////////////////////////////////////////////////////////

// Keeps all currently active queries
list<Query> queries;

// Keeps all currently available results that has not been returned yet
list<Document> docs;

///////////////////////////////////////////////////////////////////////////////////////////////

ErrorCode InitializeIndex(){return EC_SUCCESS;}

///////////////////////////////////////////////////////////////////////////////////////////////

ErrorCode DestroyIndex(){return EC_SUCCESS;}

///////////////////////////////////////////////////////////////////////////////////////////////

ErrorCode StartQuery(QueryID query_id, const char* query_str, MatchType match_type, unsigned int match_dist)
{
	Query query;
	query.query_id=query_id;
	strcpy(query.str, query_str);
	query.match_type=match_type;
	query.match_dist=match_dist;
	queries.push_back(query);
	return EC_SUCCESS;
}


///////////////////////////////////////////////////////////////////////////////////////////////
ErrorCode EndQuery(QueryID query_id)
{
    // Find the query to remove
    auto it_to_erase = queries.end(); // Default to end (not found)
    for (auto it = queries.begin(); it != queries.end(); ++it)
    {
        if (it->query_id == query_id)
        {
            it_to_erase = it;
            break; // Exit the loop as the target query is found
        }
    }

    // If found, erase the query outside the loop
    if (it_to_erase != queries.end())
    {
        queries.erase(it_to_erase);
        return EC_SUCCESS; // Query removed successfully
    }

}
///////////////////////////////////////////////////////////////////////////////////////////////

bool one_query_check(Query* quer, list<string> document_list,int * query_ids){
	list<string> query_list = split_doc(quer->str);
	
	bool matching_query=false;

		if(quer->match_type==MT_EXACT_MATCH)
				{
					matching_query = hamming_rec_matching(query_list,document_list,0);
				}
				else if(quer->match_type==MT_HAMMING_DIST)
				{
					matching_query = hamming_rec_matching(query_list,document_list, quer->match_dist);
				}
				else if(quer->match_type==MT_EDIT_DIST)
				{
					matching_query = edit_matching(query_list,document_list, quer->match_dist);
				}
	if (matching_query) {
			// This query matches the document
			*query_ids = int(quer->query_id);
		} else {
			*query_ids = -1;
		}
	return matching_query;
}

///////////////////////////////////////////////////////////////////////////////////////////////

ErrorCode MatchDocument(DocID doc_id, const char* doc_str)
{
	list<string> document_list = split_doc(doc_str);//split the doc into list of words
	unsigned int i, n=queries.size(); //length of queries list
	int query_ids[n]; //array of query ids

	// Iterate on all active queries to compare them with this new document
	
	i = 0; // Index for query_ids array
for (auto it = queries.begin(); it != queries.end(); ++it, ++i)
{
    Query* quer = &(*it); // Take one query
    one_query_check(quer, document_list, &query_ids[i]);
}

	list<unsigned int> query_ids_results;
	for(i=0;i<n;i++) 
	{
		if (query_ids[i] > -1) query_ids_results.push_back(query_ids[i]);	
	}

	query_ids_results.sort();

	Document doc;
	doc.doc_id=doc_id;
	doc.num_res=query_ids_results.size();
	doc.query_ids=0;
	if(doc.num_res) doc.query_ids=(unsigned int*)malloc(doc.num_res*sizeof(unsigned int));
	for(i=0;i<doc.num_res;i++){
		doc.query_ids[i]=query_ids_results.front();
		query_ids_results.pop_front();
	} 
	// Add this result to the set of undelivered results
	docs.push_back(doc);

	return EC_SUCCESS;
}

///////////////////////////////////////////////////////////////////////////////////////////////

ErrorCode GetNextAvailRes(DocID* p_doc_id, unsigned int* p_num_res, QueryID** p_query_ids)
{
	// Get the first undeliverd resuilt from "docs" and return it
	*p_doc_id=0; *p_num_res=0; *p_query_ids=0;
	if(docs.size()==0) return EC_NO_AVAIL_RES;
	*p_doc_id=docs.back().doc_id; *p_num_res=docs.back().num_res; *p_query_ids=docs.back().query_ids;
	docs.pop_back();
	return EC_SUCCESS;
}

///////////////////////////////////////////////////////////////////////////////////////////////
