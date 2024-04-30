#include "CW.h"
#include <stdlib.h>

// 顺序和Letters保持一致, 最后增加一个空字符串作为默认兜底值
const char* MorseDictionary[] = {
    ".-",   // A
    "-...", // B
    "-.-.", // C
    "-..",  // D
    ".",    // E
    "..-.", // F
    "--.",  // G
    "....", // H
    "..",   // I
    ".---", // J
    "-.-",  // K
    ".-..", // L
    "--",   // M
    "-.",   // N
    "---",  // O
    ".--.", // P
    "--.-", // Q
    ".-.",  // R
    "...",  // S
    "-",    // T
    "..-",  // U
    "...-", // V
    ".--",  // W
    "-..-", // X
    "-.--", // Y
    "--..", // Z
    "-----",  // 0
    ".----",  // 1
    "..---",  // 2
    "...--",  // 3
    "....-",  // 4
    ".....",  // 5
    "-....",  // 6
    "--...",  // 7
    "---..",  // 8
    "----.",   // 9
    ".-.-.-",  // Full stop (.)
    "--..--",  // Comma (,)
    "..--..",  // Question mark (?)
    "-.-.--",  // !
    ".----.",  // '
    ".-..-.",  // "
    "-.--.",   // (
    "-.--.-",  // )
    ".-...",   // &
    "---...",  // :
    "-.-.-.",  // ;
    "-..-.",   // /
    "..--.-",  // _
    "-...-",   // =
    ".-.-.",   // +
    "-....-",  // -
    ".--.-.",   // @
    // "...-..-" // $
    ""
};

const char* MorseStringAsLength[] = {
	"ET",
	"AIMN",
	"DGKORSUW",
	"BCFHJLPQVXYZ",
	"0123456789(&/=+",
	".,?!'\"):;_-@"
    //"$"
};

const char Letters[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789.,?!'\"()&:;/_=+-@";

// 一个字母摩斯码
u8 CW_Code_idx = 0;
char CW_Code[MAX_MORSE_LEN]={0};
char CW_Code_last[MAX_MORSE_LEN]={0};
// 
char CW_Letter;
// 发送队列
u8 CW_Send_Queue_idx = 0;
u8 CW_Send_Queue[CW_SEND_QUEUE_LENGTH]={0};

MorseNode MorseTreeRoot;
MorseNode MorseTreeNode[64];//子节点

u8 node_index = 0;
void ClearStr(char *str, size_t length) 
{
    if (str != NULL && length > 0) {
        memset(str, 0, length); // 使用memset函数将整个字符串内存块设置为0
    }
		CW_Code_idx = 0;
}

// 使用strncpy复制字符串，并确保目标字符串以'\0'结尾
void CopyStr(char *dest, const char *src, size_t destSize) 
	{
    // 使用strncpy复制字符串，并确保目标字符串以'\0'结尾
    strncpy(dest, src, destSize - 1); // 留一个字符给终止符'\0'
    dest[destSize - 1] = '\0'; // 手动添加终止符，防止溢出
}

// 函数：生成一个在[min, max)范围内的随机数
int generate_random_number(int min, int max)
{    
    // 生成随机数
    int random_num = (rand() % (max - min)) + min;
    return random_num;
}

char num2letter(signed char num)
{
    // 输入值必须在合法长度内
    if (num < 0 || num >= strlen(Letters)) {
        return '\0'; // 如果输入不合法，返回空字符
    }
    return Letters[num];
}

u8 letter2num(char ch)
{
    u8 i = 0;
    for (; i < strlen(Letters); i++) {
        if (Letters[i] == ch) {
            return i;
        }
    }
    // 如果是大写字母，返回0-25之间的一个数字
    //if (ch >= 'A' && ch <= 'Z') {
    //    return ch - 'A';
    //}
    // 如果是数字字符，返回26-35之间的一个数字
    //else if (ch >= '0' && ch <= '9') {
    //    return ch - '0' + 26;
    //}

    // 如果输入不合法，返回最后一个(对应空串)
    return i;
}

void letter2MorseCode(char ch,char* morsecode)
{
    strcpy(morsecode,MorseDictionary[letter2num(ch)]);
}



// 创建莫斯电码树的根节点
MorseNode* createMorseTree() 
{
    MorseNode* root = (MorseNode*) MorseTreeNode+node_index;
    node_index++;
		if (root != NULL) {
        root->character = '\0';
        root->left = NULL;
        root->right = NULL;
				return root;
    }
		else
			return NULL;
}

// 仅限insertMorseCode函数使用
u8 temp_cnt_length = 0;
// 向莫斯电码树中插入电码和对应的字符
void insertMorseCode(MorseNode* node, const char* code, char character) {
		if (*code == '\0') 
		{
        node->character = character;
        return;
    } 
		else if (*code == '.') 
		{
				if (node->left == NULL) 
				{
						node->left = createMorseTree();
				}
				insertMorseCode(node->left, code + 1, character);
		} 
		else if (*code == '-') 
		{
				if (node->right == NULL) 
				{
						node->right = createMorseTree();
				}
				insertMorseCode(node->right, code + 1, character);
		}
}

// 在莫斯电码树中搜索电码对应的字符
MorseNode* searchMorseCode(MorseNode* node, const char* code) {
    if (node == NULL || *code == '\0') {
        return node;
    } else if (*code == '.') {
        return searchMorseCode(node->left, code + 1);
    } else if (*code == '-') {
        return searchMorseCode(node->right, code + 1);
    }
    return NULL;
}

char GetCharFromMorseCode(MorseNode* node, const char* code)
{
	MorseNode* ans;
	ans = searchMorseCode(node, code);
	if (ans != NULL)
		return ans->character;
	else
		return ' ';
}
// 释放莫斯电码树占用的内存
void freeMorseTree(MorseNode* node) {
    if (node != NULL) {
        freeMorseTree(node->left);
        freeMorseTree(node->right);
        free(node);
    }
}

void MorseTree_Init(void)
{
	// 创建莫斯电码树
    MorseNode* root = createMorseTree();;
	
	// 这里可以添加更多的莫斯电码
    for (int i = 0; i < strlen(Letters); i++) {
        insertMorseCode(root, MorseDictionary[i], Letters[i]);
    }
//    insertMorseCode(root, ".-", 'A');
//    insertMorseCode(root, "-...", 'B');
//    insertMorseCode(root, "-.-.", 'C');
//    insertMorseCode(root, "-..", 'D');
//    insertMorseCode(root, ".", 'E');
//    insertMorseCode(root, "..-.", 'F');
//    insertMorseCode(root, "--.", 'G');
//    insertMorseCode(root, "....", 'H');
//    insertMorseCode(root, "..", 'I');
//    insertMorseCode(root, ".---", 'J');
//    insertMorseCode(root, "-.-", 'K');
//    insertMorseCode(root, ".-..", 'L');
//    insertMorseCode(root, "--", 'M');
//    insertMorseCode(root, "-.", 'N');
//    insertMorseCode(root, "---", 'O');
//    insertMorseCode(root, ".--.", 'P');
//    insertMorseCode(root, "--.-", 'Q');
//    insertMorseCode(root, ".-.", 'R');
//    insertMorseCode(root, "...", 'S');
//    insertMorseCode(root, "-", 'T');
//    insertMorseCode(root, "..-", 'U');
//    insertMorseCode(root, "...-", 'V');
//    insertMorseCode(root, ".--", 'W');
//    insertMorseCode(root, "-..-", 'X');
//    insertMorseCode(root, "-.--", 'Y');
//    insertMorseCode(root, "--..", 'Z');
//    insertMorseCode(root, "-----", '0');
//    insertMorseCode(root, ".----", '1');
//    insertMorseCode(root, "..---", '2');
//    insertMorseCode(root, "...--", '3');
//    insertMorseCode(root, "....-", '4');
//    insertMorseCode(root, ".....", '5');
//    insertMorseCode(root, "-....", '6');
//    insertMorseCode(root, "--...", '7');
//    insertMorseCode(root, "---..", '8');
//    insertMorseCode(root, "----.", '9');
//    insertMorseCode(root, ".-.-.-", '.');
//    insertMorseCode(root, "--..--", ',');
//    insertMorseCode(root, "..--..", '?');
//    insertMorseCode(root, "-.-.--", '!');
//    insertMorseCode(root, ".----.", '\'');
//    insertMorseCode(root, ".-..-.", '"');
//    insertMorseCode(root, "-.--.", '(');
//    insertMorseCode(root, "-.--.-", ')');
//    insertMorseCode(root, ".-...", '&');
//    insertMorseCode(root, "---...", ':');
//    insertMorseCode(root, "-.-.-.", ';');
//    insertMorseCode(root, "-..-.", '/');
//    insertMorseCode(root, "..--.-", '_');
//    insertMorseCode(root, "-...-", '=');
//    insertMorseCode(root, ".-.-.", '+');
//    insertMorseCode(root, "-....-", '-');
//    insertMorseCode(root, ".--.-.", '@');
    // insertMorseCode(root, "...-..-", '$');

	// 将搭建完成的树根节点传出
    MorseTreeRoot = *root;
}
