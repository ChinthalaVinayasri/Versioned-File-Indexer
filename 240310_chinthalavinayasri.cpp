#include <iostream>
#include <fstream>
#include <unordered_map>
#include <vector>
#include <algorithm>
#include <chrono>
#include <cctype>
#include <stdexcept>

using namespace std;

/* -------- Template Example -------- */

template<typename T>
void printLine(const T &v){
    cout << v << endl;
}

/* -------- Utility -------- */

string toLowerWord(string s){
    for(char &c : s)
        c = tolower(c);
    return s;
}

/* -------- Buffered Reader -------- */

class BufferedReader{

    ifstream file;
    size_t bufferSize;

public:

    BufferedReader(const string &filename,int kb){

        if(kb < 256 || kb > 1024)
            throw runtime_error("Buffer must be between 256 and 1024 KB");

        bufferSize = kb * 1024;

        file.open(filename);

        if(!file)
            throw runtime_error("Cannot open file: " + filename);
    }

    bool readChunk(string &chunk){

        vector<char> buffer(bufferSize);

        file.read(buffer.data(),bufferSize);

        size_t bytes = file.gcount();

        if(bytes==0)
            return false;

        chunk.assign(buffer.begin(),buffer.begin()+bytes);

        return true;
    }
};

/* -------- Tokenizer -------- */

class Tokenizer{

public:

    vector<string> tokenize(const string &text){

        vector<string> words;
        string temp;

        for(char c : text){

            if(isalnum(c))
                temp += c;

            else{

                if(!temp.empty()){
                    words.push_back(toLowerWord(temp));
                    temp.clear();
                }

            }
        }

        if(!temp.empty())
            words.push_back(toLowerWord(temp));

        return words;
    }
};

/* -------- Word Index -------- */

class WordIndex{

    unordered_map<string,int> freq;

public:

    void addWord(const string &word){
        freq[word]++;
    }

    void addWord(const string &word,int count){   // function overloading
        freq[word] += count;
    }

    int getCount(const string &word){

        auto it = freq.find(word);

        if(it == freq.end())
            return 0;

        return it->second;
    }

    const unordered_map<string,int>& data() const{
        return freq;
    }
};

/* -------- Build Index (handles buffer boundary words) -------- */

void buildIndex(const string &filename,
                int buffer,
                WordIndex &index){

    BufferedReader reader(filename,buffer);
    Tokenizer tokenizer;

    string chunk;
    string leftover;

    while(reader.readChunk(chunk)){

        chunk = leftover + chunk;
        leftover.clear();

        if(isalnum(chunk.back())){

            int pos = chunk.size()-1;

            while(pos>=0 && isalnum(chunk[pos]))
                pos--;

            leftover = chunk.substr(pos+1);
            chunk = chunk.substr(0,pos+1);
        }

        vector<string> words =
            tokenizer.tokenize(chunk);

        for(const string &w : words)
            index.addWord(w);
    }

    if(!leftover.empty())
        index.addWord(toLowerWord(leftover));
}

/* -------- Abstract Query -------- */

class Query{
public:
    virtual void execute() = 0;
    virtual ~Query(){}
};

/* -------- Word Query -------- */

class WordQuery : public Query{

    WordIndex &index;
    string version;
    string word;

public:

    WordQuery(WordIndex &i,string v,string w)
        : index(i),version(v),word(w){}

    void execute() override{

        cout<<"Version: "<<version<<endl;
        cout<<"Word: "<<word<<endl;

        cout<<"Count: "
            <<index.getCount(toLowerWord(word))
            <<endl;
    }
};

/* -------- Top K Query -------- */

class TopQuery : public Query{

    WordIndex &index;
    string version;
    int k;

public:

    TopQuery(WordIndex &i,string v,int top)
        : index(i),version(v),k(top){}

    void execute() override{

        cout<<"Version: "<<version<<endl;

        vector<pair<string,int>> vec(
            index.data().begin(),
            index.data().end()
        );

        sort(vec.begin(),vec.end(),
        [](auto &a,auto &b){
            return a.second > b.second;
        });

        cout<<"Top "<<k<<" words:"<<endl;

        for(int i=0;i<k && i<vec.size();i++)
            cout<<vec[i].first<<" "
                <<vec[i].second<<endl;
    }
};

/* -------- Difference Query -------- */

class DiffQuery : public Query{

    WordIndex &v1;
    WordIndex &v2;
    string ver1;
    string ver2;
    string word;

public:

    DiffQuery(WordIndex &a,
              WordIndex &b,
              string version1,
              string version2,
              string w)
        : v1(a),v2(b),
          ver1(version1),
          ver2(version2),
          word(w){}

    void execute() override{

        int c1 = v1.getCount(toLowerWord(word));
        int c2 = v2.getCount(toLowerWord(word));

        cout<<"Version1: "<<ver1
            <<" Count: "<<c1<<endl;

        cout<<"Version2: "<<ver2
            <<" Count: "<<c2<<endl;

        cout<<"Difference: "<<(c2-c1)<<endl;
    }
};

/* -------- Main Program -------- */

int main(int argc,char* argv[]){

    string file,file1,file2;
    string version,version1,version2;
    string queryType,word;

    int buffer=512;
    int top=10;

    for(int i=1;i<argc;i++){

        string arg = argv[i];

        if(arg=="--file") file = argv[++i];
        else if(arg=="--file1") file1 = argv[++i];
        else if(arg=="--file2") file2 = argv[++i];

        else if(arg=="--version") version = argv[++i];
        else if(arg=="--version1") version1 = argv[++i];
        else if(arg=="--version2") version2 = argv[++i];

        else if(arg=="--buffer") buffer = stoi(argv[++i]);
        else if(arg=="--query") queryType = argv[++i];
        else if(arg=="--word") word = argv[++i];
        else if(arg=="--top") top = stoi(argv[++i]);
    }

    try{

        auto start =
        chrono::high_resolution_clock::now();

        Query* q = nullptr;

        WordIndex index1,index2;

        if(queryType=="word"){

            buildIndex(file,buffer,index1);

            q = new WordQuery(index1,version,word);
        }

        else if(queryType=="top"){

            buildIndex(file,buffer,index1);

            q = new TopQuery(index1,version,top);
        }

        else if(queryType=="diff"){

            buildIndex(file1,buffer,index1);
            buildIndex(file2,buffer,index2);

            q = new DiffQuery(index1,index2,
                              version1,
                              version2,
                              word);
        }

        if(q){
            q->execute();
            delete q;
        }

        auto end =
        chrono::high_resolution_clock::now();

        chrono::duration<double> time =
            end-start;

        cout<<"Buffer size: "<<buffer<<" KB"<<endl;
        cout<<"Execution time: "<<time.count()<<" sec"<<endl;
    }

    catch(exception &e){
        cout<<"Error: "<<e.what()<<endl;
    }
}