// **********************************************************************************
// Author: Shubham Annigeri	github.com/ShubhamAnnigeri
// co-Author: Sunit Raut   	github.com/SunitRaut
// **********************************************************************************
// License
// **********************************************************************************
// This program is free software; you can redistribute it 
// and/or modify it under the terms of the GNU General    
// Public License as published by the Free Software       
// Foundation; either version 3 of the License, or        
// (at your option) any later version.                    
//                                                        
// This program is distributed in the hope that it will   
// be useful, but WITHOUT ANY WARRANTY; without even the  
// implied warranty of MERCHANTABILITY or FITNESS FOR A   
// PARTICULAR PURPOSE. See the GNU General Public        
// License for more details.                              
//                                                        
// Licence can be viewed at                               
// http://www.gnu.org/licenses/gpl-3.0.txt
//
// Please maintain this license information along with authorship
// and copyright notices in any redistribution of this code
// **********************************************************************************

#ifndef tinyECC_h
#define tinyECC_h

#include <tinyECC_mapping_table.h>


class tinyECC
{
public:
static String plaintext;
static String ciphertext;
int Sig[2];

tinyECC();
void encrypt();
void genSig();
void decrypt();
bool verifySig();


protected:
static int a;
static int b;
static int p;

static int PrivKey;
static int PubKey[2];
static int Pbase[2];
static int PubSer[2];
static int PbaseSer[2];
static int TempArr[2];

int E[4];
int P[4];
int m;

void encode(int m[2],int Pb[2],int Pbase[2]);

void sclr_mult(int k,int P[2]);
void add( int P[2],int Q[2]);

int isPAI(int *point);
int* getPAI(int *point);
long int inverse(long int num);
int inverse1(int num);

void processCipherText();
void decode();
    
};

#endif