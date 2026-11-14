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
    }

    while (lista) {
        int prio = lista->paciente.prioridade;
        if (prio >= 1 && prio <= 3) {
            double espera = difftime(lista->hora_atendimento, lista->paciente.hora_chegada);
            soma[prio] += espera;
            total[prio]++;
        }
        lista = lista->prox;
    }

    fprintf(fp, "=== Relatorio de Tempo Medio de Espera por Prioridade ===\n");
    if (total[EMERGENCIA])
        fprintf(fp, "Emergencia: %.2f segundos (total de %d pacientes)\n", soma[EMERGENCIA]/total[EMERGENCIA], total[EMERGENCIA]);
    else
        fprintf(fp, "Emergencia: nenhum atendimento registrado\n");

    if (total[URGENCIA])
        fprintf(fp, "Urgencia: %.2f segundos (total de %d pacientes)\n", soma[URGENCIA]/total[URGENCIA], total[URGENCIA]);
    else
        fprintf(fp, "Urgencia: nenhum atendimento registrado\n");

    if (total[NAO_URGENCIA])
        fprintf(fp, "Nao Urgente: %.2f segundos (total de %d pacientes)\n", soma[NAO_URGENCIA]/total[NAO_URGENCIA], total[NAO_URGENCIA]);
    else
        fprintf(fp, "Nao Urgente: nenhum atendimento registrado\n");

    fclose(fp);
    printf("Relatorio exportado com sucesso para %s.\n", arquivo);
}

void mostrarFilaAtual(Fila *fEmerg, Fila *fUrg, Fila *fNao) {
    printf("\nOrdem de Atendimento Atual:\n");
    if (!filaVazia(fEmerg))
        printf("Prox: Emergencia - %s (Senha %03d)\n", fEmerg->dados[fEmerg->inicio].nome, fEmerg->dados[fEmerg->inicio].senha);
    else if (!filaVazia(fUrg))
        printf("Prox: Urgencia - %s (Senha %03d)\n", fUrg->dados[fUrg->inicio].nome, fUrg->dados[fUrg->inicio].senha);
    else if (!filaVazia(fNao))
        printf("Prox: Nao Urgente - %s (Senha %03d)\n", fNao->dados[fNao->inicio].nome, fNao->dados[fNao->inicio].senha);
    else
        printf("Nenhum paciente aguardando.\n");
}

void chamarProximo(NoLista **historico, Fila *fEmerg, Fila *fUrg, Fila *fNao) {
    Paciente p;
    if (!filaVazia(fEmerg))      p = desenfileirar(fEmerg);
    else if (!filaVazia(fUrg))   p = desenfileirar(fUrg);
    else if (!filaVazia(fNao))   p = desenfileirar(fNao);
    else {
        puts("Nenhum paciente aguardando.\n");
        return;
    }

    printf("\n[Atendimento] %s (Senha %03d | Prioridade %d)\n", p.nome, p.senha, p.prioridade);
    printf("Pressione ENTER para finalizar atendimento...");
    limparBuffer();

    char ts[TAM_TS];
    obterTimestamp(ts, TAM_TS);
    inserirHistorico(historico, p, ts);
    puts("Atendimento concluido e registrado.\n");
}

void filtrarPorData(NoLista *lista, const char *data) {
    printf("\n-- Atendimentos em %s --\n", data);
    int encontrados = 0;
    while (lista) {
        if (strncmp(lista->timestamp, data, 10) == 0) {
            printf("%s | Senha %03d | %-20s | Prioridade %d\n",
                   lista->timestamp,
                   lista->paciente.senha,
                   lista->paciente.nome,
                   lista->paciente.prioridade);
            encontrados++;
        }
        lista = lista->prox;
    }
    if (!encontrados)
        printf("Nenhum atendimento encontrado nesta data.\n");
}

void excluirPorSenha(NoLista **lista, int senha) {
    NoLista *atual = *lista, *anterior = NULL;
    while (atual) {
        if (atual->paciente.senha == senha) {
            if (anterior) anterior->prox = atual->prox;
            else *lista = atual->prox;
            free(atual);
            printf("Registro com senha %03d removido.\n", senha);
            return;
        }
        anterior = atual;
        atual = atual->prox;
    }
    printf("Registro com senha %03d nao encontrado.\n", senha);
}

void exportarHistoricoParaJSON(NoLista *lista, const char *arquivo) {
    FILE *fp = fopen(arquivo, "w");
    if (!fp) {
        perror("Erro ao criar arquivo JSON");
        return;
    }
    fprintf(fp, "[\n");
    while (lista) {
        fprintf(fp, "  {\"timestamp\": \"%s\", \"senha\": %d, \"nome\": \"%s\", \"nascimento\": \"%02d/%02d/%04d\", \"cpf\": \"%s\", \"prioridade\": %d}%s\n",
                lista->timestamp,
                lista->paciente.senha,
                lista->paciente.nome,
                lista->paciente.dia_nasc,
                lista->paciente.mes_nasc,
                lista->paciente.ano_nasc,
                lista->paciente.cpf,
                lista->paciente.prioridade,
                lista->prox ? "," : "");
        lista = lista->prox;
    }
    fprintf(fp, "]\n");
    fclose(fp);
    printf("Historico exportado com sucesso para %s.\n", arquivo);
}

void exportarHistoricoParaCSV(NoLista *lista, const char *arquivo) {
    FILE *fp = fopen(arquivo, "w");
    if (!fp) {
        perror("Erro ao criar arquivo CSV");
        return;
    }
    fprintf(fp, "Timestamp,Senha,Nome,Data de Nascimento,CPF,Prioridade\n");
    while (lista) {
        fprintf(fp, "\"%s\",%d,\"%s\",\"%02d/%02d/%04d\",\"%s\",%d\n",
                lista->timestamp,
                lista->paciente.senha,
                lista->paciente.nome,
                lista->paciente.dia_nasc,
                lista->paciente.mes_nasc,
                lista->paciente.ano_nasc,
                lista->paciente.cpf,
                lista->paciente.prioridade);
        lista = lista->prox;
    }
    fclose(fp);
    printf("Historico exportado com sucesso para %s.\n", arquivo);
}

// --- Menu Principal ---

void menu() {
    NoLista *historico = NULL;
    Fila filaEmerg, filaUrg, filaNao;
    int contadorSenha = 1;
    int opcao, senha, sucesso;
    char data[TAM_TS];
    Paciente p;

    inicializarFila(&filaEmerg);
    inicializarFila(&filaUrg);
    inicializarFila(&filaNao);

    do {
        printf("\n=========== MENU ===========\n");
        printf("1. Inserir novo atendimento\n");
        printf("2. Ver ordem de atendimento\n");
        printf("3. Chamar proximo paciente\n");
        printf("4. Exibir historico\n");
        printf("5. Filtrar por data\n");
        printf("6. Excluir por senha\n");
        printf("7. Exportar para JSON\n");
        printf("8. Exportar para CSV\n");
        printf("9. Exportar tempo medio de espera\n");
        printf("0. Sair\n");
        
        opcao = lerInteiro("Escolha: ");

        switch(opcao) {
        case 1:
            lerString("\nNome: ", p.nome, TAM_NOME);
            printf("Data de nascimento:\n");
            p.dia_nasc = lerInteiro("  Dia: ");
            p.mes_nasc = lerInteiro("  Mes: ");
            p.ano_nasc = lerInteiro("  Ano: ");
            lerString("CPF: ", p.cpf, sizeof(p.cpf));

            // Garante que a prioridade seja valida antes de gerar senha
            do {
                p.prioridade = lerInteiro("Prioridade (1-Emergencia, 2-Urgente, 3-Nao Urgente): ");
                if (p.prioridade < 1 || p.prioridade > 3) {
                    printf("Prioridade invalida! Escolha 1, 2 ou 3.\n");
                }
            } while (p.prioridade < 1 || p.prioridade > 3);

            p.senha = contadorSenha;
            p.hora_chegada = time(NULL);

            sucesso = 0;
            if (p.prioridade == EMERGENCIA)        sucesso = enfileirar(&filaEmerg, p);
            else if (p.prioridade == URGENCIA)     sucesso = enfileirar(&filaUrg, p);
            else if (p.prioridade == NAO_URGENCIA) sucesso = enfileirar(&filaNao, p);

            if (sucesso) {
                printf("Paciente registrado. Senha: %03d\n", p.senha);
                contadorSenha++;
            } else {
                printf("Erro: A fila para esta prioridade esta lotada!\n");
            }
            break;

        case 2:
            mostrarFilaAtual(&filaEmerg, &filaUrg, &filaNao);
            break;
        case 3:
            chamarProximo(&historico, &filaEmerg, &filaUrg, &filaNao);
            break;
        case 4:
            imprimirHistorico(historico);
            break;
        case 5:
            lerString("Informe a data (dd/mm/yyyy): ", data, TAM_TS);
            filtrarPorData(historico, data);
            break;
        case 6:
            senha = lerInteiro("Informe a senha a excluir: ");
            excluirPorSenha(&historico, senha);
            break;
        case 7:
            exportarHistoricoParaJSON(historico, "historico.json");
            break;
        case 8:
            exportarHistoricoParaCSV(historico, "historico.csv");
            break;
        case 9:
            calcularTempoMedioEspera(historico, "relatorio_tempo_espera.txt");
            break;
        case 0:
            printf("Liberando memoria e saindo...\n");
            liberarHistorico(&historico);
            break;
        default:
            printf("Opcao invalida!\n");
        }
        SLEEP(1);
    } while(opcao != 0);
}

int main() {
    menu();
    return 0;
}
