#ifndef __CW_H
#define __CW_H

#include "debug.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>



// 定义莫斯电码树节点的结构体
typedef struct MorseNode {
    char character; // 存储字符，叶节点有效
		struct MorseNode *left; // 点（.）子树
    struct MorseNode *right; // 划（-）子树
} MorseNode;

// 函数声明
MorseNode* createMorseTree(void);
void insertMorseCode(MorseNode* node, const char* code, char character);
MorseNode* searchMorseCode(MorseNode* node, const char* code);
void freeMorseTree(MorseNode* node);


extern MorseNode MorseTreeRoot;
extern void MorseTree_Init(void);
extern char GetCharFromMorseCode(MorseNode* node, const char* code);
extern const char* MorseDictionary_En[];
extern const char* MorseDictionary_Num[];
extern const char* MorseDictionary_Symb[];
extern const char* MorseStringAsLength[];

#define CW_SEND_QUEUE_LENGTH 10
extern void ClearStr(char *str, size_t length);

extern char CW_Code[7];
extern char CW_Code_last[7];
extern u8 CW_Code_idx;

extern char CW_Letter;
extern void CopyStr(char *dest, const char *src, size_t destSize);

// 发送时间
extern int time_dit;
extern int time_dah;
extern int time_space_bit;
extern int time_space_letter;
extern int time_space_word;
// 发送队列
extern u8 CW_Send_Queue_idx;
extern u8 CW_Send_Queue[CW_SEND_QUEUE_LENGTH];

extern int generate_random_number(int min, int max);
extern char num2letter(signed char num);
extern u8 letter2num(char ch);
extern void letter2MorseCode(char ch,char* morsecode);
#endif 
