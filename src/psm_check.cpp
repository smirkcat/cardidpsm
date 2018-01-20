#include <cstdio>
#include <string>
#include <sstream>
#include "psm_data.h"
using namespace std;

int card_search(const char *buf, int len, int policy, int &bnum, int &cnum, string &bstr, string &cstr) {
	int i = 0;		// 缓冲区指针下标
	int n = 0;		// 号码个数
	int num, c_i;
	int j, s = 0;
	char c;			//分隔符
	unsigned char temp_buf; //必须为证书验证
	int name_i = 0;
	char dbuf[40];
	string temps;
	stringstream ss;
	//从13位开始检测
	i = 13;
	while (i <= len) {
		temp_buf = buf[i];
		if (card_data->valid[temp_buf] == 0) {
			i += 13;	// 每隔13位检测一次数字和相关字符 银行卡最小位数设定13
			continue;
		}
		num = 0;
		c = 0; //分隔符初始化
		//i位倒着检测
		for (j = 1; j <= 13; j++) {
			temp_buf = buf[i - j];
			switch (card_data->valid[temp_buf]) {
			case 0:	// 其他字符
				break;
			case 1:	// space,-.\,= 同一种分隔符
				if (c == 0) {
					c = buf[i - j];
					continue;
				}
				else if (c == buf[i - j]) {
					continue;
				}
				else
					break;
			case 2:	// 0-9
				dbuf[13 - num] = buf[i - j];
				num++;
				continue;
			default:
				break;
			}
			break;
		}
		//dbuf数字卡号开头位置
		c_i = 13 - num + 1;
		//buf中数字检测的开始位置
		j = i - j + 1;
		//没有检验的位置开始继续检验
		for (i = i; i < len; i++) {
			//检测24位
			if (num >= 24)
				break;
			temp_buf = buf[i];
			switch (card_data->valid[temp_buf]) {
			case 0:	// 其他字符
				break;
			case 1:	// space,-.\,=
				if (c == 0) {
					c = buf[i];
					continue;
				}
				else if (c == buf[i]) {
					continue;
				}
				else
					break;
			case 2:	// 0-9
				dbuf[c_i + num] = buf[i];
				num++;
				continue;
			default:
				break;
			}
			break;
		}
		//末尾标记零  dbuf[c_i + num]='\0'
		dbuf[c_i + num] = 0;
		//****          验证银行卡      ********
		if ((policy & 0x02) == 0x02)
			s = card_check(&dbuf[c_i], num);
		else
			s = -1;
		if (s >= 0) {
			//获取银行卡名称id ，及检验是否存在列表中,源程序有白名单list
			name_i = get_card_name(&dbuf[c_i], num);
			if (name_i > 0 && card_data->name[name_i][0] != 0) {
				s = 1; n++;
				bnum++;
				ss.str("");
				ss.clear();
				ss << &dbuf[c_i];
				ss >> temps;
				bstr = bstr + temps + ' ';
			}
			else
				s = 0;
		}
		else
			s = 0;
		//****          验证身份证      ******** 如果银行卡验证失败
		if ((s == 0) && ((policy & 0x01) == 0x01)) {
			if (num == 15)	// 15位身份证
				s = czn_check15(&dbuf[c_i]);
			else if (num == 18)	// 18位身份证
				s = czn_check18(&dbuf[c_i]);
			else if (num == 17) {	// 18位身份证,最后一位X确认
				if ((buf[i] == 'x') || (buf[i] == 'X')) {
					dbuf[c_i + 17] = 'X';
					dbuf[c_i + 18] = 0;
					s = czn_check18(&dbuf[c_i]);
				}
			}
			if (s > 0) {
				n++;
				cnum++;
				ss.str("");
				ss.clear();
				ss << &dbuf[c_i];
				ss >> temps;
				cstr = cstr + temps + ' ';
			}
		}
		i++;
	}
	return n;
}


