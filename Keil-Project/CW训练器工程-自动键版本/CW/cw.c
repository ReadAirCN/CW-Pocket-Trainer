#include "CW.h"
#include <stdlib.h>

// ˳���Letters����һ��, �������һ�����ַ�����ΪĬ�϶���ֵ
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

// һ����ĸĦ˹��
u8 CW_Code_idx = 0;
char CW_Code[MAX_MORSE_LEN]={0};
char CW_Code_last[MAX_MORSE_LEN]={0};
// 
char CW_Letter;
// ���Ͷ���
u8 CW_Send_Queue_idx = 0;
u8 CW_Send_Queue[CW_SEND_QUEUE_LENGTH]={0};

MorseNode MorseTreeRoot;
MorseNode MorseTreeNode[64];//�ӽڵ�

u8 node_index = 0;
void ClearStr(char *str, size_t length) 
{
    if (str != NULL && length > 0) {
        memset(str, 0, length); // ʹ��memset�����������ַ����ڴ������Ϊ0
    }
		CW_Code_idx = 0;
}

// ʹ��strncpy�����ַ�������ȷ��Ŀ���ַ�����'\0'��β
void CopyStr(char *dest, const char *src, size_t destSize) 
	{
    // ʹ��strncpy�����ַ�������ȷ��Ŀ���ַ�����'\0'��β
    strncpy(dest, src, destSize - 1); // ��һ���ַ�����ֹ��'\0'
    dest[destSize - 1] = '\0'; // �ֶ������ֹ������ֹ���
}

// ����������һ����[min, max)��Χ�ڵ������
int generate_random_number(int min, int max)
{    
    // ���������
    srand((unsigned int)TIM2->CNT);
    int random_num = (rand() % (max - min)) + min;
    return random_num;
}

char num2letter(signed char num)
{
    // ����ֵ�����ںϷ�������
    if (num < 0 || num >= strlen(Letters)) {
        return '\0'; // ������벻�Ϸ������ؿ��ַ�
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
    // ����Ǵ�д��ĸ������0-25֮���һ������
    //if (ch >= 'A' && ch <= 'Z') {
    //    return ch - 'A';
    //}
    // ����������ַ�������26-35֮���һ������
    //else if (ch >= '0' && ch <= '9') {
    //    return ch - '0' + 26;
    //}

    // ������벻�Ϸ����������һ��(��Ӧ�մ�)
    return i;
}

void letter2MorseCode(char ch,char* morsecode)
{
    strcpy(morsecode,MorseDictionary[letter2num(ch)]);
}



// ����Ī˹�������ĸ��ڵ�
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

// ����insertMorseCode����ʹ��
u8 temp_cnt_length = 0;
// ��Ī˹�������в������Ͷ�Ӧ���ַ�
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

// ��Ī˹�����������������Ӧ���ַ�
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
// �ͷ�Ī˹������ռ�õ��ڴ�
void freeMorseTree(MorseNode* node) {
    if (node != NULL) {
        freeMorseTree(node->left);
        freeMorseTree(node->right);
        free(node);
    }
}

void MorseTree_Init(void)
{
	// ����Ī˹������
    MorseNode* root = createMorseTree();;
	
	// ���������Ӹ����Ī˹����
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

	// �����ɵ������ڵ㴫��
    MorseTreeRoot = *root;
}
