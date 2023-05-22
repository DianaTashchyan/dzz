#include <iostream>
#include <thread>
#include <mutex>
#include <condition_variable>

using namespace std;

mutex mmutex;
condition_variable pp;
bool Available=true;

void print(const string& message){
    unique_lock<mutex> lock(mmutex);
    pp.wait(lock, []{ return Available; });
    Available=false;
    cout<<"Printing: "<< message<<endl;
  
    Available=true;
    pp.notify_all();
}

void threadFunction(const string& message){
    print(message);
}

int main(){
    thread thread1(threadFunction, "Message 1");
    thread thread2(threadFunction, "Message 2");
    thread thread3(threadFunction, "Message 3");
    thread1.join();
    thread2.join();
    thread3.join();
    return 0;
}
