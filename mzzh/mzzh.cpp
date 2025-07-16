#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <ctype.h>

// ����0-7���������
void generate_permutation(int perm[8]) {
    for (int i = 0; i < 8; i++) perm[i] = i;
    for (int i = 7; i > 0; i--) {
        int j = rand() % (i + 1);
        int tmp = perm[i];
        perm[i] = perm[j];
        perm[j] = tmp;
    }
}

// ����������
void inverse_permutation(const int perm[8], int inv[8]) {
    for (int i = 0; i < 8; i++) inv[perm[i]] = i;
}

// ��32λ����ת��ΪHO�ַ���
void seed_to_ho(unsigned seed, char ho[33]) {
    for (int i = 0; i < 32; i++) {
        ho[i] = (seed & (1 << (31 - i))) ? 'Ŷ' : '�J';
    }
    ho[32] = '\0';
}

// ��HO�ַ���ת��Ϊ32λ����
unsigned ho_to_seed(const char ho[33]) {
    unsigned seed = 0;
    for (int i = 0; i < 32; i++) {
        if (ho[i] == 'Ŷ') seed |= (1 << (31 - i));
    }
    return seed;
}

// ���ܺ���
char* encrypt(const char* input) {
    // ���time_tתunsigned int�ľ���
    srand((unsigned int)time(NULL));
    unsigned seed = (rand() << 16) | rand();
    char ho_seed[33];
    seed_to_ho(seed, ho_seed);

    srand(seed);
    int perm[8];
    generate_permutation(perm);

    size_t len = strlen(input);
    // ���void*תchar*�Ĵ�����ʽ����ת����
    char* output = (char*)malloc(32 + len * 8 + 1);
    if (!output) return NULL;

    // ���strcpy����ȫ�Ĵ���ʹ��strcpy_s��
    strcpy_s(output, 32 + len * 8 + 1, ho_seed);
    char* ptr = output + 32;

    for (size_t i = 0; i < len; i++) {
        unsigned char ch = input[i];
        for (int j = 0; j < 8; j++) {
            int bit_pos = perm[j];
            *ptr++ = (ch & (1 << (7 - bit_pos))) ? 'Ŷ' : '�J';
        }
    }
    *ptr = '\0';
    return output;
}

// ���ܺ���
char* decrypt(const char* input) {
    char ho_seed[33];
    // ���strncpy����ȫ�Ĵ���ʹ��strncpy_s��
    strncpy_s(ho_seed, 33, input, 32);
    ho_seed[32] = '\0';

    unsigned seed = ho_to_seed(ho_seed);
    srand(seed);
    int perm[8], inv_perm[8];
    generate_permutation(perm);
    inverse_permutation(perm, inv_perm);

    size_t len = strlen(input);
    // ���void*תchar*�Ĵ�����ʽ����ת����
    char* buffer = (char*)malloc(len + 1);
    char* buf_ptr = buffer;

    // ��ȡHO�ַ������Է�HO�ַ���
    for (const char* ptr = input + 32; *ptr; ptr++) {
        if (*ptr == '�J' || *ptr == 'Ŷ') *buf_ptr++ = *ptr;
    }
    *buf_ptr = '\0';

    size_t ho_len = strlen(buffer);
    if (ho_len % 8 != 0) {
        free(buffer);
        return NULL;
    }

    size_t out_len = ho_len / 8;
    // ���void*תchar*�Ĵ�����ʽ����ת����
    char* output = (char*)malloc(out_len + 1);
    if (!output) {
        free(buffer);
        return NULL;
    }

    for (size_t i = 0; i < out_len; i++) {
        unsigned char ch = 0;
        for (int j = 0; j < 8; j++) {
            char ho_char = buffer[i * 8 + j];
            if (ho_char == 'Ŷ') {
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

// �û�����
int main() {
    printf("=== �JŶ����ϵͳ ===\n");

    while (1) {
        printf("\n��ѡ�����:\n1. ����\n2. ����\n3. �˳�\n> ");

        char choice[10];
        fgets(choice, sizeof(choice), stdin);

        if (choice[0] == '1') {
            printf("������Ҫ���ܵ�����: ");
            char input[1024];
            fgets(input, sizeof(input), stdin);
            input[strcspn(input, "\n")] = '\0'; // �Ƴ����з�

            char* encrypted = encrypt(input);
            if (encrypted) {
                printf("\n���ܽ��: %s\n", encrypted);
                free(encrypted);
            }
            else {
                printf("����ʧ��!\n");
            }

        }
        else if (choice[0] == '2') {
            printf("������Ҫ���ܵ�����: ");
            char input[4096];
            fgets(input, sizeof(input), stdin);
            input[strcspn(input, "\n")] = '\0';

            char* decrypted = decrypt(input);
            if (decrypted) {
                printf("\n���ܽ��: %s\n", decrypted);
                free(decrypted);
            }
            else {
                printf("����ʧ��! ���������ʽ\n");
            }

        }
        else if (choice[0] == '3') {
            break;
        }
        else {
            printf("��Чѡ��!\n");
        }
    }
    return 0;
}
