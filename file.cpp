#include<iostream>
#include<fstream>
#include<iomanip>
#include<conio.h>
#include<Windows.h>
#include<string>
using namespace std;
int show(string s[], int size) {
	HANDLE h = GetStdHandle(STD_OUTPUT_HANDLE);
	char c;
	static int p = 0;
	if (p > size - 1)p = 0;
	for (int i = 0; i < size; i++) {
		if (i == 0)cout << "\n\n\n\n\n\n\n";
		cout << "\t\t\t\t";
		if (i == p) {
			SetConsoleTextAttribute(h, 10);
			cout << ">>>  " << s[i] << "\n";
			SetConsoleTextAttribute(h, 15);
		}
		else cout << "     " << s[i] << "\n";
		if (i == size - 1) {
			c = _getch();
			if (c >= 49 && c <= size + 48)
				return (c - 49);
			switch (c) {
			case 72:
				if (p > 0)p--;
				i = -1;
				break;
			case 80:
				if (p < size - 1)p++;
				i = -1;
				break;
			case ' ':
			case '\r':
				break;
			default:
				i = -1;
				break;
			}
			system("cls");
		}
	}
	return p;
}
struct patient {
	int Id;
	float phone;
	char Name[20], Addr[50];
};
struct primarydata {
	int id, offset;
};
struct secondarydata {
	char name[20];
	int id;
};
class File {

private:

	int size1;
	fstream file;
	int size;
	bool n = false;
	primarydata pdata[100];
	secondarydata sdata[100];

	int comper(char a[], char b[]) {
		for (int i = 0; i < strlen(a); i++) {
			if (a[i] != b[i]) return 0;
		}
		return 1;
	}

	void copys(char a[], char s[]) {
		for (int i = 0; i < 20; i++)
			a[i] = s[i];
	}

	int search_in_primarykey(int id) {
		int start = 0, end = size, mid = size / 2, p = -1;
		while (pdata[mid].id != id && mid != p) {
			if (pdata[mid].id > id)
				end = mid;
			else start = mid;
			p = mid;
			mid = (end + start) / 2;
		}
		if (pdata[mid].id != id)
			mid = -1;
		return mid;
	}

	int AddToEnd(patient p) {
		int of = -1;
		file.open("data.txt", ios::in | ios::out | ios::binary);
		if (file.is_open()) {
			file.seekp(0, ios::end);
			int len1 = strlen(p.Name), len2 = strlen(p.Addr);
			short size = len1 + len2 + 8;
			of = file.tellp();
			file.write((char*)&size, 2);
			file.write((char*)&p.Id, 4);     file.put('|');
			file.write(p.Name, len1);        file.put('|');
			file.write((char*)&p.phone, 4);    file.put('|');
			file.write(p.Addr, len2);        file.put('|');
		}
		file.close();
		return of;
	}

	int UsingDeletedRecord(patient p) {
		file.open("data.txt", ios::in | ios::out | ios::binary);
		if (file.is_open()) {
			short s1 = 8 + strlen(p.Addr) + strlen(p.Name), size1;
			int offset = -3, next;
			file.seekg(0, ios::beg);
			file.read((char*)&next, sizeof(next));
			while (next != -1) {
				file.seekg(next, ios::beg);
				file.read((char*)&size1, 2);
				file.seekg(1, ios::cur);
				if (s1 <= size1) {
					int df;
					file.read((char*)&df, 4);
					file.seekg(offset + 3, ios::beg);
					file.write((char*)&df, 4);
					file.seekg(next, ios::beg);
					file.write((char*)&size1, 2);
					file.write((char*)&p.Id, 4);						 file.put('|');
					file.write(p.Name, strlen(p.Name));          file.put('|');
					file.write((char*)&p.phone, 4);						 file.put('|');
					file.write(p.Addr, strlen(p.Addr));
					while (s1 < size1) {
						file.put(' ');
						s1++;
					}
					file.put('|');
					file.close();
					return next;
				}
				else {
					offset = next;
					file.read((char*)&next, 4);
				}
			}
		}
		file.close();
		return -1;
	}

	void sort_prim_index() {
		for (int i = 0; i < size - 1; i++) {
			for (int k = i + 1; k < size; k++) {
				if (pdata[i].id > pdata[k].id) {
					pdata[i].id += pdata[k].id;
					pdata[k].id = pdata[i].id - pdata[k].id;
					pdata[i].id -= pdata[k].id;
					pdata[i].offset += pdata[k].offset;
					pdata[k].offset = pdata[i].offset - pdata[k].offset;
					pdata[i].offset -= pdata[k].offset;
				}
			}
		}
	}

	void sort_sec_index() {
		secondarydata d;
		for (int i = 0; i < size - 1; i++) {
			for (int k = i + 1; k < size; k++) {
				if (sdata[i].id > sdata[k].id) {
					d.id = sdata[i].id;
					copys(d.name, sdata[i].name);
					sdata[i].id = sdata[k].id;
					copys(sdata[i].name, sdata[k].name);
					sdata[k].id = d.id;
					copys(sdata[k].name, d.name);
				}
			}
		}
	}

	void delete_from_primarykey(int id) {
		bool in = false;
		for (int i = 0; i < size; i++) {
			if (!in && pdata[i].id == id)in = true;
			else if (in) {
				pdata[i - 1].id = pdata[i].id;
				pdata[i - 1].offset = pdata[i].offset;
			}
		}
	}

	void delete_from_secondarykey(int id) {
		secondarydata d;
		bool in = false;
		for (int i = 0; i < size; i++) {
			if (!in && sdata[i].id == id) in = true;
			else if (in) {
				sdata[i - 1].id = sdata[i].id;
				copys(sdata[i - 1].name, sdata[i].name);
			}
		}
	}

public:

	File() {
		do {
			size1 = 0;
			file.open("data.txt", ios::in | ios::out | ios::binary);
			if (file.is_open()) {
				size = 0;
				fstream prim_index("prim_index.txt", ios::in | ios::out | ios::binary);
				fstream sec_index("sec_index.txt", ios::in | ios::out | ios::binary);
				if (sec_index.is_open() && prim_index.is_open()) {
					char z, x;
					prim_index.seekg(0, ios::beg);
					prim_index.get(z);
					sec_index.seekg(0, ios::beg);
					sec_index.get(x);
					if (x == '0' || z == '0') {
						short g = 1;
						int ID = 0, m = 0;
						file.seekg(0, ios::end);
						m = file.tellg();
						short s1;
						patient s;
						file.seekg(4, ios::beg);
						char c;
						while (!file.eof() && m > file.tellg()) {
							pdata[size].offset = file.tellg();
							file.read((char*)&s1, 2);
							file.read((char*)&z, 1);
							if (z != '*') {
								file.seekg(-1, ios::cur);
								file.read((char*)&pdata[size].id, 4);
								file.seekg(1, ios::cur);
								file.getline((char*)&sdata[size].name, 20, '|');
								file.read((char*)&s.phone, 4);
								file.seekg(1, ios::cur);
								file.getline((char*)&s.Addr, 50, '|');
								sdata[size].id = pdata[size].id;
								size++;
							}
							else file.seekg(s1 + 3, ios::cur);
						}
					}
					else if (x == '1' && z == '1') {
						prim_index.seekg(0, ios::end);
						int d = prim_index.tellg();
						prim_index.seekg(1, ios::beg);
						while (!prim_index.eof() && prim_index.tellg() < d) {
							prim_index.read((char*)&pdata[size], sizeof(pdata[size]));
							size++;
						}
						sec_index.seekg(0, ios::end);
						int bd = sec_index.tellg();
						sec_index.seekg(1, ios::beg);
						while (!sec_index.eof() && sec_index.tellg() < bd) {
							sec_index.read((char*)&sdata[size1], sizeof(sdata[size1]));
							size1++;
						}
					}
					else cout << "error\n";
					prim_index.seekg(0, ios::beg);
					prim_index.put('0');
					sec_index.seekg(0, ios::beg);
					sec_index.put('0');
					prim_index.close();
					sec_index.close();
				}
			}
			file.close();
		} while (size != size1 && size1 != 0);
		sort_prim_index();
		sort_sec_index();
		size1 = size;
	}

	bool Add() {
		patient s;
		cout << "\t\t\tenter name : ";
		if (n)
			cin.ignore();
		n = false;
		cin.getline(s.Name, 20);
		cout << "\t\t\tenter Id : ";
		cin >> s.Id;
		cout << "\t\t\tenter phone : ";
		cin >> s.phone;
		cin.ignore();
		cout << "\t\t\tenter addresss : ";
		cin.getline(s.Addr, 50);
		int offset = UsingDeletedRecord(s);
		if (offset == -1) {
			offset = AddToEnd(s);
		}
		pdata[size].id = s.Id;
		pdata[size].offset = offset;
		sdata[size].id = s.Id;
		copys(sdata[size].name, s.Name);
		size++;
		sort_prim_index();
		sort_sec_index();
		return true;
	}

	void Search_id() {
		bool b = false;
		patient s;
		file.open("data.txt", ios::in | ios::out | ios::binary);
		if (file.is_open()) {
			s.Id = -1;
			s.phone = -1;
			int id = 0;
			short size;
			n = true;
			cout << "\t\t\t\tenter Id : ";
			cin >> id;
			int offset = search_in_primarykey(id);
			if (offset != -1) {
				file.seekg(pdata[offset].offset + 2, ios::beg);
				file.read((char*)&s.Id, 4);
				file.seekg(1, ios::cur);
				file.getline((char*)&s.Name, 20, '|');
				file.read((char*)&s.phone, 4);
				file.seekg(1, ios::cur);
				file.getline((char*)&s.Addr, 50, '|');
				b = true;
			}
		}
		file.close();
		if (!b)cout << "\n\t\t\t\tNot found  -_- \n\n";
		else {
			cout << "\t\t    \xDA\xC4\xC4\xC2\xC4\xC4\xC4\xC4\xC2\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC2\xC4\xC4\xC4\xC4\xC2\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xBF\n";
			cout << "\t\t    \xB3  \xB3 ID \xB3        Name        \xB3 phone\xB3       Address      \xB3\n";
			cout << "\t\t    \xC3\xC4\xC4\xC5\xC4\xC4\xC4\xC4\xC5\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC5\xC4\xC4\xC4\xC4\xC5\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xB4\n";
			cout << "\t\t    \xB3" << setw(2) << setiosflags(ios::left) << 1 << "\xB3";
			cout << setw(4) << setiosflags(ios::left) << s.Id << "\xB3";
			cout << setw(20) << setiosflags(ios::left) << s.Name << "\xB3";
			cout << setw(4) << setiosflags(ios::left) << s.phone << "\xB3";
			cout << setw(20) << setiosflags(ios::left) << s.Addr << "\xB3\n";
			cout << "\t\t    \xD4\xC4\xC4\xC1\xC4\xC4\xC4\xC4\xC1\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC1\xC4\xC4\xC4\xC4\xC1\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xD9\n";
		}
	}

	void Search_name() {
		int b = 1;
		patient s;
		file.open("data.txt", ios::in | ios::out | ios::binary);
		if (file.is_open()) {
			s.Id = -1;
			s.phone = -1;
			char name[20];
			short s1;
			if (n)
				cin.ignore();
			n = false;
			cout << "\t\t\t\tenter name : ";
			cin.getline(name, 20);
			n = true;
			for (int i = 0; i < size1; i++) {
				if (comper(sdata[i].name, name) == 1) {
					int offset = search_in_primarykey(sdata[i].id);
					if (offset != -1) {
						if (b == 1) {
							cout << "\t\t    \xDA\xC4\xC4\xC2\xC4\xC4\xC4\xC4\xC2\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC2\xC4\xC4\xC4\xC4\xC2\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xBF\n";
							cout << "\t\t    \xB3  \xB3 ID \xB3        Name        \xB3 phone\xB3       Address      \xB3\n";
						}
						file.seekg(pdata[offset].offset + 2, ios::beg);
						file.read((char*)&s.Id, 4);
						file.seekg(1, ios::cur);
						file.getline((char*)&s.Name, 20, '|');
						file.read((char*)&s.phone, 4);
						file.seekg(1, ios::cur);
						file.getline((char*)&s.Addr, 50, '|');
						cout << "\t\t    \xC3\xC4\xC4\xC5\xC4\xC4\xC4\xC4\xC5\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC5\xC4\xC4\xC4\xC4\xC5\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xB4\n";
						cout << "\t\t    \xB3" << setw(2) << setiosflags(ios::left) << b++ << "\xB3";
						cout << setw(4) << setiosflags(ios::left) << s.Id << "\xB3";
						cout << setw(20) << setiosflags(ios::left) << s.Name << "\xB3";
						cout << setw(4) << setiosflags(ios::left) << s.phone << "\xB3";
						cout << setw(20) << setiosflags(ios::left) << s.Addr << "\xB3\n";
						b++;
					}
					else {
						delete_from_primarykey(sdata[i].id);
						size--;
					}
				}
			}
			if (b == 1)cout << "\n\t\t\t\tNot found  -_- \n\n";
			else cout << "\t\t    \xD4\xC4\xC4\xC1\xC4\xC4\xC4\xC4\xC1\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC1\xC4\xC4\xC4\xC4\xC1\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xD9\n";
		}
		file.close();
	}

	void Display() {
		short g = 1;
		short size1;
		patient s;
		file.open("data.txt", ios::in | ios::out | ios::binary);
		if (file.is_open()) {
			char c;
			file.seekg(0, ios::end);
			if (file.tellg() > 4) {
				cout << "\t\t    \xDA\xC4\xC4\xC2\xC4\xC4\xC4\xC4\xC2\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC2\xC4\xC4\xC4\xC4\xC2\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xBF\n";
				cout << "\t\t    \xB3  \xB3 ID \xB3        Name        \xB3 phone\xB3       Address      \xB3\n";
				for (int i = 0; i < size; i++) {
					file.seekg(pdata[i].offset, ios::beg);
					file.read((char*)&size1, 2);
					file.read((char*)&s.Id, 4);
					file.seekg(1, ios::cur);
					file.getline((char*)&s.Name, 20, '|');
					file.read((char*)&s.phone, 4);
					file.seekg(1, ios::cur);
					file.getline((char*)&s.Addr, 50, '|');
					cout << "\t\t    \xC3\xC4\xC4\xC5\xC4\xC4\xC4\xC4\xC5\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC5\xC4\xC4\xC4\xC4\xC5\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xB4\n";
					cout << "\t\t    \xB3" << setw(2) << setiosflags(ios::left) << g++ << "\xB3";
					cout << setw(4) << setiosflags(ios::left) << s.Id << "\xB3";
					cout << setw(20) << setiosflags(ios::left) << s.Name << "\xB3";
					cout << setw(4) << setiosflags(ios::left) << s.phone << "\xB3";
					cout << setw(20) << setiosflags(ios::left) << s.Addr << "\xB3\n";
				}
				cout << "\t\t    \xD4\xC4\xC4\xC1\xC4\xC4\xC4\xC4\xC1\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC1\xC4\xC4\xC4\xC4\xC1\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xC4\xD9\n";
			}
		}
		if (g == 1)
			cout << "\n\t\t\tNothing to display -_-\n\n";
		file.close();
	}

	bool Delete() {
		int id = 0, ID = 0;
		file.open("data.txt", ios::in | ios::out | ios::binary);
		if (file.is_open()) {
			cout << "\t\t\tenter Id : ";
			cin >> id;
			cin.ignore();
			file.seekg(0, ios::beg);
			int pre;
			file.read((char*)&pre, 4);
			int mid = search_in_primarykey(id);
			if (mid != -1) {
				file.seekg(pdata[mid].offset + 2, ios::beg);
				file.put('*');
				file.write((char*)&pre, 4);
				file.seekg(0, ios::beg);
				file.write((char*)&pdata[mid].offset, 4);
				delete_from_primarykey(id);
				delete_from_secondarykey(id);
				size--;
				file.close();
				return true;
			}
		}
		file.close();
		return false;
	}

	bool Update() {
		Delete();
		cout << "\n\n";
		return Add();
	}

	short clear() {
		string s[3] = { "1) Clear All Record ", "2) Clear Unused Record ", "3) Back" };
		int p = show(s, 3);
		if (p == 0) {
			cout << "\t\t\tAre you Sure : (Y/N)  "; char c = _getch();
			cout << "\n\n";
			if (c == 'Y' || c == 'y') {
				ofstream ff("data.txt", ios::trunc);
				ofstream b("prim_index.txt", ios::trunc);
				ofstream c("sec_index.txt", ios::trunc);
				ff.seekp(0, ios::beg);
				int of = -1;
				ff.write((char*)&of, sizeof(of));
				b.seekp(0, ios::beg);
				b.put('0');
				c.seekp(0, ios::beg);
				c.put('0');
				ff.close();
				c.close();
				b.close();
				size = 0;
				return 1;
			}
		}
		else if (p == 1) {
			char c;
			patient s[100];
			file.open("data.txt", ios::in | ios::out | ios::binary);
			if (file.is_open()) {
				file.seekg(4, ios::beg);
				short siz;
				for (int i = 0; i < size; i++) {
					file.read((char*)&siz, 2);
					file.read((char*)&c, 1);
					if (c != '*') {
						file.seekg(-1, ios::cur);
						file.read((char*)&s[i].Id, 4);
						file.seekg(1, ios::cur);
						file.getline((char*)&s[i].Name, 20, '|');
						file.read((char*)&s[i].phone, 4);
						file.seekg(1, ios::cur);
						file.getline((char*)&s[i].Addr, 50, '|');
					}
					else {
						file.seekg(siz + 3, ios::cur);
						i--;
					}
				}
				file.close();
				remove("data.txt");
				ofstream ff("data.txt");
				ff.seekp(0, ios::beg);
				int of = -1;
				ff.write((char*)&of, sizeof(of));
				ff.close();
				fstream jf("data.txt", ios::in | ios::out | ios::binary);
				int len1, len2;
				short si;
				jf.seekp(4, ios::beg);
				for (int i = 0; i < size; i++) {
					len1 = strlen(s[i].Name);
					len2 = strlen(s[i].Addr);
					si = len1 + len2 + 8;
					of = jf.tellp();
					jf.write((char*)&si, 2);
					jf.write((char*)&s[i].Id, 4);     jf.put('|');
					jf.write(s[i].Name, len1);        jf.put('|');
					jf.write((char*)&s[i].phone, 4);    jf.put('|');
					jf.write(s[i].Addr, len2);        jf.put('|');
					pdata[i].id = s[i].Id;
					pdata[i].offset = of;
				}
				sort_prim_index();
				jf.close();
				return 2;
			}
		}
		else file.close();
		return 0;
	}

	void Exit() {
		ofstream prim_index("prim_index.txt", ios::out | ios::binary);
		ofstream sec_index("sec_index.txt", ios::out | ios::binary);
		if (prim_index.is_open() && sec_index.is_open()) {
			prim_index.seekp(1, ios::beg);
			sec_index.seekp(1, ios::beg);
			for (int i = 0; i < size; i++) {
				prim_index.write((char*)&pdata[i], sizeof(pdata[i]));
			}
			for (int i = 0; i < size1; i++) {
				sec_index.write((char*)&sdata[i], sizeof(sdata[i]));
			}
			prim_index.seekp(0, ios::beg);
			prim_index.put('1');
			sec_index.seekp(0, ios::beg);
			sec_index.put('1');
		}
		else cout << "not open\n";
		prim_index.close();
		sec_index.close();
	}

};
void main()
{
	int p = 0;
	char c;
	File f;
	string set[8] = { "  1) Add new patient", "  2) Update patient", "  3) Delete patient", "  4) Display all patient", "  5) Search by ID", "  6) Searchby Name ", "  7) clear file", "  8) Exit" };
	do {
		p = show(set, 8);
		system("cls");
		if (p == 0) {
			if (f.Add())cout << "\n\t\t\tsuccessful Adding\n\n";
			else cout << "\n\t\t\tunsuccessful Adding\n\n";
		}
		else if (p == 1) {
			if (f.Update())cout << "\n\t\t\tsuccessful Updating\n\n";
			else cout << "\n\t\t\tunsuccessful Updating\n\n";
		}
		else if (p == 2) {
			if (f.Delete())cout << "\n\t\t\t The patient data is deleted \n\n";
			else cout << "\n\t\t\tunsuccessful Deleting\n\n";
		}
		else if (p == 4) f.Search_id();
		else if (p == 5) f.Search_name();
		else if (p == 3) f.Display();
		else if (p == 6) {
			p = f.clear();
			if (p == 1)
				cout << "\n\t\t\tdeleted all record successfuly\n\n";
			else if (p == 2)
				cout << "\n\t\t\tdeleted unused record successfuly\n\n";
		}
		else if (p == 7) {
			f.Exit();
			break;
		}
		else cout << "\t\t\tErorr choice \n\n";
		cout << "press eny key to go back to the main screen....."; _getch();
		system("cls");
	} while (true);
	cout << "\n\t\t\t\t\t You exit the system :)\n\n";
	system("pause");
}
