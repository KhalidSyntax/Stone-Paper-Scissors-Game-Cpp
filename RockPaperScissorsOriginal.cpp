#define _CRT_SECURE_NO_WARNINGS

#include <iostream>
#include <cmath>
#include <vector>
#include <fstream>
#include <chrono>
#include <ctime>
#include <iomanip>
#include <string>

using namespace std;

string PlayerName = "Player1";

enum enGameChoice { Stone = 1, Paper = 2, Scissors = 3 };
enum enWinner { Player1 = 1, Computer = 2, Draw = 3 };

struct stRoundInfo
{
	short RoundNumber = 0;
	enGameChoice Player1Choice;
	enGameChoice ComputerChoice;
	enWinner Winner;
	string WinnerName;
};

struct stGameResults
{
	short GameRound = 0;
	short Player1WinTimes = 0;
	short ComputerWinTimes = 0;
	short DrawTimes = 0;
	int PlayerPoints = 0; // new
	int ComputerPoints = 0; // new
	enWinner GameWinner;
	string WinnerName = "";
	vector<stRoundInfo>RoundHistory; // new, for history
};

int RandomNumber(int From, int To)
{
	int RandNum = rand() % (To - From + 1) + From;

	return RandNum;
}

string WinnerName(enWinner Winner)
{
	string arrWinnerName[3] = { PlayerName, "Computer", "Draw" };
	return arrWinnerName[Winner - 1];
}

enWinner WhoWonTheRound(stRoundInfo RoundInfo)
{
	if (RoundInfo.Player1Choice == RoundInfo.ComputerChoice)
	{
		return enWinner::Draw;
	}

	switch (RoundInfo.Player1Choice)
	{
	case enGameChoice::Stone:
		if (RoundInfo.ComputerChoice == enGameChoice::Paper)
		{
			return enWinner::Computer;
		}
		break;

	case enGameChoice::Paper:
		if (RoundInfo.ComputerChoice == enGameChoice::Scissors)
		{
			return enWinner::Computer;
		}
		break;

	case enGameChoice::Scissors:
		if (RoundInfo.ComputerChoice == enGameChoice::Stone)
		{
			return enWinner::Computer;
		}
		break;
	}

	return enWinner::Player1;
}

string ChoiceName(enGameChoice Choice)
{
	string arrGameChoice[3] = { "Stone", "paper", "Scissors" };
	return arrGameChoice[Choice - 1];
}

void SetWinnerScreenColor(enWinner Winner)
{
	switch (Winner)
	{
	case enWinner::Player1:
		system("color 2F");
		break;

	case enWinner::Computer:
		system("color 4F");
		cout << "\a";
		break;

	default:
		system("color 6F");
		break;
	}
}

void PrintRoundResults(stRoundInfo RoundInfo)
{
	cout << "\n________________________________Round[" << RoundInfo.RoundNumber << "]___________________\n\n";

	cout << PlayerName << " Choice  : " << ChoiceName(RoundInfo.Player1Choice) << endl;
	cout << "Computer Choice : " << ChoiceName(RoundInfo.ComputerChoice) << endl;
	cout << "Round Winner    : [" << RoundInfo.WinnerName << "]\n";

	cout << "\n___________________________________________________________\n\n";

	SetWinnerScreenColor(RoundInfo.Winner);
}

enWinner WhoWonTheGame(short Player1WinTimes, short ComputerWinTimes)
{
	if (Player1WinTimes > ComputerWinTimes)
		return enWinner::Player1;

	else if (ComputerWinTimes > Player1WinTimes)
		return enWinner::Computer;

	else
		return enWinner::Draw;
}

stGameResults FillGameResults(int GameRounds, short Player1WinTimes, short ComputerWinTimes, short DrawTimes, const vector<stRoundInfo>& RoundHistory)
{
	stGameResults GameResults;

	GameResults.GameRound = GameRounds;
	GameResults.Player1WinTimes = Player1WinTimes;
	GameResults.ComputerWinTimes = ComputerWinTimes;
	GameResults.DrawTimes = DrawTimes;

	// scoring: win = 3, draw = 1
	GameResults.PlayerPoints = Player1WinTimes * 3 + DrawTimes * 1;
	GameResults.ComputerPoints = ComputerWinTimes * 3 + DrawTimes * 1;

	GameResults.GameWinner = WhoWonTheGame(Player1WinTimes, ComputerWinTimes);
	GameResults.WinnerName = WinnerName(GameResults.GameWinner);
	GameResults.RoundHistory = RoundHistory;

	return GameResults;
}

enGameChoice ReadPlayer1Choice()
{
	short Choice = 1;

	do
	{
		cout << "\nYour Choice: [1]:Stone, [2]:Paper, [3]:Scissors? ";
		cin >> Choice;

	} while (Choice < 1 || Choice > 3);

	return (enGameChoice)Choice;
}

enGameChoice GetComputerChoice()
{
	return (enGameChoice)RandomNumber(1, 3);
}

stGameResults PlayGame(short HowManyRounds)
{
	stRoundInfo RoundInfo;

	vector<stRoundInfo>RoundHistory; // collect Rounds
	short Player1WinTimes = 0, ComputerWinTimes = 0, DrawTimes = 0;

	for (short GameRound = 1; GameRound <= HowManyRounds; GameRound++)
	{
		cout << "\nRound [" << GameRound << "] begins: \n";
		RoundInfo.RoundNumber = GameRound;
		RoundInfo.Player1Choice = ReadPlayer1Choice();
		RoundInfo.ComputerChoice = GetComputerChoice();
		RoundInfo.Winner = WhoWonTheRound(RoundInfo);
		RoundInfo.WinnerName = WinnerName(RoundInfo.Winner);

		if (RoundInfo.Winner == enWinner::Player1)
			Player1WinTimes++;
		else if (RoundInfo.Winner == enWinner::Computer)
			ComputerWinTimes++;
		else
			DrawTimes++;

		PrintRoundResults(RoundInfo);

		RoundHistory.push_back(RoundInfo);
	}

	return FillGameResults(HowManyRounds, Player1WinTimes, ComputerWinTimes, DrawTimes, RoundHistory);
}

void SaveResultsToFile(const stGameResults& game)
{
	ofstream file("GameResults.txt", ios::app);
	auto now = chrono::system_clock::to_time_t(chrono::system_clock::now());

	struct tm timeinfo;
	localtime_s(&timeinfo, &now);

	file << "Date         : " << put_time(&timeinfo, "%Y-%m-%d %H:%M:%S") << "\n";
	file << "Player       : " << PlayerName << "\n";
	file << "Rounds       : " << game.GameRound << "\n";
	file << PlayerName << " Wins  : " << game.Player1WinTimes << "  | Player Points  : " << game.PlayerPoints << "\n";
	file << "Computer Wins: " << game.ComputerWinTimes << " | Computer Points: " << game.ComputerPoints << "\n";
	file << "Draws        : " << game.DrawTimes << "\n";
	file << "Final Winner : " << game.WinnerName << "\n\n";
	file << "Round History:\n";

	for (const auto& r : game.RoundHistory)
	{
		file << "Round [" << r.RoundNumber << "] " <<
			PlayerName << " (" << ChoiceName(r.Player1Choice) <<
			") vs Computer (" << ChoiceName(r.ComputerChoice) <<
			") -> " << r.WinnerName << "\n";
	}

	file << "----------------------------------------------------\n\n";
	file.close();
}

string Tabs(short NumberOfTabs)
{
	string t = "";
	for (int i = 1; i <= NumberOfTabs; i++)
	{
		t = t + "\t";
	}
	return t;
}

void ShowGameOverScreen()
{
	cout << Tabs(2) << "______________________________________________________\n\n";
	cout << Tabs(2) << "__________________+++ Game Over +++___________________\n\n";
	cout << Tabs(2) << "______________________________________________________\n\n";
}

void ShowFinalGameResults(stGameResults GameResults)
{
	cout << Tabs(2) << "____________________[Game Results]____________________\n\n";

	cout << Tabs(2) << "Game Round          : " << GameResults.GameRound << endl;
	cout << Tabs(2) << PlayerName << " Won Times   : " << GameResults.Player1WinTimes << endl;
	cout << Tabs(2) << "Computer Won Times  : " << GameResults.ComputerWinTimes << endl;
	cout << Tabs(2) << "Draw Times          : " << GameResults.DrawTimes << endl;
	cout << Tabs(2) << "Final Winner        : " << GameResults.WinnerName << endl;
	cout << Tabs(2) << PlayerName << " Points       : " << GameResults.PlayerPoints << endl;
	cout << Tabs(2) << "Computer Points     : " << GameResults.ComputerPoints << endl;

	cout << Tabs(2) << "______________________________________________________\n\n";

	SetWinnerScreenColor(GameResults.GameWinner);
}

short ReadHowManyRounds()
{
	short GameRounds = 1;

	do
	{
		cout << "How Many Rounds 1 To 10? \n";
		cin >> GameRounds;

	} while (GameRounds < 1 || GameRounds > 10);

	return GameRounds;
}

void ResetScreen()
{
	system("cls");
	system("color 0F");
}

void StartGame()
{
	char PlayAgain = 'Y';

	cout << "Enter Your Name:";
	getline(cin >> ws, PlayerName);

	do
	{
		ResetScreen();
		stGameResults GameResults = PlayGame(ReadHowManyRounds());
		ShowGameOverScreen();
		ShowFinalGameResults(GameResults);
		SaveResultsToFile(GameResults);

		cout << endl << Tabs(4) << "Do you want to play Again? Y/N?";
		cin >> PlayAgain;

	} while (PlayAgain == 'Y' || PlayAgain == 'y');
}

int main()
{
	srand((unsigned)time(NULL));

	StartGame();

	return 0;
}