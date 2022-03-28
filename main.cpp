#include <fstream>
#include <iostream>
#include <future>
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
    size_t res_size = input_size/max_size + (input_size%max_size == 0 ? 0 : 1);
    vector<string> res(res_size);
    vector<unsigned> fragment(max_size);

    for(size_t i = 0; i < res_size; i++){
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

void mergefiles(const string& first_file, const string& second_file, const string& output_file){
    unsigned first, second;
    bool firstflag = false, secondflag = false;
    fstream first_in(first_file, ios::binary | ios::in);
    fstream second_in(second_file, ios::binary | ios::in);
    fstream out(output_file, ios::binary | ios::out);
    if(first_in.read((char*)&first, sizeof(unsigned))) {
        firstflag = true;
    }
    if(second_in.read((char*)&second, sizeof(unsigned))) {
        secondflag = true;
    }
    while(firstflag && secondflag){
        if(first < second){
            out.write((char*)&first, sizeof(unsigned));
            if(!first_in.read((char*)&first, sizeof(unsigned)))
                firstflag = false;
        } else {
            out.write((char*)&second, sizeof(unsigned));
            if(!second_in.read((char*)&second, sizeof(unsigned)))
                secondflag = false;
        }
    }
    if(secondflag){
        out.write((char*)&second, sizeof(unsigned));
        while(second_in.read((char*)&second, sizeof(unsigned))){
            out.write((char*)&second, sizeof(unsigned));
        }
    } else if(firstflag){
        out.write((char*)&first, sizeof(unsigned));
        while(first_in.read((char*)&first, sizeof(unsigned))){
            out.write((char*)&first, sizeof(unsigned));
        }
    }

}

void write_with_multithreading(vector<string> buffers_name, const string& output_file){
    size_t count = buffers_name.size();
    if(count == 0) {
        cout << "Input file is empty or incorrect\n";
        return;
    }
    while(count > 1){
        vector<future<void>> futures;
        futures.reserve(count/2);
        size_t i = 0;
        while(i < count-1){
            count--;
            const string res_name = buffers_name[i] + '_' + to_string(count),
                    first = buffers_name[i],
                    second = buffers_name[count];
            futures.push_back(
                    async([first, second, res_name]{
                        mergefiles(first, second, res_name);
                        remove((char*)&first[0]);
                        remove((char*)&second[0]);
                    })
            );
            buffers_name[i] = res_name;
            i++;
        }

    }
    rename((const char*)&buffers_name[0][0], (const char*)&output_file[0]);
}

int main () {
    ifstream in("input", ios::binary);
    if(in.is_open()){
        write_with_multithreading(read_and_sort(in, MAX_SIZE), "output");
    } else {
        cout << "input file not found.\n";
    }
}