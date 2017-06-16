#include <iostream>
#include <fstream>
#include <cstdlib>
#include <vector>
#include <map>
#include <bitset>
#include <string>
#include <math.h>

using namespace std;

map<int, vector<int> > rgb;
map<int, vector< bitset<16> > >  intrl;

long quantize8(long no)
{
    return (8*ceil(no/8.0));
}

bitset<8> inttobit(int a)
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

vector <string> bin_truncate(int div)
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
//struct golombcode
//{
//    string quo;
//    string rem;
//    golombcode(int q, int r)
//    {
//
//    }
//};

void golomb_enc(int siz, int div, int colors)
{
    string temp[colors];
    ofstream golomb;
    int q,r;
    string foo;
    vector <string> trunc_table = bin_truncate(div);
    golomb.open("golomb.txt", ios::out | ios::trunc);
    for(int i = 0; i < siz; i++)
    {
        for(int j = 0; j < colors; j++)
        {

            q = int(rgb[j][i]/div);
            r = int(rgb[j][i]%div);
            foo.assign(q,'1');                      // Fill q number of ones
            //foo.push_back('0');
            temp[j] = foo + "0" + trunc_table[r];

            if(j!=colors-1)
            {
                golomb << temp[j]<<",";
            }
            else
            {
                golomb<<temp[j]<<endl;
            }
        }
    }
    golomb.close();
    return;

}

void golomb_enc2(int siz, int div = 16, int div2 = 16)
{
    //string temp[2];
    //ofstream golomb;
    int q,r;
    long sum = 48, agg = 0;
    //string foo;
    //vector <string> trunc_table = bin_truncate(div);
    //golomb.open("golomb_enc_sub.txt", ios::out | ios::trunc);

    for(int i = 0; i < siz; i = i + 16)
    {
        for(int l = 1; l < 16; l++)
        {
            //for(int j = 0; j < 2; j++)
            //{

                q = int((intrl[0][i+l]).to_ulong()/div);
                r = int((intrl[0][i+l]).to_ulong()%div);
                //foo.assign(q,'1');                      // Fill q number of ones
                //foo.push_back('0');
                //temp[j] = foo + "0" + trunc_table[r];
                //sum += q + 1 + trunc_table[r].size();
                sum += (q + 1 + log2(div));
                q = int((intrl[1][i+l]).to_ulong()/div2);
                r = int((intrl[1][i+l]).to_ulong()%div2);
                sum += (q + 1 + log2(div2));
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
        sum = (sum > 384) ? 384 : quantize8(sum);
        //cout<<i<<" : "<<sum<<endl;
        agg += sum;
        sum = 48;

    }
    cout<<agg;
    //golomb.close();
    return;

}

map<int, vector< int > > PrintInOrder (int siz)
{
    ofstream print;
    print.open("Imageorder.csv", ios::out | ios::trunc);
    map<int, vector< int > > temp;
    int len = sqrt(siz);
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
    image.open("Image.csv");                                //Istream is ios::in by default
    string col;
    while(1)
    //for(int i = 0; i<8; i++)
    // Reads image file
    {

        getline(image, col, ',');
        if(!image.good()) break;                            //Not included in while loop since 'getline' needs to run one extra time to toggle end
        rgb[0].push_back(atoi(col.c_str()));
        getline(image, col, ',');
        rgb[1].push_back(atoi(col.c_str()));
        getline(image, col) ;                              //Third entry is not delimited by ","
        rgb[2].push_back(atoi(col.c_str()));

    }
    image.close();

        //cout<<rgb['r'][0]<<endl;
//    cout<<rgb[2][rgb[0].size()-1]<<endl;

// Method one involves xor

    int len = sqrt(rgb[0].size());
    int siz = len*len;
    rgb = PrintInOrder(siz);

    for(int i = 0; i < siz; i = i + 16)
    {
        for(int l = i + 1; l < i + 16; l++)
        {
            //for(int j = 3; j >= 0; j--)
            //{
                //for(int k = 3*len; k >= 0 ; k = k - len)
                //{
                    //if(k!=0 | j!=0)
                    //{
                        rgb[0][l] = abs(rgb[0][l] - rgb[0][i]);
                        rgb[1][l] = abs(rgb[1][l] - rgb[1][i]);
                        rgb[2][l] = abs(rgb[2][l] - rgb[2][i]);

                    //}
                //}

            //}

        }

    }
    cout<<rgb[0][2]<<endl;



    for(int i = 0; i < siz; i++)
    {
        for(int j = 0; j < 16; j++)
        {
            enc1[j] = inttobit(rgb[j%2][i])[j/2];
            enc2[j] = (j < 8) ? inttobit(rgb[2][i])[j] : 0;
        }
        intrl[0].push_back(enc1);
        intrl[1].push_back(enc2);

    }

//    hist.open("Output_sub8int.csv", ios::out | ios::trunc);   // Output mode OR Truncate mode
//
//    for(int i = 0; i < siz; i++)
//    {
//        hist << rgb[0][i]<<","<<rgb[1][i]<<","<<rgb[2][i]<<endl;
//    }
//
//    hist.close();
//
//    hist.open("Output_sub16.csv", ios::out | ios::trunc);   // Output mode OR Truncate mode
//    for(int i = 0; i < siz; i++)
//    {
//        hist << intrl[0][i].to_string()<<","<<intrl[1][i].to_string()<<endl;//","<<rgb[2][i].to_string()<<endl;
//    }
//
//    hist.close();

//vector <string> x = bin_truncate(6);
//for(int i = 0; i<6;i++)
//{
//    cout<<x[i]<<endl;
//}
golomb_enc2(siz,256,4);


   return 0;
}


