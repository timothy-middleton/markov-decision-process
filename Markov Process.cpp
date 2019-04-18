

#include <string>
#include <vector>
#include <fstream>
#include <sstream>
#include <cstdlib>
#include <iostream>

using namespace std;

struct strategyInfo
{
	string strategy;
	double** price;
	double** prob;
};

struct solutionInfo
{
	double sum;
	int* strategies;
};

static void ReadStates(vector<string>& states, int numStates, ifstream& fin)
{
	string strTmp;
	int i = 0;

	do
	{
		getline(fin, strTmp);
		if (strTmp[strTmp.size() - 1] == '\r')
		{
			strTmp.erase(strTmp.size() - 1);
		}
		states.push_back(strTmp);
		i++;
	} while (i < numStates);
}

static void ClearStrategy(strategyInfo strtg, int numStates)
{
	int i;

	for (i = 0; i < numStates; i++)
	{
		delete[] strtg.prob[i];
		delete[] strtg.price[i];
	}
	delete[] strtg.prob;
	delete[] strtg.price;
}

static void AddStrategy(string strategyName, vector<strategyInfo>& strategies, int numStates, ifstream& fin)
{
	strategyInfo strtg;
	string strTmp;
	int i = 0;

	strtg.strategy = strategyName;
	strtg.prob = new double*[numStates];
	for (i = 0; i < numStates; i++)
	{
		strtg.prob[i] = new double[numStates];
	}

	i = 0;
	do
	{
		getline(fin, strTmp);
		if (strTmp[strTmp.size() - 1] == '\r')
		{
			strTmp.erase(strTmp.size() - 1);
		}

		istringstream iss(strTmp);
		string prob;
		int j = 0;
		while (getline(iss, prob, ' '))
		{
			strtg.prob[i][j] = stof(prob);
			j++;
		}
		i++;
	} while (i < numStates);

	strtg.price = new double*[numStates];
	for (i = 0; i < numStates; i++)
	{
		strtg.price[i] = new double[numStates];
	}

	i = 0;
	do
	{
		getline(fin, strTmp);
		if (strTmp[strTmp.size() - 1] == '\r')
		{
			strTmp.erase(strTmp.size() - 1);
		}

		istringstream iss(strTmp);
		string price;
		int j = 0;
		while (getline(iss, price, ' '))
		{
			strtg.price[i][j] = stoi(price);
			j++;
		}
		i++;
	} while (i < numStates);

	strategies.push_back(strtg);
}

bool ReadFile(string filename, vector<string>& states, vector<strategyInfo>& strategies, int& numStages)
{
	ifstream fin(filename, ios::in);
	string strTmp;
	unsigned int lineCount = 0;
	int numStates = 0;
	int numStrategies = 0;
	int strategyCount = 0;

	if (fin.fail())
	{
		return false;
	}
	while (getline(fin, strTmp))
	{
		if (strTmp[strTmp.size() - 1] == '\r')
		{
			strTmp.erase(strTmp.size() - 1);
		}

		if (lineCount == 0)
		{
			numStates = stoi(strTmp);
			lineCount++;
			ReadStates(states, numStates, fin);
			lineCount += states.size();
		}
		else if (lineCount == states.size() + 1)
		{
			numStages = stoi(strTmp);
			lineCount++;
		}
		else if (lineCount == states.size() + 2)
		{
			numStrategies = stoi(strTmp);
			lineCount++;
		}
		else
		{
			AddStrategy(strTmp, strategies, numStates, fin);
			strategyCount++;
		}

		if (numStrategies > 0 && strategyCount == numStrategies)
		{
			break;
		}
	}
	fin.close();

	return true;
}

bool WriteOutput(vector<string>& states, vector<strategyInfo>& strategies, vector<solutionInfo>& solutions, int numStages)
{
	unsigned int i;
	int j;
	ofstream fout("result.txt", ios::out);

	fout << "Optimal strategies:\n";

	if (fout.fail())
	{
		return false;
	}
	for (i = 0; i < states.size(); i++)
	{
		fout << "Start state: " << i << " " << states[i] << "\n";
		for (j = 0; j < numStages; j++)
		{
			int strNum = solutions[i].strategies[j];
			fout << j + 1 << " month. Use strategy " << strNum + 1 << " : " << strategies[strNum].strategy << "\n";
		}
		fout << "Total profit: " << solutions[i].sum << "\n";
		fout << "\n";
	}

	fout.close();

	return true;
}

static double CountStageSum(double prevLevelSum, double* probabilities, double* price, int numStates)
{
	double result = 0;
	int i;

	result += prevLevelSum;
	for (i = 0; i < numStates; i++)
	{
		result += probabilities[i] * price[i];
	}
	return result;
}

void SolveMarcovDesision(vector<strategyInfo>& strategies, vector<solutionInfo>& solutions, int numStates, int numStages)
{
	int i, j, k;
	int numStrategies = strategies.size();
	int* optimalStrategy = new int[numStates];
	double* optimalSum = new double[numStates];
	double* prevStageSum = new double[numStates];
	double** curSums = new double*[numStates];
	double** startSums = new double*[numStates];


	for (i = 0; i < numStates; i++)
	{
		curSums[i] = new double[numStrategies];
		startSums[i] = new double[numStrategies];


		solutionInfo stTmp;
		stTmp.sum = 0;
		stTmp.strategies = new int[numStages];
		for (j = 0; j < numStages; j++)
		{
			stTmp.strategies[j] = 0;
		}
		solutions.push_back(stTmp);
	}

	for (j = 0; j < numStates; j++)
	{
		for (k = 0; k < numStrategies; k++)
		{
			curSums[j][k] = CountStageSum(0, strategies[k].prob[j], strategies[k].price[j], numStates);
			startSums[j][k] = curSums[j][k];
			if (curSums[j][k] > optimalSum[j])
			{
				optimalSum[j] = curSums[j][k];
				optimalStrategy[j] = k;
			}
		}
		solutions[j].strategies[numStages - 1] = optimalStrategy[j];
	}


	for (i = numStages - 2; i >= 0; i--)
	{
		for (j = 0; j < numStates; j++)
		{
			prevStageSum[j] = optimalSum[j];
		}
		for (j = 0; j < numStates; j++)
		{
			for (k = 0; k < numStrategies; k++)
			{
				curSums[j][k] = CountStageSum(startSums[j][k], strategies[k].prob[j], prevStageSum, numStates);
				if (curSums[j][k] > optimalSum[j])
				{
					optimalSum[j] = curSums[j][k];
					optimalStrategy[j] = k;
				}
			}
			solutions[j].strategies[i] = optimalStrategy[j];
		}
	}

	for (i = 0; i < numStates; i++)
	{
		solutions[i].sum = optimalSum[i];
	}
	delete[] optimalStrategy;
	delete[] optimalSum;
	delete[] prevStageSum;
	for (i = 0; i < numStates; i++)
	{
		delete[] curSums[i];
		delete[] startSums[i];
	}
	delete[] curSums;
	delete[] startSums;
}

int main(int argc, char** argv)
{
	string filename;
	unsigned int i;
	if (argc == 2)
	{
		filename = argv[1];
	}
	else
	{
		cout << "Enter input file name\n";
		cin >> filename;
	}

	vector<string> states;
	vector<strategyInfo> strategies;
	int numStages;

	if (!ReadFile(filename, states, strategies, numStages))
	{
		cout << "Can not open file\n";
	}
	else
	{
		cout << "Data is successfully loaded\n";
	}



	/*	for (i = 0; i < strategies.size(); i++)
		{
			cout << strategies[i].strategy << "\n";
			for (int j = 0; j < states.size(); j++)
			{
				for (int k = 0; k < states.size(); k++)
				{
					cout << strategies[i].prob[j][k] << " ";
				}
				cout << "\n";
			}
			for (int j = 0; j < states.size(); j++)
			{
				for (int k = 0; k < states.size(); k++)
				{
					cout << strategies[i].price[j][k] << " ";
				}
				cout << "\n";
			}
		}*/

	vector<solutionInfo> solutions;

	SolveMarcovDesision(strategies, solutions, states.size(), numStages);


	if (!WriteOutput(states, strategies, solutions, numStages))
	{
		cout << "Failed to create output file\n";
	}
	else
	{
		cout << "Output file result.txt was successfully created\n";
	}

	for (i = 0; i < strategies.size(); i++)
	{
		ClearStrategy(strategies[i], states.size());
	}
	for (i = 0; i < states.size(); i++)
	{
		delete[] solutions[i].strategies;
	}

	system("pause");
	return 0;
}
