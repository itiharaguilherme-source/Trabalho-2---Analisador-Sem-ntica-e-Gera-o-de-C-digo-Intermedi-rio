#ifndef LEX_H
#define LEX_H

#include <stdio.h>
#include <ctype.h>
#include <string.h>

// codigos internos dos atomos
typedef enum{
    EOS,
    ERRO,
    ALGORITMO,
    CARACTERE,
    DIV,
    E,
    ENQUANTO,
    ENTAO,
    ESCREVA,
    FACA,
    FALSO,
    FIM,
    FUNCAO,
    IDENTIFICADOR,
    INICIO,
    INTEIRO,
    LEIA,
    LOGICO,
    MOD,
    NAO,
    OU,
    PROCEDIMENTO,
    SE,
    SENAO,
    VAR,
    VERDADEIRO,
    CONSTCHAR,
    CONSTINT,
    SOMA,
    MULT,
    SUBTRACAO,
    PONTO,
    VIRGULA,
    PONTO_VIRGULA,
    ABRE_PAR,
    FECHA_PAR,
    ATRIBUICAO,
    DIFERENCA,
    MENOR_IGUAL,
    MENOR,
    MAIOR_IGUAL,
    MAIOR,
    IGUAL,
    DOIS_PONTOS,
    COMENTARIO
}TAtomo;

// Estrutura para comunicar com o analisador sintatico
typedef struct{
   TAtomo atomo;
   int linha;
   union{
       int numero;   // atributo do atomo constint
       char id[16];  // atributo do atomo identificador
       char ch;      // atributo do atomo constchar
   }atributo;
}TInfoAtomo;

// variaveis globais (definidas no main.c e no Lex.c)
extern char *simbolo_atomo[]; // simbolo do atomo nas mensagens de erro (ex: ;)
extern char *buffer;

// analisador lexico
void reconhece_comentario(TInfoAtomo *info_atomo);
void reconhece_numero(TInfoAtomo *info_atomo);
void reconhece_id(TInfoAtomo *info_atomo);
void reconhece_char(TInfoAtomo *info_atomo);
int calcula_valor(char *lex);
TAtomo verifica_reservada(char *lexema_original);
TInfoAtomo obter_atomo(void);

// analisador sintatico
void analisador_sintatico(void);
void consome(TAtomo atomo);

#endif
