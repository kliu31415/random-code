#include <iostream>
#include <chrono>
#include <ctime>
#include <algorithm>
#include <cassert>
#include <random>
#include <iomanip>
#include <cstdint>
#include <sstream>
#define ull unsigned long long
#define ll long long
#define RANDUZ_MAX 0xffffffffffffffff
using namespace std;
double *a;
ll getTicksNs()
{
    static auto startTime = std::chrono::high_resolution_clock::now();
    auto t = std::chrono::high_resolution_clock::now();
    return std::chrono::duration_cast<std::chrono::nanoseconds>(t - startTime).count();
}
ull randuz()
{
    static std::mt19937_64 gen(time(NULL));
    static std::uniform_int_distribution<ull> d(0, RANDUZ_MAX);
    return d(gen);
}
template<class T> string to_str(T x)
{
    stringstream ss;
    ss << x;
    return ss.str();
}
template<class T> void lsd_radix_sort64_u(T *aIn, int n)
{
    uint64_t *a = reinterpret_cast<uint64_t*>(aIn);
    uint64_t *t = new uint64_t[n];
    int *cnt = new int[256];
    for(int_fast8_t f=0, z=0; f<64; f+=8, z++)
    {
        for(int i=0; i<=0xff; i++)
            cnt[i] = 0;
        uint8_t *a8 = reinterpret_cast<uint8_t*>(a);
        for(int i=0; i<n; i++)
            cnt[a8[i*8+z]]++;
        for(int i=1; i<=0xff; i++)
            cnt[i] += cnt[i-1];
        for(int i=n-1; i>=0; i--)
            t[--cnt[a8[i*8+z]]] = a[i];
        swap(a, t);
    }
    delete[] cnt;
    delete[] t;
}
template<class T> void lsd_radix_sort64(T *aIn, int n)
{
    uint64_t *a = reinterpret_cast<uint64_t*>(aIn);
    for(int i=0; i<n; i++)
    {
        if(a[i] & 0x8000000000000000) //is negative?
            a[i] ^= 0xffffffffffffffff;
        else a[i] ^= 0x8000000000000000;
    }
    lsd_radix_sort64_u<T>(aIn, n);
    for(int i=0; i<n; i++)
    {
        if(a[i] & 0x8000000000000000) //is negative?
            a[i] ^= 0x8000000000000000;
        else a[i] ^= 0xffffffffffffffff;
    }
}
void testFunc(void (*func)(double*, int), int n, string name)
{
    a = new double[n];
    for(int i=0; i<n; i++)
    {
        unsigned ll v = randuz();
        a[i] = reinterpret_cast<double&>(v);
    }
    ll s = getTicksNs();
    func(a, n);
    ll e = getTicksNs();
    ios_base::fmtflags init(cout.flags());
    cout.width(20);
    cout.setf(ios_base::left);
    cout << (name + ":");
    cout.width(15);
    cout.flags(init);
    cout << e-s << "ns" << endl;
    /*for(int i=0; i<n; i++)
    {
        cout << a[i] << endl;
        if(i!=0 && a[i]<a[i-1])
            cout << "BAD\n";
    }*/
    assert(is_sorted(a, a+n));
    delete[] a;
}
void sort_n(double *a, int n)
{
    sort(a, a+n);
}
int main()
{
    /*for(int i=0; i<10; i++)
    {
        ull v = randuz();
        double d = reinterpret_cast<double&>(v);
        cout << d << "\n";
        v = reinterpret_cast<ull&>(d);
        cout << ((1<<11)^(v>>52)) << "\n";
    }*/
    for(int i=1<<15; i<=5e7; i*=2)
    {
        ios_base::fmtflags init(cout.flags());
        cout.setf(ios_base::left);
        cout.fill('-');
        cout.width(37);
        cout << to_str(i) + " elements";
        cout << "\n";
        cout.fill(' ');
        cout.flags(init);
        testFunc(sort_n, i, "std::sort");
        testFunc(lsd_radix_sort64<double>, i, "LSD radix sort");
    }
    return 0;
}
