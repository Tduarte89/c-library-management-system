# 📚 Sistema de Gerenciamento de Biblioteca em C

🌍 **Idioma:** [English](README.md) | Português

![Linguagem C](https://img.shields.io/badge/Linguagem-C99%20%2F%20C11-00599C?logo=c&logoColor=white)
![Compilador](https://img.shields.io/badge/Compilador-GCC%20%7C%20Clang%20%7C%20MSVC-blue)
![Build](https://img.shields.io/badge/Build-Makefile-orange)
![Armazenamento](https://img.shields.io/badge/Armazenamento-Arquivos%20Binários%20(.dat)-lightgrey)
![Status](https://img.shields.io/badge/Status-Concluído-brightgreen)

Sistema de Gerenciamento de Biblioteca relacional, operado via console, implementado em **C padrão (C99)** sem dependências externas. A aplicação modela um domínio completo de biblioteca com operações **CRUD** completas para Livros, Autores, Editoras e Usuários, combinadas com um mecanismo de **controle de empréstimos e devoluções** e **persistência em arquivos binários**.

Originalmente concebido e desenvolvido como projeto acadêmico na **Universidade São Francisco (USF)** para a disciplina de *Linguagem de Programação*.

---

## 🎯 Visão Geral do Domínio e Arquitetura

Bibliotecas comunitárias e escolares frequentemente controlam seu acervo e circulação usando planilhas avulsas ou registros em papel. Este software oferece uma solução leve, determinística e autossuficiente que roda em qualquer computador sem necessidade de conexão com internet ou servidores de banco de dados.

---

## 🚀 Funcionalidades Principais

### 1. Gerenciamento Relacional de Entidades (CRUD)
- **Autores e Editoras:** Entidades independentes do catálogo com geração automática de IDs.
- **Livros:** Entidade relacional referenciando Autor e Editora via chaves estrangeiras.
- **Usuários:** Cadastro de leitores com dados de contato e identificação.

### 2. Motor de Transações de Empréstimo e Devolução
- **Validação de Estoque:** Impede empréstimos quando `quantidadeDisponivel == 0`.
- **Atualização Automática de Estoque:** Decrementa o acervo no empréstimo e restaura na devolução.
- **Registro de Data Automático:** Registra as datas de empréstimo e devolução via `<time.h>`.
- **Máquina de Status:** Garante transições válidas (`EMPRESTADO` → `DEVOLVIDO`).

### 3. Integridade Referencial e Proteção de Dados
- **Sem Registros Órfãos:** Bloqueia a exclusão de autores ou editoras que ainda possuem livros cadastrados.
- **Proteção por Empréstimo Ativo:** Impede a exclusão de livros ou usuários com empréstimos em aberto.

### 4. Tratamento Robusto de Entradas
- **Proteção contra Buffer Overflow:** Entradas de texto usam `fgets` com remoção de `\n` e limpeza de stream.
- **Validação Numérica:** Usa `strtol` para rejeitar caracteres não numéricos e entradas inválidas.
- **CLI Multiplataforma:** Suporte a compilação condicional (`#ifdef _WIN32`) para limpeza nativa de tela em Windows, macOS e Linux.

### 5. Persistência em Arquivos Binários
- O estado é serializado em arquivos `.dat` a cada alteração usando `fwrite` e carregado na inicialização com `fread`.

---

## 📁 Estrutura do Repositório

```text
c-library-management-system/
│
├── src/
│   ├── main.c                 # Código-fonte principal (versão em Inglês)
│   └── sistema_biblioteca.c   # Código-fonte original (versão em Português)
│
├── docs/
│   └── academic-report-ptbr.pdf # Relatório acadêmico completo
│
├── Makefile                   # Script de automação de build para GCC
├── .gitignore                 # Ignora binários, objetos compilados e arquivos .dat
├── README.md                  # Documentação principal em Inglês
└── README.pt-br.md            # Documentação em Português
```

---

## ⚙️ Compilação e Execução

### Pré-requisitos
- GCC, Clang ou qualquer compilador compatível com C99.
- Make (opcional, para build automatizado).

### Opção 1: Compilar com `make` (Linux / macOS / MinGW)

```bash
# Compilar o executável
make

# Executar o programa
make run

# Limpar artefatos de build
make clean
```

### Opção 2: Compilar diretamente com `gcc`

#### Linux / macOS:
```bash
gcc -Wall -Wextra -std=c99 src/main.c -o biblioteca
./biblioteca
```

#### Windows (Prompt de Comando / PowerShell):
```cmd
gcc -Wall -Wextra -std=c99 src\main.c -o biblioteca.exe
biblioteca.exe
```

> [!NOTE]
> **Usuários Windows:** Se você usa o **Dev-C++** ou **Code::Blocks**, basta criar um novo projeto e importar o arquivo `src/sistema_biblioteca.c` diretamente para compilar e executar pela IDE.

---

## 👥 Autores e Contexto Acadêmico

Projeto desenvolvido na **Universidade São Francisco (USF)**.

- **Thiago A. Duarte**
  - LinkedIn: [Thiago Duarte](https://www.linkedin.com/in/thiago-duarte-32a64839)
  - GitHub: [Tduarte89](https://github.com/Tduarte89)

**Colaboradores do Projeto Acadêmico:**  
Guilherme Henrique de Souza Ramos, Júlio Cesar Grilo dos Santos, Pablo Pericles de Assis Souza.
