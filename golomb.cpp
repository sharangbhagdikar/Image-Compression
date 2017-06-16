#include <iostream>
#include <fstream>
#include <cstdlib>
#include <vector>
#include <map>
#include <bitset>
#include <string>
#include <math.h>

using namespace std;

map<int, vector< bitset<8> > > rgb;
map<int, vector< bitset<16> > >  intrl;

map<int, vector<bitset<8> > > PrintInOrder (int siz)
{
    ofstream print;
    print.open("Imageorder.csv", ios::out | ios::trunc);
    map<int, vector<bitset<8> > > temp;
    int len = sqrt(siz);
    for(int i = 0; i < siz; i = i + 4*len)
    {
        for(int l = 0; l < len; l = l + 4)
        {
            for(int j = 0; j < 4*len; j = j+ len)
            {
                for(int k = 0; k < 4 ; k++)
                {
                    print<<rgb[0][i+l+j+k].to_ulong()<<","<<rgb[1][i+j+k+l].to_ulong()<<","<<rgb[2][i+j+k+l].to_ulong()<<endl;
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


long quantize8(long no)
{
    return (8*ceil(no/8.0));
}

void golomb_enc(int siz, int div, int colors)
{
    string temp[colors];
    ofstream golomb;
    int q,r;
    string foo;
    vector <string> trunc_table = bin_truncate(div);
    golomb.open("golomb4_enc.txt", ios::out | ios::trunc);
    for(int i = 0; i < siz; i++)
    {
        for(int j = 0; j < colors; j++)
        {

            q = int((rgb[j][i]).to_ulong()/div);
            r = int((rgb[j][i]).to_ulong()%div);
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
    int r = 0;
    long q = 0;
    long sum = 48,agg = 0;
    //int len = sqrt(siz);
    //string foo;
    //vector <string> trunc_table = bin_truncate(div);
    //golomb.open("golomb.txt", ios::out | ios::trunc);

    for(int i = 0; i < siz; i = i + 16)
    {
        for(int l = 1; l < 16; l++)
        {
            //for(int p = 3; p >= 0; p--)
            //{
                //for(int k = 3*len; k >= 0 ; k = k - len)
                //{
                    //for(int j = 0; j < 2; j++)
                    //{

                        q = ((intrl[0][l+i]).to_ulong()/div);
                        r = ((intrl[0][l+i]).to_ulong()%div);
                        //foo.assign(q,'1');                      // Fill q number of ones
                        //foo.push_back('0');
                        //temp[j] = foo + "0" + trunc_table[r];
                        //cout<<p+k<<" : "<<(q + 1 + trunc_table[r].size())<<endl;
                        sum += q+1+(log2(div));

                        q = ((intrl[1][l+i]).to_ulong()/div2);
                        r = ((intrl[1][l+i]).to_ulong()%div2);
                        sum += q+1+(log2(div2));

                        //sum += (q + 1 + trunc_table[r].size())
//                        if(j!=1)
//                        {
//                            golomb <<temp[j]<<",";
//                        }
//                        else
//                        {
//                            golomb<<temp[j]<<endl;
//                        }

                    //}
                //}
            //}

            //golomb<<i+l<<" :"<<sum<<endl;

        }
        sum = (sum > 384) ? 384 : quantize8(sum);
        //cout<<i<<" : "<<sum<<endl;
        agg += sum;
        sum = 48;

    }
    cout<<agg/(1024.0*1024*24);
    //golomb.close();
    return ;

}

int main()
{
    //map<char, vector< bitset<8> > > rgb;
    ifstream image;
    ofstream hist;
    bitset<16> enc1, enc2;                                  //ios::out by default

    image.open("Image.csv");                                //Istream is ios::in by default
    string col;
    while(1)
    //for(int i = 0; i<8; i++)
    // Reads image file
    {
        getline(image, col, ',');
        if(!image.good()) break;                            //Not included in while loop since 'getline' needs to run one extra time to toggle end
        rgb[0].push_back(inttobit(atoi(col.c_str())));
        getline(image, col, ',');
        rgb[1].push_back(inttobit(atoi(col.c_str())));
        getline(image, col) ;                              //Third entry is not delimited by ","
        rgb[2].push_back(inttobit(atoi(col.c_str())));

    }
    image.close();

// Method one involves xor

    int len = sqrt(rgb[0].size());
    int siz = len*len;
    rgb = PrintInOrder(siz);

   for(int i = 0; i < siz; i = i + 16)
    {
        for(int l = 1; l < 16; l++)
        {
            //for(int j = 3; j >= 0; j--)
            //{
                //for(int k = 3*len; k >= 0 ; k = k - len)
                //{
                    //if(k!=0 | j!=0)
                    //{
                        rgb[0][i+l] ^= rgb[0][i];
                        rgb[1][i+l] ^= rgb[1][i];
                        rgb[2][i+l] ^= rgb[2][i];

                    //}
                //}

            //}

        }

    }

// Interleaving operation
    for(int i = 0; i < siz; i++)
    {
        for(int j = 0; j < 16; j++)
        {
            enc1[j] = rgb[j%2][i][j/2];
            enc2[j] = (j < 8) ? rgb[2][i][j] : 0 ;
        }
        intrl[0].push_back(enc1);
        intrl[1].push_back(enc2);
    }

//cout<<intrl[0][4097]<<endl;
//cout<<intrl[0][2]<<endl;

//    hist.open("Output_xor8.csv", ios::out | ios::trunc);   // Output mode OR Truncate mode
//    for(int i = 0; i < siz; i++)
//    {
//        hist << rgb[0][i].to_string()<<","<<rgb[1][i].to_string()<<","<<rgb[2][i].to_string()<<endl;
//    }
//
//    hist.close();
//    hist.open("Output_xor16.csv", ios::out | ios::trunc);   // Output mode OR Truncate mode
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
//int div[2] = {256,16};
//cout<<div[0];
golomb_enc2(siz,128,8);
//cout<<quantize8(344);


   return 0;
}


