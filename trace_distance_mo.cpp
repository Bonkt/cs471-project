#include <bits/stdc++.h>

#include <vector>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>

using namespace std;

static const int MAX_ID = 20000;
int freq[MAX_ID+1]; // frequency array of IDs
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
    int L, R, idx;
};

// Mo’s ordering:  
// - Calculate block size ~ sqrt(n)
// - Sort queries by (L/block) then by R
int block_size;

int main(int argc, char *argv[]) {
    char *input_file = NULL;

    if (argc < 2) {
        fprintf(stderr, "Usage: %s  <input_file> [-o <output_file>]]\n", argv[0]);
        return EXIT_FAILURE;
    }

    input_file = argv[1];
    char* output_file_name = "default_output.csv";

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
    vector<int> arr;
    arr.reserve(20000);
    int temp;
    string line;

    // Read the entire file line by line
    while (getline(file, line)) {
        stringstream ss(line); // Create a stringstream for each line

        // Read comma-separated values
        string value;
        while (getline(ss, value, ',')) {
            temp = stoi(value); // Convert to integer
            arr.push_back(temp);
        }
    }


    // Last occurrence of each ID
    vector<int> last_occurrence(MAX_ID+1, -1);
    vector<Query> queries;
    int q = 0;

    long int n = arr.size();
    // Identify queries (each repeated occurrence forms a query)
    for (int i = 0; i < n; i++) {
        int id = arr[i];
        if (last_occurrence[id] != -1) {
            // query from last_occurrence[id]+1 to i-1
            if(i - last_occurrence[id] > 0) {
                Query Q;
                Q.L = last_occurrence[id] + 1;
                Q.R = i - 1;
                Q.idx = q++;
                queries.push_back(Q);
            }
        }
        last_occurrence[id] = i;
    }

    // If no queries, just end here
    if (q == 0) {
        // no repeats
        return 0;
    }

    // Prepare Mo’s algorithm
    block_size = (int) sqrt(n);
    sort(queries.begin(), queries.end(), [&](const Query &a, const Query &b){
        int blockA = a.L / block_size;
        int blockB = b.L / block_size;
        if(blockA == blockB) return a.R < b.R;
        return blockA < blockB;
    });

    vector<int> answers(q);
    int curL = 0, curR = -1;


    // Initialize frequency array
    for(int i=0; i<=MAX_ID; i++) freq[i] = 0;

    // Process queries
    for (auto &query : queries) {
        int L = query.L, R = query.R;
        // Move curR
        while (curR < R) addID(arr[++curR]);
        while (curR > R) removeID(arr[curR--]);

        // Move curL
        while (curL < L) removeID(arr[curL++]);
        while (curL > L) addID(arr[--curL]);

        // Now [curL, curR] is [L,R]
        answers[query.idx] = distinct_count;
    }

    ofstream out_file(output_file_name);

    // Print answers (number of distinct IDs for each query)
    for (int ans : answers) {
//        cout << ans << "\n";
        out_file << ans << "\n";
    }

    return 0;
}
