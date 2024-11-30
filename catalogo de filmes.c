#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>

// Definição de estrutura para armazenar informações sobre o filme
struct Filme {
    char titulo[50];    // Título do filme
    char genero[30];    // Gênero do filme
    int ano;            // Ano de lançamento do filme
    char status;        // Status do filme: ' ' (ativo), '*' (excluído)
};

typedef struct Filme Filme;  // definição de tipo

// Funções para manipulação de filmes
int tamanho(FILE *);            // Função que retorna a quantidade de filmes no arquivo
void cadastrar(FILE *);         // Função para cadastrar um novo filme
void consultar(FILE *);         // Função para consultar um filme pelo código
void gerarRelatorio(FILE *);    // Função para gerar um relatório dos filmes cadastrados
void excluir(FILE *);           // Função para excluir um filme do catálogo (marcando como excluído)

int main() {
    int op;
    FILE *arq;

    // Abre o arquivo binário para leitura e escrita. Se não existir, cria o arquivo.
    if ((arq = fopen("catalogo.dat", "rb+")) == NULL) {
        if ((arq = fopen("catalogo.dat", "wb+")) == NULL) {
            printf("Erro ao abrir o arquivo!\n");
            exit(1);
        }
    }

    // Loop principal do menu
    do {
        system("CLS");  // Limpa a tela
        printf("\n<<<< CATALOGO DE FILMES >>>>\n");
        printf("\nMENU:\n");
        printf("\n1. Cadastrar Filme\n");
        printf("2. Consultar Filmes Disponiveis pelo Codigo\n");
        printf("3. Gerar Relatorio\n");
        printf("4. Excluir Filmes Fora do Catalogo\n");
        printf("5. Sair\n");
        printf("__________________________________\n");
        printf("\nFilmes no catalogo: %d\n", tamanho(arq));  // Exibe a quantidade de filmes no catálogo
        printf("Escolha uma opcao: ");
        scanf("%d", &op);  // Lê a opção escolhida pelo usuário

        // Chama a função correspondente à opção escolhida
        switch (op) {
            case 1:
                cadastrar(arq);  // Cadastrar filme
                break;
            case 2:
                consultar(arq);  // Consultar filme
                break;
            case 3:
                gerarRelatorio(arq);  // Gerar relatório
                break;
            case 4:
                excluir(arq);  // Excluir filme
                break;
            case 5:
                fclose(arq);  // Fecha o arquivo ao sair
                printf("Saindo...\n");
                break;
            default:
                printf("Opcao invalida!\n");
                system("pause");  // Pausa a execução até o usuário pressionar uma tecla
        }
    } while (op != 5);  // Repete o menu até o usuário escolher sair

    return 0;
}

// Função para cadastrar um novo filme
void cadastrar(FILE *arq) {
    Filme filme;
    char confirma;

    filme.status = ' ';  // Status inicial como ativo
    fflush(stdin);  // Limpa o buffer de entrada

    printf("\n--- Cadastrar Um Novo Filme ---\n");
    printf("\nTitulo: ");
    getchar();  // Limpa a linha anterior
    fgets(filme.titulo, sizeof(filme.titulo), stdin);  // Lê o título
    filme.titulo[strcspn(filme.titulo, "\n")] = '\0';  // Remove a quebra de linha
    printf("Genero: ");
    fgets(filme.genero, sizeof(filme.genero), stdin);  // Lê o gênero
    filme.genero[strcspn(filme.genero, "\n")] = '\0';  // Remove a quebra de linha
    printf("Ano de lancamento: ");
    scanf("%d", &filme.ano);  // Lê o ano de lançamento
    printf("\nConfirma cadastro (s/n)? ");
    fflush(stdin);  // Limpa o buffer de entrada
    scanf("%c", &confirma);  // Lê a confirmação

    if (toupper(confirma) == 'S') {  // Se a resposta for 's' ou 'S'
        printf("\nSalvando filme...\n");
        fseek(arq, 0, SEEK_END);  // Move o ponteiro para o final do arquivo
        fwrite(&filme, sizeof(Filme), 1, arq);  // Grava o filme no arquivo
    } else {
        printf("\nCadastro cancelado.\n");
    }

    system("pause");  // Pausa a execução até o usuário pressionar uma tecla
}

// Função para consultar um filme pelo código
void consultar(FILE *arq) {
    Filme filme;
    int codigo;

    printf("\n--- Consultar Filme ---\n");
    printf("Informe o codigo do filme: ");
    scanf("%d", &codigo);  // Lê o código do filme

    if (codigo > 0 && codigo <= tamanho(arq)) {  // Verifica se o código é válido
        fseek(arq, (codigo - 1) * sizeof(Filme), SEEK_SET);  // Move o ponteiro para o filme desejado
        fread(&filme, sizeof(Filme), 1, arq);  // Lê o filme do arquivo

        if (filme.status == ' ') {  // Verifica se o filme está ativo
            printf("\nTitulo: %s", filme.titulo);
            printf("\nGenero: %s", filme.genero);
            printf("\nAno: %d\n", filme.ano);
        } else {
            printf("\nRegistro excluido.\n");
        }
    } else {
        printf("\nCodigo invalido!\n");  // Se o código for inválido
    }

    system("pause");  // Pausa a execução até o usuário pressionar uma tecla
}

// Função para gerar um relatório dos filmes
void gerarRelatorio(FILE *arq) {
    char nomeArquivo[50];
    Filme filme;
    int i;

    printf("\n--- Gerar Relatorio ---\n");
    printf("Nome do arquivo : ");
    scanf("%s", nomeArquivo);  // Lê o nome do arquivo de relatório
    strcat(nomeArquivo, ".txt");  // Adiciona a extensão .txt

    FILE *arqTexto = fopen(nomeArquivo, "w");  // Abre o arquivo de relatório para escrita
    if (!arqTexto) {
        printf("Erro ao criar arquivo de relatorio!\n");
        system("pause");
        return;
    }

    // Cabeçalho do relatório
    fprintf(arqTexto, "Codigo:          Titulo:            Genero:               Ano:\n");

    // Loop para escrever todos os filmes no relatório
    for (i = 0; i < tamanho(arq); i++) {
        fseek(arq, i * sizeof(Filme), SEEK_SET);  // Move o ponteiro para o filme
        fread(&filme, sizeof(Filme), 1, arq);  // Lê o filme

        if (filme.status == ' ') {  // Só inclui filmes ativos
            fprintf(arqTexto, "%-6d    %-25s %-20s %-4d\n", i + 1, filme.titulo, filme.genero, filme.ano);
        }
    }

    fprintf(arqTexto, "_____________________________________________________________\n");
    fclose(arqTexto);  // Fecha o arquivo de relatório

    printf("\nRelatorio gerado com sucesso em %s\n", nomeArquivo);
    system("pause");  // Pausa a execução até o usuário pressionar uma tecla
}

// Função para excluir um filme do catálogo (marcando como excluído)
void excluir(FILE *arq) {
    Filme filme;
    int codigo;
    char confirma;

    printf("\n--- Excluir Filme ---\n");
    printf("Informe o codigo do filme: ");
    scanf("%d", &codigo);  // Lê o código do filme

    if (codigo > 0 && codigo <= tamanho(arq)) {  // Verifica se o código é válido
        fseek(arq, (codigo - 1) * sizeof(Filme), SEEK_SET);  // Move o ponteiro para o filme desejado
        fread(&filme, sizeof(Filme), 1, arq);  // Lê o filme

        if (filme.status == ' ') {  // Verifica se o filme está ativo
            printf("\nTitulo: %s", filme.titulo);
            printf("\nGenero: %s", filme.genero);
            printf("\nAno: %d\n", filme.ano);
            printf("\nConfirma exclusao (s/n)? ");
            getchar();  // Limpa o buffer
            scanf("%c", &confirma);  // Lê a confirmação

            if (toupper(confirma) == 'S') {  // Se o usuário confirmar
                filme.status = '*';  // Marca o filme como excluído
                fseek(arq, (codigo - 1) * sizeof(Filme), SEEK_SET);  // Move o ponteiro para o filme
                fwrite(&filme, sizeof(Filme), 1, arq);  // Atualiza o filme no arquivo
                printf("\nFilme excluido com sucesso.\n");
            }
        } else {
            printf("\nRegistro ja excluido.\n");  // Se o filme já estiver excluído
        }
    } else {
        printf("\nCodigo invalido!\n");  // Se o código for inválido
    }

    system("pause");  // Pausa a execução até o usuário pressionar uma tecla
}

// Função que retorna a quantidade de filmes no arquivo
int tamanho(FILE *arq) {
    fseek(arq, 0, SEEK_END);  // Move o ponteiro para o final do arquivo
    return ftell(arq) / sizeof(Filme);  // Calcula a quantidade de filmes no arquivo
}
