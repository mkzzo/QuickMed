# QuickMed

**Simulador de gerenciamento de filas hospitalares desenvolvido em C.** O projeto demonstra como estruturas de dados podem organizar pacientes por prioridade e registrar atendimentos em um fluxo simples executado pelo terminal.

> **Aviso:** o QuickMed é um projeto acadêmico e demonstrativo. Não substitui avaliação profissional, protocolos oficiais de triagem ou sistemas utilizados em serviços de saúde. Use somente dados fictícios.

## Funcionalidades

- Cadastro de pacientes com geração sequencial de senha.
- Classificação em três níveis: emergência, urgência e não urgente.
- Filas circulares independentes para cada nível, com atendimento prioritário e ordem FIFO dentro de cada fila.
- Consulta do próximo paciente a ser chamado.
- Registro, consulta, filtragem por data e exclusão de atendimentos do histórico durante a execução.
- Exportação manual do histórico para JSON e CSV.
- Geração de relatório de tempo médio de espera por prioridade.

## Estruturas de dados

O programa utiliza filas circulares para organizar os pacientes aguardando atendimento e uma lista encadeada para manter o histórico dos atendimentos concluídos. A prioridade de atendimento é definida nesta ordem:

1. Emergência
2. Urgência
3. Não urgente

Em cada categoria, os pacientes são chamados na ordem em que foram cadastrados.

## Requisitos

- Compilador C, como o GCC.
- Terminal com suporte à entrada de texto.

O código seleciona automaticamente a rotina de pausa compatível com Windows ou sistemas POSIX (Linux e macOS).

## Compilação e execução

### Linux e macOS

```bash
gcc -std=c99 -Wall -Wextra QuickMed.c -o quickmed
./quickmed
```

### Windows (GCC/MinGW)

```bash
gcc -std=c99 -Wall -Wextra QuickMed.c -o quickmed.exe
.\quickmed.exe
```

Após iniciar, siga as opções apresentadas no menu para cadastrar pacientes, chamar o próximo atendimento e consultar ou exportar os registros.

## Arquivos gerados

Os arquivos são criados no diretório de execução quando a opção correspondente é selecionada no menu:

| Arquivo | Conteúdo |
| --- | --- |
| `historico.json` | Histórico de atendimentos em JSON |
| `historico.csv` | Histórico de atendimentos em CSV |
| `relatorio_tempo_espera.txt` | Médias de espera agrupadas por prioridade |

As filas e o histórico permanecem em memória durante a execução. Para conservar os dados, exporte o histórico antes de encerrar o programa. Os arquivos exportados incluem informações pessoais, como nome e CPF: não utilize dados reais nem compartilhe esses arquivos publicamente.

## Estrutura do repositório

```text
QuickMed/
├── QuickMed.c
└── README.md
```

## Equipe

| Integrante | GitHub |
| --- | --- |
| Benito Juarez | [benito8448](https://github.com/benito8448) |
| Hiago Melo | [mkzzo](https://github.com/mkzzo) |

## Contexto acadêmico

Projeto desenvolvido no curso de Ciência da Computação da Universidade Braz Cubas, como demonstração de conceitos de Estrutura de Dados, Programação Procedural e Algoritmos. Apresentado na Mostra de Tecnologia do primeiro semestre de 2025, sob orientação da professora Dra. Andréa Ono Sakai.
