/* ============================================================================
 *  SISTEMA DE GERENCIAMENTO DE BIBLIOTECA - LINGUAGEM C
 *  Universidade Sao Francisco - Linguagem de Programacao
 *
 *  O sistema permite cadastrar, consultar, alterar e excluir (CRUD):
 *      - Autores
 *      - Editoras
 *      - Livros
 *      - Usuarios
 *  E tambem controlar Emprestimos e Devolucoes de livros, atualizando
 *  automaticamente a quantidade de exemplares disponiveis.
 *
 *  Todos os dados sao salvos em arquivos binarios na mesma pasta do
 *  programa, entao nada se perde quando o programa e fechado.
 *
 *  COMO COMPILAR:   gcc sistema_biblioteca.c -o biblioteca
 *  COMO EXECUTAR:   ./biblioteca        (Linux/Mac)
 *                   biblioteca.exe      (Windows)
 *
 *  OBS: os textos do programa foram escritos sem acentuacao de proposito.
 *  Isso evita o problema classico de acentos aparecendo como simbolos
 *  estranhos no Prompt de Comando do Windows quando o compilador nao usa
 *  UTF-8 por padrao (um problema muito comum no Dev-C++/Code::Blocks).
 * ========================================================================== */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

/* ----------------------------------------------------------------------
 *  CONSTANTES DO SISTEMA
 *  MAX_... define o tamanho de cada vetor, ou seja, quantos registros
 *  cada entidade pode guardar ao mesmo tempo. ARQ_... e o nome do
 *  arquivo em disco usado para guardar cada tipo de dado.
 * ---------------------------------------------------------------------- */
#define MAX_LIVROS       200
#define MAX_AUTORES      100
#define MAX_EDITORAS     100
#define MAX_USUARIOS     200
#define MAX_EMPRESTIMOS  500

#define ARQ_LIVROS       "livros.dat"
#define ARQ_AUTORES      "autores.dat"
#define ARQ_EDITORAS     "editoras.dat"
#define ARQ_USUARIOS     "usuarios.dat"
#define ARQ_EMPRESTIMOS  "emprestimos.dat"

#define STATUS_EMPRESTADO 0   /* emprestimo em aberto, livro ainda com o usuario */
#define STATUS_DEVOLVIDO  1   /* emprestimo encerrado, livro ja devolvido        */

/* ----------------------------------------------------------------------
 *  ESTRUTURAS (structs)
 *  Cada struct agrupa as informacoes de uma "entidade" do sistema.
 *  Mantidas exatamente como definidas no planejamento do projeto.
 * ---------------------------------------------------------------------- */
struct Livro {
    int codigo;                  /* identificador unico do livro            */
    char titulo[100];            /* titulo do livro                         */
    int codigoAutor;             /* liga o livro a um registro de Autor     */
    int codigoEditora;           /* liga o livro a um registro de Editora   */
    int ano;                     /* ano de publicacao                       */
    int quantidade;              /* total de exemplares que a biblioteca tem */
    int quantidadeDisponivel;    /* quantos exemplares NAO estao emprestados */
};

struct Autor {
    int codigo;
    char nome[100];
    char nacionalidade[50];
};

struct Editora {
    int codigo;
    char nome[100];
    char cidade[50];
};

struct Usuario {
    int codigo;
    char nome[100];
    char cpf[15];
    char telefone[20];
};

struct Emprestimo {
    int codigo;
    int codigoUsuario;           /* liga o emprestimo a um Usuario */
    int codigoLivro;             /* liga o emprestimo a um Livro   */
    char dataEmprestimo[11];     /* formato DD/MM/AAAA (10 caracteres + '\0') */
    char dataDevolucao[11];      /* fica vazia ("") ate o livro ser devolvido */
    int status;                  /* STATUS_EMPRESTADO ou STATUS_DEVOLVIDO     */
};

/* ----------------------------------------------------------------------
 *  VARIAVEIS GLOBAIS
 *  Os vetores guardam todos os registros enquanto o programa roda na
 *  memoria RAM. As variaveis "total..." dizem quantas posicoes de cada
 *  vetor estao realmente sendo usadas (o resto do vetor fica "vazio").
 *  Sao globais porque quase toda funcao do sistema precisa ler ou
 *  alterar esses dados - assim evitamos passar 10 parametros toda hora.
 * ---------------------------------------------------------------------- */
struct Livro livros[MAX_LIVROS];
int totalLivros = 0;

struct Autor autores[MAX_AUTORES];
int totalAutores = 0;

struct Editora editoras[MAX_EDITORAS];
int totalEditoras = 0;

struct Usuario usuarios[MAX_USUARIOS];
int totalUsuarios = 0;

struct Emprestimo emprestimos[MAX_EMPRESTIMOS];
int totalEmprestimos = 0;

/* ----------------------------------------------------------------------
 *  PROTOTIPOS DAS FUNCOES
 *  Avisam o compilador que essas funcoes existem antes de serem usadas,
 *  permitindo que uma funcao chame outra que so foi escrita mais abaixo
 *  no arquivo.
 * ---------------------------------------------------------------------- */

/* Utilitarios de entrada e tela */
void lerLinha(char *destino, int tamanho);
int lerInteiro(const char *mensagem);
void limparTela(void);
void pausar(void);
void obterDataAtual(char *destino);

/* Busca (retornam a POSICAO no vetor, ou -1 se nao encontrar) */
int buscarAutorPorCodigo(int codigo);
int buscarEditoraPorCodigo(int codigo);
int buscarLivroPorCodigo(int codigo);
int buscarUsuarioPorCodigo(int codigo);

/* Exibicao das tabelas na tela */
void exibirAutores(void);
void exibirEditoras(void);
void exibirLivros(void);
void exibirUsuarios(void);
void exibirEmprestimos(void);

/* CRUD - Autor */
void cadastrarAutor(void);
void consultarAutores(void);
void alterarAutor(void);
void excluirAutor(void);

/* CRUD - Editora */
void cadastrarEditora(void);
void consultarEditoras(void);
void alterarEditora(void);
void excluirEditora(void);

/* CRUD - Livro */
void cadastrarLivro(void);
void consultarLivros(void);
void alterarLivro(void);
void excluirLivro(void);

/* CRUD - Usuario */
void cadastrarUsuario(void);
void consultarUsuarios(void);
void alterarUsuario(void);
void excluirUsuario(void);

/* Emprestimos e devolucoes */
void registrarEmprestimo(void);
void registrarDevolucao(void);
void consultarEmprestimos(void);

/* Persistencia (gravar/ler os arquivos em disco) */
void salvarAutores(void);
void carregarAutores(void);
void salvarEditoras(void);
void carregarEditoras(void);
void salvarLivros(void);
void carregarLivros(void);
void salvarUsuarios(void);
void carregarUsuarios(void);
void salvarEmprestimos(void);
void carregarEmprestimos(void);
void carregarTudo(void);

/* Menus */
void menuPrincipal(void);
void menuLivros(void);
void menuAutores(void);
void menuEditoras(void);
void menuUsuarios(void);
void menuEmprestimos(void);


/* ============================================================================
 *  FUNCAO PRINCIPAL (main)
 *  E o ponto de partida do programa: carrega os dados salvos, mostra uma
 *  mensagem de boas-vindas e entra no menu principal. Quando o menu
 *  principal termina (usuario escolheu Sair), o programa se encerra.
 * ========================================================================== */
int main(void) {
    carregarTudo();

    limparTela();
    printf("======================================================\n");
    printf("   BEM-VINDO AO SISTEMA DE GERENCIAMENTO DE BIBLIOTECA\n");
    printf("======================================================\n\n");
    printf("Dados carregados do disco:\n");
    printf("  - %d livro(s)\n", totalLivros);
    printf("  - %d autor(es)\n", totalAutores);
    printf("  - %d editora(s)\n", totalEditoras);
    printf("  - %d usuario(s)\n", totalUsuarios);
    printf("  - %d emprestimo(s)\n", totalEmprestimos);
    pausar();

    menuPrincipal();

    printf("\nTodos os dados foram salvos. Ate logo!\n");
    return 0;
}


/* ============================================================================
 *  FUNCOES UTILITARIAS
 * ========================================================================== */

/* ----------------------------------------------------------------------
 *  lerLinha
 *  Le uma linha de texto digitada pelo usuario e guarda em "destino".
 *  Usamos fgets (em vez de scanf) porque fgets le a linha inteira de
 *  uma vez, incluindo espacos, e evita o classico bug em C de misturar
 *  leitura de numero com leitura de texto e "sobrar" um Enter no buffer.
 * ---------------------------------------------------------------------- */
void lerLinha(char *destino, int tamanho) {
    if (fgets(destino, tamanho, stdin) == NULL) {
        /* Entrada terminou de forma inesperada (fim de arquivo, por
           exemplo). Para o programa nao "quebrar", tratamos como uma
           linha vazia. */
        destino[0] = '\0';
        return;
    }

    int len = strlen(destino);

    if (len > 0 && destino[len - 1] == '\n') {
        /* Remove o '\n' que o fgets deixa no final da string */
        destino[len - 1] = '\0';
    } else if (len == tamanho - 1) {
        /* O texto digitado era maior que o buffer: ainda sobrou parte
           da linha esperando no teclado. Descartamos essa sobra para
           que ela nao seja lida sem querer na proxima pergunta. */
        int c;
        while ((c = getchar()) != '\n' && c != EOF) {
            /* apenas descarta os caracteres */
        }
    }
}

/* ----------------------------------------------------------------------
 *  lerInteiro
 *  Mostra uma mensagem, le uma linha de texto e converte para numero
 *  inteiro usando strtol. Se o usuario digitar algo que nao seja um
 *  numero valido (letras, vazio, etc.), o programa avisa e pede de
 *  novo, em vez de travar ou aceitar lixo.
 * ---------------------------------------------------------------------- */
int lerInteiro(const char *mensagem) {
    char buffer[50];
    char *sobra;
    long valor;

    while (1) {
        printf("%s", mensagem);
        lerLinha(buffer, sizeof(buffer));

        valor = strtol(buffer, &sobra, 10);

        /* "sobra" aponta para o primeiro caractere que NAO fez parte do
           numero. Se sobra == buffer, nada foi convertido (texto vazio
           ou nao numerico). Se *sobra != '\0', sobrou lixo depois do
           numero (ex: "12a"). Nos dois casos, o valor e invalido. */
        if (sobra != buffer && *sobra == '\0') {
            return (int) valor;
        }

        printf("  >> Valor invalido. Digite somente numeros.\n");
    }
}

/* ----------------------------------------------------------------------
 *  limparTela
 *  Limpa a tela do terminal para deixar os menus mais organizados.
 *  Usa o comando certo dependendo do sistema operacional.
 * ---------------------------------------------------------------------- */
void limparTela(void) {
#ifdef _WIN32
    system("cls");
#else
    system("clear");
#endif
}

/* ----------------------------------------------------------------------
 *  pausar
 *  Para a execucao ate o usuario apertar ENTER, dando tempo de ler as
 *  mensagens na tela antes de voltar para o menu. Descarta qualquer
 *  coisa digitada antes do ENTER, para nao atrapalhar a proxima leitura.
 * ---------------------------------------------------------------------- */
void pausar(void) {
    printf("\nPressione ENTER para continuar...");
    int c;
    while ((c = getchar()) != '\n' && c != EOF) {
        /* descarta */
    }
}

/* ----------------------------------------------------------------------
 *  obterDataAtual
 *  Pega a data atual do relogio do computador e escreve no formato
 *  DD/MM/AAAA dentro de "destino". Usada para preencher automaticamente
 *  as datas de emprestimo e devolucao, sem depender de o usuario
 *  digitar a data certa.
 * ---------------------------------------------------------------------- */
void obterDataAtual(char *destino) {
    time_t agora = time(NULL);
    struct tm *tempoLocal = localtime(&agora);
    strftime(destino, 11, "%d/%m/%Y", tempoLocal);
}


/* ============================================================================
 *  FUNCOES DE BUSCA
 *  Percorrem o vetor procurando um registro com o codigo informado.
 *  Retornam a POSICAO (indice) do registro no vetor, ou -1 se nao achar.
 *  Retornar a posicao (em vez do registro inteiro) permite tanto LER
 *  quanto ALTERAR o registro original a partir do vetor global.
 * ========================================================================== */
int buscarAutorPorCodigo(int codigo) {
    int i;
    for (i = 0; i < totalAutores; i++) {
        if (autores[i].codigo == codigo) {
            return i;
        }
    }
    return -1;
}

int buscarEditoraPorCodigo(int codigo) {
    int i;
    for (i = 0; i < totalEditoras; i++) {
        if (editoras[i].codigo == codigo) {
            return i;
        }
    }
    return -1;
}

int buscarLivroPorCodigo(int codigo) {
    int i;
    for (i = 0; i < totalLivros; i++) {
        if (livros[i].codigo == codigo) {
            return i;
        }
    }
    return -1;
}

int buscarUsuarioPorCodigo(int codigo) {
    int i;
    for (i = 0; i < totalUsuarios; i++) {
        if (usuarios[i].codigo == codigo) {
            return i;
        }
    }
    return -1;
}


/* ============================================================================
 *  FUNCOES DE EXIBICAO (tabelas)
 *  Apenas imprimem os dados na tela, sem pausar. Sao reaproveitadas
 *  tanto pelas telas de "consultar" quanto pelas de "alterar/excluir"
 *  (que precisam mostrar a lista antes de pedir um codigo).
 * ========================================================================== */
void exibirAutores(void) {
    if (totalAutores == 0) {
        printf("Nenhum autor cadastrado.\n");
        return;
    }
    printf("%-6s %-30s %-20s\n", "Cod.", "Nome", "Nacionalidade");
    printf("---------------------------------------------------------\n");
    int i;
    for (i = 0; i < totalAutores; i++) {
        printf("%-6d %-30s %-20s\n", autores[i].codigo, autores[i].nome, autores[i].nacionalidade);
    }
}

void exibirEditoras(void) {
    if (totalEditoras == 0) {
        printf("Nenhuma editora cadastrada.\n");
        return;
    }
    printf("%-6s %-30s %-20s\n", "Cod.", "Nome", "Cidade");
    printf("---------------------------------------------------------\n");
    int i;
    for (i = 0; i < totalEditoras; i++) {
        printf("%-6d %-30s %-20s\n", editoras[i].codigo, editoras[i].nome, editoras[i].cidade);
    }
}

void exibirLivros(void) {
    if (totalLivros == 0) {
        printf("Nenhum livro cadastrado.\n");
        return;
    }
    printf("%-6s %-30s %-6s %-6s %-6s\n", "Cod.", "Titulo", "Ano", "Qtd", "Disp.");
    printf("-------------------------------------------------------------\n");
    int i;
    for (i = 0; i < totalLivros; i++) {
        printf("%-6d %-30s %-6d %-6d %-6d\n",
               livros[i].codigo, livros[i].titulo, livros[i].ano,
               livros[i].quantidade, livros[i].quantidadeDisponivel);
    }
}

void exibirUsuarios(void) {
    if (totalUsuarios == 0) {
        printf("Nenhum usuario cadastrado.\n");
        return;
    }
    printf("%-6s %-30s %-15s %-15s\n", "Cod.", "Nome", "CPF", "Telefone");
    printf("-------------------------------------------------------------------\n");
    int i;
    for (i = 0; i < totalUsuarios; i++) {
        printf("%-6d %-30s %-15s %-15s\n",
               usuarios[i].codigo, usuarios[i].nome, usuarios[i].cpf, usuarios[i].telefone);
    }
}

void exibirEmprestimos(void) {
    if (totalEmprestimos == 0) {
        printf("Nenhum emprestimo registrado.\n");
        return;
    }
    printf("%-6s %-20s %-20s %-12s %-12s %-12s\n",
           "Cod.", "Usuario", "Livro", "Emprestimo", "Devolucao", "Status");
    printf("---------------------------------------------------------------------------------\n");
    int i;
    for (i = 0; i < totalEmprestimos; i++) {
        int posU = buscarUsuarioPorCodigo(emprestimos[i].codigoUsuario);
        int posL = buscarLivroPorCodigo(emprestimos[i].codigoLivro);

        const char *nomeUsuario = (posU != -1) ? usuarios[posU].nome : "(removido)";
        const char *tituloLivro = (posL != -1) ? livros[posL].titulo : "(removido)";
        const char *statusTexto = (emprestimos[i].status == STATUS_EMPRESTADO) ? "Emprestado" : "Devolvido";
        const char *devolucao   = (strlen(emprestimos[i].dataDevolucao) > 0) ? emprestimos[i].dataDevolucao : "--";

        printf("%-6d %-20s %-20s %-12s %-12s %-12s\n",
               emprestimos[i].codigo, nomeUsuario, tituloLivro,
               emprestimos[i].dataEmprestimo, devolucao, statusTexto);
    }
}


/* ============================================================================
 *  CRUD - AUTOR
 * ========================================================================== */

/* ----------------------------------------------------------------------
 *  cadastrarAutor
 *  O codigo do novo autor e gerado automaticamente (maior codigo ja
 *  usado + 1), assim o usuario nunca digita nem repete um codigo.
 * ---------------------------------------------------------------------- */
void cadastrarAutor(void) {
    limparTela();
    printf("===== CADASTRAR AUTOR =====\n\n");

    if (totalAutores >= MAX_AUTORES) {
        printf("Limite maximo de autores atingido!\n");
        pausar();
        return;
    }

    struct Autor novo;
    int maior = 0, i;
    for (i = 0; i < totalAutores; i++) {
        if (autores[i].codigo > maior) maior = autores[i].codigo;
    }
    novo.codigo = maior + 1;
    printf("Codigo gerado automaticamente: %d\n\n", novo.codigo);

    printf("Nome do autor: ");
    lerLinha(novo.nome, sizeof(novo.nome));

    printf("Nacionalidade: ");
    lerLinha(novo.nacionalidade, sizeof(novo.nacionalidade));

    autores[totalAutores] = novo;   /* copia a struct inteira para o vetor */
    totalAutores++;

    salvarAutores();                /* grava no arquivo imediatamente */

    printf("\nAutor cadastrado com sucesso!\n");
    pausar();
}

void consultarAutores(void) {
    limparTela();
    printf("===== LISTA DE AUTORES =====\n\n");
    exibirAutores();
    pausar();
}

/* ----------------------------------------------------------------------
 *  alterarAutor
 *  Mostra a lista, pede o codigo do autor a alterar, mostra os dados
 *  atuais e pede os novos valores para sobrescrever.
 * ---------------------------------------------------------------------- */
void alterarAutor(void) {
    limparTela();
    printf("===== ALTERAR AUTOR =====\n\n");

    if (totalAutores == 0) {
        printf("Nenhum autor cadastrado.\n");
        pausar();
        return;
    }

    exibirAutores();

    int codigo = lerInteiro("\nCodigo do autor a alterar (0 para cancelar): ");
    if (codigo == 0) return;

    int pos = buscarAutorPorCodigo(codigo);
    if (pos == -1) {
        printf("\nAutor nao encontrado!\n");
        pausar();
        return;
    }

    printf("\nDados atuais -> Nome: %s | Nacionalidade: %s\n", autores[pos].nome, autores[pos].nacionalidade);

    printf("Novo nome: ");
    lerLinha(autores[pos].nome, sizeof(autores[pos].nome));

    printf("Nova nacionalidade: ");
    lerLinha(autores[pos].nacionalidade, sizeof(autores[pos].nacionalidade));

    salvarAutores();

    printf("\nAutor atualizado com sucesso!\n");
    pausar();
}

/* ----------------------------------------------------------------------
 *  excluirAutor
 *  Antes de excluir, verifica se algum Livro usa este autor. Se usar,
 *  bloqueia a exclusao para nao deixar um livro "orfao" (apontando
 *  para um autor que nao existe mais).
 * ---------------------------------------------------------------------- */
void excluirAutor(void) {
    limparTela();
    printf("===== EXCLUIR AUTOR =====\n\n");

    if (totalAutores == 0) {
        printf("Nenhum autor cadastrado.\n");
        pausar();
        return;
    }

    exibirAutores();

    int codigo = lerInteiro("\nCodigo do autor a excluir (0 para cancelar): ");
    if (codigo == 0) return;

    int pos = buscarAutorPorCodigo(codigo);
    if (pos == -1) {
        printf("\nAutor nao encontrado!\n");
        pausar();
        return;
    }

    int i;
    for (i = 0; i < totalLivros; i++) {
        if (livros[i].codigoAutor == codigo) {
            printf("\nNao e possivel excluir: existe(m) livro(s) cadastrado(s) com este autor.\n");
            pausar();
            return;
        }
    }

    /* "Desloca" todos os registros depois da posicao excluida uma casa
       para tras, cobrindo o espaco do registro removido. */
    for (i = pos; i < totalAutores - 1; i++) {
        autores[i] = autores[i + 1];
    }
    totalAutores--;

    salvarAutores();

    printf("\nAutor excluido com sucesso!\n");
    pausar();
}


/* ============================================================================
 *  CRUD - EDITORA  (mesma logica do Autor)
 * ========================================================================== */
void cadastrarEditora(void) {
    limparTela();
    printf("===== CADASTRAR EDITORA =====\n\n");

    if (totalEditoras >= MAX_EDITORAS) {
        printf("Limite maximo de editoras atingido!\n");
        pausar();
        return;
    }

    struct Editora nova;
    int maior = 0, i;
    for (i = 0; i < totalEditoras; i++) {
        if (editoras[i].codigo > maior) maior = editoras[i].codigo;
    }
    nova.codigo = maior + 1;
    printf("Codigo gerado automaticamente: %d\n\n", nova.codigo);

    printf("Nome da editora: ");
    lerLinha(nova.nome, sizeof(nova.nome));

    printf("Cidade: ");
    lerLinha(nova.cidade, sizeof(nova.cidade));

    editoras[totalEditoras] = nova;
    totalEditoras++;

    salvarEditoras();

    printf("\nEditora cadastrada com sucesso!\n");
    pausar();
}

void consultarEditoras(void) {
    limparTela();
    printf("===== LISTA DE EDITORAS =====\n\n");
    exibirEditoras();
    pausar();
}

void alterarEditora(void) {
    limparTela();
    printf("===== ALTERAR EDITORA =====\n\n");

    if (totalEditoras == 0) {
        printf("Nenhuma editora cadastrada.\n");
        pausar();
        return;
    }

    exibirEditoras();

    int codigo = lerInteiro("\nCodigo da editora a alterar (0 para cancelar): ");
    if (codigo == 0) return;

    int pos = buscarEditoraPorCodigo(codigo);
    if (pos == -1) {
        printf("\nEditora nao encontrada!\n");
        pausar();
        return;
    }

    printf("\nDados atuais -> Nome: %s | Cidade: %s\n", editoras[pos].nome, editoras[pos].cidade);

    printf("Novo nome: ");
    lerLinha(editoras[pos].nome, sizeof(editoras[pos].nome));

    printf("Nova cidade: ");
    lerLinha(editoras[pos].cidade, sizeof(editoras[pos].cidade));

    salvarEditoras();

    printf("\nEditora atualizada com sucesso!\n");
    pausar();
}

void excluirEditora(void) {
    limparTela();
    printf("===== EXCLUIR EDITORA =====\n\n");

    if (totalEditoras == 0) {
        printf("Nenhuma editora cadastrada.\n");
        pausar();
        return;
    }

    exibirEditoras();

    int codigo = lerInteiro("\nCodigo da editora a excluir (0 para cancelar): ");
    if (codigo == 0) return;

    int pos = buscarEditoraPorCodigo(codigo);
    if (pos == -1) {
        printf("\nEditora nao encontrada!\n");
        pausar();
        return;
    }

    int i;
    for (i = 0; i < totalLivros; i++) {
        if (livros[i].codigoEditora == codigo) {
            printf("\nNao e possivel excluir: existe(m) livro(s) cadastrado(s) com esta editora.\n");
            pausar();
            return;
        }
    }

    for (i = pos; i < totalEditoras - 1; i++) {
        editoras[i] = editoras[i + 1];
    }
    totalEditoras--;

    salvarEditoras();

    printf("\nEditora excluida com sucesso!\n");
    pausar();
}


/* ============================================================================
 *  CRUD - LIVRO
 *  E o mais complexo, porque um Livro se relaciona com um Autor e com
 *  uma Editora (guarda apenas o CODIGO deles, nao os dados inteiros).
 * ========================================================================== */
void cadastrarLivro(void) {
    limparTela();
    printf("===== CADASTRAR LIVRO =====\n\n");

    if (totalLivros >= MAX_LIVROS) {
        printf("Limite maximo de livros atingido!\n");
        pausar();
        return;
    }
    if (totalAutores == 0) {
        printf("Cadastre pelo menos um autor antes de cadastrar um livro.\n");
        pausar();
        return;
    }
    if (totalEditoras == 0) {
        printf("Cadastre pelo menos uma editora antes de cadastrar um livro.\n");
        pausar();
        return;
    }

    struct Livro novo;
    int maior = 0, i;
    for (i = 0; i < totalLivros; i++) {
        if (livros[i].codigo > maior) maior = livros[i].codigo;
    }
    novo.codigo = maior + 1;
    printf("Codigo gerado automaticamente: %d\n\n", novo.codigo);

    printf("Titulo do livro: ");
    lerLinha(novo.titulo, sizeof(novo.titulo));

    /* Mostra os autores cadastrados para o usuario escolher um codigo
       valido, em vez de ter que decorar ou adivinhar o numero. */
    printf("\nAutores cadastrados:\n");
    exibirAutores();
    int codAutor;
    while (1) {
        codAutor = lerInteiro("\nCodigo do autor deste livro (0 para cancelar o cadastro): ");
        if (codAutor == 0) {
            printf("\nCadastro cancelado.\n");
            pausar();
            return;
        }
        if (buscarAutorPorCodigo(codAutor) != -1) break;
        printf("  >> Codigo de autor invalido, tente novamente.\n");
    }
    novo.codigoAutor = codAutor;

    printf("\nEditoras cadastradas:\n");
    exibirEditoras();
    int codEditora;
    while (1) {
        codEditora = lerInteiro("\nCodigo da editora deste livro (0 para cancelar o cadastro): ");
        if (codEditora == 0) {
            printf("\nCadastro cancelado.\n");
            pausar();
            return;
        }
        if (buscarEditoraPorCodigo(codEditora) != -1) break;
        printf("  >> Codigo de editora invalido, tente novamente.\n");
    }
    novo.codigoEditora = codEditora;

    novo.ano = lerInteiro("Ano de publicacao: ");

    int qtd;
    do {
        qtd = lerInteiro("Quantidade de exemplares: ");
        if (qtd <= 0) printf("  >> A quantidade deve ser maior que zero.\n");
    } while (qtd <= 0);

    novo.quantidade = qtd;
    novo.quantidadeDisponivel = qtd;  /* no cadastro, nenhum exemplar esta emprestado ainda */

    livros[totalLivros] = novo;
    totalLivros++;

    salvarLivros();

    printf("\nLivro cadastrado com sucesso!\n");
    pausar();
}

/* ----------------------------------------------------------------------
 *  consultarLivros
 *  Alem de listar todos os livros, permite ver os detalhes de um livro
 *  especifico, ja "traduzindo" o codigoAutor/codigoEditora para o nome
 *  de verdade (isso e o "relacionamento" entre as structs mencionado
 *  no planejamento do projeto).
 * ---------------------------------------------------------------------- */
void consultarLivros(void) {
    limparTela();
    printf("===== LISTA DE LIVROS =====\n\n");
    exibirLivros();

    if (totalLivros == 0) {
        pausar();
        return;
    }

    int codigo = lerInteiro("\nCodigo de um livro para ver detalhes (0 para voltar): ");
    if (codigo != 0) {
        int pos = buscarLivroPorCodigo(codigo);
        if (pos == -1) {
            printf("\nLivro nao encontrado!\n");
        } else {
            int posAutor = buscarAutorPorCodigo(livros[pos].codigoAutor);
            int posEditora = buscarEditoraPorCodigo(livros[pos].codigoEditora);

            printf("\n----- DETALHES DO LIVRO -----\n");
            printf("Codigo.......: %d\n", livros[pos].codigo);
            printf("Titulo.......: %s\n", livros[pos].titulo);
            printf("Autor........: %s\n", (posAutor != -1) ? autores[posAutor].nome : "(nao encontrado)");
            printf("Editora......: %s\n", (posEditora != -1) ? editoras[posEditora].nome : "(nao encontrada)");
            printf("Ano..........: %d\n", livros[pos].ano);
            printf("Exemplares...: %d (disponiveis: %d)\n", livros[pos].quantidade, livros[pos].quantidadeDisponivel);
        }
    }
    pausar();
}

/* ----------------------------------------------------------------------
 *  alterarLivro
 *  Ao mudar a quantidade TOTAL de exemplares, a quantidade DISPONIVEL
 *  e ajustada pela diferenca, sem nunca ficar negativa nem maior que
 *  o novo total (protege a consistencia dos dados).
 * ---------------------------------------------------------------------- */
void alterarLivro(void) {
    limparTela();
    printf("===== ALTERAR LIVRO =====\n\n");

    if (totalLivros == 0) {
        printf("Nenhum livro cadastrado.\n");
        pausar();
        return;
    }

    exibirLivros();

    int codigo = lerInteiro("\nCodigo do livro a alterar (0 para cancelar): ");
    if (codigo == 0) return;

    int pos = buscarLivroPorCodigo(codigo);
    if (pos == -1) {
        printf("\nLivro nao encontrado!\n");
        pausar();
        return;
    }

    printf("\nTitulo atual: %s\n", livros[pos].titulo);
    printf("Novo titulo: ");
    lerLinha(livros[pos].titulo, sizeof(livros[pos].titulo));

    printf("\nAno atual: %d\n", livros[pos].ano);
    livros[pos].ano = lerInteiro("Novo ano: ");

    printf("\nQuantidade atual: %d (disponiveis: %d)\n", livros[pos].quantidade, livros[pos].quantidadeDisponivel);
    int novaQtd;
    do {
        novaQtd = lerInteiro("Nova quantidade total de exemplares: ");
        if (novaQtd <= 0) printf("  >> A quantidade deve ser maior que zero.\n");
    } while (novaQtd <= 0);

    int diferenca = novaQtd - livros[pos].quantidade;
    livros[pos].quantidadeDisponivel += diferenca;
    if (livros[pos].quantidadeDisponivel < 0) livros[pos].quantidadeDisponivel = 0;
    if (livros[pos].quantidadeDisponivel > novaQtd) livros[pos].quantidadeDisponivel = novaQtd;
    livros[pos].quantidade = novaQtd;

    salvarLivros();

    printf("\nLivro atualizado com sucesso!\n");
    pausar();
}

/* ----------------------------------------------------------------------
 *  excluirLivro
 *  Bloqueia a exclusao se o livro tiver algum emprestimo em aberto
 *  (status == STATUS_EMPRESTADO), para nao perder o historico de um
 *  livro que ainda esta com um usuario.
 * ---------------------------------------------------------------------- */
void excluirLivro(void) {
    limparTela();
    printf("===== EXCLUIR LIVRO =====\n\n");

    if (totalLivros == 0) {
        printf("Nenhum livro cadastrado.\n");
        pausar();
        return;
    }

    exibirLivros();

    int codigo = lerInteiro("\nCodigo do livro a excluir (0 para cancelar): ");
    if (codigo == 0) return;

    int pos = buscarLivroPorCodigo(codigo);
    if (pos == -1) {
        printf("\nLivro nao encontrado!\n");
        pausar();
        return;
    }

    int i;
    for (i = 0; i < totalEmprestimos; i++) {
        if (emprestimos[i].codigoLivro == codigo && emprestimos[i].status == STATUS_EMPRESTADO) {
            printf("\nNao e possivel excluir: este livro possui emprestimo(s) em aberto.\n");
            pausar();
            return;
        }
    }

    for (i = pos; i < totalLivros - 1; i++) {
        livros[i] = livros[i + 1];
    }
    totalLivros--;

    salvarLivros();

    printf("\nLivro excluido com sucesso!\n");
    pausar();
}


/* ============================================================================
 *  CRUD - USUARIO  (mesma logica do Autor/Editora)
 * ========================================================================== */
void cadastrarUsuario(void) {
    limparTela();
    printf("===== CADASTRAR USUARIO =====\n\n");

    if (totalUsuarios >= MAX_USUARIOS) {
        printf("Limite maximo de usuarios atingido!\n");
        pausar();
        return;
    }

    struct Usuario novo;
    int maior = 0, i;
    for (i = 0; i < totalUsuarios; i++) {
        if (usuarios[i].codigo > maior) maior = usuarios[i].codigo;
    }
    novo.codigo = maior + 1;
    printf("Codigo gerado automaticamente: %d\n\n", novo.codigo);

    printf("Nome: ");
    lerLinha(novo.nome, sizeof(novo.nome));

    printf("CPF: ");
    lerLinha(novo.cpf, sizeof(novo.cpf));

    printf("Telefone: ");
    lerLinha(novo.telefone, sizeof(novo.telefone));

    usuarios[totalUsuarios] = novo;
    totalUsuarios++;

    salvarUsuarios();

    printf("\nUsuario cadastrado com sucesso!\n");
    pausar();
}

void consultarUsuarios(void) {
    limparTela();
    printf("===== LISTA DE USUARIOS =====\n\n");
    exibirUsuarios();
    pausar();
}

void alterarUsuario(void) {
    limparTela();
    printf("===== ALTERAR USUARIO =====\n\n");

    if (totalUsuarios == 0) {
        printf("Nenhum usuario cadastrado.\n");
        pausar();
        return;
    }

    exibirUsuarios();

    int codigo = lerInteiro("\nCodigo do usuario a alterar (0 para cancelar): ");
    if (codigo == 0) return;

    int pos = buscarUsuarioPorCodigo(codigo);
    if (pos == -1) {
        printf("\nUsuario nao encontrado!\n");
        pausar();
        return;
    }

    printf("\nDados atuais -> Nome: %s | CPF: %s | Telefone: %s\n",
           usuarios[pos].nome, usuarios[pos].cpf, usuarios[pos].telefone);

    printf("Novo nome: ");
    lerLinha(usuarios[pos].nome, sizeof(usuarios[pos].nome));

    printf("Novo CPF: ");
    lerLinha(usuarios[pos].cpf, sizeof(usuarios[pos].cpf));

    printf("Novo telefone: ");
    lerLinha(usuarios[pos].telefone, sizeof(usuarios[pos].telefone));

    salvarUsuarios();

    printf("\nUsuario atualizado com sucesso!\n");
    pausar();
}

void excluirUsuario(void) {
    limparTela();
    printf("===== EXCLUIR USUARIO =====\n\n");

    if (totalUsuarios == 0) {
        printf("Nenhum usuario cadastrado.\n");
        pausar();
        return;
    }

    exibirUsuarios();

    int codigo = lerInteiro("\nCodigo do usuario a excluir (0 para cancelar): ");
    if (codigo == 0) return;

    int pos = buscarUsuarioPorCodigo(codigo);
    if (pos == -1) {
        printf("\nUsuario nao encontrado!\n");
        pausar();
        return;
    }

    int i;
    for (i = 0; i < totalEmprestimos; i++) {
        if (emprestimos[i].codigoUsuario == codigo && emprestimos[i].status == STATUS_EMPRESTADO) {
            printf("\nNao e possivel excluir: este usuario possui emprestimo(s) em aberto.\n");
            pausar();
            return;
        }
    }

    for (i = pos; i < totalUsuarios - 1; i++) {
        usuarios[i] = usuarios[i + 1];
    }
    totalUsuarios--;

    salvarUsuarios();

    printf("\nUsuario excluido com sucesso!\n");
    pausar();
}


/* ============================================================================
 *  EMPRESTIMOS E DEVOLUCOES
 *  Este e o modulo que "liga" Usuario e Livro. Ele e responsavel por
 *  verificar a disponibilidade e atualizar a quantidadeDisponivel do
 *  livro automaticamente, como pedido no planejamento do projeto.
 * ========================================================================== */
void consultarEmprestimos(void) {
    limparTela();
    printf("===== LISTA DE EMPRESTIMOS =====\n\n");
    exibirEmprestimos();
    pausar();
}

/* ----------------------------------------------------------------------
 *  registrarEmprestimo
 *  So permite o emprestimo se existir pelo menos 1 exemplar disponivel.
 *  A data do emprestimo e preenchida sozinha com a data de hoje.
 * ---------------------------------------------------------------------- */
void registrarEmprestimo(void) {
    limparTela();
    printf("===== REGISTRAR EMPRESTIMO =====\n\n");

    if (totalEmprestimos >= MAX_EMPRESTIMOS) {
        printf("Limite maximo de emprestimos atingido!\n");
        pausar();
        return;
    }
    if (totalUsuarios == 0) {
        printf("Nenhum usuario cadastrado.\n");
        pausar();
        return;
    }
    if (totalLivros == 0) {
        printf("Nenhum livro cadastrado.\n");
        pausar();
        return;
    }

    printf("Usuarios cadastrados:\n");
    exibirUsuarios();
    int codUsuario;
    while (1) {
        codUsuario = lerInteiro("\nCodigo do usuario (0 para cancelar): ");
        if (codUsuario == 0) {
            printf("\nOperacao cancelada.\n");
            pausar();
            return;
        }
        if (buscarUsuarioPorCodigo(codUsuario) != -1) break;
        printf("  >> Usuario nao encontrado, tente novamente.\n");
    }

    printf("\nLivros cadastrados:\n");
    exibirLivros();
    int codLivro, posLivro = -1;
    while (1) {
        codLivro = lerInteiro("\nCodigo do livro (0 para cancelar): ");
        if (codLivro == 0) {
            printf("\nOperacao cancelada.\n");
            pausar();
            return;
        }
        posLivro = buscarLivroPorCodigo(codLivro);
        if (posLivro == -1) {
            printf("  >> Livro nao encontrado, tente novamente.\n");
            continue;
        }
        if (livros[posLivro].quantidadeDisponivel <= 0) {
            printf("  >> Nao ha exemplares disponiveis deste livro no momento.\n");
            continue;
        }
        break;
    }

    struct Emprestimo novo;
    int maior = 0, i;
    for (i = 0; i < totalEmprestimos; i++) {
        if (emprestimos[i].codigo > maior) maior = emprestimos[i].codigo;
    }
    novo.codigo = maior + 1;
    novo.codigoUsuario = codUsuario;
    novo.codigoLivro = codLivro;
    obterDataAtual(novo.dataEmprestimo);   /* data de hoje, automatica */
    novo.dataDevolucao[0] = '\0';          /* ainda nao foi devolvido  */
    novo.status = STATUS_EMPRESTADO;

    emprestimos[totalEmprestimos] = novo;
    totalEmprestimos++;

    livros[posLivro].quantidadeDisponivel--;   /* um exemplar a menos disponivel */

    salvarEmprestimos();
    salvarLivros();

    printf("\nEmprestimo numero %d registrado com sucesso em %s!\n", novo.codigo, novo.dataEmprestimo);
    pausar();
}

/* ----------------------------------------------------------------------
 *  registrarDevolucao
 *  Marca um emprestimo em aberto como devolvido (preenchendo a data de
 *  hoje automaticamente) e devolve o exemplar ao estoque disponivel.
 * ---------------------------------------------------------------------- */
void registrarDevolucao(void) {
    limparTela();
    printf("===== REGISTRAR DEVOLUCAO =====\n\n");

    if (totalEmprestimos == 0) {
        printf("Nenhum emprestimo registrado.\n");
        pausar();
        return;
    }

    printf("Emprestimos em aberto:\n\n");
    int existeAberto = 0, i;
    for (i = 0; i < totalEmprestimos; i++) {
        if (emprestimos[i].status == STATUS_EMPRESTADO) {
            int posL = buscarLivroPorCodigo(emprestimos[i].codigoLivro);
            printf("Codigo %d - Livro: %s (emprestado em %s)\n",
                   emprestimos[i].codigo,
                   (posL != -1) ? livros[posL].titulo : "(removido)",
                   emprestimos[i].dataEmprestimo);
            existeAberto = 1;
        }
    }

    if (!existeAberto) {
        printf("Nao ha emprestimos em aberto no momento.\n");
        pausar();
        return;
    }

    int codigo = lerInteiro("\nCodigo do emprestimo a devolver (0 para cancelar): ");
    if (codigo == 0) return;

    int pos = -1;
    for (i = 0; i < totalEmprestimos; i++) {
        if (emprestimos[i].codigo == codigo) {
            pos = i;
            break;
        }
    }

    if (pos == -1) {
        printf("\nEmprestimo nao encontrado!\n");
        pausar();
        return;
    }
    if (emprestimos[pos].status == STATUS_DEVOLVIDO) {
        printf("\nEste emprestimo ja foi devolvido anteriormente.\n");
        pausar();
        return;
    }

    obterDataAtual(emprestimos[pos].dataDevolucao);
    emprestimos[pos].status = STATUS_DEVOLVIDO;

    int posLivro = buscarLivroPorCodigo(emprestimos[pos].codigoLivro);
    if (posLivro != -1) {
        livros[posLivro].quantidadeDisponivel++;
        if (livros[posLivro].quantidadeDisponivel > livros[posLivro].quantidade) {
            livros[posLivro].quantidadeDisponivel = livros[posLivro].quantidade;
        }
    }

    salvarEmprestimos();
    salvarLivros();

    printf("\nDevolucao registrada com sucesso em %s!\n", emprestimos[pos].dataDevolucao);
    pausar();
}


/* ============================================================================
 *  PERSISTENCIA EM ARQUIVO
 *  Cada par salvar/carregar grava (ou le) primeiro a quantidade de
 *  registros (um int) e depois o vetor inteiro de structs, em modo
 *  binario ("wb"/"rb"). Assim o proximo carregamento sabe exatamente
 *  quantos registros ler de volta.
 * ========================================================================== */
void salvarAutores(void) {
    FILE *arq = fopen(ARQ_AUTORES, "wb");
    if (arq == NULL) { printf("Erro ao salvar autores!\n"); return; }
    fwrite(&totalAutores, sizeof(int), 1, arq);
    fwrite(autores, sizeof(struct Autor), totalAutores, arq);
    fclose(arq);
}

void carregarAutores(void) {
    FILE *arq = fopen(ARQ_AUTORES, "rb");
    if (arq == NULL) { totalAutores = 0; return; }   /* arquivo ainda nao existe */
    fread(&totalAutores, sizeof(int), 1, arq);
    if (totalAutores < 0 || totalAutores > MAX_AUTORES) totalAutores = 0;
    else fread(autores, sizeof(struct Autor), totalAutores, arq);
    fclose(arq);
}

void salvarEditoras(void) {
    FILE *arq = fopen(ARQ_EDITORAS, "wb");
    if (arq == NULL) { printf("Erro ao salvar editoras!\n"); return; }
    fwrite(&totalEditoras, sizeof(int), 1, arq);
    fwrite(editoras, sizeof(struct Editora), totalEditoras, arq);
    fclose(arq);
}

void carregarEditoras(void) {
    FILE *arq = fopen(ARQ_EDITORAS, "rb");
    if (arq == NULL) { totalEditoras = 0; return; }
    fread(&totalEditoras, sizeof(int), 1, arq);
    if (totalEditoras < 0 || totalEditoras > MAX_EDITORAS) totalEditoras = 0;
    else fread(editoras, sizeof(struct Editora), totalEditoras, arq);
    fclose(arq);
}

void salvarLivros(void) {
    FILE *arq = fopen(ARQ_LIVROS, "wb");
    if (arq == NULL) { printf("Erro ao salvar livros!\n"); return; }
    fwrite(&totalLivros, sizeof(int), 1, arq);
    fwrite(livros, sizeof(struct Livro), totalLivros, arq);
    fclose(arq);
}

void carregarLivros(void) {
    FILE *arq = fopen(ARQ_LIVROS, "rb");
    if (arq == NULL) { totalLivros = 0; return; }
    fread(&totalLivros, sizeof(int), 1, arq);
    if (totalLivros < 0 || totalLivros > MAX_LIVROS) totalLivros = 0;
    else fread(livros, sizeof(struct Livro), totalLivros, arq);
    fclose(arq);
}

void salvarUsuarios(void) {
    FILE *arq = fopen(ARQ_USUARIOS, "wb");
    if (arq == NULL) { printf("Erro ao salvar usuarios!\n"); return; }
    fwrite(&totalUsuarios, sizeof(int), 1, arq);
    fwrite(usuarios, sizeof(struct Usuario), totalUsuarios, arq);
    fclose(arq);
}

void carregarUsuarios(void) {
    FILE *arq = fopen(ARQ_USUARIOS, "rb");
    if (arq == NULL) { totalUsuarios = 0; return; }
    fread(&totalUsuarios, sizeof(int), 1, arq);
    if (totalUsuarios < 0 || totalUsuarios > MAX_USUARIOS) totalUsuarios = 0;
    else fread(usuarios, sizeof(struct Usuario), totalUsuarios, arq);
    fclose(arq);
}

void salvarEmprestimos(void) {
    FILE *arq = fopen(ARQ_EMPRESTIMOS, "wb");
    if (arq == NULL) { printf("Erro ao salvar emprestimos!\n"); return; }
    fwrite(&totalEmprestimos, sizeof(int), 1, arq);
    fwrite(emprestimos, sizeof(struct Emprestimo), totalEmprestimos, arq);
    fclose(arq);
}

void carregarEmprestimos(void) {
    FILE *arq = fopen(ARQ_EMPRESTIMOS, "rb");
    if (arq == NULL) { totalEmprestimos = 0; return; }
    fread(&totalEmprestimos, sizeof(int), 1, arq);
    if (totalEmprestimos < 0 || totalEmprestimos > MAX_EMPRESTIMOS) totalEmprestimos = 0;
    else fread(emprestimos, sizeof(struct Emprestimo), totalEmprestimos, arq);
    fclose(arq);
}

/* Chama todos os "carregar" de uma vez so, usada no inicio do programa */
void carregarTudo(void) {
    carregarAutores();
    carregarEditoras();
    carregarLivros();
    carregarUsuarios();
    carregarEmprestimos();
}


/* ============================================================================
 *  MENUS
 *  Cada menu segue o mesmo padrao: um "do...while" que mostra as
 *  opcoes, le a escolha do usuario com lerInteiro, e usa um "switch"
 *  para chamar a funcao correspondente. O laco so termina quando o
 *  usuario digita 0 (Voltar/Sair).
 * ========================================================================== */
void menuPrincipal(void) {
    int opcao;
    do {
        limparTela();
        printf("======================================================\n");
        printf("     SISTEMA DE GERENCIAMENTO DE BIBLIOTECA\n");
        printf("======================================================\n\n");
        printf("  1 - Gerenciar Livros\n");
        printf("  2 - Gerenciar Autores\n");
        printf("  3 - Gerenciar Editoras\n");
        printf("  4 - Gerenciar Usuarios\n");
        printf("  5 - Gerenciar Emprestimos e Devolucoes\n");
        printf("  0 - Sair\n");
        printf("------------------------------------------------------\n");

        opcao = lerInteiro("Escolha uma opcao: ");

        switch (opcao) {
            case 1: menuLivros();      break;
            case 2: menuAutores();     break;
            case 3: menuEditoras();    break;
            case 4: menuUsuarios();    break;
            case 5: menuEmprestimos(); break;
            case 0: printf("\nEncerrando o sistema...\n"); break;
            default:
                printf("\nOpcao invalida!\n");
                pausar();
        }
    } while (opcao != 0);
}

void menuLivros(void) {
    int opcao;
    do {
        limparTela();
        printf("===== GERENCIAR LIVROS =====\n\n");
        printf("  1 - Cadastrar Livro\n");
        printf("  2 - Consultar Livros\n");
        printf("  3 - Alterar Livro\n");
        printf("  4 - Excluir Livro\n");
        printf("  0 - Voltar ao menu principal\n");
        printf("------------------------------------------------------\n");

        opcao = lerInteiro("Escolha uma opcao: ");

        switch (opcao) {
            case 1: cadastrarLivro(); break;
            case 2: consultarLivros(); break;
            case 3: alterarLivro(); break;
            case 4: excluirLivro(); break;
            case 0: break;
            default:
                printf("\nOpcao invalida!\n");
                pausar();
        }
    } while (opcao != 0);
}

void menuAutores(void) {
    int opcao;
    do {
        limparTela();
        printf("===== GERENCIAR AUTORES =====\n\n");
        printf("  1 - Cadastrar Autor\n");
        printf("  2 - Consultar Autores\n");
        printf("  3 - Alterar Autor\n");
        printf("  4 - Excluir Autor\n");
        printf("  0 - Voltar ao menu principal\n");
        printf("------------------------------------------------------\n");

        opcao = lerInteiro("Escolha uma opcao: ");

        switch (opcao) {
            case 1: cadastrarAutor(); break;
            case 2: consultarAutores(); break;
            case 3: alterarAutor(); break;
            case 4: excluirAutor(); break;
            case 0: break;
            default:
                printf("\nOpcao invalida!\n");
                pausar();
        }
    } while (opcao != 0);
}

void menuEditoras(void) {
    int opcao;
    do {
        limparTela();
        printf("===== GERENCIAR EDITORAS =====\n\n");
        printf("  1 - Cadastrar Editora\n");
        printf("  2 - Consultar Editoras\n");
        printf("  3 - Alterar Editora\n");
        printf("  4 - Excluir Editora\n");
        printf("  0 - Voltar ao menu principal\n");
        printf("------------------------------------------------------\n");

        opcao = lerInteiro("Escolha uma opcao: ");

        switch (opcao) {
            case 1: cadastrarEditora(); break;
            case 2: consultarEditoras(); break;
            case 3: alterarEditora(); break;
            case 4: excluirEditora(); break;
            case 0: break;
            default:
                printf("\nOpcao invalida!\n");
                pausar();
        }
    } while (opcao != 0);
}

void menuUsuarios(void) {
    int opcao;
    do {
        limparTela();
        printf("===== GERENCIAR USUARIOS =====\n\n");
        printf("  1 - Cadastrar Usuario\n");
        printf("  2 - Consultar Usuarios\n");
        printf("  3 - Alterar Usuario\n");
        printf("  4 - Excluir Usuario\n");
        printf("  0 - Voltar ao menu principal\n");
        printf("------------------------------------------------------\n");

        opcao = lerInteiro("Escolha uma opcao: ");

        switch (opcao) {
            case 1: cadastrarUsuario(); break;
            case 2: consultarUsuarios(); break;
            case 3: alterarUsuario(); break;
            case 4: excluirUsuario(); break;
            case 0: break;
            default:
                printf("\nOpcao invalida!\n");
                pausar();
        }
    } while (opcao != 0);
}

void menuEmprestimos(void) {
    int opcao;
    do {
        limparTela();
        printf("===== GERENCIAR EMPRESTIMOS E DEVOLUCOES =====\n\n");
        printf("  1 - Registrar Emprestimo\n");
        printf("  2 - Registrar Devolucao\n");
        printf("  3 - Consultar Emprestimos\n");
        printf("  0 - Voltar ao menu principal\n");
        printf("------------------------------------------------------\n");

        opcao = lerInteiro("Escolha uma opcao: ");

        switch (opcao) {
            case 1: registrarEmprestimo(); break;
            case 2: registrarDevolucao(); break;
            case 3: consultarEmprestimos(); break;
            case 0: break;
            default:
                printf("\nOpcao invalida!\n");
                pausar();
        }
    } while (opcao != 0);
}
