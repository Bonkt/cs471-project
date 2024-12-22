#include <bits/stdc++.h>

#include <unordered_set>
#include <vector>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>

using namespace std;




int trace_no_reuse_count = 0;

using TraceId = int;
TraceId largest_trace_id = 0;

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

// Struct to store queries (each query is a trace reuse.)
struct Query {

    int left;   // first index of trace (not trace id!) 
    int right;  // last index of trace
    int idx;   // idx is 
};

// Mo’s ordering:  
// - Calculate block size ~ sqrt(n)
// - Sort queries by (left/block) then by right
int block_size;

int investigated_trace_reuse_distance = 12;
const char* investigated_trace_reuse_distance_histogram_output_file = nullptr;

    // map<trace_id, trace_reuses_with_distance_x_count>
std::unordered_map<TraceId, int> trace_reuse_histogram;

int main(int argc, char *argv[]) {
    char *input_file = NULL;

    if (argc < 2) {
        fprintf(stderr, "Usage: %s  <input_file> [-o <output_file>] [-n <trace_reuse_distance>] [-O <trace_id_histogram>]\n", argv[0]);
        return EXIT_FAILURE;
    }

    input_file = argv[1];
    char* output_file_name = nullptr; 


    // Parse command line arguments
    for (int i = 2; i < argc; i++) {
        if (strcmp(argv[i], "-o") == 0 && i + 1 < argc) {
            output_file_name = argv[++i];
        } else if (strcmp(argv[i], "-n") == 0 && i + 1 < argc) {
            investigated_trace_reuse_distance = stoi(argv[++i]);
        } else if (strcmp(argv[i], "-O") == 0 && i + 1 < argc) {
            investigated_trace_reuse_distance_histogram_output_file = argv[++i];
        }
    }

    cout << "command line parsed!\n"; 

    // string input = "1,2,3,4,5,5,4,3, 4, 5, 3, 4, 4, 1, 6, 5, 6, 2";
    // stringstream ss(input);
    ifstream file;
    file.open(input_file);
    vector<int> input_array;
    input_array.reserve(20000);
    freq.reserve(20000);
    int temp;
    string line;
    
    // need to store a map<trace_id, >
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

 	
    cout << "after reading in file!" << std::endl;
    // Last occurrence of each ID
    vector<int> last_occurrence(largest_trace_id+1, -1);
    vector<Query> queries;
    int q = 0;

    long int n = input_array.size();
    for (int i = 0; i < n; i++) {
        int id = input_array[i];
        if (last_occurrence[id] != -1) {
            // query from last_occurrence[id]+1 to i-1
            if(i - last_occurrence[id] > 0) {
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
        int block_a = a.left / block_size;
        int block_b = b.left / block_size;
        if(block_a == block_b) return a.right < b.right;
        return block_a < block_b;
    });

    // vector of trace reuse distances for each reuse of a trace.
    vector<int> answers(q);
    int cur_l = 0, cur_r = -1;

    // Initialize frequency array

    for(int i=0; i<=largest_trace_id; i++) freq[i] = 0;

    // Process queries
    for (auto &query : queries) {
        int left = query.left, right = query.right;
        // Move cur_r
        while (cur_r < right) addID(input_array[++cur_r]);
        while (cur_r > right) removeID(input_array[cur_r--]);

        // Move cur_l
        while (cur_l < left) removeID(input_array[cur_l++]);
        while (cur_l > left) addID(input_array[--cur_l]);

        // for every query, if (distinct_count == investigated_trace_reuse_distance)
        //      Then we store its input_array[query.left] to recover the trace_id 
        if (investigated_trace_reuse_distance_histogram_output_file) {
            if (distinct_count == investigated_trace_reuse_distance) {
                trace_reuse_histogram[input_array[query.left]]++;
            }
        }

        // Now [cur_l, cur_r] is [left,right]
        answers[query.idx] = distinct_count;
    }



    ofstream out_file(output_file_name);
    //cout << "after processing queries!" << std::endl;
    ofstream histogram_out_file;
    if(investigated_trace_reuse_distance_histogram_output_file) {
        histogram_out_file = ofstream(investigated_trace_reuse_distance_histogram_output_file);
    }

    using TraceDistance = int;
    std::unordered_map<TraceDistance, int> reuse_distance_count;

    // Print answers (number of distinct IDs for each query)
    for (auto& ans : answers) {
    //        cout << ans << "\n";
        reuse_distance_count[ans]++;
        // out_file << ans << "\n";
    }

    for (auto& [distance, count] : reuse_distance_count) {
        out_file << distance << ", " << count << "\n";
    } 



    //cout << "running! \n";    
    // Add the nr of traces that only occur once to the log.
//    int single_occurance_trace_count = 0;
//    for (auto& i : freq) {
 //       if(i == 1) {
//	    single_occurance_trace_count++;
//	}
//    }	    
//    out_file << single_occurance_trace_count;
    if (investigated_trace_reuse_distance_histogram_output_file) {
        cout << "logging histogram to: " 
            << investigated_trace_reuse_distance_histogram_output_file
            << std::endl;
    }

    // Sort by value;
    std::vector<std::pair<int, int>> sorted_vector(trace_reuse_histogram.begin(), trace_reuse_histogram.end());
    std::sort(sorted_vector.begin(), sorted_vector.end(),
              [](const std::pair<int, int>& a, const std::pair<int, int>& b) {
                  return a.second > b.second; // Sort by value in ascending order
              });

    // log the histogram:
    if (investigated_trace_reuse_distance_histogram_output_file) {
        for (auto& [trace_id, count] : sorted_vector) {
            histogram_out_file << trace_id << ", " << count << "\n";
        }
    }

    return 0;
}
