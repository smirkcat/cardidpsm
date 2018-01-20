/***********************************************************************
*		 身份证号码校验算程序		*		 银行卡号码校验程序        *
************************************************************************
*	标准:<公民身份号码>				*	标准：						   *
*		 GB11643-1999				*								   *
*		 GB11643-1989				*								   *
************************************************************************
*		   18位身份证号码			*		  13-19位银行卡号码		   *
*	XX YY ZZ YYYY-MM-DD SSS C		*	XXX XXX Y{6,12} Z              *
*	XX: 省市						*	XXX XXX： BIN位 匹配后台数据   *
*	YY: 地区、市					*	Y{6,12}： 任意6-12位数字       *
*	ZZ:	区、县						*	Z： 校验码 Luhn校验            *
*	YYYY:	年						*								   *
*	MM:		月						*								   *
*	DD:		日						*								   *
*	SSS:	顺序号					*								   *
*			男:奇数,女:偶数			*								   *
*	C:		校验码					*								   *
*************************************								   *
*		   15位身份证号码			*								   *
*	XX YY ZZ YY-MM-DD SSS 			*								   *
*	与18位对应						*								   *
************************************************************************/
#include <cstdlib>
#include "psm_data.h"
using namespace std;

#define	CZN_LEN		18
//根据校验位来判断身份证是否正确：∑(Ai×Wi)(mod 11)
/*∑(Ai×Wi)(mod 11):	0	1	2	3	4	5	6	7	8	9	10
校验码字符值		V:	1	0	X	9	8	7	6	5	4	3	2*/
int czn_wi[CZN_LEN] = { 7, 9, 10, 5, 8, 4, 2, 1, 6, 3, 7, 9, 10, 5, 8, 4, 2, 1 };
int czn_y[CZN_LEN] = { 1, 0, 'X', 9, 8, 7, 6, 5, 4, 3, 2 };

/**************************88**
*	身份证校验和计算程序	  *
*	IN:	id:	身份证字符串	  *
*	RET: 1:	正确		      *
*		-1:	校验码或者位数错误*
***************************8**/
int czn_checksum(char *id) {
	int i, s, y;
	int status;
	int c;

	s = 0;
	for (i = 0; i < 17; i++)
		s += (id[i] - 0x30) * czn_wi[i];
	y = s % 11;

	if (y == 2) //对应X
		c = id[17];
	else
		c = id[17] - 0x30;
	if (czn_y[y] == c)
		status = 1;
	else
		status = -1;

	return status;
}

/****************************
*	身份证日期校验程序		*
*	IN:	date:	日期字符串	*
*	RET:	1:	正确		*
*			-1:	错误		*
*****************************/
int czn_date18_check(char *date) {
	int y, m, d, s;
	y = 0;
	for (int i = 0, temp = 1000; i < 4; i++) {
		y = y + (date[i] - 0x30) * temp;
		temp = temp / 10;
	}

	//y = (date[0] - 0x30) * 1000;
	//y += (date[1] - 0x30) * 100;
	//y += (date[2] - 0x30) * 10;
	//y += (date[3] - 0x30);

	m = (date[4] - 0x30) * 10;
	m += (date[5] - 0x30);

	d = (date[6] - 0x30) * 10;
	d += (date[7] - 0x30);

	s = 1;
	//验证身份证日期从1880到2030年
	if ((y >= 1880) && (y < 2030)) {
		switch (m) {
		case 1:
		case 3:
		case 5:
		case 7:
		case 8:
		case 10:
		case 12:
		{
			if ((d < 1) || (d > 31)) {
				s = -1;
				goto ret;
			}
			break;
		}
		case 4:
		case 6:
		case 9:
		case 11:
		{
			if ((d < 1) || (d > 30)) {
				s = -1;
				goto ret;
			}
			break;
		}
		case 2:
		{
			if ((d > 0) && (d < 29))
				goto ret;
			if (d == 29) {
				//当年份是百年时必须验证是否被400整除，不能整除则不是闰年
				//这种验证模式显然用不到，还是不加了，显然还活不到2100年，/(ㄒoㄒ)/~~
				//if (((y % 4) == 0) && ((y % 100) != 0 || (y % 400) == 0))
				if ((y % 4) == 0)
					goto ret;
			}
			s = -1;
			break;
		}
		default:
			s = -1;
			break;
		}
	} else {
		s = -1;
	}
ret:
	return s;
}

int czn_date15_check(char *date)
{
	int y, m, d, s;

	y = 1900;
	y += (date[0] - 0x30) * 10;
	y += (date[1] - 0x30);

	m = (date[2] - 0x30) * 10;
	m += (date[3] - 0x30);

	d = (date[4] - 0x30) * 10;
	d += (date[5] - 0x30);

	s = 1;

	switch (m) {
	case 1:
	case 3:
	case 5:
	case 7:
	case 8:
	case 10:
	case 12:
	{
		if ((d < 1) || (d > 31)) {
			s = -1;
			goto ret;
		}
		break;
	}
	case 4:
	case 6:
	case 9:
	case 11:
	{
		if ((d < 1) || (d > 30)) {
			s = -1;
			goto ret;
		}
		break;
	}
	case 2:
	{
		if ((d > 0) && (d < 29))
			goto ret;
		if (d == 29) {
			if ((y % 4) == 0)
				goto ret;
		}
		s = -1;
		break;
	}
	default:
	{
		s = -1;
		break;
	}
	}


ret:
	return s;
}

/****************************
*		行政区校验程序		*
*	IN:	id:	行政区字符串	*
*	RET:	0:	错误的行政区*
*		   >0:	正确		*
*****************************/
int czn_dist_check(char *id) {
	int num;
	char c;
	//写这个程序的人很有意思
	c = id[6];
	id[6] = 0;
	num = atoi(id);
	id[6] = c;

	return psm_data->czn_hash2[psm_data->czn_hash1[num / CZN_MAX_PC]][num %CZN_MAX_PC];
}

/************************************
*			身份证验证程序			*
*	IN:	czn:	一个身份证字符串	*
*	RET:1:		正确的身份证		*
*		-1:		错误的身份证		*
*************************************/
int czn_check18(char *czn) {
	int s;

	s = czn_checksum(czn);
	if (s == -1)
		goto ret;

	s = czn_date18_check(&czn[6]);
	if (s == -1)
		goto ret;

	s = czn_dist_check(czn);
	if (s == 0) {
		s = -1;
		goto ret;
	}

	s = 1;
ret:
	return s;
}

/************************************
*			身份证验证程序			*
*	IN:	czn:	一个身份证字符串	*
*	RET:1:		正确的身份证		*
*		-1:		错误的身份证		*
*************************************/
int czn_check15(char *czn) {
	int s;

	s = czn_date15_check(&czn[6]);
	if (s == -1)
		goto ret;
	s = czn_dist_check(czn);
	if (s == 0) {
		s = -1;
		goto ret;
	}

	s = 1;
ret:
	return s;
}

/********************************
*		银行卡卡号最后一位校验	*
*	IN:	num:	号码位数		*
*		str:	号码字符串		*
*	RET:		-1:	错误		*
*				>=0:正确,卡类型	*
*********************************/
int card_check(char *str, int num) {
	int i, s, s1, s2;
	//13位 19位
	if ((num < 13) || (num > 19)) {
		return -1;
	}

	s1 = 0;
	s2 = 0;
	//Luhn算法校验最后一位
	if ((num & 0x01) == 0) { //偶数位
		for (i = 0; i < num - 1; i++) {
			s = 2 * (str[i] - 0x30);
			if (s > 9)
				s -= 9;
			s1 = s1 + s;
			i++;
			s2 = s2 + str[i] - 0x30;
		}
	}
	else {
		for (i = 0; i < num - 2; i++) {
			s2 = s2 + str[i] - 0x30;
			i++;

			s = 2 * (str[i] - 0x30);
			if (s > 9)
				s -= 9;
			s1 = s1 + s;
		}
		s2 = s2 + str[i] - 0x30;
	}
	s = s1 + s2;
	if ((s % 10) == 0)
		s = 1;
	else
		s = -1;
	return s;
}

/********************************
*		银行卡号有效性校验		*
*	IN:	num:	号码位数		*
*		len:	总长度			*
*		str:	号码字符串		*
*	RET:		-1:	错误		*
*				0 : 未确定		*
*				>0:正确,卡类型	*
*********************************/
int get_card_name(char *ptr, int num) {
	int	n, i;
	n = 0;
	if ((num < 13) || (num > 19)) {
		return -1;
	}
	//计算六位bin
	for (i = 0; i < 6; i++) {
		n = n * 10 + ptr[i] - 0x30;
	}

	//*******	国内银行卡长度验证 *********
	if (card_data->len[n] != 0) {
		if (card_data->len[n] != num) {
			return 0;
		}
	}
	return card_data->name_id[n];
}