#include <iostream>
#include <vector>
#include <fstream>
#include <string>
#include <map>
#include <Windows.h>
#include <math.h>
using namespace std;
// read file
/**
 * @brief 
 *
 * ������� ������ ���� � ���������� � ��� ��������� � ������ � ���������� ��
 *
 * @param string filename - �������� ��������� �����
 * @return ���������� string � ���������� �� �����
 */
string readfile(string filename)
{
	char buf;
	string output = "";
	ifstream inp(filename);
	if (inp.is_open())
	{
		while (inp)
		{
			inp >> noskipws >> buf;
			output += buf;
		}
		output.pop_back();
		return output;
	}
	else return "";
}
// read file skipws
/**
 * @brief
 *
 * ������� ������ ���� � ���������� � ��� ��������� � ������ � ���������� ��, ��������� �������
 *
 * @param string filename - �������� ��������� �����
 * @return ���������� string � ���������� �� ����� ��� ��������
 */
string readfileSKIPWS(string filename)
{
	char buf;
	string output = "";
	ifstream inp(filename);
	if (inp.is_open())
	{
		while (inp)
		{
			inp >> noskipws >> buf;
			if (!isspace(buf))output += buf;
		}
		output.pop_back();
		return output;
	}
	else return "";
}
//huffman functions
struct node
{
	int weight = 0;
	char symbol = '\0';
	struct node* left=NULL;
	struct node* right=NULL;
};

struct dictionary
{
	vector <char> symbols;
	vector <int> repetitions;
};
// dict init
/**
 * @brief
 *
 * ������� ������� ��������� dictionary, ���� �������� ��� ���������� ������� ��������� � �� ���-�� ����������
 *
 * @param string input - �������� ���������
 * @return ���������� dictionary � ������� �������� ��� ���������� ������� ����������� ��������� � �� ���-��� ����������
 */
dictionary dict_init(string input)
{
	dictionary dict;
	for (char symbol : input)
	{
		int i = 0;
		while (i < dict.symbols.size() and symbol != dict.symbols[i])i++;
		if(i==dict.symbols.size())
		{
			dict.symbols.push_back(symbol);
			dict.repetitions.push_back(1);
		}
		else
		{
			dict.repetitions[i]++;
		}
	}

	return dict;
}
// build tree
/**
 * @brief
 *
 * ������� ������� ������ �� ����������� ���������� ������� � ���������� ��������� �� ��� ������.
 *
 * @param dictionary - ������� �� �������� �������� ������
 * @return node* - ��������� �� ������ ������ ������������ �� ������� �������
 */
node* build_tree(dictionary input)
{
	vector<node*> tree;
	while (input.symbols.size())
	{
		int min_weight = 0;
		for (int i = 0; i < input.symbols.size(); i++)
		{//����� ������� ������� � ����������� ���-��� ����������
			if (input.repetitions[min_weight] > input.repetitions[i]) min_weight = i;
		}
		node* buf=new node;
		//���������� ������ �� ������� ������������ ���-�� ���������� � node
		buf->weight = input.repetitions[min_weight];
		buf->symbol = input.symbols[min_weight];
		tree.push_back(buf);//��������� � ������
		//������� ����������� �������� �� �������, ��������� ���� �� ��������� ��� ��������
		//����� ������� ������ ����� �������� � ������ tree � ������� ��������.
		input.repetitions.erase(input.repetitions.begin() + min_weight);
		input.symbols.erase(input.symbols.begin() + min_weight);
	}
	while (tree.size() > 1)
	{//�������� ������ � ����� (������ � ������� tree - ���������� ������ ������)
		node* buf = new node;
		//������� ����� ����, ������������ � ���� ����� �������� 2 ����������� ����� �� tree.
		//����� ��������� ���� �����, ���� ������ - ������.
		buf->weight = tree[0]->weight + tree[1]->weight;
		buf->left = tree[0];
		buf->right = tree[1];
		//������� 2 ������ ��������, ��������� ���������� ����� ������,
		//�������� ������� ����������� �� ����
		tree.erase(tree.begin(), tree.begin() + 2);
		int j = 0;
		while (j < tree.size() and tree[j]->weight < buf->weight)j++;
		if (j == tree.size())
		{
			tree.push_back(buf);
		}
		else
		{
			tree.insert(tree.begin() + j, buf);
		}
	}
	//������ ��������� ���� ���� � ������� tree �� ��������� 1 ������� - ������.
	node* root = tree[0];
	return root;
}
// run tree
/**
 * @brief
 *
 * ������� ��������� ���������� ������� ������, ������������ �� ������� ������.
 * ��� ������ � ����� ����� � ���� ����������� 0, ����� - 1. ����� ������� �������� �� ����, �� ���������� ��� ����������� � �������.
 *
 * @param node* root - ������ ������ � �������� �������� ����
 * @param string code - ��� ������ ������� ������ �������� ������ ������ ("")
 * @param map<char,string> codes - �������, ���� ������������ ���, ��������������� ������� �������
 * @return ������ �� ����������, ���� ��������� ���������� map ����������
 */
void runtree(node* root,string code,map<char,string>& codes)
{
	// ��������� ������ ���� � ������� ���� ������ "\0"
	if (root == NULL) 
	{
		code.pop_back();
		codes['\0'] = code;
		return;
	}
	// ��� ������ ������� ������� �� ����� ������ (� ��� ����� ������ �������� �� 0)
	// ��������� ��� �� ������� = ������ � map.
	if (root->symbol)
	{
		codes[root->symbol] = code;
		return;
	}
	runtree(root->left,code+'0',codes);
	runtree(root->right,code+'1',codes);
}
// serialize
/**
 * @brief
 *
 * ������� �������� ��������� �� ���������� ������� � ������, ���������� � ������ ���������� �������, �� �������� ������������ ���������.
 *
 * @param string message - ���������� ���������
 * @param map<char, string> codes - ������� � ������ ��� ���������.
 * @param dictionary dict - ������ ������� ��������� � ������ ���������, ������ � ���������� �����������.
 * @return string - �������������� ���������.
 */
string serialize(string& message, map<char, string>& codes,dictionary& dict)
{
	string output;
	int maxrep = 0;
	unsigned char buf = 0;
	int capacity = 7;
	//����� ������� ������� ��� ����� ��� ������ 1 ���-�� ���������� � �������
	for (int reps : dict.repetitions)
		if (maxrep < reps)maxrep = reps;
	unsigned char entrysize = log2(maxrep);//������ ���� ���� - 1 � �����
	output.push_back(entrysize);//���������� � ����� ����� ��� �� ���- �� ����������
	output.push_back((unsigned char)dict.symbols.size());//���������� ���-�� ������� � �������
	int messize = message.size();//���-�� �������� � ���������

	for (int u = 15; u > -1; u--)
		//���������� ���-�� �������� � ��������� �������� � �����, �������� �� ���� 16 ���
	{
		if ((messize >> u) & 1)
		{
			buf = buf + (1 << capacity);
		}
		capacity--;
		if (capacity == -1)
		{
			capacity = 7;
			output.push_back(buf);
			buf = 0;
		}
	}
	buf = 0;
	capacity = 7;
	
	
	//���������� ������� � ����� ��������
	//������� ������������ ����� ����������, ����� ������
	for (int i = 0; i < dict.symbols.size(); i++)
	{
		int writerep = entrysize;
		while (writerep != -1)
		{
			if ((dict.repetitions[i] >> writerep) & 1)
			{
				buf = buf + (1 << capacity);
			}
			capacity--;
			if (capacity == -1)
			{
				output.push_back(buf);
				buf = 0;
				capacity = 7;
			}
			writerep--;
		}
		int writesymb = 7;
		while (writesymb != -1)
		{
			if ((dict.symbols[i] >> writesymb) & 1)
			{
				buf = buf + (1 << capacity);
			}
			capacity--;
			if (capacity == -1)
			{
				output.push_back(buf);
				buf = 0;
				capacity = 7;
			}
			writesymb--;
		}
	}
	
	//�������� ���� ��������� � ���������� � �����
	for (char symbol : message)
	{
		string code = codes[symbol];
		for ( char bit : code)
		{
			
			if (bit == '1')
			{
				buf += 1 << capacity;
			}
			capacity--;
			if (capacity == -1)
			{
				output.push_back(buf);
				buf = 0;
				capacity = 7;
			}
		}
	}
	output.push_back(buf);



	return output;
}
// decode
/**
 * @brief
 *
 * ������� ���������� ��������� ������ ������� �������� (���������� �� ������� serialize).
 *
 * 
 * 
 * @param string input - �������������� ����������.
 * @return string - ��������������� ���������.
 */
string decode(string& input)
{
	dictionary dict;
	int debuf = 0;
	int readcapacity = 7;
	int i = 4;
	//��������� ���-�� ������ �������
	int entries = (unsigned char)input[1];
	//�.�. �������� � ascii ���� 256, � � unsigned char ������������
	//�������� �� 255 (���� ����� �� ����� 0), ���� ������ ����� ������
	if (entries == 0) entries = 256;
	//��������� ���-�� ��� �� ������ ���������� �������
	int entrysize =(unsigned char) input[0];
	//��������� ���-�� �������� � ������������� ���������
	int todecode = (((unsigned char)input[2]) << 8);
	todecode = todecode + (unsigned char)input[3];
	//������ �������
	while (entries > 0 and i<input.size())
		// ������� c .size � while ����� ����� �������� ������ ���������
		// � ������ ��������������� ��������� ������� (� ������� �������� �� �� ���������) 
	{
		int toread = entrysize;
		while (toread != -1)
		{
			if ((input[i] >> readcapacity) & 1)
			{
				debuf += (1 << toread);
			}
			toread--;
			readcapacity--;
			if (readcapacity == -1)
			{
				i++;
				readcapacity = 7;
			}
		}
		dict.repetitions.push_back(debuf);
		debuf = 0;
		toread = 7;
		while (toread != -1)
		{
			if ((input[i] >> readcapacity) & 1)
			{
				debuf += (1 << toread);
			}
			toread--;
			readcapacity--;
			if (readcapacity == -1)
			{
				i++;
				readcapacity = 7;
			}
		}
		dict.symbols.push_back(debuf);
		debuf = 0;
		entries--;
	}
	// ��������������� ������ �� �������
	node* root = build_tree(dict);

	node* cur = root;

	string result;
	//�������� ������������
	for (i;todecode and i<input.size();i++)
	{
		
		debuf = input[i];
		while (readcapacity != -1)
		{
			//�� ���������� ��� ���, ��� 0 - ����� �� ������
			//���� ��� ������� ������ ���� �� ��������� �� ������
			//���� ������ ����� - ���������� ���
			if ((debuf >> readcapacity--) & 1)
			{
				cur = cur->right;
			}
			else
			{
				cur = cur->left;
			}
			if (!(cur->left or cur->right))
			{
				result.push_back(cur->symbol);
				cur = root;
				todecode--;
				if (!todecode)
				{
					break;
				}
			}
		}
		readcapacity = 7;
		
	}
	return result;
}

//lzw functions

// serializeLZW
/**
 * @brief
 * ������� ����������� ������ ���������� LZW ���������. ���������� ������ ���������� ���������� ��������� ������ ���, ������� � 9
 * @param vector<int> input - �������������� �� LZW ���������.
 * @return string - ��������������� ���������.
 */
string serializeLZW(vector<int>& input)
{
	string output;
	unsigned char sebuf = 0;
	char bitsize = 9;
	int bufpos = 7;
	int towrite = bitsize - 1;
	//���������� ���������� ������ ��������
	//���������� 9 ��� �� ��������
	for (int i = 0; i < input.size(); i++)
	{

		while (towrite > -1)
		{
			if (input[i] & (int)pow(2, towrite))
			{
				sebuf += (int)pow(2, bufpos);
				bufpos--;
			}
			else
			{
				bufpos--;
			}
			if (bufpos == -1)
			{
				output.push_back(sebuf);
				sebuf = 0;
				bufpos = 7;
			}
			towrite--;
		}
		if (input[i] == 256)
		{//���� 256, �� �������� ���-�� ��� �� ��������
			bitsize++;
		}
		towrite = bitsize - 1;
	}
	output.push_back(sebuf);

	return output;
}
// deserializeLZW
/**
 * @brief
 *
 * ������� ������������� ���������, ��������� �������� serializeLZW � �������� ������ ����������.
 *
 * @param string input - ��������������� ���������.
 * @return vector<int> - ����������������� ���������.
 */
vector<int> deserializeLZW(string& input)
{
	vector<int> output;
	int bitsize = 9;

	int bits = bitsize - 1;

	int desbuf = 0;

	for (int i = 0; i < input.size(); i++)
	{
		int toread = 7;

		while (toread > -1)
		{
			if (input[i] & (int)pow(2, toread))
			{
				desbuf += (int)pow(2, bits);
				bits--;
			}
			else
			{
				bits--;
			}
			if (bits == -1)
			{
				if (desbuf == 256)
				{//������� �� 1 ��� ������ ��� ������
					bitsize++;
				}
				bits = bitsize - 1;
				output.push_back(desbuf);

				desbuf = 0;
			}
			toread--;
		}
	}

	return output;
}
// compressLZW_p
/**
 * @brief
 *
 * ������� �������������� ������� ��������� �� ��������� LZW.
 * ������ ��������� ���������� ������������� �������� serializeLZW.
 *
 * @param string input - ���������, ������� ����� ���������.
 * @return vector<int> - ������ ����������, ��� ��� �������� ����� 255 - ������� ASCII, 256 - ���, ��������������� ��� ��� ������������ ���������� ��������� ���-�� ��� �� ������.
 */
vector<int> compressLZW_p(string& input)
{
	vector<int> result;
	vector<string> dict;//������� ���������� ��������
	int bitsize = 9;//���-�� ��� ��� ������ 1 ���������� ��������
	for (int i = 0; i < input.length(); i++)
	{
		int j = 0;
		//���� ���������� ����� ��������� �� ��������
		for (j; j < dict.size() and (input.compare(i, dict[j].length(), dict[j]) != 0); j++);
		if (j == dict.size())
		{
			//���������� ���,
			//��������� ���� ������ � �����
			//���������� ���� ������ � ��������� ��������� � ������.
			dict.push_back(input.substr(i, 2));
			result.push_back((unsigned char)input[i]);
		}
		else
		{
			//����� ���������� �� ��������
			//��������� � ����� ����� ������ � �������, ������� 257 (�.�. 0-255 ������ ascii
			//256 ������ ����� ��� ��������� ��� �� ������)
			dict.push_back(dict[j] + input[i + dict[j].length()]);
			//���� ����� �������� ���-�� ��� �� ������
			if ((j + 257) > (2 << (bitsize - 1)))
			{
				bitsize++;
				result.push_back(256);
			}
			result.push_back(j + 257);
			i = i + dict[j].length() - 1;
		}

	}

	return result;
}
// decompressLZW_p
/**
 * @brief
 *
 * ������� ��������� ���������, ������ �� ��������� LZW.
 * ����� ������� ��������� ��� ����� ��������������� ��� ������ deserializeLZW.
 * @param vector<int> input - ����������������� ���������.
 * @return string - �������� ���������.
 */
string decompressLZW_p(vector<int>& input)
{
	vector<int> outp;
	vector<vector<int>> dickt;
	vector<int>buf;


	outp.push_back(input[0]);
	for (int i = 1; i < input.size(); i++)
	{
		//���������� 256, ��� ����� ������
		//��� ������������
		if (input[i] == 256) continue;
		//���� �������� - ascii ��� �����
		if (input[i] < 257)
		{
			buf.push_back(input[i - 1]);
			buf.push_back(input[i]);
			outp.push_back(input[i]);
			//��������� �������� ������ � ����������
			//� �������
			dickt.push_back(buf);
			buf.clear();
		}
		else//���� ��� - ���������� �������� �� �������
		{
			if (input[i] - 257 == dickt.size())
			{//��������� ������, ����� ��� ������ ������� ����� �������
				// � ����� ������������ ��� �� ��������� ���������� ��������
				if (input[i - 1] < 257)
				{
					buf.push_back(input[i - 1]);
					buf.push_back(input[i - 1]);
					dickt.push_back(buf);
				}
				else
				{
					buf = dickt[input[i - 1] - 257];
					buf.push_back(dickt[input[i - 1] - 257][0]);
					dickt.push_back(buf);
				}
				buf.clear();
			}
			else
			{
				buf.push_back(input[i - 1]);
				buf.push_back(dickt[input[i] - 257][0]);
				dickt.push_back(buf);
				buf.clear();
			}
			vector<int> outpbuf;
			outpbuf.push_back(input[i]);
			bool flag;
			do
			{
				//����� �� ������������ ���������� ��������
				//�� ascii ����, ������ ���� ���������� �������� �� ���������
				flag = false;
				for (int j = 0; j < outpbuf.size(); j++)
				{
					if (outpbuf[j] > 256)
					{
						outpbuf.insert(outpbuf.begin() + j + 1, dickt[outpbuf[j] - 257].begin(), dickt[outpbuf[j] - 257].end());
						outpbuf.erase(outpbuf.begin() + j);
						flag = true;
					}
				}
			} while (flag);
			outp.insert(outp.end(), outpbuf.begin(), outpbuf.end());
			outpbuf.clear();
		}
	}

	string output;
	for (char symb : outp)
		output.push_back(symb);
	return output;
}

//compressions

// compressHUFFMAN
/**
 * @brief
 *
 * ������� ��������� ����� ��������� �� ��������� �������� ��� ������������� ��������.
 * ����� ��� ��������� � ����� ������� ��������� ����� "1".
 * @param string input - ��������� ���������.
 * @param bool logs - ������ ���������� �������� �� ������� ����� ������. ��� true � ����� � ����������� ������ �������� Clogs.txt (1 ����� ������ - ������ �� 2 - ����� 3 - �. ������) .
 * @return string - ������ ���������.
 */
string compressHUFFMAN(string& input, bool logs)
{
	
	dictionary dict = dict_init(input);
	node* root = build_tree(dict);
	map<char, string> codes;
	runtree(root, "", codes);
	string result = serialize(input, codes, dict);
	if (logs)
	{
		ofstream of("Clogs.txt", ios::app);
		of << input.size() << " " << result.size()<<" " << (double)input.size() / result.size() << endl;
		of.close();
	}
	result.push_back(1);
	return result;
}
// decompressHUFFMAN
/**
 * @brief
 * 
 * ������� ��������� ������� ��������� ���������� �� ������� compressHUFFMAN ��� ������������� ��������.
 * @param string input - ����������� ���������.
 * @return string - �������� ���������.
 */
string decompressHUFFMAN(string& input)
{
	string inp = input;
	inp.pop_back();
	string result = decode(inp);
	return result;
}
// compressLZW
/**
 * @brief
 *
 * ������� ��������� ����� ��������� �� ��������� LZW ��� ������������� ��������.
 * ����� ��� ��������� � ����� ������� ��������� ����� "2".
 * @param string input - ��������� ���������.
 * @param bool logs - ������ ���������� �������� �� ������� ����� ������. ��� true � ����� � ����������� ������ �������� Clogs.txt (1 ����� ������ - ������ �� 2 - ����� 3 - �. ������) .
 * @return string - ������ ���������.
 */
string compressLZW(string& input,bool logs)
{
	vector<int> compressed = compressLZW_p(input);
	string serialized = serializeLZW(compressed);
	if (logs)
	{
		ofstream of("Clogs.txt", ios::app);
		of << input.size() << " " << serialized.size()<<" " << (double)input.size() / serialized.size() << endl;
		of.close();
	}
	serialized.push_back(2);
	return serialized;
}
// decompressLZW
/**
 * \brief
 ������� ��������� ������� ��������� ���������� �� ������� compressLZW ��� ������������� ��������.
 \param string input - ����������� ���������.
 \return string - �������� ���������.
 */
string decompressLZW(string& input)
{
	string inp = input;
	inp.pop_back();
	vector<int> deserialized = deserializeLZW(inp);
	string result = decompressLZW_p(deserialized);
	return result;
}
// compressHLZW
/**
 * @brief
 *
 * ������� ��������� ����� ��������� �� ��������� �������� � ����� LZW ��� ������������� ��������.
 * ����� ��� ��������� � ����� ������� ��������� ����� "3".
 * @param string input - ��������� ���������.
 * @param bool logs - ������ ���������� �������� �� ������� ����� ������. ��� true � ����� � ����������� ������ �������� Clogs.txt (1 ����� ������ - ������ �� 2 - ����� 3 - �. ������) .
 * @return string - ������ ���������.
 */
string compressHLZW(string& input, bool logs)
{
	dictionary dict = dict_init(input);
	node* root = build_tree(dict);
	map<char, string> codes;
	runtree(root, "", codes);
	string result = serialize(input, codes, dict);
	vector<int> compressed = compressLZW_p(result); 
	string serialized = serializeLZW(compressed); 
	if (logs)
	{
		ofstream of("Clogs.txt", ios::app);
		of << input.size() << " " << serialized.size() << " " << (double)input.size() / serialized.size() << endl;
		of.close();
	}
	serialized.push_back(3);
	return serialized;
}
// decompressHLZW
/**
 * @brief
 *
 * ������� ��������� ������� ��������� ���������� �� ������� compressHLZW ��� ������������� ��������.
 * @param string input - ����������� ���������.
 * @return string - �������� ���������.
 */
string decompressHLZW(string& input)
{
	string inp = input;
	inp.pop_back();
	vector<int> deserialized = deserializeLZW(inp);
	string result = decompressLZW_p(deserialized);
	result = decode(result);
	
	return result;
}

// compressLZWH
/**
 * @brief
 *
 * ������� ��������� ����� ��������� �� ��������� LZW � ����� �������� ��� ������������� ��������.
 * ����� ��� ��������� � ����� ������� ��������� ����� "4".
 * @param string input - ��������� ���������.
 * @param bool logs - ������ ���������� �������� �� ������� ����� ������. ��� true � ����� � ����������� ������ �������� Clogs.txt (1 ����� ������ - ������ �� 2 - ����� 3 - �. ������) .
 * @return string - ������ ���������.
 */
string compressLZWH(string& input, bool logs)
{

	vector<int> compressed = compressLZW_p(input);
	string serialized = serializeLZW(compressed);

	dictionary dict = dict_init(serialized);
	node* root = build_tree(dict);
	map<char, string> codes; 
	runtree(root, "", codes); 
	string result = serialize(serialized, codes, dict);

	if (logs)
	{
		ofstream of("Clogs.txt", ios::app);
		of << input.size() << " " << result.size() << " " << (double)input.size() / result.size() << endl;
		of.close();
	}
	result.push_back(4);
	return result;
}

// decompressLZWH
/**
 * @brief
 *
 * ������� ��������� ������� ��������� ���������� �� ������� compressLZWH ��� ������������� ��������.
 * @param string input - ����������� ���������.
 * @return string - �������� ���������.
 */
string decompressLZWH(string& input)
{
	string inp = input;
	inp.pop_back();
	string result = decode(inp);
	vector<int> deserialized = deserializeLZW(result); 
	result = decompressLZW_p(deserialized); 
	return result;
}

// decompressLZWH
/**
 * @brief
 *
 * ������� ��������� ������� ��������� ���������� �� ������� compressLZWH, compressHLZW, compressHUFFMAN, compressLZW ��� ������������� ��������. ������� ���������� ��� ������ ��������������.
 * @param string input - ����������� ���������.
 * @return string - �������� ���������.
 */
string decompressSMART(string& input)
{
	int method = input.back();
	string result;
	switch (method)
	{
	case 1:
	{
		result = decompressHUFFMAN(input);
		return result;
	}
	case 2:
	{
		result = decompressLZW(input);
		return result;
	}
	case 3:
	{
		result = decompressHLZW(input);
		return result;
	}
	case 4:
	{
		result = decompressLZWH(input);
		return result;
	}
	default: return "";
	}
}
// clearlogs
/**
 * @brief
 *
 * ������� ������� ���� � ������ ������.
 * @return ������ �� ����������. ������� ���� "Clogs.txt".
 */
void clearlogs()
{
	ofstream of("Clogs.txt");
	of << "\0";
	of.close();
	return;
}
// clearlogs
/**
 * @brief
 *
 * ������� ������� ������� (����� �������� � json �������), ������� �������� ���������� �� ������:
 ��������� ��� ���� ������ �� ������, ����� ������, ������� ���������� ������.
 * @return string - ������, ���� ������� ������� ��������, ������������ �� "Clogs.txt".
 */
string analyzelogs()
{ 
	int unsize = 0;
	int compsize = 0;
	int buf = 0;
	double skip;
	int messages=0;
	ifstream in("Clogs.txt");
	while (in)
	{
		in >> buf;
		unsize += buf;
		in >> buf;
		compsize += buf;
		in >> skip;
		messages++;
	}
	messages--;
	string result;
	result += "{";
	result += "\"TotalUncompressed\":";
	result += to_string(unsize);
	result += ",\"TotalCompressed\":";
	result += to_string(compsize);
	result += ",\"AverageRatio\":";
	result += to_string((double)unsize / (double)compsize);
	result += ",\"MessageAmount\":";
	result += to_string(messages);
	result += "}";
	return result;
}

int main()
{
	std::cout << __cplusplus << std::endl;
	//string message = readfile("test.txt");
	//string coded = compressHUFFMAN(message, true);
	//cout << decompressSMART(coded) << endl;
	//cout << analyzelogs();
	//clearlogs();
}