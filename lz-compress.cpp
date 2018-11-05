//LZ style compressor, Kevin Liu, 1/25/18
//could be optimized a little further with extra bit manipulation
#include <fstream>
#include <iostream>
#include <vector>
#include <string>
#include <cstdint>
#include <cstring>
#include <cstdlib>
#include <map>
using namespace std;
typedef unsigned char byte;
unsigned getlen(ifstream &fin)
{
    fin.seekg(0, ios::end);
    unsigned len = fin.tellg();
    fin.seekg(0, ios::beg);
    return len;
}
bool checkOverwrite(const char *file)
{
    ifstream fin(file);
    if(!fin.fail())
    {
        cout << "File " << file << " already exists. Overwrite? (Y/n)\n";
        string s;
        while(true)
        {
            cin >> s;
            if(s=="y" || s=="Y")
            {
                fin.close();
                remove(file);
                return true;
            }
            else if(s=="n" || s=="N")
            {
                cout << "Not overwriting file\n";
                fin.close();
                return false;
            }
        }
    }
    fin.close();
    return true;
}
struct node;
vector<node> nodes;
struct node
{
    byte v;
    uint32_t prv, d;
    map<byte, uint32_t> nxt;
    node()
    {
        v = 0;
        d = 0;
    }
    node(byte x, uint32_t p)
    {
        v = x;
        prv = p;
        d = nodes[prv].d + 1; //set depth
    }
};
inline void addData(vector<bool> &dat, uint32_t v, int bits) //little endian
{
    for(int j=0; j<bits; j++)
        dat.push_back((v>>j) & 1);
}
inline uint32_t readData(vector<bool> &dat, int &pos, int len) //little endian
{
    uint32_t res = 0;
    for(int i=pos+len-1; i>=pos; i--)
    {
        res <<= 1;
        res += dat[i];
    }
    pos += len;
    return res;
}
vector<byte> compressAlgo1(byte *idata, uint32_t ilen)
{
    nodes.push_back({});
    uint32_t lastMatchPos = 0;
    for(uint32_t i=0; i<ilen;)
    {
        uint32_t pos = 0;
        bool go = true;
        while(go)
        {
            if(i == ilen)
            {
                lastMatchPos = pos;
                go = false;
            }
            else if(nodes[pos].nxt[idata[i]] == 0)
            {
                nodes[pos].nxt[idata[i]] = nodes.size();
                nodes.push_back({idata[i], pos});
                go = false;
            }
            else pos = nodes[pos].nxt[idata[i]];
            i++;
        }
    }
    cout << "Trie nodes: " << nodes.size() << "\n";
    vector<bool> odata;
    odata.reserve(ilen); //the size of the compressed data is going to be roughly similar to the size of the input data
    addData(odata, 0x21970001u, 32); //magic number
    addData(odata, ilen, 32); //uncompressed size
    addData(odata, nodes.size() - 1, 32); //trie size (the root doesn't need to be stored because it's always the same)
    addData(odata, lastMatchPos, 32); //position of last match (needs to be stored)
    //output the trie
    int cur = 0;
    for(uint32_t i=1; i<nodes.size(); i++)
    {
        addData(odata, (byte)nodes[i].v, 8);
        addData(odata, nodes[i].prv, cur);
        if(1u<<cur == i)
            cur++;
    }
    while(odata.size()%8 != 0)
        odata.push_back(0);
    //
    vector<byte> byteOD;
    byteOD.resize(odata.size() / 8);
    cout << "Compressed file size: " << byteOD.size() << " bytes" << endl;
    int pos = 0;
    for(int i=0; i<byteOD.size(); i++)
        byteOD[i] = readData(odata, pos, 8);
    return byteOD;
}
void compress(const char *in, const char *out)
{
    ifstream fin(in, ios::binary);
    if(fin.fail())
    {
        cout << "Failed to open file " << in << endl;
        return;
    }
    uint32_t ilen = getlen(fin);
    cout << "Uncompressed file size: " << ilen << " bytes" << endl;
    byte *idata = new byte[ilen];
    fin.read(reinterpret_cast<char*>(idata), ilen);
    fin.close();
    vector<byte> odata = compressAlgo1(idata, ilen);
    delete idata;
    ofstream fout(out, ios::binary);
    if(fout.fail())
    {
        cout << "Failed to create file " << out << endl;
        return;
    }
    fout.write(reinterpret_cast<char*>(&odata[0]), odata.size());
    fout.close();
}
void writeFromTrie(vector<byte> &odata, int pos)
{
    odata.resize(odata.size() + nodes[pos].d);
    int opos = odata.size()-1;
    while(pos != 0)
    {
        odata[opos] = nodes[pos].v;
        opos--;
        pos = nodes[pos].prv;
    }
}
vector<byte> decompressAlgo1(byte *_idata, uint32_t ilen)
{
    nodes.clear();
    vector<bool> idata;
    idata.resize(ilen * 8);
    for(uint32_t i=0; i<ilen; i++)
    {
        for(int j=0; j<8; j++)
            idata[i*8 + j] = (_idata[i]>>j) & 1;
    }
    int pos = 0;
    uint32_t MAGIC = readData(idata, pos, 32);
    if(MAGIC != 0x21970001u)
    {
        cout << "Wrong file type. Aborting." << endl;
        exit(EXIT_FAILURE);
    }
    vector<byte> odata;
    uint32_t uncomp_size = readData(idata, pos, 32);
    odata.reserve(uncomp_size);
    cout << "Uncompressed file size: " << uncomp_size << " bytes" << endl;
    uint32_t num_nodes = 1 + readData(idata, pos, 32);
    cout << "Trie nodes: " << num_nodes << endl;
    nodes.resize(num_nodes);
    uint32_t lastMatchPos = readData(idata, pos, 32);
    int cur = 0;
    for(uint32_t i=1; i<num_nodes; i++)
    {
        byte v = readData(idata, pos, 8);
        uint32_t prv = readData(idata, pos, cur);
        nodes[i] = {v, prv};
        writeFromTrie(odata, i);
        if(1u<<cur == i)
            cur++;
    }
    writeFromTrie(odata, lastMatchPos);
    return odata;
}
void decompress(const char *in, const char *out)
{
    nodes.clear();
    ifstream fin(in, ios::binary);
    if(fin.fail())
    {
        cout << "Failed to open file " << in << endl;
        return;
    }
    uint32_t ilen = getlen(fin);
    cout << "Compressed file size: " << ilen << " bytes" << endl;
    byte *idata = new byte[ilen];
    fin.read(reinterpret_cast<char*>(idata), ilen);
    fin.close();
    vector<byte> odata = decompressAlgo1(idata, ilen);
    delete idata;
    ofstream fout(out, ios::binary);
    if(fout.fail())
    {
        cout << "Failed to create file " << out << endl;
        return;
    }
    fout.write(reinterpret_cast<char*>(&odata[0]), odata.size());
    fout.close();
}
int main(int argc, char **argv)
{
    ios::sync_with_stdio(false);
    if(argc == 1)
    {
        cout << "Compress or decompress (c/d)? ";
        string option = "@@";
        while(option!="c" && option!="d")
            cin >> option;
        string inputName, outputName;
        cout << "Enter input file name: ";
        cin >> inputName;
        cout << "Enter output file name: ";
        cin >> outputName;
        if(option == "c")
            compress(inputName.c_str(), outputName.c_str());
        else decompress(inputName.c_str(), outputName.c_str());
        cout << "Press any key to exit";
        cin.get();
        cin.get();
    }
    else
    {
        if(argc != 4)
            cout << "Invalid number of arguments\n";
        else
        {
            if(!strcmp(argv[1], "c"))
                compress(argv[2], argv[3]);
            else if(!strcmp(argv[1], "d"))
                decompress(argv[2], argv[3]);
            else cout << "Invalid first argument " << argv[1] << endl;
        }
    }
    cout << "Program finished" << endl;
    return 0;
}
