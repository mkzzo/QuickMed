#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#ifdef _WIN32
#include <windows.h>
#define SLEEP(x) Sleep(1000 * (x))
#else
#include <unistd.h>
#define SLEEP(x) sleep(x)
#endif

// Definicoes de tamanhos fixos e prioridades
#define TAM_NOME     100
#define TAM_TS       20
#define TAM_FILA     100
#define EMERGENCIA   1
#define URGENCIA     2
#define NAO_URGENCIA 3

typedef struct Paciente {
    int    senha;
    char   nome[TAM_NOME];
    int    dia_nasc;
    int    mes_nasc;
    int    ano_nasc;
    char   cpf[20]; // Aumentado para caber 000.000.000-00 + \n + \0
    int    prioridade;
    time_t hora_chegada;
} Paciente;

typedef struct NoLista {
    Paciente        paciente;
    char            timestamp[TAM_TS];
    time_t          hora_atendimento;
    struct NoLista *prox;
} NoLista;

// Estrutura de Fila Circular Real
typedef struct Fila {
    Paciente dados[TAM_FILA];
    int inicio;
    int fim;
    int total; // Controla quantos elementos existem na fila
} Fila;

// --- Funcoes Auxiliares de Entrada Segura ---

void limparBuffer() {
    int c;
    while ((c = getchar()) != '\n' && c != EOF);
}

int lerInteiro(const char *mensagem) {
    int valor;
    while (1) {
        printf("%s", mensagem);
        if (scanf("%d", &valor) == 1) {
            limparBuffer();
            return valor;
        }
        printf("Entrada invalida! Digite apenas numeros.\n");
        limparBuffer();
    }
}

void lerString(const char *mensagem, char *destino, int tamanho) {
    printf("%s", mensagem);
    if (fgets(destino, tamanho, stdin) != NULL) {
        size_t len = strlen(destino);
        if (len > 0 && destino[len - 1] == '\n') {
            destino[len - 1] = '\0';
        } else {
            limparBuffer(); // Limpa excesso caso o usuario digite mais que o limite
        }
    }
}

// --- Funcoes da Fila Circular ---

void inicializarFila(Fila *f) {
    f->inicio = 0;
    f->fim = 0;
    f->total = 0;
}

int filaVazia(Fila *f) {
    return f->total == 0;
}

int filaCheia(Fila *f) {
    return f->total == TAM_FILA;
}

int enfileirar(Fila *f, Paciente p) {
    if (filaCheia(f)) {
        return 0; // Falha: fila cheia
    }
    f->dados[f->fim] = p;
    f->fim = (f->fim + 1) % TAM_FILA; // Faz o indice girar
    f->total++;
    return 1; // Sucesso
}

Paciente desenfileirar(Fila *f) {
    Paciente p = f->dados[f->inicio];
    f->inicio = (f->inicio + 1) % TAM_FILA; // Faz o indice girar
    f->total--;
    return p;
}

// --- Funcoes de Historico e Relatorios ---

void obterTimestamp(char *buf, size_t tam) {
    time_t now = time(NULL);
    struct tm *t = localtime(&now);
    strftime(buf, tam, "%d/%m/%Y %H:%M:%S", t);
}

void inserirHistorico(NoLista **lista, Paciente p, const char *ts) {
    NoLista *novo = (NoLista *)malloc(sizeof(NoLista));
    if (!novo) {
        perror("Erro critico de alocacao de memoria");
        return;
    }
    novo->paciente = p;
    strncpy(novo->timestamp, ts, TAM_TS - 1);
    novo->timestamp[TAM_TS - 1] = '\0';
    novo->hora_atendimento = time(NULL);
    novo->prox = *lista;
    *lista = novo;
}

void liberarHistorico(NoLista **lista) {
    NoLista *atual = *lista;
    while (atual != NULL) {
        NoLista *prox = atual->prox;
        free(atual);
        atual = prox;
    }
    *lista = NULL;
}

void imprimirHistorico(NoLista *lista) {
    puts("\n=== Historico de Atendimentos ===");
    if (!lista) {
        puts("Nenhum atendimento registrado ainda.\n");
        return;
    }
    while (lista) {
        printf("%s | Senha %03d | %-20s | Prioridade %d\n",
               lista->timestamp,
               lista->paciente.senha,
               lista->paciente.nome,
               lista->paciente.prioridade);
        lista = lista->prox;
    }
    putchar('\n');
}

void calcularTempoMedioEspera(NoLista *lista, const char *arquivo) {
    int total[4] = {0};
    double soma[4] = {0};

    FILE *fp = fopen(arquivo, "w");
    if (!fp) {
        perror("Erro ao salvar relatorio");
        return;
