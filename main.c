#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "terminal.h"

#define MAX_WORD_LEN 20
#define MAX_KEY_LEN 30

#define MAX_ID_LEN 4
#define MAX_ROWS 2
#define MAX_ROW_LEN 10

#define MAX_TEXT_LEN 100

#define is_space(c) (c == ' ' || c == '\n' || c == '\r' || c == '\t' || c == '\0')

typedef struct _alpha_t{
    char id[MAX_ID_LEN+1];
    char charset[MAX_ROWS][MAX_ROW_LEN];
}alpha_t;

void circular_rshift(char* str){
    int len = strlen(str);
    char last = str[len-1];
    for(int i=len-1; i>0; i--)
        str[i] = str[i-1];
    str[0] = last;
}

void remove_used_chars(char *str, char* word){
    int len = strlen(str);
    int word_len = strlen(word);

    for(int i = 0; i < len; i++){
        for(int j = 0; j < word_len; j++){
            if(str[i] == word[j]){
                for(int k = i; k < (len - 1); k++)
                    str[k] = str[k + 1];
                str[len-1] = '\0';
            }  
        }
    }
}


int search(char* str, char c){

    int index = -1;
    int len = strlen(str);

    for(int i=0; i < len && index < 0; i++){
        if(str[i] == c)
            index = i;
    }

    return index;
}

void remove_duplicates(char* str){
    int len = strlen(str);
    int i = 0, j = 0;
    char tmp[MAX_WORD_LEN], c;

    memset(tmp, '\0', MAX_WORD_LEN);

    do{
        c = str[i++];
        if(search(tmp, c) == -1) tmp[j++] = c;
    }while(i < len);

    strcpy(str, tmp);
}

void prepare_string(char *str){

    int len = strlen(str);

    for(int i = 0; i < len; i++){
        if(str[i] >= 'a' && str[i] <= 'z'){
            str[i] -= 32; 
        }
    }

    for(int i = 0; i < len; i++){
        if(str[i] == 'U') 
            str[i] = 'V';
    }
}

void generate_alpha_id(alpha_t *a, char charset[MAX_ROWS][MAX_ROW_LEN], int pos){
    a->id[0] = charset[0][0+pos];
    a->id[1] = charset[1][0+pos];
    a->id[2] = charset[0][5+pos];
    a->id[3] = charset[1][5+pos];
    a->id[4] = '\0';
}

// generate first alphabet
alpha_t generate_falphabet(char* main_word, char* str){
    alpha_t falpha;

    int last = 0, count;
    int word_len = strlen(main_word);
    int index = (word_len / 2) + (word_len % 2);

    for(int i=0; i<MAX_ROWS; i++){
        count = 0;
        for(int j=0; j<MAX_ROW_LEN; j++)
            falpha.charset[i][j] = (j<index) ? main_word[last++] : str[count++];    
        remove_used_chars(str, falpha.charset[i]);
        index = word_len - index;
    }

    // generate id of alphabet
    generate_alpha_id(&falpha, falpha.charset, 0);

    return falpha;
}

alpha_t* generate_alphabets(char* main_word, char* str){

    alpha_t *alphabets = NULL;

    alphabets = (alpha_t*)calloc(5, sizeof(alpha_t));

    if(alphabets == NULL) return alphabets;

    alphabets[0] = generate_falphabet(main_word, str);
    
    // generate other alphabets
    for(int i=0; i<4; i++){
        strncpy(alphabets[i+1].charset[0], alphabets[0].charset[0], MAX_ROW_LEN);
        strncpy(alphabets[i+1].charset[1], alphabets[i].charset[1], MAX_ROW_LEN);
        circular_rshift(alphabets[i+1].charset[1]);
        generate_alpha_id(&alphabets[i+1], alphabets[0].charset, i+1);
    }

    return alphabets;
}

int search_alpha_id(char c, alpha_t* alphabeths){
    int index = -1;
    
    for(int i = 0; i < 5 && index < 0; i++){
        if(search(alphabeths[i].id, c) > -1) 
            index = i;
    }

    return index;
}

char search_char(char alphabet[MAX_ROWS][MAX_ROW_LEN], char c){
    for(int i = 0; i < MAX_ROW_LEN; i++){
        if(c == alphabet[0][i]){
            return alphabet[1][i];
        }else if(c == alphabet[1][i]){
            return alphabet[0][i];
        }
    }
    return '\0';
}

char* encrypt_text(char *text, char *key, alpha_t *alphabets){
    int alpha_pos, i = 0, j = 0;
    char *str = (char*)calloc(strlen(text), sizeof(char));
    
    do{
        alpha_pos = search_alpha_id(key[j++], alphabets);
        while(!is_space(text[i])){
            str[i] = search_char(alphabets[alpha_pos].charset, text[i]);
            i++;
        }
        str[i++] = ' ';
        if(key[j] == '\0') j = 0;
    }while(text[i] != '\0');

    return str;
}

char* decrypt_string(char *text, char *key, alpha_t *alphabets){
    int alpha_pos, i = 0, j = 0;
    char *str = (char*)calloc(strlen(text), sizeof(char));
    
    do{
        alpha_pos = search_alpha_id(key[j++], alphabets);
        while(!is_space(text[i])){
            str[i] = search_char(alphabets[alpha_pos].charset, text[i]);
            i++;
        }
        str[i++] = ' ';
        if(key[j] == '\0') j = 0;
    }while(text[i] != '\0');
    
    return str;
}

void get_string(char *dest, int len){
    char c;
    int i = 0;
    do{
        c = getche();
        if(!is_space(c))
            dest[i++] = (c >= 'a' && c <= 'z') ? c -= 32 : c;
    }while (i<len && c != '\r');
    dest[i] = '\0';
}

int main(int argc, char **argv){

    char base_charset[] = "ABCDEFGHILMNOPQRSTVZ";
    alpha_t* alphabets;

    char key[MAX_KEY_LEN+1];
    char word[MAX_WORD_LEN+1];
    char text[MAX_TEXT_LEN+1];
    
    char *crypted_text, *decrypt_text;

    printf("Insert word: ");
    get_string(word, MAX_WORD_LEN);

    printf("Insert key: ");
    get_string(key, MAX_KEY_LEN);

    printf("Insert your text: ");
    fgets(text, 100, stdin);
    prepare_string(text);

    prepare_string(word);
    remove_duplicates(word);
    remove_used_chars(base_charset, word);
    
    alphabets = generate_alphabets(word, base_charset);

    crypted_text = encrypt_text(text, key, alphabets);

    printf("\nCrypted String: %s\n", crypted_text);

    decrypt_text = decrypt_string(crypted_text, key, alphabets);

    printf("\nDecrypted String: %s\n", decrypt_text);

    free(alphabets);
    free(crypted_text);
    free(decrypt_text);

    return 0;
}