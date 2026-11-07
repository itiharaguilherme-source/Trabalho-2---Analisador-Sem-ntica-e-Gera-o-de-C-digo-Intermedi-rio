#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int hashMack(char *s);

#define PRIME_NUMER 211  // numero primo para reduzir colisoes 
 
typedef struct _TNo{    
    char ID[16];    
    int endereco;    
    enum{
        CAT_VARIAVEL,
        CAT_PARAMETRO,
        CAT_PROCEDIMENTO,
        CAT_FUNCAO
    } categoria;
    int escopo;   // 0 = global, 1 = local
    struct _TNo *prox; 
}TNo;  
typedef struct {     
    TNo *entradas[PRIME_NUMER]; 
}TTabelaSimbolos; 

TTabelaSimbolos tabela;

// insere um identificador; devolve NULL se ja existe no mesmo escopo
TNo *insere_simbolo(char *id, int categoria, int escopo, int endereco){
    int h = hashMack (id) % PRIME_NUMER;
    TNo *p;

    for(p = tabela.entradas[h]; p != NULL; p = p->prox){
        if(strcmp(p->ID, id)== 0 && p->escopo == escopo)
            return NULL;
    }
    TNo *novo = calloc(1, sizeof(TNo));
    if(novo == NULL){
        printf("Erro: memória insuficiente\n");
        exit(1);
    }
    strcpy(novo->ID, id);
    novo -> categoria = categoria;
    novo -> escopo = escopo;
    novo -> endereco = endereco;
    novo -> prox = tabela.entradas[h];  // insere no inicio da lista
    tabela.entradas[h] = novo;
    return novo;  
}

TNo *busca_simbolo(char *id, int escopo_atual){
    int h = hashMack(id) % PRIME_NUMER;
    TNo *p;
    TNo *global = NULL;

    for(p = tabela_entradas[h]; p ! = NULL; p = p -> prox){
        if(strcmp(p->ID, id)== 0){
            if(p->escopo == escopo_atual)
                return p;       // achou no escopo atual
            if(p -> escopo == 0)
                global = p;     // guarda o global como reserva
        }
    }
    return global;          // NULL se nao existe em nenhum
}

// imprime a tabela de simbolos: entrada, ID e, se variavel ou parametro, o endereco
void imprime_tabela(void){
    int i;
    TNo *p;

    printf("\nTABELA DE SIMBOLOS\n");
    for(i = 0; i < PRIME_NUMER; i++){
        for(p = tabela.entradas[i]; p != NULL; p = p->prox){
            printf("entrada %d: %s", i, p->ID);
            if(p->categoria == CAT_VARIAVEL || p->categoria == CAT_PARAMETRO)
                printf(" endereco %d", p->endereco);
            printf("\n");
        }
    }
}

// libera todos os nos da tabela (evita vazamento no valgrind)
void libera_tabela(void){
    int i;
    TNo *p, *prox;

    for(i = 0; i < PRIME_NUMER; i++){
        p = tabela.entradas[i];
        while(p != NULL){
            prox = p->prox;   // guarda o proximo antes de liberar
            free(p);
            p = prox;
        }
        tabela.entradas[i] = NULL;
    }
}
