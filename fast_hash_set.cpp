#include <iostream>
#include <set>
#include <unordered_set>
#include <functional>
#include <chrono>
#include <cmath>
#include <random>
#include <ctime>
#include <ext/pb_ds/assoc_container.hpp>
using namespace __gnu_pbds;
using namespace std;
int getTicks()
{
    static auto startTime = std::chrono::high_resolution_clock::now();
    auto t = std::chrono::high_resolution_clock::now();
    return std::chrono::duration_cast<std::chrono::milliseconds>(t - startTime).count();
}
template<class T> struct hset
{
    size_t sz, (*hfunc)(T);
    static constexpr uint8_t dead = 0, filled = 1, empty = 2;
    uint8_t *state;
    T *x;
    int buffer;
    hset(int s,
         size_t (*hf)(T) = [](T val)->size_t{hash<T> hfunc; return hfunc(val);})
    {
        hfunc = hf;
        sz = s;
        buffer = 2*log2(sz) + 5; //a buffer lets us work faster because we don't have to check that pos<sz at the cost of a minimal chance of runtime error + a little memory
        state = new uint8_t[sz+buffer];
        x = new T[sz+buffer];
        fill(state, state+sz+buffer, empty);
    }
    ~hset()
    {
        delete state;
        delete x;
    }
    void clear()
    {
        fill(state, state+sz+buffer, empty);
    }
    bool erase(const T &v)
    {
        int pos = hfunc(v) % sz;
        while(true)
        {
            if(state[pos] == empty)
                return false;
            if(state[pos]==filled && x[pos]==v)
            {
                state[pos] = dead;
                return true;
            }
            pos++;
        }
    }
    void erase_x(const T &v) //UNSAFE! crashes if element is not found
    {
        int pos = hfunc(v) % sz;
        while(true)
        {
            if(state[pos]==filled && x[pos]==v)
            {
                state[pos] = dead;
                return;
            }
            pos++;
        }
    }
    bool insert(const T &v)
    {
        int pos = hfunc(v) % sz;
        int first = -1;
        while(true)
        {
            if(state[pos] != filled)
            {
                if(state[pos] == empty)
                {
                    if(first == -1)
                        first = pos;
                    state[first] = filled;
                    x[first] = v;
                    return true;
                }
                else if(first == -1)
                    first = pos;
            }
            else if(x[pos] == v)
                return false;
            pos++;
        }
    }
    int count(const T &v) const
    {
        int pos = hfunc(v) % sz;
        while(true)
        {
            if(state[pos] == empty)
                return 0;
            if(state[pos]==filled && x[pos]==v)
                return 1;
            pos++;
        }
    }
    T *find(const T &v) const
    {
        int pos = hfunc(v) % sz;
        while(true)
        {
            if(state[pos] == empty)
                return NULL;
            else if(state[pos]==filled && x[pos]==v)
                return &x[pos];
            pos++;
        }
    }
    size_t size()
    {
        return sz;
    }
};
const int n = 3e6;
int a[n], b[n];
void testHSET()
{
    hset<int> v(4*n);
    for(int i=0; i<n; i++)
        v.insert(a[i]);
    for(int i=0; i<n; i++)
        v.erase(b[i]);
}
void testPBuset()
{
    gp_hash_table<int, null_type> v;
    for(int i=0; i<n; i++)
        v.insert(a[i]);
    for(int i=0; i<n; i++)
        v.erase(b[i]);
}
void testPBuset2()
{
    cc_hash_table<int, null_type> v;
    for(int i=0; i<n; i++)
        v.insert(a[i]);
    for(int i=0; i<n; i++)
        v.erase(b[i]);
}
void teststluset()
{
    unordered_set<int> v;
    v.max_load_factor(0.5);
    v.reserve(n);
    for(int i=0; i<n; i++)
        v.insert(a[i]);
    for(int i=0; i<n; i++)
        v.erase(b[i]);
}
#define RANDUZ_MAX 0x0fffffff
int randuz()
{
    static std::mt19937 gen(time(NULL));
    static std::uniform_int_distribution<int> d(0, RANDUZ_MAX);
    return d(gen);
}
void testgood()
{
    unordered_set<int> s1;
    hset<int> s2(10000, [](int x)->size_t{return x*x;});
    for(int i=0; i<0xfff; i++)
    {
        s1.insert(i);
        s2.insert(i);
    }
    for(int i=0; i<20000; i++)
    {
        int x = randuz() & 0xfff;
        switch(randuz() % 3)
        {
        case 0:
            s1.erase(x);
            s2.erase(x);
            break;
        case 1:
            s1.insert(x);
            s2.insert(x);
            break;
        case 2:
            if(s1.count(x) != s2.count(x))
                cout << "BAD\n";
            break;
        }
    }
}
int main()
{
    testgood(); //test to make sure our hash set actually works
    for(int i=0; i<n; i++)
        a[i] = randuz(), b[i] = randuz();;
    int s = getTicks();
    testHSET();
    int t = getTicks();
    cout << "Custom hash set: " << t-s << "ms\n";
    for(int i=0; i<n; i++)
        a[i] = randuz(), b[i] = randuz();
    s = getTicks();
    testPBuset();
    t = getTicks();
    cout << "gp_hash_table: " << t-s << "ms\n";
    for(int i=0; i<n; i++)
        a[i] = randuz(), b[i] = randuz();
    s = getTicks();
    testPBuset2();
    t = getTicks();
    cout << "cc_hash_table: " << t-s << "ms\n";
    for(int i=0; i<n; i++)
        a[i] = randuz(), b[i] = randuz();;
    s = getTicks();
    teststluset();
    t = getTicks();
    cout << "unordered_set: " << t-s << "ms\n";
    return 0;
}
