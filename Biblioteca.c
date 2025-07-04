#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <locale.h>
#include <time.h>

// Enum para gêneros de livros
typedef enum {
    FICCAO,
    DIDATICO,
    BIOGRAFIA,
    GENERO_MAX
} Genero;

// Enum para status do empréstimo/livro
typedef enum {
    CONCLUIDO,
    ANDAMENTO,
    CANCELADO,
    DISPONIVEL,
    EMPRESTADO
} Status;

const char *nomeGeneros[] = {
    "Ficção",
    "Didático",
    "Biografia",
    "Inválido"
};

const char* statusLivroToStr(Status status) {
    switch (status) {
        case DISPONIVEL: return "Disponível";
        case EMPRESTADO: return "Emprestado";
        case CONCLUIDO: return "Concluído";
        case ANDAMENTO: return "Em Andamento";
        case CANCELADO: return "Cancelado";
        default: return "Desconhecido";
    }
}

const char* generoToStr(Genero genero) {
    if (genero >= 0 && genero < GENERO_MAX)
        return nomeGeneros[genero];
    return "Outro";
}

// struct para livro
typedef struct Livro {
    char titulo[75];
    char autor[50];
    long long int isbn;
    Genero genero;
    Status status;
    struct Livro* prox;
} Livro;

// struct para empréstimo
typedef struct Emprestimo {
    int id;
    char leitor[50];
    long long int isbnLivro;
    char dataEmprestimo[11];
    Status status;
    struct Emprestimo* prox;
} Emprestimo;

// Listas encadeadas
Livro* listaLivros = NULL;
Emprestimo* listaEmprestimos = NULL;

// Gerador de ID único
int gerarIdEmprestimo() {
    static int id = 1;
    return id++;
}

// Buscar livro por ISBN
Livro* buscarLivroPorISBN(long long int isbn) {
    Livro* atual = listaLivros;
    while (atual != NULL) {
        if (atual->isbn == isbn)
            return atual;
        atual = atual->prox;
    }
    return NULL;
}

// Cadastrar livro
void cadastrarLivro() {
    Livro* novo = malloc(sizeof(Livro));
    if (!novo) {
        printf("Erro de memória!\n");
        return;
    }
    printf("Informe o titulo do livro: ");
    scanf(" %74[^\n]", novo->titulo);
    printf("Informe o autor do livro: ");
    scanf(" %49[^\n]", novo->autor);
    printf("Informe o ISBN do livro: ");
    scanf("%lld", &novo->isbn);
    printf("Informe o genero do livro (0-Ficção, 1-Didático, 2-Biografia): ");
    int genero;
    scanf("%d", &genero);
    if (genero < 0 || genero >= GENERO_MAX) {
        printf("Genero invalido! Tente novamente.\n");
        free(novo);
        return;
    }
    novo->genero = (Genero)genero;
    novo->status = DISPONIVEL;
    novo->prox = listaLivros;
    listaLivros = novo;
    printf("Livro cadastrado com sucesso!\n");
    printf("Titulo: %s\n", novo->titulo);
    printf("Autor: %s\n", novo->autor);
    printf("ISBN: %lld\n", novo->isbn);
    printf("Genero: %s\n", nomeGeneros[genero]);
}

// Remover livro
void removerLivro(){
    long long int isbn;
    printf("Informe o ISBN do livro a ser removido: ");
    scanf("%lld", &isbn);
    Livro *atual = listaLivros, *anterior = NULL;
    while (atual != NULL && atual->isbn != isbn) {
        anterior = atual;
        atual = atual->prox;
    }
    if (atual == NULL) {
        printf("Livro nao encontrado.\n");
        return;
    }
    if (atual->status == EMPRESTADO) {
        printf("Não é possível remover um livro emprestado.\n");
        return;
    }
    if (anterior == NULL){
        listaLivros = atual->prox;
    } else {
        anterior->prox = atual->prox;
    }
    free(atual);
    printf("Livro removido com sucesso!\n");
}

// Atualizar livro
void atualizarLivro(){
    long long int isbn;
    printf("Informe o ISBN do livro a ser atualizado: ");
    scanf("%lld", &isbn);

    Livro* atual = buscarLivroPorISBN(isbn);
    if (atual == NULL) {
        printf("Livro nao encontrado.\n");
        return;
    }
    printf("Novo titulo: ");
    scanf(" %74[^\n]", atual->titulo);
    printf("Novo autor: ");
    scanf(" %49[^\n]", atual->autor);
    printf("Novo ISBN: ");
    scanf("%lld", &atual->isbn);
    printf("Novo genero (0-Ficção, 1-Didático, 2-Biografia): ");
    int genero;
    scanf("%d", &genero);
    if (genero < 0 || genero >= GENERO_MAX) {
        printf("Genero invalido! Tente novamente.\n");
        return;
    }
    atual->genero = (Genero)genero;
    printf("Dados do livro atualizados com sucesso!\n");
    printf("Titulo: %s\n", atual->titulo);
    printf("Autor: %s\n", atual->autor);
    printf("ISBN: %lld\n", atual->isbn);
    printf("Genero: %s\n", nomeGeneros[atual->genero]);
}

// Salvar livros em arquivo
void salvarLivrosEmArquivo() {
    FILE* arquivo = fopen("livros.txt", "w");
    if (!arquivo) {
        printf("Erro ao abrir o arquivo para salvar livros.\n");
        return;
    }
    Livro* atual = listaLivros;
    while (atual != NULL) {
        fprintf(arquivo, "%s;%s;%lld;%d;%d\n", atual->titulo, atual->autor, atual->isbn, atual->genero, atual->status);
        atual = atual->prox;
    }
    fclose(arquivo);
    printf("Livros salvos com sucesso!\n");
}

// Carregar livros do arquivo
void carregarLivrosDoArquivo() {
    FILE* arquivo = fopen("livros.txt", "r");
    if (!arquivo) return;

    char linha[256];
    while(fgets(linha, sizeof(linha), arquivo)){
        Livro* novo = malloc(sizeof(Livro));
        if (!novo) continue;
        int genero, status;
        sscanf(linha, "%74[^;];%49[^;];%lld;%d;%d", novo->titulo, novo->autor, &novo->isbn, &genero, &status);
        novo->genero = (Genero)genero;
        novo->status = (Status)status;
        novo->prox = listaLivros;
        listaLivros = novo;
    }
    fclose(arquivo);
    printf("Livros carregados do arquivo com sucesso!\n");
}

// Cadastrar empréstimo
void cadastrarEmprestimo() {
    Emprestimo* novo = malloc(sizeof(Emprestimo));
    if (!novo) {
        printf("Erro de memória!\n");
        return;
    }
    printf("Informe o nome do leitor: ");
    scanf(" %49[^\n]", novo->leitor);
    printf("Informe o ISBN do livro: ");
    scanf("%lld", &novo->isbnLivro);

    Livro* livro = buscarLivroPorISBN(novo->isbnLivro);
    if (!livro) {
        printf("Livro não encontrado!\n");
        free(novo);
        return;
    }
    if (livro->status == EMPRESTADO) {
        printf("Livro já está emprestado!\n");
        free(novo);
        return;
    }

    printf("Informe a data do emprestimo (dd/mm/aaaa): ");
    scanf(" %10[^\n]", novo->dataEmprestimo);
    novo->id = gerarIdEmprestimo();
    novo->status = ANDAMENTO;
    novo->prox = listaEmprestimos;
    listaEmprestimos = novo;
    livro->status = EMPRESTADO;
    printf("Emprestimo cadastrado com sucesso!\n");
    printf("ID: %d, Leitor: %s, Livro: %s, Data: %s, Status: %s\n", novo->id, novo->leitor, livro->titulo, novo->dataEmprestimo, statusLivroToStr(novo->status));
}

// Cancelar empréstimo
void cancelarEmprestimo(int id) {
    Emprestimo* atual = listaEmprestimos;
    while (atual != NULL && atual->id != id) {
        atual = atual->prox;
    }
    if (atual == NULL) {
        printf("Emprestimo nao encontrado.\n");
        return;
    }
    Livro* livro = buscarLivroPorISBN(atual->isbnLivro);
    if (livro) livro->status = DISPONIVEL;
    atual->status = CANCELADO;
    printf("Emprestimo cancelado com sucesso!\n");
    printf("ID: %d, Leitor: %s, Livro ISBN: %lld, Data: %s, Status: %s\n", atual->id, atual->leitor, atual->isbnLivro, atual->dataEmprestimo, statusLivroToStr(atual->status));
}

// Registrar devolução de empréstimo (concluir)
void registrarEmprestimo(int id) {
    Emprestimo* atual = listaEmprestimos;
    while (atual != NULL && atual->id != id) {
        atual = atual->prox;
    }
    if (atual == NULL) {
        printf("Emprestimo nao encontrado.\n");
        return;
    }
    Livro* livro = buscarLivroPorISBN(atual->isbnLivro);
    if (livro) livro->status = DISPONIVEL;
    atual->status = CONCLUIDO;
    printf("Emprestimo registrado como concluido com sucesso!\n");
    printf("ID: %d, Leitor: %s, Livro ISBN: %lld, Data: %s, Status: %s\n", atual->id, atual->leitor, atual->isbnLivro, atual->dataEmprestimo, statusLivroToStr(atual->status));
}

// Listar empréstimos do dia atual
void listarEmprestimos(){
    time_t t = time(NULL);
    struct tm tm = *localtime(&t);
    char dataAtual[11];
    snprintf(dataAtual, sizeof(dataAtual), "%02d/%02d/%04d", tm.tm_mday, tm.tm_mon + 1, tm.tm_year + 1900);
    Emprestimo* atual = listaEmprestimos;
    printf("\nEmprestimos realizados no dia %s:\n", dataAtual);
    int encontrou = 0;
    while(atual != NULL){
        if(strcmp(atual->dataEmprestimo, dataAtual) == 0){
            Livro* livro = buscarLivroPorISBN(atual->isbnLivro);
            printf("ID: %d, Leitor: %s, Livro: %s, Status: %s\n", atual->id, atual->leitor, livro ? livro->titulo : "Desconhecido", statusLivroToStr(atual->status));
            encontrou = 1;
        }
        atual = atual->prox;
    }
    if (!encontrou) {
        printf("Nenhum emprestimo realizado hoje.\n");
    }
}

// Listar empréstimos por status
void listarEmprestimosPorStatus(Status status) {
    Emprestimo* atual = listaEmprestimos;
    int encontrou = 0;
    printf("Emprestimos com status %s:\n", statusLivroToStr(status));
    while (atual != NULL) {
        if (atual->status == status) {
            Livro* livro = buscarLivroPorISBN(atual->isbnLivro);
            printf("ID: %d, Leitor: %s, Livro: %s, Data: %s, Status: %s\n", atual->id, atual->leitor, livro ? livro->titulo : "Desconhecido", atual->dataEmprestimo, statusLivroToStr(atual->status));
            encontrou = 1;
        }
        atual = atual->prox;
    }
    if (!encontrou) {
        printf("Nenhum emprestimo encontrado com o status %s.\n", statusLivroToStr(status));
    }
}

// Salvar empréstimos em arquivo
void salvarEmprestimosEmArquivo() {
    FILE* arquivo = fopen("emprestimos.txt", "w");
    if (!arquivo) {
        printf("Erro ao abrir o arquivo para salvar emprestimos.\n");
        return;
    }
    Emprestimo* atual = listaEmprestimos;
    while (atual != NULL) {
        fprintf(arquivo, "%d;%s;%lld;%s;%d\n", atual->id, atual->leitor, atual->isbnLivro, atual->dataEmprestimo, atual->status);
        atual = atual->prox;
    }
    fclose(arquivo);
    printf("Emprestimos salvos com sucesso!\n");
}

// Carregar empréstimos do arquivo
void carregarEmprestimosDoArquivo() {
    FILE* arquivo = fopen("emprestimos.txt", "r");
    if (!arquivo) return;

    char linha[256];
    while (fgets(linha, sizeof(linha), arquivo)) {
        Emprestimo* novo = malloc(sizeof(Emprestimo));
        if (!novo) continue;
        int status;
        sscanf(linha, "%d;%49[^;];%lld;%10[^;];%d", &novo->id, novo->leitor, &novo->isbnLivro, novo->dataEmprestimo, &status);
        novo->status = (Status)status;
        novo->prox = listaEmprestimos;
        listaEmprestimos = novo;
    }
    fclose(arquivo);
    printf("Emprestimos carregados do arquivo com sucesso!\n");
}

// Listar livros por status
void listarLivrosPorStatus() {
    int status;
    printf("\nEscolha o status para listar:\n");
    printf("3 - Disponível\n4 - Emprestado\n");
    printf("Opção: ");
    scanf("%d", &status);

    if (status != DISPONIVEL && status != EMPRESTADO) {
        printf("Status inválido!\n");
        return;
    }

    printf("\n--- Livros com status %s ---\n", statusLivroToStr((Status)status));
    int encontrou = 0;
    Livro* atual = listaLivros;
    while (atual != NULL) {
        if (atual->status == status) {
            printf("Título: %s | Autor: %s | ISBN: %lld | Gênero: %s\n",
                atual->titulo, atual->autor, atual->isbn, generoToStr(atual->genero));
            encontrou = 1;
        }
        atual = atual->prox;
    }
    if (!encontrou) {
        printf("Nenhum livro encontrado com esse status.\n");
    }
}

// Listar livros por gênero
void listarLivrosPorGenero() {
    int genero;
    printf("\nEscolha o gênero para listar:\n");
    printf("0 - Ficção\n1 - Didático\n2 - Biografia\n");
    printf("Opção: ");
    scanf("%d", &genero);

    if (genero < 0 || genero >= GENERO_MAX) {
        printf("Gênero inválido!\n");
        return;
    }

    printf("\n--- Livros do gênero %s ---\n", generoToStr(genero));
    int encontrou = 0;
    Livro* atual = listaLivros;
    while (atual != NULL) {
        if ((int)atual->genero == genero) {
            printf("Título: %s | Autor: %s | ISBN: %lld | Status: %s\n",
                atual->titulo, atual->autor, atual->isbn, statusLivroToStr(atual->status));
            encontrou = 1;
        }
        atual = atual->prox;
    }
    if (!encontrou) {
        printf("Nenhum livro encontrado desse gênero.\n");
    }
}

// Menu principal
void menu() {
    setlocale(LC_ALL, "Portuguese");
    carregarLivrosDoArquivo();
    carregarEmprestimosDoArquivo();
    int opcao;
    do {
        printf("\n--- MENU ---\n");
        printf("1 - Cadastrar Livro\n");
        printf("2 - Remover Livro\n");
        printf("3 - Atualizar Livro\n");
        printf("4 - Cadastrar Empréstimo\n");
        printf("5 - Cancelar Empréstimo\n");
        printf("6 - Registrar Devolução de Empréstimo\n");
        printf("7 - Listar Empréstimos do Dia\n");
        printf("8 - Listar Empréstimos por Status\n");
        printf("9 - Listar Livros por Status\n");
        printf("10 - Listar Livros por Gênero\n");
        printf("11 - Salvar Livros em Arquivo\n");
        printf("12 - Salvar Empréstimos em Arquivo\n");
        printf("13 - Carregar Livros do Arquivo\n");
        printf("14 - Carregar Empréstimos do Arquivo\n");
        printf("0 - Sair\n");
        printf("Escolha uma opção: ");
        scanf("%d", &opcao);
        switch (opcao) {
            case 1: cadastrarLivro(); break;
            case 2: removerLivro(); break;
            case 3: atualizarLivro(); break;
            case 4: cadastrarEmprestimo(); break;
            case 5: {
                int id;
                printf("Informe o ID do emprestimo a ser cancelado: ");
                scanf("%d", &id);
                cancelarEmprestimo(id);
                break;
            }
            case 6: {
                int id;
                printf("Informe o ID do emprestimo a ser registrado como devolvido: ");
                scanf("%d", &id);
                registrarEmprestimo(id);
                break;
            }
            case 7: listarEmprestimos(); break;
            case 8: {
                int status;
                printf("Informe o status do emprestimo (0 - Concluído, 1 - Em Andamento, 2 - Cancelado): ");
                scanf("%d", &status);
                if (status < CONCLUIDO || status > CANCELADO) {
                    printf("Status inválido!\n");
                    break;
                }
                listarEmprestimosPorStatus((Status)status);
                break;
            }
            case 9: listarLivrosPorStatus(); break;
            case 10: listarLivrosPorGenero(); break;
            case 11: salvarLivrosEmArquivo(); break;
            case 12: salvarEmprestimosEmArquivo(); break;
            case 13: carregarLivrosDoArquivo(); break;
            case 14: carregarEmprestimosDoArquivo(); break;
            case 0: printf("Saindo...\n"); break;
            default: printf("Opção inválida! Tente novamente.\n");
        }
    } while (opcao != 0);

    // Libera memória alocada
    Livro* atualLivro = listaLivros;
    while (atualLivro != NULL) {
        Livro* temp = atualLivro;
        atualLivro = atualLivro->prox;
        free(temp);
    }
    Emprestimo* atualEmprestimo = listaEmprestimos;
    while (atualEmprestimo != NULL) {
        Emprestimo* temp = atualEmprestimo;
        atualEmprestimo = atualEmprestimo->prox;
        free(temp);
    }
    printf("Memória liberada. Programa encerrado.\n");
}

int main() {
    menu();
    return 0;
}
