/*
Analisador sintatico descendente recursivo da linguagem Portugol

Para compilar:
    gcc Lex.c Sintatico.c main.c -Wall -Og -g -o compilador

Para executar:
    ./compilador arquivo_fonte.txt
*/

#include <stdio.h>
#include <stdlib.h>
#include "Lex.h"

// variavel global do analisador sintatico
TInfoAtomo lookahead;

// prototipos das funcoes da gramatica
void programa(void);
void bloco(void);
void declaracao_variaveis(void);
void lista_variaveis(void);
void declaracao_de_rotinas(void);
void declaracao_de_funcao(void);
void declaracao_de_procedimento(void);
void tipo(void);
void parametros_formais(void);
void parametro_formal(void);
void comando_composto(void);
void comando(void);
void comando_atribuicao_ou_chamada_procedimento(void);
void comando_entrada(void);
void comando_saida(void);
void comando_condicional(void);
void comando_repeticao(void);
void lista_expressao(void);
void expressao(void);
void operador_relacional(void);
void expressao_simples(void);
void operador_adicao(void);
void termo(void);
void operador_multiplicacao(void);
void fator(void);

// imprime um atomo reconhecido no formato "# linha:atomo[: atributo]"
void imprime_atomo(TInfoAtomo info){
    switch(info.atomo){
        case IDENTIFICADOR:
            printf("# %d:%s: %s\n", info.linha, nome_atomo[info.atomo], info.atributo.id);
            break;
        case CONSTINT:
            printf("# %d:%s: %d\n", info.linha, nome_atomo[info.atomo], info.atributo.numero);
            break;
        case CONSTCHAR:
            printf("# %d:%s: %c\n", info.linha, nome_atomo[info.atomo], info.atributo.ch);
            break;
        default:
            printf("# %d:%s\n", info.linha, nome_atomo[info.atomo]);
    }
}

// pede o proximo atomo ao lexico.
// comentarios sao impressos e descartados aqui; erro lexico encerra o programa.
TInfoAtomo proximo_atomo(void){
    TInfoAtomo info = obter_atomo();
    while(info.atomo == COMENTARIO){
        imprime_atomo(info);
        info = obter_atomo();
    }
    if(info.atomo == ERRO){
        printf("# %d:erro lexico\n", info.linha);
        exit(1);
    }
    return info;
}

// mensagem de erro sintatico com o esperado e o encontrado
void erro_sintatico(char *esperado){
    printf("# %d:erro sintatico, esperado [%s] encontrado [%s]\n",
           lookahead.linha, esperado, simbolo_atomo[lookahead.atomo]);
    exit(1);
}

// ponto de entrada do sintatico (chamado pelo main)
void analisador_sintatico(void){
    lookahead = proximo_atomo();
    programa();
    if(lookahead.atomo != EOS)
        erro_sintatico(simbolo_atomo[EOS]);
}

void consome(TAtomo atomo){
    if(lookahead.atomo == atomo){
        imprime_atomo(lookahead);
        lookahead = proximo_atomo();
    }
    else{
        erro_sintatico(simbolo_atomo[atomo]);
    }
}

// <programa> ::= algoritmo identificador ';' <bloco> '.'
void programa(void){
    consome(ALGORITMO);
    consome(IDENTIFICADOR);
    consome(PONTO_VIRGULA);
    bloco();
    consome(PONTO);
}

// <bloco> ::= <declaracao_variaveis> <declaracao_de_rotinas> <comando_composto>
void bloco(void){
    declaracao_variaveis();
    declaracao_de_rotinas();
    comando_composto();
}

// <declaracao_variaveis> ::= [ var <lista_variaveis> ';' { <lista_variaveis> ';' } ]
void declaracao_variaveis(void){
    if(lookahead.atomo == VAR){
        consome(VAR);
        lista_variaveis();
        consome(PONTO_VIRGULA);
        while(lookahead.atomo == IDENTIFICADOR){
            lista_variaveis();
            consome(PONTO_VIRGULA);
        }
    }
}

// <lista_variaveis> ::= identificador { ',' identificador } ':' <tipo>
void lista_variaveis(void){
    consome(IDENTIFICADOR);
    while(lookahead.atomo == VIRGULA){
        consome(VIRGULA);
        consome(IDENTIFICADOR);
    }
    consome(DOIS_PONTOS);
    tipo();
}

// <declaracao_de_rotinas> ::= { <declaracao_de_funcao> | <declaracao_de_procedimento> }
void declaracao_de_rotinas(void){
    while(lookahead.atomo == FUNCAO || lookahead.atomo == PROCEDIMENTO){
        if(lookahead.atomo == FUNCAO)
            declaracao_de_funcao();
        else
            declaracao_de_procedimento();
    }
}

// <declaracao_de_funcao> ::= funcao <tipo> identificador <parametros_formais>
//                            <declaracao_variaveis> <comando_composto>
void declaracao_de_funcao(void){
    consome(FUNCAO);
    tipo();
    consome(IDENTIFICADOR);
    parametros_formais();
    declaracao_variaveis();
    comando_composto();
}

// <declaracao_de_procedimento> ::= procedimento identificador <parametros_formais>
//                                  <declaracao_variaveis> <comando_composto>
void declaracao_de_procedimento(void){
    consome(PROCEDIMENTO);
    consome(IDENTIFICADOR);
    parametros_formais();
    declaracao_variaveis();
    comando_composto();
}

// <tipo> ::= caractere | inteiro | logico
void tipo(void){
    switch(lookahead.atomo){
        case CARACTERE: consome(CARACTERE); break;
        case INTEIRO:   consome(INTEIRO);   break;
        case LOGICO:    consome(LOGICO);    break;
        default:        erro_sintatico("tipo");
    }
}

// <parametros_formais> ::= '(' <parametro_formal> { ';' <parametro_formal> } ')' | '(' ')'
void parametros_formais(void){
    consome(ABRE_PAR);
    if(lookahead.atomo != FECHA_PAR){
        parametro_formal();
        while(lookahead.atomo == PONTO_VIRGULA){
            consome(PONTO_VIRGULA);
            parametro_formal();
        }
    }
    consome(FECHA_PAR);
}

// <parametro_formal> ::= [var] <lista_variaveis>
void parametro_formal(void){
    if(lookahead.atomo == VAR)
        consome(VAR);
    lista_variaveis();
}

// <comando_composto> ::= inicio <comando> { ';' <comando> } fim
void comando_composto(void){
    consome(INICIO);
    comando();
    while(lookahead.atomo == PONTO_VIRGULA){
        consome(PONTO_VIRGULA);
        comando();
    }
    consome(FIM);
}

// <comando> ::= <comando_atribuicao> | <comando_entrada> | <comando_saida> |
//               <comando_condicional> | <comando_repeticao> |
//               <chamada_procedimento> | <comando_composto>
void comando(void){
    switch(lookahead.atomo){
        case IDENTIFICADOR: comando_atribuicao_ou_chamada_procedimento(); break;
        case LEIA:          comando_entrada();     break;
        case ESCREVA:       comando_saida();       break;
        case SE:            comando_condicional(); break;
        case ENQUANTO:      comando_repeticao();   break;
        case INICIO:        comando_composto();    break;
        default:            erro_sintatico("comando");
    }
}

// <comando_atribuicao>   ::= identificador ':=' <expressao>
// <chamada_procedimento> ::= identificador [ '(' <lista_expressao> ')' ]
// as duas comecam com identificador, entao sao tratadas juntas
void comando_atribuicao_ou_chamada_procedimento(void){
    consome(IDENTIFICADOR);
    if(lookahead.atomo == ATRIBUICAO){
        consome(ATRIBUICAO);
        expressao();
    }
    else if(lookahead.atomo == ABRE_PAR){
        consome(ABRE_PAR);
        lista_expressao();
        consome(FECHA_PAR);
    }
}

// <comando_entrada> ::= leia '(' identificador { ',' identificador } ')'
void comando_entrada(void){
    consome(LEIA);
    consome(ABRE_PAR);
    consome(IDENTIFICADOR);
    while(lookahead.atomo == VIRGULA){
        consome(VIRGULA);
        consome(IDENTIFICADOR);
    }
    consome(FECHA_PAR);
}

// <comando_saida> ::= escreva '(' <lista_expressao> ')'
void comando_saida(void){
    consome(ESCREVA);
    consome(ABRE_PAR);
    lista_expressao();
    consome(FECHA_PAR);
}

// <comando_condicional> ::= se <expressao> entao <comando> [ senao <comando> ]
void comando_condicional(void){
    consome(SE);
    expressao();
    consome(ENTAO);
    comando();
    if(lookahead.atomo == SENAO){
        consome(SENAO);
        comando();
    }
}

// <comando_repeticao> ::= enquanto <expressao> faca <comando>
void comando_repeticao(void){
    consome(ENQUANTO);
    expressao();
    consome(FACA);
    comando();
}

// <lista_expressao> ::= <expressao> { ',' <expressao> }
void lista_expressao(void){
    expressao();
    while(lookahead.atomo == VIRGULA){
        consome(VIRGULA);
        expressao();
    }
}

// <expressao> ::= <expressao_simples> [ <operador_relacional> <expressao_simples> ]
void expressao(void){
    expressao_simples();
    if(lookahead.atomo == DIFERENCA || lookahead.atomo == MENOR ||
       lookahead.atomo == MENOR_IGUAL || lookahead.atomo == MAIOR_IGUAL ||
       lookahead.atomo == MAIOR || lookahead.atomo == IGUAL){
        operador_relacional();
        expressao_simples();
    }
}

// <operador_relacional> ::= '<>' | '<' | '<=' | '>=' | '>' | '='
void operador_relacional(void){
    switch(lookahead.atomo){
        case DIFERENCA:   consome(DIFERENCA);   break;
        case MENOR:       consome(MENOR);       break;
        case MENOR_IGUAL: consome(MENOR_IGUAL); break;
        case MAIOR_IGUAL: consome(MAIOR_IGUAL); break;
        case MAIOR:       consome(MAIOR);       break;
        case IGUAL:       consome(IGUAL);       break;
        default:          erro_sintatico("operador relacional");
    }
}

// <expressao_simples> ::= <termo> { <operador_adicao> <termo> }
void expressao_simples(void){
    termo();
    while(lookahead.atomo == SOMA || lookahead.atomo == SUBTRACAO ||
          lookahead.atomo == MOD  || lookahead.atomo == OU){
        operador_adicao();
        termo();
    }
}

// <operador_adicao> ::= '+' | '-' | mod | ou
void operador_adicao(void){
    switch(lookahead.atomo){
        case SOMA:      consome(SOMA);      break;
        case SUBTRACAO: consome(SUBTRACAO); break;
        case MOD:       consome(MOD);       break;
        case OU:        consome(OU);        break;
        default:        erro_sintatico("operador de adicao");
    }
}

// <termo> ::= <fator> { <operador_multiplicacao> <fator> }
void termo(void){
    fator();
    while(lookahead.atomo == MULT || lookahead.atomo == DIV || lookahead.atomo == E){
        operador_multiplicacao();
        fator();
    }
}

// <operador_multiplicacao> ::= '*' | div | e
void operador_multiplicacao(void){
    switch(lookahead.atomo){
        case MULT: consome(MULT); break;
        case DIV:  consome(DIV);  break;
        case E:    consome(E);    break;
        default:   erro_sintatico("operador de multiplicacao");
    }
}

// <fator> ::= identificador [ '(' <lista_expressao> ')' ] | constint | constchar |
//             '(' <expressao> ')' | ( '+' | '-' | nao ) <fator> | verdadeiro | falso
void fator(void){
    switch(lookahead.atomo){
        case IDENTIFICADOR:
            consome(IDENTIFICADOR);
            if(lookahead.atomo == ABRE_PAR){
                consome(ABRE_PAR);
                lista_expressao();
                consome(FECHA_PAR);
            }
            break;
        case CONSTINT:
            consome(CONSTINT);
            break;
        case CONSTCHAR:
            consome(CONSTCHAR);
            break;
        case ABRE_PAR:
            consome(ABRE_PAR);
            expressao();
            consome(FECHA_PAR);
            break;
        case SOMA:
            consome(SOMA);
            fator();
            break;
        case SUBTRACAO:
            consome(SUBTRACAO);
            fator();
            break;
        case NAO:
            consome(NAO);
            fator();
            break;
        case VERDADEIRO:
            consome(VERDADEIRO);
            break;
        case FALSO:
            consome(FALSO);
            break;
        default:
            erro_sintatico("fator");
    }
}
