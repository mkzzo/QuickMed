# 🚀 QuickMed – Sistema Inteligente de Gerenciamento de Filas Hospitalares

**Resumo:**
O QuickMed é um sistema desenvolvido em linguagem C que simula o gerenciamento inteligente de filas em ambientes hospitalares. Ele busca promover eficiência, organização e priorização clínica no atendimento de pacientes, aplicando conceitos fundamentais de estruturas de dados como listas e filas dinâmicas.

---

## 🎯 Objetivo

Este projeto tem como objetivo central demonstrar a aplicação prática de estruturas de dados no contexto da saúde pública, modelando o fluxo de atendimento hospitalar em três etapas: cadastro, triagem e atendimento médico. O sistema resolve problemas como ausência de controle no fluxo de pacientes, falta de priorização clínica, e dificuldade de rastreamento de atendimentos. Serve também como ferramenta didática para alunos e professores de disciplinas como Estrutura de Dados.

---

## 👨‍💻 Tecnologias Utilizadas

* Linguagem C
* Bibliotecas padrão (`stdio.h`, `stdlib.h`, `string.h`, `time.h`)
* Compatibilidade multiplataforma com `windows.h` e `unistd.h`
* Geração de relatórios em arquivos `.txt` e `.json`

---

## 🗂️ Estrutura do Projeto

```
📆 quickmed
├─ QuickMed.c             # Código principal do sistema
├─ README.md              # Documento explicativo
├─ historico.json         # Histórico exportado em formato JSON
└─ relatorio_tempo_espera.txt # Relatório de tempo médio de espera
```

---

## ⚙️ Como Executar

### ✅ Rodando Localmente (no Windows ou Linux)

1. Compile o código com um compilador C (como GCC):

```bash
gcc QuickMed.c -o quickmed
```

2. Execute o programa no terminal:

```bash
./quickmed   # Linux/Mac
quickmed.exe # Windows
```

> É necessário um terminal com suporte a entrada de texto.

---

## 📸 Demonstrações

* [x] Cadastro de paciente com geração de senha única
* [x] Triagem por prioridade (emergência, urgência, não urgente)
* [x] Organização de filas por prioridade
* [x] Chamada automatizada de pacientes
* [x] Registro e exportação de histórico
* [x] Relatórios em .txt e .json 

*(Imagens e vídeos podem ser adicionados conforme documentação da mostra)*

---

## 👥 Equipe

| Nome                    | GitHub                                           |
| ----------------------- | ------------------------------------------------ |
| Benito Juarez           | [@benitojuarez](https://github.com/benitojuarez) |
| Hiago Melo              | [@hiagomelo](github.com/mkzzo)                   |

---

## 🧠 Disciplinas Envolvidas

* Estrutura de Dados I
* Programação Procedural
* Algoritmos

---

## 🏫 Informações Acadêmicas

* Universidade: **Universidade Braz Cubas**
* Curso: **Ciência da Computação**
* Semestre: **3º**
* Período: **Manhã**
* Professora orientadora: **Dra. Andréa Ono Sakai**
* Evento: **Mostra de Tecnologia 1º Semestre de 2025**
* Local: **Laboratório 12**
* Datas: **05 e 06 de junho de 2025**

---

## 📄 Licença

MIT License — sinta-se à vontade para utilizar, estudar e adaptar este projeto.
