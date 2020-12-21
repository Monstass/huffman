#include <iostream>
#include <string>
#include <queue>
#include <fstream>
#include <sstream>
#include <bitset>
#include <unordered_map>
using namespace std;

// Структура дерева
struct Node
{
	char ch;
	int freq;
	Node* left, * right;
};

// Создание новых ветвей
Node* getNode(char ch, int freq, Node* left, Node* right)
{
	Node* node = new Node();

	node->ch = ch;
	node->freq = freq;
	node->left = left;
	node->right = right;

	return node;
}

// распределение объектов в дереве
struct comp
{
	bool operator()(Node* l, Node* r)
	{
		// ниже вероятность - выше в дереве
		return l->freq > r->freq;
	}
};

// проход по дереву и хранение данных в map
void encode(Node* root, string str,
	unordered_map<char, string>& huffmanCode)
{

	if (root == nullptr)
		return;

	// нашли нужный лист
	if (!root->left && !root->right) 
	{
		huffmanCode[root->ch] = str;
	}

	encode(root->left, str + "0", huffmanCode);
	encode(root->right, str + "1", huffmanCode);
}

// проход по дереву, декодирвоание 
void decode(Node* root, int& index, string str, ofstream& out)
{
	if (root == nullptr) 
	{
		return;
	}

	// нашли лист
	if (!root->left && !root->right)
	{
		cout << root->ch;
		out << root->ch;

		return;
	}

	index++;

	if (str[index] == '0')
		decode(root->left, index, str, out);
	else
		decode(root->right, index, str, out);
}

// построение дерева хаффмана
void buildHuffmanTree(string text)
{
	// подсчет количеста вероятнстей кажого символа
	// хранение в map
	unordered_map<char, int> freq;
	for (char ch : text) 
	{
		freq[ch]++;
	}

	// Создание очереди узлов дерева
	priority_queue<Node*, vector<Node*>, comp> pq;

	// Создание узлов  для каждого символа и добавление
	// в приоритетную очередь.
	for (auto pair : freq) 
	{
		pq.push(getNode(pair.first, pair.second, nullptr, nullptr));
	}

	// пока в очереди будет более одного узла
	while (pq.size() != 1)
	{
		Node* left = pq.top(); pq.pop();
		Node* right = pq.top();	pq.pop();

		
		// Создаем новый внутренний узел с этими двумя узлами
		// как потомков и с частотой равной сумме
		// частот двух узлов. Добавиим новый узел
		// в приоритетную очередь.
		int sum = left->freq + right->freq;
		pq.push(getNode('\0', sum, left, right));
	}

	// root хранит указатель на корень дерева Хаффмана
	Node* root = pq.top();

	//проход по дереву, сохранение в map 
	unordered_map<char, string> huffmanCode;
	encode(root, "", huffmanCode);

	cout << "Коды:\n" << '\n';
	for (auto pair : huffmanCode) 
	{
		cout << pair.first << " " << pair.second << '\n';
	}

	cout << "\nИзначальная строка:\n" << text << '\n';

	ofstream encode_file("encoded.txt", ios::binary);
	string str = "";
	unsigned int count = 0;
	unsigned char buffer = 0;
	for (char ch : text) 
	{
		str += huffmanCode[ch];
		
		for (char c : huffmanCode[ch])
		{
			buffer <<= 1;
			if (c == '1') 
			{
				buffer |= 1;
			}
			count++;

			if (count == 8)
			{
				bitset<8> x(buffer);
				encode_file << buffer;
				buffer = 0;
				count = 0;
			}
		}
	}
	if (count != 0)
	{
		while (count != 8)
		{
			count++;
			buffer <<= 1;
		}
		encode_file << buffer;

		bitset<8> x(buffer);
	}

	cout << "\nЗакодированная строка:\n" << str << '\n';
	int index = -1;
	cout << "\nДекодированная строка: \n";
	ofstream out("decoded.txt");
	while (index < (int)str.size() - 1) 
	{
		decode(root, index, str, out);
	}
}


int main()
{
	setlocale(LC_ALL, "Russian");

	string text;
	ifstream input("Data.txt");
	
	getline(input, text);
	buildHuffmanTree(text);

	return 0;
}