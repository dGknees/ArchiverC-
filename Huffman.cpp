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
 * Функция читает файл с переданным в нее названием в строку и возвращает ее
 *
 * @param string filename - название читаемого файла
 * @return Возвращает string с сообщением из файла
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
 * Функция читает файл с переданным в нее названием в строку и возвращает ее, пропуская пробелы
 *
 * @param string filename - название читаемого файла
 * @return Возвращает string с сообщением из файла без пробелов
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
 * Функция Создает структуру dictionary, куда помещает все уникальные символы сообщения и их кол-во повторений
 *
 * @param string input - читаемое сообщение
 * @return Возвращает dictionary в котором записаны все уникальные символы переданного сообщения с их кол-вом повторений
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
 * Функция создает дерево из переданного частотного словаря и возвращает указатель на его корень.
 *
 * @param dictionary - словарь по которому строится дерево
 * @return node* - указатель на корень дерева построенного по данному словарю
 */
node* build_tree(dictionary input)
{
	vector<node*> tree;
	while (input.symbols.size())
	{
		int min_weight = 0;
		for (int i = 0; i < input.symbols.size(); i++)
		{//поиск индекса символа с минимальным кол-вом повторений
			if (input.repetitions[min_weight] > input.repetitions[i]) min_weight = i;
		}
		node* buf=new node;
		//записываем данные по индексу минимального кол-ва повторений в node
		buf->weight = input.repetitions[min_weight];
		buf->symbol = input.symbols[min_weight];
		tree.push_back(buf);//добавляем в вектор
		//удаляем добавленные значения из словаря, повторяем пока не перепишем все значения
		//таким образом данные будут записаны в вектор tree в порядке убывания.
		input.repetitions.erase(input.repetitions.begin() + min_weight);
		input.symbols.erase(input.symbols.begin() + min_weight);
	}
	while (tree.size() > 1)
	{//создаемм дерева с конца (записи в векторе tree - изначально листья дерева)
		node* buf = new node;
		//создаем новый узел, переписываем в него сумму значений 2 минимальных узлов из tree.
		//самый маленький узел слева, чуть больше - справа.
		buf->weight = tree[0]->weight + tree[1]->weight;
		buf->left = tree[0];
		buf->right = tree[1];
		//удаляем 2 первых значения, вставляем полученную ранее запись,
		//сохраняя порядок возрастания по весу
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
	//делаем описанное выше пока в массиве tree не останется 1 элемент - корень.
	node* root = tree[0];
	return root;
}
// run tree
/**
 * @brief
 *
 * Функция заполняет переданную таблицу кодами, построенными по данному дереву.
 * при заходе в левую ветвь к коду добавляется 0, иначе - 1. Когда функция попадает на лист, то полученный код вписывается в таблицу.
 *
 * @param node* root - корень дерева с которого строятся коды
 * @param string code - при вызове функции просто оставьте пустую строку ("")
 * @param map<char,string> codes - таблица, куда записывается код, соответствующий каждому символу
 * @return ничего не возвращает, лишь заполняет переданный map значениями
 */
void runtree(node* root,string code,map<char,string>& codes)
{
	// обработка случая если в словаре есть символ "\0"
	if (root == NULL) 
	{
		code.pop_back();
		codes['\0'] = code;
		return;
	}
	// как только функция доходит до листа дерева (в нем будет символ отличный от 0)
	// сохраняем код по индексу = символ в map.
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
 * Функция кодирует сообщение по переданной таблице с кодами, записывает в начало результата словарь, по которому кодировалось сообщение.
 *
 * @param string message - кодируемое сообщение
 * @param map<char, string> codes - таблица с кодами для сообщения.
 * @param dictionary dict - данный словарь запишется в начале сообщения, вместе с некоторыми параметрами.
 * @return string - закодированное сообщение.
 */
string serialize(string& message, map<char, string>& codes,dictionary& dict)
{
	string output;
	int maxrep = 0;
	unsigned char buf = 0;
	int capacity = 7;
	//далее смотрим сколько бит нужно для записи 1 кол-ва повторений в словаре
	for (int reps : dict.repetitions)
		if (maxrep < reps)maxrep = reps;
	unsigned char entrysize = log2(maxrep);//размер макс кода - 1 в битах
	output.push_back(entrysize);//записываем в вывод колво бит на кол- во повторений
	output.push_back((unsigned char)dict.symbols.size());//записываем кол-во записей в словаре
	int messize = message.size();//кол-во символов в сообщении

	for (int u = 15; u > -1; u--)
		//записываем кол-во символов в сообщении побитово в вывод, выделяем на него 16 бит
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
	
	
	//записываем словарь в вывод побитово
	//сначала записывается колво повторений, потом символ
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
	
	//кодируем само сообщение и записываем в вывод
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
 * Функция декодирует сообщение сжатое методом хаффмана (получаемое из функции serialize).
 *
 * 
 * 
 * @param string input - закодированное ссообшение.
 * @return string - раскодированное сообщение.
 */
string decode(string& input)
{
	dictionary dict;
	int debuf = 0;
	int readcapacity = 7;
	int i = 4;
	//считываем кол-во запией словаря
	int entries = (unsigned char)input[1];
	//т.к. символов в ascii коде 256, а в unsigned char записывается
	//значение до 255 (если более то будет 0), этот случай нужно учесть
	if (entries == 0) entries = 256;
	//считываем кол-во бит на запись повторений словаря
	int entrysize =(unsigned char) input[0];
	//считываем кол-во символов в зашифрованном сообщении
	int todecode = (((unsigned char)input[2]) << 8);
	todecode = todecode + (unsigned char)input[3];
	//читаем словарь
	while (entries > 0 and i<input.size())
		// условие c .size в while нужно чтобы избежать вылета программы
		// в случае непредвиденного поведения функции (к примеру передали не то сообщение) 
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
	// восстанавливаем дерево из словаря
	node* root = build_tree(dict);

	node* cur = root;

	string result;
	//начинаем декодировать
	for (i;todecode and i<input.size();i++)
	{
		
		debuf = input[i];
		while (readcapacity != -1)
		{
			//мы составляли код так, что 0 - влево по дереву
			//зная это обходим дерево пока не наткнемся на символ
			//если найден сивол - записываем его
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
 * Функция сериализует сжатое алгоритмом LZW сообщение. Записывает массив переменных минимально возможным числом бит, начиная с 9
 * @param vector<int> input - закодированное по LZW сообщение.
 * @return string - сериализованное сообщение.
 */
string serializeLZW(vector<int>& input)
{
	string output;
	unsigned char sebuf = 0;
	char bitsize = 9;
	int bufpos = 7;
	int towrite = bitsize - 1;
	//записываем полученный вектор побитово
	//изначально 9 бит на значение
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
		{//еслм 256, то повышаем кол-во бит на значение
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
 * Функция десериализует сообщение, созданное функцией serializeLZW в исходный вектор переменных.
 *
 * @param string input - сериализованное сообщение.
 * @return vector<int> - десериализованное сообщение.
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
				{//отводим на 1 бит больше для чтения
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
 * Функция предварительно сжимает сообщения по алгоритму LZW.
 * Сжатое сообщение необходимо сериализовать функцией serializeLZW.
 *
 * @param string input - сообщение, которое будет сжиматься.
 * @return vector<int> - вектор переменных, где все значения менее 255 - символы ASCII, 256 - код, сигнализирующий что при сериализации необходимо увеличить кол-во бит на символ.
 */
vector<int> compressLZW_p(string& input)
{
	vector<int> result;
	vector<string> dict;//словарь комбинаций символов
	int bitsize = 9;//кол-во бит для записи 1 комбинации символов
	for (int i = 0; i < input.length(); i++)
	{
		int j = 0;
		//ищем совпадения части сообщения со словарем
		for (j; j < dict.size() and (input.compare(i, dict[j].length(), dict[j]) != 0); j++);
		if (j == dict.size())
		{
			//совпадений нет,
			//добавляем этот символ в вывод
			//комбинацию этот символ и следующий добавляем в словар.
			dict.push_back(input.substr(i, 2));
			result.push_back((unsigned char)input[i]);
		}
		else
		{
			//нашли совпадение со словарем
			//вписываем в вывод номер записи в словаре, добавив 257 (т.к. 0-255 заняты ascii
			//256 занято кодом для повышения бит на запись)
			dict.push_back(dict[j] + input[i + dict[j].length()]);
			//если нужно повысить кол-во бит на запись
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
 * Функция расжимает сообщение, сжатое по алгоритму LZW.
 * чтобы расжать сообщение его нужно десериализовать при помощи deserializeLZW.
 * @param vector<int> input - десериализованное сообщение.
 * @return string - расжатое сообщение.
 */
string decompressLZW_p(vector<int>& input)
{
	vector<int> outp;
	vector<vector<int>> dickt;
	vector<int>buf;


	outp.push_back(input[0]);
	for (int i = 1; i < input.size(); i++)
	{
		//игнорируем 256, оно нужно только
		//для сериализации
		if (input[i] == 256) continue;
		//если значение - ascii код цифры
		if (input[i] < 257)
		{
			buf.push_back(input[i - 1]);
			buf.push_back(input[i]);
			outp.push_back(input[i]);
			//добавляем нынешний символ и предыдущий
			//в словарь
			dickt.push_back(buf);
			buf.clear();
		}
		else//если код - комбинация символов из словаря
		{
			if (input[i] - 257 == dickt.size())
			{//обработка случая, когда при сжатии создали пункт словаря
				// и сразу использовали его на следующей комбинации символов
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
				//здесь мы раскладываем комбинации символов
				//на ascii коды, делаем пока комбинаций символов не останется
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
 * функция позволяет сжать сообщение по алгоритму Хаффмана без промежуточных действий.
 * также она оставляет в конце сжатого сообщение число "1".
 * @param string input - Сжимаемое сообщение.
 * @param bool logs - данная переменная отвечает за ведение логов сжатия. При true в папке с исполняемым файлом появится Clogs.txt (1 число строки - размер до 2 - после 3 - к. сжатия) .
 * @return string - сжатое сообщение.
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
 * функция позволяет расжать сообщение полученное из функции compressHUFFMAN без промежуточных действий.
 * @param string input - расжимаемое сообщение.
 * @return string - расжатое сообщение.
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
 * функция позволяет сжать сообщение по алгоритму LZW без промежуточных действий.
 * также она оставляет в конце сжатого сообщение число "2".
 * @param string input - Сжимаемое сообщение.
 * @param bool logs - данная переменная отвечает за ведение логов сжатия. При true в папке с исполняемым файлом появится Clogs.txt (1 число строки - размер до 2 - после 3 - к. сжатия) .
 * @return string - сжатое сообщение.
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
 функция позволяет расжать сообщение полученное из функции compressLZW без промежуточных действий.
 \param string input - расжимаемое сообщение.
 \return string - расжатое сообщение.
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
 * функция позволяет сжать сообщение по алгоритму Хаффмана а затем LZW без промежуточных действий.
 * также она оставляет в конце сжатого сообщение число "3".
 * @param string input - Сжимаемое сообщение.
 * @param bool logs - данная переменная отвечает за ведение логов сжатия. При true в папке с исполняемым файлом появится Clogs.txt (1 число строки - размер до 2 - после 3 - к. сжатия) .
 * @return string - сжатое сообщение.
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
 * функция позволяет расжать сообщение полученное из функции compressHLZW без промежуточных действий.
 * @param string input - расжимаемое сообщение.
 * @return string - расжатое сообщение.
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
 * функция позволяет сжать сообщение по алгоритму LZW а затем Хаффмана без промежуточных действий.
 * также она оставляет в конце сжатого сообщение число "4".
 * @param string input - Сжимаемое сообщение.
 * @param bool logs - данная переменная отвечает за ведение логов сжатия. При true в папке с исполняемым файлом появится Clogs.txt (1 число строки - размер до 2 - после 3 - к. сжатия) .
 * @return string - сжатое сообщение.
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
 * функция позволяет расжать сообщение полученное из функции compressLZWH без промежуточных действий.
 * @param string input - расжимаемое сообщение.
 * @return string - расжатое сообщение.
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
 * функция позволяет расжать сообщение полученное из функций compressLZWH, compressHLZW, compressHUFFMAN, compressLZW без промежуточных действий. Функция распознает вид сжатия самостоятельно.
 * @param string input - расжимаемое сообщение.
 * @return string - расжатое сообщение.
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
 * функция очищает файл с логами сжатия.
 * @return ничего не возвращает. Очищает файл "Clogs.txt".
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
 * функция создает словарь (также читаемый в json формате), который содердит статистику по сжатию:
 суммарный вес всех файлов до сжатия, после сжатия, средний коэффицент сжатия.
 * @return string - строка, куда записан словарь значений, составленный по "Clogs.txt".
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
	//тестовая программа
	string message = readfile("test.txt");
	string coded = compressHUFFMAN(message, true);
	cout << decompressSMART(coded) << endl;
	cout << analyzelogs();
	clearlogs();
}
