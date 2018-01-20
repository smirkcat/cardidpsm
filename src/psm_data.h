#ifndef _PSM_DATA_H_
#define	_PSM_DATA_H_

#include <string>

#define	Bank_Card_File		"bank_card_ruler.ini"
#define	Id_Card_File		"id_card_ruler.ini"
#define	Default_Card_Dir		"conf/"

/*********************************************IdCard*********************************************/
#define	CZN_DIST_MAX		6000		// 最大行政区代码个数
#define	CZN_DIST_NAME_LEN	56			// 最大行政区名称长度

#define	CZN_MAX_PC		1000		// 最大省市个数: Province & City   : 省市
#define	CZN_MIN_PC		150			// 最小省市个数: Province & City   : 省市
#define	CZN_MAX_SC		1000		// 最大区县个数: Section  & County : 区县

//身份证行政区划结构体
typedef struct {
	int id;
	char name[CZN_DIST_NAME_LEN];
} DISTRICT_NUMBER;

extern DISTRICT_NUMBER czn_dist_data[];

//身份证行政区划hash结构校验
typedef struct {
	//注释by 李杰
	short czn_hash1[CZN_MAX_PC];	// 省份前三位的排序数HASH1  1 2 3 
	short czn_hash2[CZN_MIN_PC][CZN_MAX_SC];	// HASH1和省份末尾3位对应的唯一标识 如果为零表示省份不存在，不为零表示省份存在，及行政编码正确
} PSM_DATA;

extern PSM_DATA *psm_data;

/********************************************  BANK CARD ***********************************************/
#define		PSM_CARD_NAME_NUM		4000	// 最多银行卡名称个数
#define		PSM_CARD_NUM			1000000	// 银行卡BIN个数,6位
#define		PSM_CARD_NAME_LEN		64	// 银行名称长度

//卡验证加银行卡名称存储
typedef struct {
	char valid[256];//验证数字和分隔符，应当分离出去
	char len[PSM_CARD_NUM]; //银行卡号长度
	char name[PSM_CARD_NAME_NUM][PSM_CARD_NAME_LEN];
	unsigned short name_id[PSM_CARD_NUM];
} PSM_CARD_DATA;

extern PSM_CARD_DATA *card_data;


/********************************************
*	调用前必须初始化这个函数				*
*	RET:	-1，-2:	分配内存失败			*
*			-3:	银行卡数据初始化失败		*
*			-4:	身份证数据初始化失败		*
*			 1:	初始化成功					*
*********************************************/
int psm_init(std::string dirconf);
/********************************************
*	银行卡卡号和身份证号码搜索				*
*	buf:	缓冲区地址						*
*	len:	缓冲区长度						*
*	policy:	01:	idcard  enable				*
*			02:	bankcard enable				*
*			03:	id & bank enable			*
*	bnum:	银行卡号个数					*
*	cnum:	身份证号个数					*
*	bstr:	搜索到的银行卡结果,空格分割		*
*	cstr:	搜索到的身份证结果,空格分割		*
*	RET:	-1:	没有查到					*
*			>0:	查到的个数					*
*	包含:	.,-,space,\						*
*********************************************/
int card_search(const char *buf, int len, int policy, int &bnum, int &cnum, std::string &bstr, std::string &cstr);

int init_card(char *);
int card_check(char *, int);
int get_card_name(char *, int);

int	init_czn_dist(char *);
int	czn_dist_hash(void);
int	czn_dist_check(char *);
int czn_check15(char *);
int czn_check18(char *);

#endif
