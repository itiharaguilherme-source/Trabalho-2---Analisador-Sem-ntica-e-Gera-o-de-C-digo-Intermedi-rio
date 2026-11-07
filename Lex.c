/*
Analisador lexico da linguagem Portugol
*/
#include "Lex.h"

// variaveis globais do lexico
char *buffer = NULL;   // aponta para o conteudo do arquivo fonte (setado no main)
char lexema[20];
int contaLinha = 1;

typedef struct{
    char *palavra;
    TAtomo atomo;
}TPalavraReservada;

TPalavraReservada tabela_reservadas[] = {
    {"algoritmo",    ALGORITMO},
    {"caractere",    CARACTERE},
    {"div",          DIV},
    {"e",            E},
    {"enquanto",     ENQUANTO},
    {"entao",        ENTAO},
    {"escreva",      ESCREVA},
    {"faca",         FACA},
    {"falso",        FALSO},
    {"fim",          FIM},
    {"funcao",       FUNCAO},
    {"inicio",       INICIO},
    {"inteiro",      INTEIRO},
    {"leia",         LEIA},
    {"logico",       LOGICO},
    {"mod",          MOD},
    {"nao",          NAO},
    {"ou",           OU},
    {"procedimento", PROCEDIMENTO},
    {"se",           SE},
    {"senao",        SENAO},
    {"var",          VAR},
    {"verdadeiro",   VERDADEIRO},
};

int qtd_reservadas = sizeof(tabela_reservadas) / sizeof(tabela_reservadas[0]);

TAtomo verifica_reservada(char *lexema_original){
    char lexema_minusculo[16];
    int i;

    for(i = 0; lexema_original[i] != '\0'; i++){
        lexema_minusculo[i] = tolower(lexema_original[i]);
    }
    lexema_minusculo[i] = '\0';

    for(i = 0; i < qtd_reservadas; i++){
        if(strcmp(lexema_minusculo, tabela_reservadas[i].palavra) == 0){
            return tabela_reservadas[i].atomo;
        }
    }

    return IDENTIFICADOR;
}

TInfoAtomo obter_atomo(void){
    TInfoAtomo info_atomo;
    info_atomo.atomo = ERRO;

    // elimina delimitadores (espaco, \n, \t, \r) e faz a contagem de linhas
    while(*buffer == ' ' || *buffer == '\n' || *buffer == '\t' || *buffer == '\r'){
        if(*buffer == '\n')
            contaLinha++;
        buffer++;
    }

    // a linha do atomo e a linha onde ele COMECA
    // (importante para comentarios de varias linhas)
    info_atomo.linha = contaLinha;

    if(*buffer == '\0') // final de buffer
        info_atomo.atomo = EOS;
    else if(isdigit(*buffer)) // reconhece numero
        reconhece_numero(&info_atomo);
    else if(isalpha(*buffer)) // reconhece id ou palavra reservada
        reconhece_id(&info_atomo);
    else if(*buffer == '\'')
        reconhece_char(&info_atomo);
    else if(*buffer == '{')
        reconhece_comentario(&info_atomo);
    else if(*buffer == '*'){
        info_atomo.atomo = MULT;
        buffer++;
    }
    else if(*buffer == '+'){
        info_atomo.atomo = SOMA;
        buffer++;
    }
    else if(*buffer == '-'){
        info_atomo.atomo = SUBTRACAO;
        buffer++;
    }
    else if(*buffer == '.'){
        info_atomo.atomo = PONTO;
        buffer++;
    }
    else if(*buffer == ','){
        info_atomo.atomo = VIRGULA;
        buffer++;
    }
    else if(*buffer == ';'){
        info_atomo.atomo = PONTO_VIRGULA;
        buffer++;
    }
    else if(*buffer == '('){
        info_atomo.atomo = ABRE_PAR;
        buffer++;
    }
    else if(*buffer == ')'){
        info_atomo.atomo = FECHA_PAR;
        buffer++;
    }
    else if(*buffer == ':'){
        buffer++;
        if(*buffer == '='){
            info_atomo.atomo = ATRIBUICAO;
            buffer++;
        }
        else{
            info_atomo.atomo = DOIS_PONTOS;
        }
    }
    else if(*buffer == '<'){
        buffer++;
        if(*buffer == '>'){
            info_atomo.atomo = DIFERENCA;
            buffer++;
        }
        else if(*buffer == '='){
            info_atomo.atomo = MENOR_IGUAL;
            buffer++;
        }
        else{
            info_atomo.atomo = MENOR;
        }
    }
    else if(*buffer == '>'){
        buffer++;
        if(*buffer == '='){
            info_atomo.atomo = MAIOR_IGUAL;
            buffer++;
        }
        else{
            info_atomo.atomo = MAIOR;
        }
    }
    else if(*buffer == '='){
        info_atomo.atomo = IGUAL;
        buffer++;
    }
    else{
        info_atomo.atomo = ERRO;
        buffer++;
    }

    return info_atomo;
}

// comentario -> '{-' qualquer coisa '-}'   (pode ter varias linhas)
void reconhece_comentario(TInfoAtomo *info_atomo){
    info_atomo->atomo = ERRO;
    if(*buffer != '{'){
        return;
    }
    buffer++;

    if(*buffer != '-'){
        return;
    }
    buffer++;

    while(*buffer != '\0'){
        if(*buffer == '\n'){
            contaLinha++;
            buffer++;
        }
        else if(*buffer == '-' && *(buffer+1) == '}'){
            buffer += 2; // consome o '-}' de fechamento
            info_atomo->atomo = COMENTARIO;
            return;
        }
        else{
            buffer++;
        }
    }
    // chegou no fim do arquivo sem fechar o comentario -> ERRO
}

// constchar -> ' qualquer_caractere '
void reconhece_char(TInfoAtomo *info_atomo){
    info_atomo->atomo = ERRO;
    if(*buffer == '\''){
        buffer++;
        goto q1;
    }
    return;

q1:
    if(*buffer != '\0' && *buffer != '\n'){
        info_atomo->atributo.ch = *buffer;
        buffer++;
        goto q2;
    }
    return;
q2:
    if(*buffer == '\''){
        buffer++;
        info_atomo->atomo = CONSTCHAR;
        return;
    }
    return;
}

// constint -> digito+ ( (E (+|e) digito+) | e )
void reconhece_numero(TInfoAtomo *info_atomo){
    char *ini_lexema = buffer;
    info_atomo->atomo = ERRO;
    if(isdigit(*buffer)){
        buffer++;
        goto q1;
    }
    return;

q1:
    if(isdigit(*buffer)){
        buffer++;
        goto q1;
    }
    if(*buffer == 'E'){
        buffer++;
        goto q2;
    }
    goto fim;

q2:
    if(*buffer == '+'){
        buffer++;
    }
    if(isdigit(*buffer)){
        buffer++;
        goto q3;
    }
    return;
q3:
    if(isdigit(*buffer)){
        buffer++;
        goto q3;
    }
    goto fim;
fim:
    // numero colado em letra (ex: 12abc) e erro
    if(isalpha(*buffer) || *buffer == '_'){
        return;
    }

    // lexema grande demais para o vetor -> erro (evita estouro de memoria)
    if(buffer - ini_lexema >= (int)sizeof(lexema)){
        return;
    }

    // recorta lexema
    strncpy(lexema, ini_lexema, buffer - ini_lexema);
    lexema[buffer - ini_lexema] = '\0';
    info_atomo->atomo = CONSTINT;
    info_atomo->atributo.numero = calcula_valor(lexema);
}

int calcula_valor(char *lex){
    int mantissa = 0;
    int expoente = 0;
    char *p = lex;

    // le a parte da mantissa
    while(isdigit(*p)){
        mantissa = mantissa * 10 + (*p - '0');
        p++;
    }

    // se tiver E, le o expoente (pulando o '+' se existir)
    if(*p == 'E'){
        p++;
        if(*p == '+')
            p++;
        while(isdigit(*p)){
            expoente = expoente * 10 + (*p - '0');
            p++;
        }
    }

    // aplica o expoente: mantissa * 10^expoente
    int resultado = mantissa;
    for(int i = 0; i < expoente; i++)
        resultado *= 10;

    return resultado;
}

// identificador -> letra (letra | _ | digito)*   (maximo 15 caracteres)
void reconhece_id(TInfoAtomo *info_atomo){
    char *ini_lexema = buffer;
    info_atomo->atomo = ERRO;

    if(isalpha(*buffer)){
        buffer++;
        goto q1;
    }
    return;
q1:
    if(isalpha(*buffer) || *buffer == '_' || isdigit(*buffer)){
        buffer++;
        goto q1;
    }
    int tamanho = buffer - ini_lexema;
    if(tamanho > 15){
        info_atomo->atomo = ERRO;
        return;
    }

    // recorta lexema
    strncpy(info_atomo->atributo.id, ini_lexema, tamanho);
    info_atomo->atributo.id[tamanho] = '\0';
    info_atomo->atomo = verifica_reservada(info_atomo->atributo.id);
}
