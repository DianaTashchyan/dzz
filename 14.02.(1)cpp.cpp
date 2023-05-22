#include <iostream>
#include <pthread.h>
#include <string>
#include <vector>
#define NUM_THREADS 4

using namespace std;

string s1;
vector<std::string> dd;
string s2;

void* copy_string(void* arg) {
    int thread_id=*(static_cast<int*>(arg));
    int start=thread_id*(s1.length()/NUM_THREADS);
    int end=(thread_id==NUM_THREADS-1) ? s1.length() : (start+s1.length()/NUM_THREADS);
    dd[thread_id]=s1.substr(start, end-start);
    pthread_exit(nullptr);
}

int main(){
    cout<<"enter a string: ";
    getline(std::cin, s1);

    pthread_t threads[NUM_THREADS];
    vector<int> thread_ids(NUM_THREADS);
    dd.resize(NUM_THREADS);

    for(int i=0; i<NUM_THREADS; ++i){
        thread_ids[i]=i;
        int result=pthread_create(&threads[i], nullptr, copy_string, &thread_ids[i]);
        if(result){
            cerr<<"error creating thread "<<i<<": "<<result<<endl;
            return 1;
        }
    }
    for(int i=0; i<NUM_THREADS; ++i){
        pthread_join(threads[i], nullptr);
    }
    for(int i=0; i<NUM_THREADS; ++i){
        s2+=dd[i];
    }
    cout<<s2<<endl;
    cout<<"press enter to exit...";
    cin.ignore();
    return 0;
}
