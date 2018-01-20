# include<iostream>
# include<string>
# include<fstream>
# include"psm_data.h"
using namespace std;

int main() {
	ifstream t;
	int length;
	t.open("card.txt");      // open input file  
	t.seekg(0, ios::end);    // go to the end  
	length = t.tellg();           // report location (this is the length)  
	t.seekg(0, ios::beg);    // go back to the beginning  
	char *buffer = new char[length+1];    // allocate memory for a buffer of appropriate dimension  
	t.read(buffer, length);       // read the whole file into the buffer  
	t.close();                    // close file handle 


	int cnum=0, bnum=0;
	char * dir = "conf";
	string cstr, bstr;
	//首先调用初始化函数
	int m=psm_init(dir);
	if (m<0) {
		cout << m<<"初始化失败" << endl;
		goto end;
	}
	cout << buffer << endl;
	int n = card_search(buffer, length-1,0x03,bnum,cnum,bstr,cstr);
	cout << n << endl;
	cout << bnum << endl;
	cout << cnum << endl;
	cout << bstr << endl;
	cout << cstr << endl;
end:
	getchar();
	return 0;
}