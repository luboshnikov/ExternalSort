#include <fstream>
#include <iostream>
#include <vector>
#include <algorithm>
using namespace std;

#define MAX_SIZE 32000000

vector<string> read_and_sort(ifstream& in, size_t max_size){
    in.seekg(0, ios::end);
    size_t input_size = in.tellg()/sizeof(unsigned);
    in.seekg(0, ios::beg);

    if(input_size == 0){
        return {};
    }
    vector<string> res((input_size-1)/max_size + 1);
    vector<unsigned> fragment(max_size);

    for(size_t i = 0; i < res.size(); i++){
        if(input_size < max_size){
            max_size = input_size;
        }
        in.read((char*)&fragment[0], max_size*sizeof(unsigned));
        sort(fragment.begin(), fragment.begin()+max_size);
        res[i] = "buffer_" + to_string(i);
        fstream buf(res[i], ios::out | ios::binary);
        buf.write((char*)&fragment[0], max_size*sizeof(unsigned));

        input_size -= max_size;
    }
    return move(res);
}

void write(const vector<string>& buffers_name, ofstream& out){
    size_t count = buffers_name.size();
    vector<unsigned> res(count);
    vector<ifstream> buffers(count);
    for(int i = 0; i < count; i++){
        buffers[i].open(buffers_name[i]);
        buffers[i].read((char*)&res[i], sizeof(unsigned));
    }
    while(count != 0){
        size_t buf_with_min = min_element(res.begin(), res.begin()+count) - res.begin();
        unsigned numb = res[buf_with_min];
        out.write((char*)&numb, sizeof(unsigned));

        if (!buffers[buf_with_min].read((char*)&res[buf_with_min], sizeof(unsigned))) {
            count--;
            swap(buffers[buf_with_min], buffers[count]);
            swap(res[buf_with_min], res[count]);
            buffers[count].close();
        }
    }
    for(auto i : buffers_name){
        remove((const char*)&i[0]);
    }
}

int main () {
    ifstream in("input", ios::binary);
    if(in.is_open()){
        vector<string> buffers = read_and_sort(in, MAX_SIZE);
        ofstream out("output", ios::binary);
        write(buffers, out);
    } else {
        cout << "input file not found.\n";
    }
}