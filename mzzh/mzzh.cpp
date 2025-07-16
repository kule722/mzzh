#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <ctype.h>

// 生成0-7的随机排列
void generate_permutation(int perm[8]) {
    for (int i = 0; i < 8; i++) perm[i] = i;
    for (int i = 7; i > 0; i--) {
        int j = rand() % (i + 1);
        int tmp = perm[i];
        perm[i] = perm[j];
        perm[j] = tmp;
    }
}

// 生成逆排列
void inverse_permutation(const int perm[8], int inv[8]) {
    for (int i = 0; i < 8; i++) inv[perm[i]] = i;
}

// 将32位种子转换为HO字符串
void seed_to_ho(unsigned seed, char ho[33]) {
    for (int i = 0; i < 32; i++) {
        ho[i] = (seed & (1 << (31 - i))) ? '哦' : '齁';
    }
    ho[32] = '\0';
}

// 将HO字符串转换为32位种子
unsigned ho_to_seed(const char ho[33]) {
    unsigned seed = 0;
    for (int i = 0; i < 32; i++) {
        if (ho[i] == '哦') seed |= (1 << (31 - i));
    }
    return seed;
}

// 加密函数
char* encrypt(const char* input) {
    // 解决time_t转unsigned int的警告
    srand((unsigned int)time(NULL));
    unsigned seed = (rand() << 16) | rand();
    char ho_seed[33];
    seed_to_ho(seed, ho_seed);

    srand(seed);
    int perm[8];
    generate_permutation(perm);

    size_t len = strlen(input);
    // 解决void*转char*的错误（显式类型转换）
    char* output = (char*)malloc(32 + len * 8 + 1);
    if (!output) return NULL;

    // 解决strcpy不安全的错误（使用strcpy_s）
    strcpy_s(output, 32 + len * 8 + 1, ho_seed);
    char* ptr = output + 32;

    for (size_t i = 0; i < len; i++) {
        unsigned char ch = input[i];
        for (int j = 0; j < 8; j++) {
            int bit_pos = perm[j];
            *ptr++ = (ch & (1 << (7 - bit_pos))) ? '哦' : '齁';
        }
    }
    *ptr = '\0';
    return output;
}

// 解密函数
char* decrypt(const char* input) {
    char ho_seed[33];
    // 解决strncpy不安全的错误（使用strncpy_s）
    strncpy_s(ho_seed, 33, input, 32);
    ho_seed[32] = '\0';

    unsigned seed = ho_to_seed(ho_seed);
    srand(seed);
    int perm[8], inv_perm[8];
    generate_permutation(perm);
    inverse_permutation(perm, inv_perm);

    size_t len = strlen(input);
    // 解决void*转char*的错误（显式类型转换）
    char* buffer = (char*)malloc(len + 1);
    char* buf_ptr = buffer;

    // 提取HO字符（忽略非HO字符）
    for (const char* ptr = input + 32; *ptr; ptr++) {
        if (*ptr == '齁' || *ptr == '哦') *buf_ptr++ = *ptr;
    }
    *buf_ptr = '\0';

    size_t ho_len = strlen(buffer);
    if (ho_len % 8 != 0) {
        free(buffer);
        return NULL;
    }

    size_t out_len = ho_len / 8;
    // 解决void*转char*的错误（显式类型转换）
    char* output = (char*)malloc(out_len + 1);
    if (!output) {
        free(buffer);
        return NULL;
    }

    for (size_t i = 0; i < out_len; i++) {
        unsigned char ch = 0;
        for (int j = 0; j < 8; j++) {
            char ho_char = buffer[i * 8 + j];
            if (ho_char == '哦') {
                int bit_pos = inv_perm[j];
                ch |= (1 << (7 - bit_pos));
            }
        }
        output[i] = ch;
    }
    output[out_len] = '\0';
    free(buffer);
    return output;
}

// 用户界面
int main() {
    printf("=== 齁哦加密系统 ===\n");

    while (1) {
        printf("\n请选择操作:\n1. 加密\n2. 解密\n3. 退出\n> ");

        char choice[10];
        fgets(choice, sizeof(choice), stdin);

        if (choice[0] == '1') {
            printf("请输入要加密的内容: ");
            char input[1024];
            fgets(input, sizeof(input), stdin);
            input[strcspn(input, "\n")] = '\0'; // 移除换行符

            char* encrypted = encrypt(input);
            if (encrypted) {
                printf("\n加密结果: %s\n", encrypted);
                free(encrypted);
            }
            else {
                printf("加密失败!\n");
            }

        }
        else if (choice[0] == '2') {
            printf("请输入要解密的内容: ");
            char input[4096];
            fgets(input, sizeof(input), stdin);
            input[strcspn(input, "\n")] = '\0';

            char* decrypted = decrypt(input);
            if (decrypted) {
                printf("\n解密结果: %s\n", decrypted);
                free(decrypted);
            }
            else {
                printf("解密失败! 请检查输入格式\n");
            }

        }
        else if (choice[0] == '3') {
            break;
        }
        else {
            printf("无效选择!\n");
        }
    }
    return 0;
}
