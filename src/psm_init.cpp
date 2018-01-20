#include <cstdlib>
#include <cstdio>
#include <cstring>
#include <string>
#include <iostream>
#include "psm_data.h"
using namespace std;

PSM_CARD_DATA *card_data = NULL;
PSM_DATA *psm_data = NULL;
DISTRICT_NUMBER czn_dist_data[CZN_DIST_MAX];

/*ancii 码匹配表 - . 0-9 =  \ */
char CARD_VALID_CHAR[256] = {
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,	/* 0x00:tab,0x0a,0x0d */
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,	/* 0x10 */
	1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 0,	/* 0x20: spase - .*/
	2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 0, 0, 0, 1, 0, 0,	/* 0x30:0-9,= */
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,	/* 0x40 */
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0,	/* 0x50: \ */
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,	/* 0x60 */
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,	/* 0x70 */
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,	/* 0x80 */
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,	/* 0x90 */
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,	/* 0xa0 */
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,	/* 0xb0 */
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,	/* 0xc0 */
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,	/* 0xd0 */
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,	/* 0xe0 */
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0	/* 0xf0 */
};

/********************************
*	银行卡名称的读取初始化		*
*	RET:银行卡名称位置name_i	*
*********************************/
int init_card_name(char *name) {
	int i;

	//id是否被占用，或者对名称相同  从1开始，0表示不确定
	for (i = 1; i < PSM_CARD_NAME_NUM; i++) {
		if (card_data->name[i][0] == 0 || strncmp(name, card_data->name[i], PSM_CARD_NAME_LEN - 1) == 0)
			break;
	}
	if (i == PSM_CARD_NAME_NUM) {
		//detect_sensitive_message_debug("too many bank card name[%d].\n",i);
		i--;
	}
	//如果id没有被占用，则初始化对应银行卡名称
	if (card_data->name[i][0] == 0) {
		strncpy(card_data->name[i], name, PSM_CARD_NAME_LEN - 1);
		card_data->name[i][PSM_CARD_NAME_LEN - 1] = 0;
	}
	return i;
}

/****************************************
*			读取card_ruler.ini文件		*
*		初始化bank card_data数据结构		*
*****************************************/
int init_card(char *filename) {
	FILE *fp;
	char *p, buf[400], name[100];
	int i, num1, num2, len;
	int name_i;

	fp = fopen(filename, "rt");
	if (fp == NULL) {
		return -1;
	}
	memset(card_data, 0, sizeof(PSM_CARD_DATA));
	/*memset(card_data->len, 0, sizeof(card_data->len));
	memset(card_data->name, 0, sizeof(card_data->name));
	memset(card_data->name_id, 0, sizeof(card_data->name_id));*/
	memcpy(card_data->valid, CARD_VALID_CHAR, 256);
	while (1) {
		buf[0] = 0;
		p = fgets(buf, 400, fp);
		if (p == NULL)
			break;
		if ((buf[0] == '#') | (buf[0] <= 0x10))
			continue;

		sscanf(buf, "%d%d%d%s", &num1, &num2, &len, name);

		while (num1 > 1000000)
			num1 = num1 / 10;
		while (num2 > 1000000)
			num2 = num2 / 10;

		//获取名字id
		name_i = init_card_name(name);
		//bin1 到 bin2 循环读入
		for (i = num1; i <= num2; i++) {
			card_data->len[i] = len;
			card_data->name_id[i] = name_i;
		}
	}
	fclose(fp);
	return 1;
}

/************************************
*	身份证行政区代码文件读取程序	*
*	IN:	filename:	行政区代码文件	*
*	RET:	>0:		正确完成,个数	*
*			-1:		错误			*
*************************************/
int init_czn_dist(char *filename) {
	FILE *fp;
	char *p, buf[100];
	int i, czn_i, id;

	//初始化为零
	memset(czn_dist_data, 0, sizeof(czn_dist_data));

	fp = fopen(filename, "rt");
	if (fp == NULL)
		return -1;

	czn_i = 0;
	while (1) {
		buf[0] = 0;
		p = fgets(buf, 100, fp);
		if (p == NULL)
			break;

		if ((buf[0] <= 0x10) || (buf[0] == '#'))
			continue;
		//读取身份证前六位行政编码
		sscanf(buf, "%d", &id);
		if ((id <= 100000) || (id >= 1000000)) {
			//detect_sensitive_message_debug(	"read error data[%s] from file [%s].",buf, filename);
			continue;
		}
		i = strlen(buf);
		if (buf[i - 2] <= 0x10)
			buf[i - 2] = 0;
		if (buf[i - 1] <= 0x10)
			buf[i - 1] = 0;
		//跳到名称开始的字段下标
		for (i = 6; i < 40; i++)
			if (buf[i] != 0x20) //0x20空格
				break;

		czn_dist_data[czn_i].id = id;
		strncpy(czn_dist_data[czn_i].name, &buf[i], CZN_DIST_NAME_LEN);
		czn_i++;
		if (czn_i >= CZN_DIST_MAX) {
			/*detect_sensitive_message_debug("too many data in file[%s][%d--%d]\n",filename, czn_i, CZN_DIST_MAX);*/
			czn_i--;
		}
	}
	fclose(fp);
	return czn_i;
}

/********************************************
*		完成身份证行政区划hash数据构建		*
*	FUNC:	使用czn_data的数据计算HASH值,	*
*			填入:hash1,hash2	            *
*	RET:	省市个数		                *
********************************************/
int czn_dist_hash() {
	int i, id, id1, id2, hash_i;
	//初始化为零
	memset(psm_data->czn_hash1, 0, sizeof(psm_data->czn_hash1));
	memset(psm_data->czn_hash2, 0, sizeof(psm_data->czn_hash2));
	//先计算前三位相同的个数 hash 省份内地区市个数，及标记下标不为零的记录，便于后面hash1对应具体值得构建 零id除外。。。
	for (i = 0; i < CZN_DIST_MAX; i++) {
		id = czn_dist_data[i].id / CZN_MAX_PC;
		psm_data->czn_hash1[id]++;
	}

	//省份排号 110->1 北京 120->2 天津  hash1构建，为了节省hash2内存
	hash_i = 0;
	for (i = 110; i < CZN_MAX_PC; i++) {
		if (psm_data->czn_hash1[i] == 0)
			continue;
		hash_i++;
		if (hash_i >= CZN_MIN_PC) { //省份应该不会超过150个
			//detect_sensitive_message_debug("too many district Code[%d--%d]\n",hash_i, CZN_MIN_PC);
			hash_i--;
		}
		psm_data->czn_hash1[i] = hash_i;
	}

	//hash2构建行政区划存在的码，类似于hashmap一样
	for (i = 0; i < CZN_DIST_MAX; i++) {
		if (czn_dist_data[i].id == 0)
			break;

		id1 = czn_dist_data[i].id / CZN_MAX_PC;
		id2 = czn_dist_data[i].id % CZN_MAX_PC;
		psm_data->czn_hash2[psm_data->czn_hash1[id1]][id2] = i + 1;
	}
	return hash_i;
}

int psm_init(string dirconf) {
	psm_data = (PSM_DATA *)malloc(sizeof(PSM_DATA));
	if (psm_data == NULL)
		return -1;
	card_data = (PSM_CARD_DATA *)malloc(sizeof(PSM_CARD_DATA));
	if (card_data == NULL) {
		free(psm_data);
		return -2;
	}
	char bank[128];
	char id[128];
	if (dirconf.length() == 0) {
		sprintf(bank, "%s%s", Default_Card_Dir, Bank_Card_File);
		sprintf(id, "%s%s", Default_Card_Dir, Id_Card_File);
	}
	else {
		sprintf(bank, "%s%s%s", dirconf.c_str(), "/", Bank_Card_File);
		sprintf(id, "%s%s%s", dirconf.c_str(), "/", Id_Card_File);
	}
	/*******读取psm_card_ruler.ini数据      ***************/
	int n = init_card(bank);
	if (n < 0) {
		return -3;
	}
	/******	读取psm_dist_ruler.ini数据	***************/
	n = init_czn_dist(id);
	if (n < 0) {
		return -4;
	}
	/*hash init*/
	czn_dist_hash();
	return 1;
}
