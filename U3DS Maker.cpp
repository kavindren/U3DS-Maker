#define _CRT_SECURE_NO_WARNINGS
#include <iostream>
#include <fstream>
#include <string>
#include <stdlib.h>
#include <Windows.h>
#include <wininet.h>
#include <urlmon.h>
#include <filesystem>
#include <unzip.h>
#include <ctime>
#pragma comment(lib, "wininet.lib")
#pragma comment(lib, "urlmon.lib")

using namespace std;
namespace fs = std::filesystem;

bool checkOS() { // used to check OS
#ifdef _WIN64
	return true;
#else
	return false;
#endif
}

int main()
{
	SetConsoleTitleA("U3DS maker");
	bool bConnect = InternetCheckConnectionA("https://www.google.com", FLAG_ICC_FORCE_CONNECTION, 0);
	if (!checkOS()) {
		cout << "Sorry, this program works only for Windows x64." << endl;
		return -1;
	}

	if (!bConnect) {
		cout << "The program couldn't establish internet connection. Please check your internet connection and try again." << endl;
		return 1;
	}

	cout << "Welcome to U3DS maker." << endl << "Please enter directory where to download SteamCMD executable (e. g. C:\\Users\\kavindren\\scmd)" << endl;
	fs::path pathSCMD;
	cin >> pathSCMD;

	if (!fs::is_directory(pathSCMD) or fs::exists(pathSCMD)) {
		fs::create_directory(pathSCMD);
	}

	cout << "Starting SteamCMD download to " << pathSCMD << endl;

	fs::current_path(pathSCMD);

	if (E_OUTOFMEMORY == URLDownloadToFileA(NULL, "http://steamcdn-a.akamaihd.net/client/installer/steamcmd.zip", "steamcmd.zip", 0, NULL)) {
		cout << "There was an error while downloading SteamCMD (E_OUTOFMEMORY)" << endl;
		return 2;
	}
	if (INET_E_DOWNLOAD_FAILURE == URLDownloadToFileA(NULL, "http://steamcdn-a.akamaihd.net/client/installer/steamcmd.zip", "steamcmd.zip", 0, NULL)) {
		cout << "There was an error while downloading SteamCMD (INET_E_DOWNLOAD_FAILURE)" << endl;
		return 3;
	}

	// extracting steamcmd.zip

	cout << "Extracting steamcmd.zip" << endl;

	unzFile zipFile = unzOpen("steamcmd.zip");
	if (zipFile == nullptr) {
		cerr << "Error: Could not open zip file" << endl;;
		return 4;
	}

	unz_file_info fileInfo;
	char filename[256];
	int err;
	do {
		err = unzGetCurrentFileInfo(zipFile, &fileInfo, filename, sizeof(filename), nullptr, 0, nullptr, 0);
		if (err != UNZ_OK) {
			break;
		}

		unzOpenCurrentFile(zipFile);

		char buffer[4096];
		ofstream outFile(filename, ofstream::binary);
		int read = unzReadCurrentFile(zipFile, buffer, sizeof(buffer));

		while (read > 0) {
			outFile.write(buffer, read);
			read = unzReadCurrentFile(zipFile, buffer, sizeof(buffer));
		}

		outFile.close();
		unzCloseCurrentFile(zipFile);

	} while (unzGoToNextFile(zipFile) == UNZ_OK);

	unzClose(zipFile);

	remove("steamcmd.zip");

	// installing U3DS through SteamCMD

	system("steamcmd.exe +login anonymous +app_update 1110390 +exit");

	system("cls");

	fs::current_path("steamapps\\common\\U3DS");

	// configuring a copy of ExampleServer.bat

	char LoI;
	cout << "Choose type of server connection you would like to use:\nl - Lan\ni - Internet" << endl;
	cin >> LoI;
	string serverName;
	cout << "Enter the server name: ";
	cin >> serverName;
	cout << endl;

	ofstream startBAT;
	startBAT.open("StartU3DS.bat");
	if (LoI == 'l') {
		startBAT << "@echo off\nstart \"\" \"%~dp0ServerHelper.bat\" +LanServer/" << serverName << "\nexit";
	}
	else {
		startBAT << "@echo off\nstart \"\" \"%~dp0ServerHelper.bat\" +InternetServer/" << serverName << "\nexit";
	}
	startBAT.close();

	system("StartU3DS.bat");

	Sleep(40000);

	system("taskkill /f /im Unturned.exe");

	fs::path serverDir = serverName;

	fs::current_path("Servers");
	fs::current_path(serverDir);
	fs::current_path("Server");

	system("cls");

	// configuring Commands.dat

	string serverIp, loadout, log, map, mode, name, owner, password, perspective, welcome, gslt, filter, gold, pve, sync;
	int chatrate, maxplayers, cycle, decay, port, timeout;

	cout << "Configuring \"Commands.dat\" file. See wiki for documentation." << endl << endl;

	cout << "Enter the IP the server will be bind to." << endl;
	cin >> serverIp;
	cout << "Enter the chat rate" << endl;
	cin >> chatrate;
	cout << "Enter day cycle" << endl;
	cin >> cycle;
	cout << "Enter the decay" << endl;
	cin >> decay;
	cout << "Enable old name filter? y/n" << endl;
	cin >> filter;
	cout << "Restrict the server to only allow Gold players? y/n" << endl;
	cin >> gold;
	cout << "Enter the loadout (enter 0 to skip)" << endl;
	cin >> loadout;
	cout << "Enter logging parametrs (enter 0 to skip)" << endl;
	cin >> log;
	cout << "Enter map name" << endl;
	cin >> map;
	cout << "Enter the max amount of players" << endl;
	cin >> maxplayers;
	cout << "Enter the server difficulty" << endl;
	cin >> mode;
	cout << "Enter the server's name" << endl;
	cin >> name;
	cout << "Enter the server's owner's SteamID64 (enter 0 to skip)" << endl;
	cin >> owner;
	cout << "Enter the perspective mode" << endl;
	cin >> perspective;
	cout << "Enter server's password (enter 0 to skip)" << endl;
	cin >> password;
	cout << "Enter server's port (enter 0 to leave as default)" << endl;
	cin >> port;
	cout << "Enable PvE mode? y/n" << endl;
	cin >> pve;
	cout << "Enable sync player data between servers? y/n" << endl;
	cin >> sync;
	cout << "Enter the max ping for players before they time out" << endl;
	cin >> timeout;
	cout << "Enter the welcome message (enter 0 to skip)" << endl;
	cin >> welcome;
	cout << "Enter GSLT token" << endl;
	cin >> gslt;

	remove("Commands.dat");

	ofstream commands;
	commands.open("Commands.dat");

	commands << "Bind " << serverIp << endl;
	commands << "Chatrate " << chatrate << endl;
	commands << "Cycle" << cycle << endl;
	commands << "Decay " << decay << endl;
	if (filter != "0") commands << "Filter" << endl;
	if (gold != "0") commands << "Gold" << endl;
	if (loadout != "0") commands << "Loadout " << loadout << endl;
	if (log != "0") commands << "Log " << log << endl;
	commands << "Map " << map << endl;
	commands << "Maxplayers " << maxplayers << endl;
	commands << "Mode " << mode << endl;
	commands << "Name " << name << endl;
	if (owner != "0") commands << "Owner " << owner << endl;
	if (password != "0") commands << "Password " << password << endl;
	commands << "Perspective " << perspective << endl;
	if (port != 0) commands << "Port " << port << endl;
	if (pve != "0") commands << "Pve" << endl;
	if (sync != "0") commands << "Sync" << endl;
	commands << "Timeout " << timeout << endl;
	if (welcome != "0") commands << "Welcome " << welcome << endl;
	commands << "gslt " << gslt << endl;

	commands.close();

	cout << "The start file \"StartU3DS.bat\" was created in " << pathSCMD << "\\steamapps\\common\\U3DS" << endl;
	cout << "Done making the server!" << endl << "Press any key to exit" << endl;
	system("pause");

	return 0;
}
