// Inclusao de bibliotecas necessarias
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

// Compatibilidade com Windows e Linux para funcao de pausa
#ifdef _WIN32
#include <windows.h>
#define SLEEP(x) Sleep(1000 * x)
#else
#include <unistd.h>
#define SLEEP(x) sleep(x)
#endif

// Definicoes de tamanhos fixos e prioridades
#define TAM_NOME 100
#define TAM_TS   20
#define EMERGENCIA    1
#define URGENCIA      2
#define NAO_URGENCIA  3

// Estrutura que representa um paciente
typedef struct Paciente {
	int  senha;
	char nome[TAM_NOME];
	int  dia_nasc;
	int  mes_nasc;
	int  ano_nasc;
	char cpf[15];
	int  prioridade;
	time_t hora_chegada;
} Paciente;

//lista ligada para historico de atendimentos
typedef struct NoLista {
	Paciente        paciente;
	char            timestamp[TAM_TS];
	time_t          hora_atendimento; 
	struct NoLista *prox;
} NoLista;

// Estrutura de fila circular simples
typedef struct Fila {
	Paciente dados[100];
	int inicio, fim;
} Fila;

// Variavel global para gerar senhas sequenciais
int contadorSenha = 1;

// Filas para cada nÃ­vel de prioridade
Fila filaEmerg, filaUrg, filaNao;

// Funcoes para manipular a fila
void inicializarFila(Fila *f) {
	f->inicio = f->fim = 0;
}

int filaVazia(Fila *f) {
	return f->inicio == f->fim;
}

void enfileirar(Fila *f, Paciente p) {
	f->dados[f->fim++] = p;
}

Paciente desenfileirar(Fila *f) {
	return f->dados[f->inicio++];
}

// Gera timestamp no formato dd/mm/yyyy hh:mm:ss
void obterTimestamp(char *buf, size_t tam) {
	time_t now = time(NULL);
	struct tm *t = localtime(&now);
	strftime(buf, tam, "%d/%m/%Y %H:%M:%S", t);
}

// Adiciona atendimento ao histÃ³rico
void inserirHistorico(NoLista **lista, Paciente p, const char *ts) {
	NoLista *novo = malloc(sizeof(NoLista));
	novo->paciente = p;
	strncpy(novo->timestamp, ts, TAM_TS);
	novo->hora_atendimento = time(NULL);
	novo->prox = *lista;
	*lista = novo;
}

// Exibe todo o histÃ³rico de atendimentos
void imprimirHistorico(NoLista *lista) {
	puts("\n=== Historico de Atendimentos ===");
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

// Calcula e salva tempo medio de espera por prioridade em arquivo
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

	// Escreve o relatorio
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

// Exibe quem é o proximo da fila, considerando prioridade
void mostrarFilaAtual() {
	printf("\nOrdem de Atendimento Atual:\n");
	if (!filaVazia(&filaEmerg))
		printf("Prox: Emergencia - %s (Senha %03d)\n", filaEmerg.dados[filaEmerg.inicio].nome, filaEmerg.dados[filaEmerg.inicio].senha);
	else if (!filaVazia(&filaUrg))
		printf("Prox: Urgencia - %s (Senha %03d)\n", filaUrg.dados[filaUrg.inicio].nome, filaUrg.dados[filaUrg.inicio].senha);
	else if (!filaVazia(&filaNao))
		printf("Prox: Nao Urgente - %s (Senha %03d)\n", filaNao.dados[filaNao.inicio].nome, filaNao.dados[filaNao.inicio].senha);
	else
		printf("Nenhum paciente aguardando.\n");
}

// Chama proximo paciente baseado na prioridade e registra no histÃ³rico
void chamarProximo(NoLista **historico) {
	Paciente p;
	if (!filaVazia(&filaEmerg)) p = desenfileirar(&filaEmerg);
	else if (!filaVazia(&filaUrg)) p = desenfileirar(&filaUrg);
	else if (!filaVazia(&filaNao)) p = desenfileirar(&filaNao);
	else {
		puts("Nenhum paciente aguardando.\n");
		return;
	}

	printf("\n[Atendimento] %s (Senha %03d | Prioridade %d)\n", p.nome, p.senha, p.prioridade);
	printf("Press ENTER para finalizar atendimento...");
	getchar();

	char ts[TAM_TS];
	obterTimestamp(ts, TAM_TS);
	inserirHistorico(historico, p, ts);
	puts("Atendimento concluido e registrado.\n");
}

// Filtra atendimentos por data
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

// Remove um registro do historico com base na senha
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

// Exporta o historico de atendimentos em formato JSON
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

// Exporta o historico de atendimentos em formato CSV
void exportarHistoricoParaCSV(NoLista *lista, const char *arquivo) {
	FILE *fp = fopen(arquivo, "w");
	if (!fp) {
		perror("Erro ao criar arquivo CSV");
		return;
	}
	//fprintf(fp, "Timestamp,Senha,Nome,Data de Nascimento,CPF,Prioridade\n");
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

// Função principal de menu interativo
void menu() {
	NoLista *historico = NULL;
	int opcao, senha;
	char data[TAM_TS];
	Paciente p;
	char ts[TAM_TS];

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
		printf("Escolha: ");
		scanf("%d", &opcao);
		getchar();

		switch(opcao) {
		case 1:
			// Coleta dados do paciente e o insere na fila apropriada
			printf("\nNome: ");
			fgets(p.nome, TAM_NOME, stdin);
			p.nome[strcspn(p.nome,"\n")] = 0;

			printf("Data de nascimento:\n");
			printf("  Dia: ");
			scanf("%d", &p.dia_nasc); getchar();
			printf("  Mes: ");
			scanf("%d", &p.mes_nasc); getchar();
			printf("  Ano: ");
			scanf("%d", &p.ano_nasc); getchar();

			printf("CPF: ");
			fgets(p.cpf, 15, stdin);
			p.cpf[strcspn(p.cpf,"\n")] = 0;

			printf("Prioridade (1-Emergencia, 2-Urgente, 3-Nao Urgente): ");
			scanf("%d", &p.prioridade); getchar();

			p.senha = contadorSenha++;
			p.hora_chegada = time(NULL);

			switch (p.prioridade) {
				case EMERGENCIA: enfileirar(&filaEmerg, p); break;
				case URGENCIA: enfileirar(&filaUrg, p); break;
				case NAO_URGENCIA: enfileirar(&filaNao, p); break;
				default: puts("Prioridade invalida.\n"); break;
			}
			printf("Paciente registrado. Senha: %03d\n", p.senha);
			break;
		case 2:
			mostrarFilaAtual(); break;
		case 3:
			chamarProximo(&historico); break;
		case 4:
			imprimirHistorico(historico); break;
		case 5:
			printf("Informe a data (dd/mm/yyyy): ");
			fgets(data, TAM_TS, stdin);
			data[strcspn(data,"\n")] = 0;
			filtrarPorData(historico, data);
			break;
		case 6:
			printf("Informe a senha a excluir: ");
			scanf("%d", &senha); getchar();
			excluirPorSenha(&historico, senha);
			break;
		case 7:
			exportarHistoricoParaJSON(historico, "historico.json"); break;
		case 8:
			exportarHistoricoParaCSV(historico, "historico.csv"); break;
		case 9:
			calcularTempoMedioEspera(historico, "relatorio_tempo_espera.txt"); break;
		case 0:
			printf("Saindo...\n"); break;
		default:
			printf("Opcao invalida!\n");
		}
		SLEEP(1);
	} while(opcao != 0);
}

// Função principal que inicia o programa
int main() {
	srand(time(NULL)); // Inicializa gerador de nÃºmeros aleatÃ³rios
	menu();            // Inicia menu interativo
	return 0;
}
