#pragma once

#include <iostream>
#include <fstream>
#include <string.h>
using namespace std;

typedef	unsigned char uchar;
typedef	unsigned short ushort;
typedef	unsigned int uint;
typedef	unsigned long ulong;

//*----------------------HEADER FIELD
static struct
{
	char FileName[30][50];
	char Key[30][10];
	size_t Num;

}FileAndKey;

static struct
{
	char BMSTrack[7][6];
	char ScoreTrack[7][3];

}BMSAndScore;

static char SourePath[100];
static char DestinationPath[100];


//*---------------------- INDEX FIELD

static struct
{
	char Number[400][4];
	char MusicName[400][50];
	size_t Num;
}Index;

//*---------------------- WORKSTATION FIELD
static struct
{
	char Name[400][100];
	size_t Num;
}Works;


struct OneBMS
{
	char con[3];	//
	char XiaoJie[4];	// 000 001 002 ¡­¡­
	char data[1024];	//´æ´¢Òô·û
	size_t Num; //ÊÇdata³¤¶È£¡
};

extern "C" _declspec(dllexport) int BeginMakeFromBMS(char config[], char source[]);
