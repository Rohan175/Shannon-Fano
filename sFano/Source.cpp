#include <iostream>
#include <fstream>
#include <map>
#include <bitset>
#include <stdlib.h>
using namespace std;

class sFano
{

private:
	struct node
	{
		char ch;
		float p;
	};

	int tsize;
	node *ptable;
	map<char, string> bits;

	static int compare_probs(const void *elem1, const void *elem2)
	{
		const node a = *(node *)elem1;
		const node b = *(node *)elem2;
		if (a.p < b.p)
			return 1;
		else if (a.p > b.p)
			return -1;
		return 0;
	}

	void encodeRecursize(int start, int end)
	{
		cout << endl << "Encoding : " << start << " " << end << endl;
		if (start == end)
		{
			return;
		}
		else if (end - start == 1)
		{
			bits[ptable[start].ch] += '0';
			bits[ptable[end].ch] += '1';
		}
		else
		{
			float p = 0, splitIndex = -1, total = 0;
			
			for (int i = start; i <= end; ++i)
			{
				total += ptable[i].p;
			}

			total /= 2;

			for(int i = start; i <= end; ++i)
			{
				p += ptable[i].p;
				cout << ptable[i].p << " ";
				if (p <= total)
				{
					bits[ptable[i].ch] += '0';
				}
				else
				{
					bits[ptable[i].ch] += '1';
					if (splitIndex < 0)
						splitIndex = i;
				}
			}

			if (splitIndex < 0)
				splitIndex = start + 1;

			encodeRecursize(start, splitIndex - 1);
			encodeRecursize(splitIndex, end);
		}
	}

public:

	void Decode(const char *input, const char *output){

		ifstream inputData;
		inputData.open(input);
		cout << "Decoded called." << endl;
		if (inputData.is_open())
		{
			ifstream dataTable;
			dataTable.open("dataTable.rs17");
			if (dataTable.is_open())
			{
				string c;
				char ch;
				map<string, char> mDataMap;
				ofstream decodedFile;
				decodedFile.open("decoded.txt");
				int total;
				dataTable >> total;

				while (dataTable >> c >> ch) {
					cout << c << " : " << ch << endl;
					mDataMap.insert({ c,ch });
				}
				

				string bits = "";
				int count = 0;
				while (inputData >> noskipws >> ch)
				{
					bitset<8> b(ch);
					for (int i = 7; i > -1; i--) {
						cout << b.test(i) << endl;
						if (b.test(i) == 1) {
							bits += '1';
						}
						else {
							bits += '0';
						}
							
						cout << bits << endl;
						if (mDataMap.find(bits) != mDataMap.end()) {

							if (count == total) {
								break;
							}
							count++;
							cout << "Found : " << mDataMap[bits] << endl;
							decodedFile << mDataMap[bits];
							bits = "";
						}
					}

				}
				cout << endl;

			}
			else {
				cout << "Error opening input data table file for decoding." << endl;
			}
		}
		else
			cout << "Error opening input file for decoding." << endl;
	}

	void Encode(const char *input, const char *output)
	{
		char ch;
		ifstream inputData;
		inputData.open(input);
		if (inputData.is_open())
		{
			map<char, int> frequencyMap;
			int i;
			unsigned total = 0;
			cout << "Reading Files : \n";
			while (inputData >> noskipws >> ch)
			{
				cout << ch;
				frequencyMap[ch]++;
				total++;
			}
			cout << endl;

			tsize = (int)frequencyMap.size();

			ptable = new node[tsize];

			float ftot = float(total);
			map<char, int>::iterator fi;
			for (fi = frequencyMap.begin(), i = 0; fi != frequencyMap.end(); ++fi, ++i)
			{
				ptable[i].ch = (*fi).first;
				ptable[i].p = float((*fi).second) / ftot;
			}
			qsort(ptable, tsize, sizeof(node), compare_probs);

			ofstream logOutputFile;
			logOutputFile.open("dataTable.rs17");
			logOutputFile << total << endl;

			cout << "\nCount : " << tsize << "\nTotal :" << total << endl << "\nFrequencies : " << endl;

			for (i = 0; i < tsize; i++)
			{
				cout << ptable[i].ch << " " << ptable[i].p << endl;
			}

			encodeRecursize(0, tsize - 1);
			
			inputData.clear();
			inputData.seekg(0, inputData.beg);
			ofstream output(output, ios::binary);

			unsigned char buffer = 0;
			unsigned int count = 0;
			cout << "Writing output : " << endl;
			while (inputData >> noskipws >> ch)
			{
				for (char c : bits[ch]) {
					
					buffer <<= 1;
					if (c == '1') {
						buffer |= 1;
					}	
					count++;

					if (count == 8)
					{
						bitset<8> x(buffer);
						cout << "Buffer : " << x << endl;
						output << buffer;
						buffer = 0;
						count = 0;
					}
				}
			}

			cout << endl;

			if (count != 0)
			{
				while (count != 8)
				{
					count++;
					buffer <<= 1;
				}
				output << buffer;

				bitset<8> x(buffer);
				cout << "Buffer : " << x << endl;
			}

			cout << endl;

			cout << endl << "Generated Codes : " << endl;
			for (i = 0; i < tsize; i++)
			{
				cout << ptable[i].ch << " " << bits[ptable[i].ch] << endl;
				logOutputFile << bits[ptable[i].ch] << " " << ptable[i].ch << endl;
			}

			inputData.close();
		}
		else
			cout << "Unable to open file";
	}
};

int main()
{

	sFano mSFano;

	mSFano.Encode("data.txt", "encoded.rs17");
	mSFano.Decode("encoded.rs17","decodedData.txt");
	getchar();
	return 0;
}