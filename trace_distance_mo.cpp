#include <bits/stdc++.h>

#include <unordered_set>
#include <vector>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>

using namespace std;

// static const int MAX_ID = 20000;
int largest_trace_id = 0;
int trace_no_reuse_count = 0;

// Frequency array of ids
std::vector<int> freq{0}; 
int distinct_count = 0;

inline void addID(int x) {
    if(freq[x] == 0) distinct_count++;
    freq[x]++;
}

inline void removeID(int x) {
    freq[x]--;
    if(freq[x] == 0) distinct_count--;
}

// Struct to store queries
struct Query {
    int left, right, idx;
};

// Mo’s ordering:  
// - Calculate block size ~ sqrt(n)
// - Sort queries by (left/block) then by right
int block_size;

int main(int argc, char *argv[]) {
    char *input_file = NULL;

    if (argc < 2) {
        fprintf(stderr, "Usage: %s  <input_file> [-o <output_file>]]\n", argv[0]);
        return EXIT_FAILURE;
    }

    input_file = argv[1];
    char* output_file_name = nullptr; 

    // Parse command line arguments
    for (int i = 2; i < argc; i++) {
        if (strcmp(argv[i], "-o") == 0 && i + 1 < argc) {
            output_file_name = argv[++i];
        }
    }

    // string input = "1,2,3,4,5,5,4,3, 4, 5, 3, 4, 4, 1, 6, 5, 6, 2";
    // stringstream ss(input);
    ifstream file;
    file.open(input_file);
    vector<int> input_array;
    input_array.reserve(20000);
    freq.reserve(20000);
    int temp;
    string line;

    //cout << "here! \n";

    while (getline(file, line)) {
        stringstream ss(line); 
        string value;
        while (getline(ss, value, ',')) {
            temp = stoi(value); // Convert to integer
            input_array.push_back(temp);
            freq.push_back(0);  // default value set to 0.
            if (temp > largest_trace_id) {
                largest_trace_id = temp;
            }
        }
    }

 	
    //cout << "after reading in file!" << std::endl;
    // Last occurrence of each ID
    vector<int> last_occurrence(largest_trace_id+1, -1);
    vector<Query> queries;
    int q = 0;

    long int n = input_array.size();
    // Identify queries ()
    for (int i = 0; i < n; i++) {
        int id = input_array[i];
	//cout << "last_occurance[id]=" << last_occurrence[id] << "\n"; 
	if (last_occurrence[id] != -1) {
            // query from last_occurrence[id]+1 to i-1
            if(i - last_occurrence[id] > 0) {
                //cout << "found a query. \n"; 
		Query Q;
                Q.left = last_occurrence[id] + 1;
                Q.right = i - 1;
                Q.idx = q++;
                queries.push_back(Q);
            }
        }
	last_occurrence[id] = i;
    }

    // If noueries, just end here
    if (q == 0) {
        // no repeats
        return 0;
    }

    //cout << "before mo's algorithm:" << std::endl;

    // Mo’s algorith
    block_size = (int) sqrt(n);
    sort(queries.begin(), queries.end(), [&](const Query &a, const Query &b){
        int blockA = a.left / block_size;
        int blockB = b.left / block_size;
        if(blockA == blockB) return a.right < b.right;
        return blockA < blockB;
    });

    vector<int> answers(q);
    int curL = 0, curR = -1;


    //cout << "after mo's algoirtm" << std::endl;
    
    for(int i=0; i<=largest_trace_id; i++) freq[i] = 0;

    // Process queries
    for (auto &query : queries) {
        int left = query.left, right = query.right;
        // Move curR
        while (curR < right) addID(input_array[++curR]);
        while (curR > right) removeID(input_array[curR--]);

        // Move curL
        while (curL < left) removeID(input_array[curL++]);
        while (curL > left) addID(input_array[--curL]);

        // Now [curL, curR] is [left,right]
        answers[query.idx] = distinct_count;
    }

    ofstream out_file(output_file_name);

    //cout << "after processing queries!" << std::endl;

    // Print answers (number of distinct IDs for each query)
    for (int ans : answers) {
//        cout << ans << "\n";
        out_file << ans << "\n";
    }

    //cout << "running! \n";    
    // Add the nr of traces that only occur once to the log.
    int single_occurance_trace_count = 0;
    for (auto& i : freq) {
        if(i == 1) {
	    single_occurance_trace_count++;
	}
    }	    
//    out_file << single_occurance_trace_count;

    return 0;
}
