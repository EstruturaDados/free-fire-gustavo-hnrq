#include <stdio.h>
#include <string.h>
#include <stdbool.h>

#define CAPACIDADE 10
#define TAM_NOME 50
#define TAM_TIPO 30

typedef struct {
    char nome[TAM_NOME];
    char tipo[TAM_TIPO];
    int quantidade;
    int prioridade; // 1 a 5 (usado no Nível Mestre)
} Item;

typedef enum {
    ORDENAR_NOME = 1,
    ORDENAR_TIPO = 2,
    ORDENAR_PRIORIDADE = 3
} CriterioOrdenacao;

/* ---------- Utilitários ---------- */

void limparBuffer() {
    int c;
    while ((c = getchar()) != '\n' && c != EOF) {}
}

int lerInt(const char *msg) {
    int v;
    printf("%s", msg);
    scanf("%d", &v);
    limparBuffer();
    return v;
}

void lerString(const char *msg, char *dest, int tam) {
    printf("%s", msg);
    fgets(dest, tam, stdin);
    dest[strcspn(dest, "\n")] = '\0';
}

/* ---------- Exibição ---------- */

void exibirTabela(const Item mochila[], int qtd) {
    printf("\n========= MOCHILA =========\n");
    if (qtd == 0) {
        printf("Mochila vazia.\n");
        return;
    }

    printf("%-3s | %-20s | %-15s | %-10s | %-10s\n",
           "#", "Nome", "Tipo", "Qtd", "Prioridade");
    printf("---------------------------------------------------------------\n");
    for (int i = 0; i < qtd; i++) {
        printf("%-3d | %-20s | %-15s | %-10d | %-10d\n",
               i + 1,
               mochila[i].nome,
               mochila[i].tipo,
               mochila[i].quantidade,
               mochila[i].prioridade);
    }
}

/* ---------- Nível Novato: CRUD básico ---------- */

void adicionarItem(Item mochila[], int *qtd) {
    if (*qtd >= CAPACIDADE) {
        printf("Mochila cheia! Capacidade maxima de %d itens.\n", CAPACIDADE);
        return;
    }

    Item novo;
    lerString("Nome do item: ", novo.nome, TAM_NOME);
    lerString("Tipo do item: ", novo.tipo, TAM_TIPO);
    novo.quantidade = lerInt("Quantidade: ");
    if (novo.quantidade < 0) novo.quantidade = 0;

    // Para manter compatível com Mestre, já pedir prioridade (ou default 1)
    int prio = lerInt("Prioridade (1 a 5, padrao 1 se invalido): ");
    if (prio < 1 || prio > 5) prio = 1;
    novo.prioridade = prio;

    mochila[*qtd] = novo;
    (*qtd)++;

    printf("Item adicionado com sucesso!\n");
}

void removerItemPorNome(Item mochila[], int *qtd) {
    if (*qtd == 0) {
        printf("Mochila vazia, nada para remover.\n");
        return;
    }

    char nomeBusca[TAM_NOME];
    lerString("Nome do item a remover: ", nomeBusca, TAM_NOME);

    int indice = -1;
    for (int i = 0; i < *qtd; i++) {
        if (strcmp(mochila[i].nome, nomeBusca) == 0) {
            indice = i;
            break;
        }
    }

    if (indice == -1) {
        printf("Item nao encontrado.\n");
        return;
    }

    for (int i = indice; i < *qtd - 1; i++) {
        mochila[i] = mochila[i + 1];
    }
    (*qtd)--;

    printf("Item removido com sucesso.\n");
}

/* ---------- Nível Aventureiro: Busca sequencial ---------- */

int buscarItemSequencial(const Item mochila[], int qtd, const char *nome) {
    for (int i = 0; i < qtd; i++) {
        if (strcmp(mochila[i].nome, nome) == 0) {
            return i;
        }
    }
    return -1;
}

void menuBuscarSequencial(const Item mochila[], int qtd) {
    if (qtd == 0) {
        printf("Mochila vazia, nada para buscar.\n");
        return;
    }

    char nomeBusca[TAM_NOME];
    lerString("Nome do item para busca sequencial: ", nomeBusca, TAM_NOME);

    int indice = buscarItemSequencial(mochila, qtd, nomeBusca);
    if (indice == -1) {
        printf("Item nao encontrado (busca sequencial).\n");
    } else {
        printf("Item encontrado:\n");
        printf("Nome: %s\n", mochila[indice].nome);
        printf("Tipo: %s\n", mochila[indice].tipo);
        printf("Quantidade: %d\n", mochila[indice].quantidade);
        printf("Prioridade: %d\n", mochila[indice].prioridade);
    }
}

/* ---------- Nível Mestre: Ordenação + Busca binária ---------- */

int compararItens(const Item *a, const Item *b, CriterioOrdenacao crit) {
    if (crit == ORDENAR_NOME) {
        return strcmp(a->nome, b->nome);
    } else if (crit == ORDENAR_TIPO) {
        return strcmp(a->tipo, b->tipo);
    } else { // ORDENAR_PRIORIDADE
        if (a->prioridade < b->prioridade) return -1;
        if (a->prioridade > b->prioridade) return 1;
        return 0;
    }
}

long insertionSort(Item mochila[], int qtd, CriterioOrdenacao crit) {
    long comparacoes = 0;

    for (int i = 1; i < qtd; i++) {
        Item chave = mochila[i];
        int j = i - 1;

        while (j >= 0) {
            comparacoes++;
            if (compararItens(&mochila[j], &chave, crit) > 0) {
                mochila[j + 1] = mochila[j];
                j--;
            } else {
                break;
            }
        }
        mochila[j + 1] = chave;
    }

    return comparacoes;
}

int buscaBinariaPorNome(const Item mochila[], int qtd, const char *nome) {
    int esq = 0;
    int dir = qtd - 1;

    while (esq <= dir) {
        int meio = (esq + dir) / 2;
        int cmp = strcmp(mochila[meio].nome, nome);

        if (cmp == 0) {
            return meio;
        } else if (cmp < 0) {
            esq = meio + 1;
        } else {
            dir = meio - 1;
        }
    }

    return -1;
}

void menuOrdenacao(Item mochila[], int qtd, bool *ordenadoPorNome) {
    if (qtd == 0) {
        printf("Mochila vazia, nada para ordenar.\n");
        return;
    }

    printf("\n=== Ordenacao ===\n");
    printf("1 - Ordenar por NOME\n");
    printf("2 - Ordenar por TIPO\n");
    printf("3 - Ordenar por PRIORIDADE\n");
    int op = lerInt("Escolha o criterio: ");

    CriterioOrdenacao crit;
    switch (op) {
        case 1: crit = ORDENAR_NOME; break;
        case 2: crit = ORDENAR_TIPO; break;
        case 3: crit = ORDENAR_PRIORIDADE; break;
        default:
            printf("Opcao invalida.\n");
            return;
    }

    long comparacoes = insertionSort(mochila, qtd, crit);
    printf("Mochila ordenada.\n");
    printf("Comparacoes realizadas na ordenacao: %ld\n", comparacoes);

    *ordenadoPorNome = (crit == ORDENAR_NOME);
    exibirTabela(mochila, qtd);
}

void menuBuscaBinaria(const Item mochila[], int qtd, bool ordenadoPorNome) {
    if (qtd == 0) {
        printf("Mochila vazia, nada para buscar.\n");
        return;
    }

    if (!ordenadoPorNome) {
        printf("A busca binaria exige que a mochila esteja ORDENADA POR NOME.\n");
        printf("Use a opcao de ordenacao primeiro (por NOME).\n");
        return;
    }

    char nomeBusca[TAM_NOME];
    lerString("Nome do item para busca binaria: ", nomeBusca, TAM_NOME);

    int indice = buscaBinariaPorNome(mochila, qtd, nomeBusca);
    if (indice == -1) {
        printf("Item nao encontrado (busca binaria).\n");
    } else {
        printf("Item encontrado (busca binaria):\n");
        printf("Nome: %s\n", mochila[indice].nome);
        printf("Tipo: %s\n", mochila[indice].tipo);
        printf("Quantidade: %d\n", mochila[indice].quantidade);
        printf("Prioridade: %d\n", mochila[indice].prioridade);
    }
}

/* ---------- main: integra os três níveis ---------- */

int main() {
    Item mochila[CAPACIDADE];
    int qtd = 0;
    bool ordenadoPorNome = false;

    int opcao;
    do {
        printf("\n===== DESAFIO CODIGO DA ILHA – FREE FIRE =====\n");
        printf("1 - Adicionar item (Novato)\n");
        printf("2 - Remover item por nome (Novato)\n");
        printf("3 - Listar itens (Novato)\n");
        printf("4 - Buscar item por nome (Busca sequencial - Aventureiro)\n");
        printf("5 - Ordenar mochila (Insertion Sort - Mestre)\n");
        printf("6 - Buscar item por nome (Busca binaria - Mestre)\n");
        printf("0 - Sair\n");

        opcao = lerInt("Escolha uma opcao: ");

        switch (opcao) {
            case 1:
                adicionarItem(mochila, &qtd);
                ordenadoPorNome = false;
                break;
            case 2:
                removerItemPorNome(mochila, &qtd);
                ordenadoPorNome = false;
                break;
            case 3:
                exibirTabela(mochila, qtd);
                break;
            case 4:
                menuBuscarSequencial(mochila, qtd);
                break;
            case 5:
                menuOrdenacao(mochila, qtd, &ordenadoPorNome);
                break;
            case 6:
                menuBuscaBinaria(mochila, qtd, ordenadoPorNome);
                break;
            case 0:
                printf("Encerrando o programa...\n");
                break;
            default:
                printf("Opcao invalida.\n");
        }

    } while (opcao != 0);

    return 0;
}
