#include <iostream>
#include <fstream>
#include <cstdlib>
#include <vector>
#include <map>
#include <bitset>
#include <string>
#include <math.h>
#include <algorithm>
//#include <array>
using namespace std;


map<int, vector<int> > rgb;
map<int, multimap<int, int> > rgbmap;
map<int, vector< bitset<16> > >  intrl;
int len, siz;
int matr[4][4] = {{1, 0, 0, 0}, {1, -1, 0, 0}, {1, -2, 1, 0}, {1, -1, 1, -1}};
int matrt[4][4] = {{1, 1, 1, 1}, {0, -1, -2, -1}, {0, 0, 1, 1}, {0, 0, 0, -1}};


void adap_div2 (int &A0, int &N, int &div, int sum)
{
    if(N == 4)
    {
        A0 = A0/2;
        N = N/2;
    }
    else
    {
        N += 1;
        A0 += sum;
    }
}


void adap_div(int sum, int &div)
{
    int temp1 = (log2(div) + 3)*15, temp2 = temp1 - 15;

    if(sum > temp1) div = (div == 256) ? 256 : 2*div;
    else if (sum <= temp2) div = (div == 1) ? 1 : div/2;

    return;
}

// Reads frequency distribution of each pixel value
void update_val(map<int, multimap<int, int> > &s, int col, int pix)
{
    map<int, int>::iterator it = s[col].find(pix);
    if(it == s[col].end())
    {
        s[col].insert(pair<int, int>(pix, 1));
    }
    else
    {
        it->second += 1;
    }

    return;
}

pair<int, int> flip_pair(const pair<int, int> &p)
{
    return (pair<int, int>(-p.second,p.first));
}


// Maps pixels to actual values
void map_pixels (map<int, vector<int> > &t, vector<bool> &flag)
{
    int j;
    multimap<int, int>::iterator it;
    for(int i = 0; i < 3; i++)
    {
        j = 1;
        while (j < t[i].size())
        {
            if(j%16 == 0)
            {
                j+=1;
                continue;
            }

            it = rgbmap[i].find(t[i][j]);
            if(it != rgbmap[i].end())
            {
                if (!flag[j/16])
                {
                    j = (j/16 + 1)*16;
                    continue;
                }

                if (it->second == 256)
                {
                    flag[j/16] = 0;
                    j = (j/16 + 1)*16;
                    continue;
                }

                    t[i][j] = it->second;
                    j += 1;

            }
            else cout<<"Laudu, code check kar"<<endl;
        }
    }
}


// Output: Key is actual value found in pixel, Value to be mapped onto is value
multimap<int, int> gen_map (const multimap<int, int> &m)
{
    multimap<int, int> o,p;
    multimap<int, int>::iterator it = o.begin();
    transform(m.begin(), m.end(), inserter(o, it), flip_pair);      // Function pointer
    int counter = 0;
    for(it = o.begin(); it != o.end(); it++)
    {
        p.insert(pair<int, int>(it->second, counter));
        counter = (counter >= 256)? 256 : counter + 1;
    }
    return p;
}


// Multiplies two matrices
map<int, vector< int > > MatrixMul ()
{
    int val;
    map<int, vector< int > > temp1, temp2;
    for(int i = 0; i < 3; i++)
    {
        for(int j = 0; j < siz; j = j + 16)
        {
            for(int k = 0; k < 4; k++)
            {
                for(int m = j; m < j+4; m++)
                {
                    val = 0;

                    for(int l = 0; l < 4; l++)
                    {
                        val += matr[k][l]*rgb[i][4*l+m];
                    }
                    temp1[i].push_back((val));
                }
            }
        }
        cout<<"done";
    }

    for(int i = 0; i < 3; i++)
    {
        for(int j = 0; j < siz; j = j + 16)
        {
            for(int k = j; k < 16 + j; k = k + 4)
            {
                for(int m = 0; m < 4; m++)
                {
                    val = 0;

                    for(int l = 0; l < 4; l++)
                    {
                        val += temp1[i][l+k]*matrt[l][m];
                    }
                    temp2[i].push_back((val));
                }
            }
        }
    }


    cout<<"done";
    return temp2;
}


long quantize8 (long no)
{
    return (64*ceil(no/64.0));
}

bitset<8> inttobit (int a)
{
    int quo, rem,i = 0;
    bitset<8> bin;
    //bin.reset();
        quo = a/2;
        rem = a%2;
        while(quo != 0)
        {
            bin[i] = rem;
            i++;
            rem = quo%2;
            quo = quo/2;

        }
        bin[i] = rem;

return bin;

    }

vector <string> bin_truncate (int div)
{
    vector <string> lookup;
    int b = floor(log2(div)) + 1;

    for(int i = 0; i < div; i++)
    {
        string temp = "";
        int quo;
        int rem;

            quo = (i < pow(2, b) - div)? i/2 : (i + pow(2, b) - div)/2;
            rem = (i < pow(2, b) - div)? i%2 : int((i + pow(2, b) - div))%2;
            temp = (rem)? ("1" + temp) : ("0" + temp);
            while(quo!=0 | temp.size()<(b-1))
            {
                rem = quo%2;
                quo = quo/2;
                temp = (rem)? ("1" + temp) : ("0" + temp);
            }

        lookup.push_back(temp);
    }

return lookup;

}

void golomb_enc (vector<bool> flag, int div = 16, int div2 = 4)
{
    //string temp[2];
    //ofstream golomb;
    int q,r;
    long sum = 24, agg = 0, sumrg = 0, sumb = 0;
    //string foo;
    //vector <string> trunc_table = bin_truncate(div);
    //golomb.open("divisors.txt", ios::out | ios::trunc);
    int l = 0;
    for(int i = 0; i < siz; i = i + 16)
    {
        if(flag[i/16])
        {
            for(int p = l; p < l+15; p++)                           // Since pivot has been left out iterate only till 15 values
            {
                //for(int j = 0; j < 2; j++)
                //{
                //cout<<p<<endl;
                q = int((intrl[0][p]).to_ulong()/div);
                //r = int((intrl[0][l]).to_ulong()%div);
                //foo.assign(q,'1');                      // Fill q number of ones
                //foo.push_back('0');
                //temp[j] = foo + "0" + trunc_table[r];
                //sum += q + 1 + trunc_table[r].size();
                sumrg += (q + 1 + log2(div));
                q = int((intrl[1][p]).to_ulong()/div2);
                //r = int((intrl[1][l]).to_ulong()%div2);
                sumb += (q + 1 + log2(div2));
//                if(j!=1)
//                {
//                    golomb << temp[j]<<",";
//                }
//                else
//                {
//                    golomb<<temp[j]<<endl;
//                }
            //}
            }
            l += 15;

            // Adaptive Golomb
            // Uncomment here to get previous function up and running
            //adap_div(sumrg, div);
            //adap_div(sumb, div2);



        }
        else sum = 384;

        sum += sumrg + sumb;
        sum = (sum > 384) ? 384 : quantize8(sum);


        agg += sum;
        sum = 24;
        sumrg = 0;
        sumb = 0;

    }
    cout<<agg/(siz*24.0);
    //golomb.close();
    return;

}




map<int, vector< int > > PrintInOrder ()
{
    ofstream print;
    print.open("dataorder.csv", ios::out | ios::trunc);
    map<int, vector< int > > temp;
    for(int i = 0; i < siz; i = i + 4*len)
    {
        for(int l = 0; l < len; l = l + 4)
        {
            for(int j = 0; j < 4*len; j = j+ len)
            {
                for(int k = 0; k < 4 ; k++)
                {
                    print<<rgb[0][i+l+j+k]<<","<<rgb[1][i+j+k+l]<<","<<rgb[2][i+j+k+l]<<endl;
                    temp[0].push_back(rgb[0][i+j+k+l]);
                    temp[1].push_back(rgb[1][i+j+k+l]);
                    temp[2].push_back(rgb[2][i+j+k+l]);

                }

            }

        }

    }
    print.close();
    return temp;

}


int main()
{
    //map<char, vector< bitset<8> > > rgb;
    ifstream image;
    ofstream hist;                                          // ios::out by default
    bitset<16> enc1, enc2;
    vector<bool> check;
    image.open("data.csv");                                //Istream is ios::in by default
    string col;

    int soo;
    while(1)
    //for(int i = 0; i<8; i++)
    // Reads image file
    {
        getline(image, col, ',');
        if(!image.good()) break;                            //Not included in while loop since 'getline' needs to run one extra time to toggle end
        rgb[0].push_back(atoi(col.c_str()));
        getline(image, col, ',');
        rgb[1].push_back(atoi(col.c_str()));

        getline(image, col) ;                               //Third entry is not delimited by ",". By default end of line.
        rgb[2].push_back(atoi(col.c_str()));
    }

    image.close();

    int wid;
    cout<<"Length:"<<endl;
    cin>>len;
    cout<<"Width:"<<endl;
    cin>>wid;
    siz = wid*len;
    vector<bool> flag (siz/16,1);

    rgb = PrintInOrder();
    rgb = MatrixMul();

///////////////////////////////////////////////
    for(int i = 0; i<3; i++)
    {
        for(int j = 0; j <rgb[i].size(); j++)
        {
            update_val(rgbmap, i, rgb[i][j]);
        }
        rgbmap[i] = gen_map(rgbmap[i]);
    }

    map_pixels(rgb, flag);

///////////////////////////////////////////////
    for(int i = 0; i < siz; i++)
    {
        if(flag[i/16] && (i%16 != 0) )                             // Don't encode pivot
        {
            for(int j = 0; j < 16; j++)
            {
                enc1[j] = inttobit(rgb[j%2][i])[j/2];
                enc2[j] = (j < 8) ? inttobit(rgb[2][i])[j] : 0;
            }
            intrl[0].push_back(enc1);
            intrl[1].push_back(enc2);
        }
    }
//    cout<<intrl[0].size()<<endl<<intrl[1].size();
//    cout<<"done";
//    hist.open("Output_transform_map.csv", ios::out | ios::trunc);   // Output mode OR Truncate mode
//
//    for(int i = 0; i < siz; i++)
//    {
//        hist << rgb[0][i]<<","<<rgb[1][i]<<","<<rgb[2][i]<<endl;
//    }
//
//    hist.close();
//
//    hist.open("Output_transform16int.csv", ios::out | ios::trunc);   // Output mode OR Truncate mode
//    for(int i = 0; i < intrl[0].size(); i++)
//    {
//        hist << intrl[0][i].to_ulong()<<","<<intrl[1][i].to_ulong()<<endl;//","<<rgb[2][i].to_string()<<endl;
//    }
//
//    hist.close();

//multimap<int, int> :: iterator it;
//hist.open("freq.txt", ios::out | ios::trunc);   // Output mode OR Truncate mode
//for(it = rgbmap[0].begin(); it != rgbmap[0].end();it++)
//{
//    hist<<it->first<<","<<it->second<<endl;
//}
//hist.close();


golomb_enc(flag,4,4);


   return 0;
}


