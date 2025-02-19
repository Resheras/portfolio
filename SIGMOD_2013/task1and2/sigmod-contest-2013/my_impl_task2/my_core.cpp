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
#include <vector>
#include <stdio.h>
#include <string>
#include <algorithm>
#include <iostream>
#include <cmath>
#include <thread>
#include <unordered_map>
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
//vector<Query> queries;
unordered_map<QueryID, Query> queries;

// Keeps all currently available results that has not been returned yet
vector<Document> docs;

///////////////////////////////////////////////////////////////////////////////////////////////

ErrorCode InitializeIndex(){return EC_SUCCESS;}

///////////////////////////////////////////////////////////////////////////////////////////////

ErrorCode DestroyIndex(){return EC_SUCCESS;}

///////////////////////////////////////////////////////////////////////////////////////////////

ErrorCode StartQuery(QueryID query_id, const char* query_str, MatchType match_type, unsigned int match_dist)
{
	Query query = {
		query_id,
		"",
		match_type,
		match_dist
	};
	strcpy(query.str, query_str);
	queries.insert({query_id,query});
	return EC_SUCCESS;
}

///////////////////////////////////////////////////////////////////////////////////////////////

ErrorCode EndQuery(QueryID query_id) {
	// Remove this query from the active query set
	auto i = queries.find(query_id);
	if (i != queries.end()) {
        queries.erase(i);
	return EC_SUCCESS;
	}
}
///////////////////////////////////////////////////////////////////////////////////////////////

bool one_query_check(Query* quer, vector<string> document_vector,int * query_ids){
	vector<string> query_vector = split_doc(quer->str);
	
	bool matching_query=false;

		if(quer->match_type==MT_EXACT_MATCH)
				{
					matching_query = exact_matching(query_vector,document_vector);
				}
				else if(quer->match_type==MT_HAMMING_DIST)
				{
					matching_query = hamming_matching(query_vector,document_vector, quer->match_dist);
				}
				else if(quer->match_type==MT_EDIT_DIST)
				{
					matching_query = edit_matching(query_vector,document_vector, quer->match_dist);
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

	vector<string> document_vector = split_doc(doc_str);//split the doc into vector of words
	sort(document_vector.begin(), document_vector.end());
	unsigned int i, n=queries.size(); //length of queries vector
	int query_ids[n]; //array of query ids
	thread parallel_queries[n];

	// Iterate on all active queries to compare them with this new document
	auto it = queries.begin();
	i = 0;
	//i=0;i<n;i++
	while (it != queries.end()) 
	{
		Query* quer=&it->second; //take one query
		parallel_queries[i] = thread(one_query_check,quer,document_vector,&query_ids[i]);
		i++;
		it++;	
	}
	for(i=0;i<n;i++) 
	{
		parallel_queries[i].join();
			
	}

	vector<unsigned int> query_ids_results;
	for(i=0;i<n;i++) 
	{
		if (query_ids[i] > -1) query_ids_results.push_back(query_ids[i]);	
	}

	sort(query_ids_results.begin(),query_ids_results.end());

	Document doc;
	doc.doc_id=doc_id;
	doc.num_res=query_ids_results.size();
	doc.query_ids=0;
	if(doc.num_res) doc.query_ids=(unsigned int*)malloc(doc.num_res*sizeof(unsigned int));
	for(i=0;i<doc.num_res;i++) doc.query_ids[i]=query_ids_results[i];
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
	*p_doc_id=docs[0].doc_id; *p_num_res=docs[0].num_res; *p_query_ids=docs[0].query_ids;
	docs.erase(docs.begin());
	return EC_SUCCESS;
}

///////////////////////////////////////////////////////////////////////////////////////////////
