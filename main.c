#include <stdio.h>
#include <stdlib.h>
#include "Lex.h"

// nomes dos atomos, na MESMA ORDEM do enum TAtomo no Lex.h
// (se voce adicionar/remover algo do enum, atualize aqui tambem)
char *nome_atomo[] = {
    "fim_de_arquivo",       // EOS
    "erro_lexico",          // ERRO
    "algoritmo",            // ALGORITMO
    "caractere",            // CARACTERE
    "div",                  // DIV
    "e",                    // E
    "enquanto",             // ENQUANTO
    "entao",                // ENTAO
    "escreva",              // ESCREVA
    "faca",                 // FACA
    "falso",                // FALSO
    "fim",                  // FIM
    "funcao",               // FUNCAO
    "identificador",        // IDENTIFICADOR
    "inicio",               // INICIO
    "inteiro",              // INTEIRO
    "leia",                 // LEIA
    "logico",               // LOGICO
    "mod",                  // MOD
    "nao",                  // NAO
    "ou",                   // OU
    "procedimento",         // PROCEDIMENTO
    "se",                   // SE
    "senao",                // SENAO
    "var",                  // VAR
    "verdadeiro",           // VERDADEIRO
    "constchar",            // CONSTCHAR
    "constint",             // CONSTINT
    "soma",                 // SOMA
    "mult",                 // MULT
    "subtracao",            // SUBTRACAO
    "ponto",                // PONTO
    "virgula",              // VIRGULA
    "ponto_virgula",        // PONTO_VIRGULA
    "abre_par",             // ABRE_PAR
    "fecha_par",            // FECHA_PAR
    "atribuicao",           // ATRIBUICAO
    "diferenca",            // DIFERENCA
    "menor_igual",          // MENOR_IGUAL
    "menor",                // MENOR
    "maior_igual",          // MAIOR_IGUAL
    "maior",                // MAIOR
    "igual",                // IGUAL
    "dois_pontos",          // DOIS_PONTOS
    "comentario",           // COMENTARIO
};

// simbolos usados nas mensagens de erro sintatico (ex: esperado [)] encontrado [;])
// tambem na MESMA ORDEM do enum TAtomo
char *simbolo_atomo[] = {
    "fim de arquivo",       // EOS
    "erro lexico",          // ERRO
    "algoritmo",            // ALGORITMO
    "caractere",            // CARACTERE
    "div",                  // DIV
    "e",                    // E
    "enquanto",             // ENQUANTO
    "entao",                // ENTAO
    "escreva",              // ESCREVA
    "faca",                 // FACA
    "falso",                // FALSO
    "fim",                  // FIM
    "funcao",               // FUNCAO
    "identificador",        // IDENTIFICADOR
    "inicio",               // INICIO
    "inteiro",              // INTEIRO
    "leia",                 // LEIA
    "logico",               // LOGICO
    "mod",                  // MOD
    "nao",                  // NAO
    "ou",                   // OU
    "procedimento",         // PROCEDIMENTO
    "se",                   // SE
    "senao",                // SENAO
    "var",                  // VAR
    "verdadeiro",           // VERDADEIRO
    "constchar",            // CONSTCHAR
    "constint",             // CONSTINT
    "+",                    // SOMA
    "*",                    // MULT
    "-",                    // SUBTRACAO
    ".",                    // PONTO
    ",",                    // VIRGULA
    ";",                    // PONTO_VIRGULA
    "(",                    // ABRE_PAR
    ")",                    // FECHA_PAR
    ":=",                   // ATRIBUICAO
    "<>",                   // DIFERENCA
    "<=",                   // MENOR_IGUAL
    "<",                    // MENOR
    ">=",                   // MAIOR_IGUAL
    ">",                    // MAIOR
    "=",                    // IGUAL
    ":",                    // DOIS_PONTOS
    "comentario",           // COMENTARIO
};

// le o arquivo inteiro para uma string alocada dinamicamente
char *le_arquivo(char *nome_arquivo){
    FILE *f = fopen(nome_arquivo, "rb");
    if(f == NULL){
        printf("Erro: nao foi possivel abrir o arquivo '%s'\n", nome_arquivo);
        exit(1);
    }

    // descobre o tamanho do arquivo
    fseek(f, 0, SEEK_END);
    long tamanho = ftell(f);
    fseek(f, 0, SEEK_SET);

    // aloca memoria (+1 para o '\0' final)
    char *conteudo = malloc(tamanho + 1);
    if(conteudo == NULL){
        printf("Erro: memoria insuficiente\n");
        fclose(f);
        exit(1);
    }

    // le o conteudo todo de uma vez
    size_t lidos = fread(conteudo, 1, tamanho, f);
    conteudo[lidos] = '\0';

    fclose(f);
    return conteudo;
}

// conta as linhas do arquivo (a ultima linha pode ou nao terminar com '\n')
int conta_linhas(char *texto){
    int linhas = 0;
    char *p = texto;
    if(*p == '\0')
        return 0;
    while(*p != '\0'){
        if(*p == '\n')
            linhas++;
        p++;
    }
    if(*(p-1) != '\n') // ultima linha sem '\n' no final
        linhas++;
    return linhas;
}

int main(int argc, char *argv[]){
    if(argc < 2){
        printf("Uso: %s <arquivo_fonte>\n", argv[0]);
        return 1;
    }

    char *conteudo = le_arquivo(argv[1]);
    buffer = conteudo;

    // o sintatico pede os atomos ao lexico via consome() -> obter_atomo()
    // em caso de erro lexico ou sintatico o programa e encerrado la dentro
    analisador_sintatico();

    printf("%d linhas analisadas, programa sintaticamente correto\n", conta_linhas(conteudo));

    free(conteudo);
    return 0;
}
