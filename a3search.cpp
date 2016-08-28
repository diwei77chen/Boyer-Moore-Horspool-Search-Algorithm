// Date Created: 10 MAY 2016
// Date Revised: 18 MAY 2016
// Author: Diwei Chen

#include <iostream>
#include <stdio.h>
#include <vector>
#include <dirent.h>
#include <fstream>
#include <algorithm>
#include <string.h>

class Filename_Frequency {
public:
    std::string filename;
    int frequency;
};

// initialise the last occurence matrix with given vector of queries
void LastOcc(int (&last_occ)[5][128], std::vector<std::string> vec_query);
// return the index of a given char in the last occurence matrix
int IndexOfLastOcc(char c, int (&last_occ)[5][128], size_t i_query);
// return the index of the character of text which is unmatched
int Compare(std::string query, std::vector<char> text);
// return the frequency of a query in a given file
int BoyerMooreHorspool(std::string query, int (&last_occ)[5][128], size_t i_query, std::ifstream &input_file);


int main(int argc, const char * argv[]) {
    const char* dir_path = argv[1];     // the path of directory
    std::string idx_name = argv[2];     // the name of the output index
    std::string arg3 = argv[3];
    std::vector<std::string> vec_query;     // the set of queries
    if (arg3 == "-s") {
        for (int i = 5; i < argc; ++i) {
            std::string str = argv[i];
            vec_query.push_back(str);
        }
    }
    else {
        for (int i = 3; i < argc; ++i) {
            std::string str = argv[i];
            vec_query.push_back(str);
        }
    }
    
    
    std::vector<std::string> vec_filenames;     // the set of filenames in the directory
    DIR *dir;
    struct dirent *ent;
    if ((dir = opendir(dir_path)) != NULL) {
        while ((ent = readdir(dir)) != NULL) {
            if (strcmp(ent->d_name, ".") != 0 && strcmp(ent->d_name, "..") != 0) {
                vec_filenames.push_back(ent->d_name);
            }
            
            
        }
        closedir(dir);
    }
    else {
        perror("");
        return EXIT_FAILURE;
    }
    
    int last_occ[5][128] = {{0}};         // at most 5 queries, for simplification, we declare each dimension with size of 128
    for (int i = 0; i < 5; ++i) {
        for (int j = 0; j < 128; ++j) {
            last_occ[i][j] = -1;        // each position has initial value -1
        }
    }
    
    LastOcc(last_occ, vec_query);      // initialise last_occ matrix
    
    std::vector<Filename_Frequency> vec_filename_frequency;     // store the file, frequency pairs
    for (size_t i = 0; i < vec_filenames.size(); ++i) {
        std::string path_filename = static_cast<std::string>(dir_path) + '/' + vec_filenames.at(i);
        Filename_Frequency f;
        f.filename = vec_filenames.at(i);
        f.frequency = 0;
        
        std::ifstream input_file;
        std::vector<int> vec_frequency_tmp;
        for (size_t j = 0; j < vec_query.size(); ++j) {
            input_file.open(path_filename.c_str());
            int frequency = BoyerMooreHorspool(vec_query.at(j), last_occ, j, input_file);
            f.frequency += frequency;
            if (frequency != 0) {
                vec_frequency_tmp.push_back(frequency);
            }
            input_file.close();
        }
        if (vec_frequency_tmp.size() == vec_query.size()) {
            vec_filename_frequency.push_back(f);
        }
        
        
    }
    std::sort(vec_filename_frequency.begin(), vec_filename_frequency.end(), [](Filename_Frequency &a, Filename_Frequency &b) {
        if (a.frequency != b.frequency) {
            return a.frequency > b.frequency;
        }
        return a.filename < b.filename;
    });
    
    for (size_t i = 0; i < vec_filename_frequency.size(); ++i) {
        std::cout << vec_filename_frequency.at(i).filename << std::endl;
    }
    
    return 0;
}

void LastOcc(int (&last_occ)[5][128], std::vector<std::string> vec_query) {
    size_t size = vec_query.size();
    for (size_t i = 0; i < size; ++i) {
        int length = static_cast<int>(vec_query.at(i).length());
        for (int j = length - 2; j >= 0; --j) {     // ignore the occurence of the last character of the query
            char c = vec_query.at(i).at(j);
            unsigned int ascii = static_cast<unsigned int>(c);
            
            if (ascii >= 65 && ascii <= 90) {       // handle case insensitive
                if (last_occ[i][ascii] == -1 && last_occ[i][ascii + 32] == -1) {
                    last_occ[i][ascii] = j;
                    last_occ[i][ascii + 32] = j;
                }
            }
            else if (ascii >= 97 && ascii <= 122) {
                if (last_occ[i][ascii] == -1 && last_occ[i][ascii - 32] == -1) {
                    last_occ[i][ascii] = j;
                    last_occ[i][ascii - 32] = j;
                }
            }
            else if (ascii == 32 && last_occ[i][ascii] == -1) {     // the occ of space
                last_occ[i][ascii] = j;
            }
        }
    }
}

int IndexOfLastOcc(char c, int (&last_occ)[5][128], size_t i_query) {
    unsigned int ascii = static_cast<unsigned int>(c);
    return last_occ[i_query][ascii];
}

int Compare(std::string query, std::vector<char> text) {
    int c = -2;
    int i = static_cast<int>(query.length()) - 1;
    for (; i >= 0; --i) {       // compare from backwards
        if (tolower(query.at(i)) == tolower(text.at(i))) {      // ignore case
            continue;
        }
        else {
            return i;
        }
    }
    
    return c;       // match
}

int BoyerMooreHorspool(std::string query, int (&last_occ)[5][128], size_t i_query, std::ifstream &input_file) {
    int frequency = 0;      // the frequency of the query in a file
    unsigned int query_length = static_cast<unsigned int>(query.length());
    unsigned int shift = 0;
    std::vector<char> text;
    std::string str_line;
    
    while (std::getline(input_file, str_line)) {
        shift = query_length;       // the number of chracters that can be shifted each time
        text.clear();
        int index_of_unmatched_in_text = -10;
        unsigned int str_line_length = static_cast<unsigned int>(str_line.length());
        unsigned int i = 0;
        int flag_handle_tail = 0;
        for (; i < str_line_length;) {
            if (shift == query_length) {
                text.clear();
                unsigned int j = 0;
                for (j = i; j < i + shift; ++j) {
                    if (j < str_line_length) {
                        text.push_back(str_line.at(j));         // build the text of the same size of query
                    }
                    else {
                        flag_handle_tail = 1;
                        break;
                    }
                }
                if (flag_handle_tail == 1 ) {
                    break;
                }
                
                i += shift;
                
                if (j == str_line_length) {
                    break;
                }
            }
            index_of_unmatched_in_text = Compare(query, text);
            if (index_of_unmatched_in_text == -2) {     // find match
                shift = query_length;
                ++frequency;
                continue;
            }
            if (index_of_unmatched_in_text >= 0) {      // unmatched character exists between text and query
                int index_of_last_occ_in_query = IndexOfLastOcc(text.at(text.size() - 1), last_occ, i_query);
                if (index_of_last_occ_in_query == -1) {    // this character does not exist in query
                    shift = query_length;
                }
                else {      // this character exists in query
                    shift = query_length - (index_of_last_occ_in_query + 1);        // shift always >= 1
                    for (unsigned int k = i; k < i + shift; ++k) {
                        if (k < str_line_length) {
                            text.erase(text.begin());
                            text.push_back(str_line.at(k));
                        }
                        else {
                            flag_handle_tail = 1;
                            break;
                        }
                    }
                    if (flag_handle_tail == 1) {
                        break;
                    }
                    i += shift;
                }
            }
        }
        if (i == str_line_length) {      // handle if the last match occurs in the last characters
            if (text.size() == query.length()) {
                if (Compare(query, text) == -2) {
                    ++frequency;
                }
            }
        }
        
    }
    
    return frequency;
}






























