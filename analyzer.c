/*
 * ============================================================================
 *  ANALISADOR LÉXICO — Colônia Espacial Ares-7
 *  Disciplina: Linguagens, Autômatos e Computabilidade
 * ============================================================================
 *
 *  Descrição:
 *    Realiza a varredura caractere por caractere de um script Python
 *    interceptado, classificando cada cadeia de caracteres em Lexemas e
 *    Tokens, e alimentando uma Tabela de Símbolos (somente identificadores,
 *    sem palavras reservadas nem literais).
 *
 *  Compilação:
 *    gcc -Wall -Wextra -pedantic -o analyzer analyzer.c
 *
 *  Execução:
 *    ./analyzer
 *
 * ============================================================================
 */

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* ---------------------------------------------------------------------------
 * Constantes
 * --------------------------------------------------------------------------- */
#define MAX_LEXEME_LEN 512

/* ---------------------------------------------------------------------------
 * Tipos de Tokens reconhecidos pelo analisador
 * --------------------------------------------------------------------------- */
typedef enum {
    T_KEYWORD,
    T_IDENTIFIER,
    T_NUMBER,
    T_STRING,
    T_OPERATOR,
    T_DELIMITER,
    T_COMMENT,
    T_UNKNOWN
} TokenType;

/* ---------------------------------------------------------------------------
 * Struct que modela cada nó da Tabela de Símbolos (lista encadeada simples)
 * Requisito: uso obrigatório de struct + alocação dinâmica (malloc)
 * --------------------------------------------------------------------------- */
typedef struct SymbolNode {
    char name[MAX_LEXEME_LEN];   /* nome do identificador                     */
    int  first_line;             /* linha da primeira ocorrência               */
    struct SymbolNode *next;     /* ponteiro para o próximo nó da lista        */
} SymbolNode;

/* Cabeça da lista encadeada da Tabela de Símbolos */
static SymbolNode *symbol_table_head = NULL;

/* ---------------------------------------------------------------------------
 * Tabela de palavras reservadas do Python (subconjunto relevante ao input)
 *
 * Inclui keywords verdadeiras (class, def, while, None, True, False, return,
 * if, else, for, import, from, pass, break, continue, and, or, not, in, is)
 * e builtins tratados como "reservados" para o escopo da avaliação
 * (print, self, end, range, len, int, str, float, list, dict, set, tuple).
 *
 * Justificativa: `self` e `print` não são keywords sintáticas puras do Python,
 * mas no contexto desta análise léxica acadêmica, tratá-los como reservados
 * evita poluir a Tabela de Símbolos com nomes que não representam variáveis
 * criadas pelo programador.
 * --------------------------------------------------------------------------- */
static const char *KEYWORDS[] = {
    /* --- keywords sintáticas do Python --- */
    "class",    "def",      "return",   "if",       "elif",
    "else",     "while",    "for",      "import",   "from",
    "pass",     "break",    "continue", "and",      "or",
    "not",      "in",       "is",       "with",     "as",
    "try",      "except",   "finally",  "raise",    "yield",
    "lambda",   "global",   "nonlocal", "assert",   "del",
    /* --- constantes built-in --- */
    "None",     "True",     "False",
    /* --- builtins tratados como reservados para a análise --- */
    "print",    "self",     "end",      "range",    "len",
    "int",      "str",      "float",    "list",     "dict",
    "set",      "tuple",    "type",     "super",    "input",
    NULL  /* sentinela */
};

/* ---------------------------------------------------------------------------
 * is_keyword — verifica se um lexema é palavra reservada
 *
 * Percorre a tabela KEYWORDS comparando com o lexema recebido.
 * Retorna 1 (verdadeiro) se encontrar, 0 caso contrário.
 * --------------------------------------------------------------------------- */
static int is_keyword(const char *lexeme)
{
    for (int i = 0; KEYWORDS[i] != NULL; i++) {
        if (strcmp(lexeme, KEYWORDS[i]) == 0)
            return 1;
    }
    return 0;
}

/* ---------------------------------------------------------------------------
 * token_type_to_string — converte o enum TokenType em string legível
 * --------------------------------------------------------------------------- */
static const char *token_type_to_string(TokenType t)
{
    switch (t) {
        case T_KEYWORD:    return "KEYWORD";
        case T_IDENTIFIER: return "IDENTIFIER";
        case T_NUMBER:     return "NUMBER";
        case T_STRING:     return "STRING";
        case T_OPERATOR:   return "OPERATOR";
        case T_DELIMITER:  return "DELIMITER";
        case T_COMMENT:    return "COMMENT";
        case T_UNKNOWN:    return "UNKNOWN";
    }
    return "UNKNOWN";
}

/* ---------------------------------------------------------------------------
 * emit_token — imprime um token no formato padronizado da saída
 * --------------------------------------------------------------------------- */
static void emit_token(TokenType type, const char *lexeme, int line)
{
    printf("  Linha %3d | [%-10s] Lexema: %s\n",
           line, token_type_to_string(type), lexeme);
}

/* ---------------------------------------------------------------------------
 * insert_symbol — insere um identificador na Tabela de Símbolos
 *
 * • Evita duplicatas: percorre a lista antes de inserir.
 * • Usa malloc para alocação dinâmica (requisito obrigatório).
 * • Insere na cabeça da lista (O(1) para inserção).
 * --------------------------------------------------------------------------- */
static void insert_symbol(const char *name, int line)
{
    /* Verificação de duplicata */
    SymbolNode *cur = symbol_table_head;
    while (cur != NULL) {
        if (strcmp(cur->name, name) == 0)
            return;  /* já existe, não insere novamente */
        cur = cur->next;
    }

    /* Alocação dinâmica do novo nó */
    SymbolNode *node = (SymbolNode *)malloc(sizeof(SymbolNode));
    if (node == NULL) {
        fprintf(stderr, "Erro fatal: falha na alocacao de memoria.\n");
        exit(EXIT_FAILURE);
    }

    strncpy(node->name, name, MAX_LEXEME_LEN - 1);
    node->name[MAX_LEXEME_LEN - 1] = '\0';
    node->first_line = line;
    node->next = symbol_table_head;
    symbol_table_head = node;
}

/* ---------------------------------------------------------------------------
 * free_symbol_table — libera toda a memória da Tabela de Símbolos
 *
 * Boa prática: sempre liberar o que foi alocado com malloc.
 * --------------------------------------------------------------------------- */
static void free_symbol_table(void)
{
    SymbolNode *cur = symbol_table_head;
    while (cur != NULL) {
        SymbolNode *tmp = cur;
        cur = cur->next;
        free(tmp);
    }
    symbol_table_head = NULL;
}

/* ---------------------------------------------------------------------------
 * print_symbol_table — imprime o conteúdo completo da Tabela de Símbolos
 * --------------------------------------------------------------------------- */
static void print_symbol_table(void)
{
    printf("\n");
    printf("  ============================================================\n");
    printf("  ||            TABELA DE SIMBOLOS (Identificadores)        ||\n");
    printf("  ============================================================\n");
    printf("  | %-4s | %-30s | %-10s |\n", "#", "Identificador", "Linha");
    printf("  |------|--------------------------------|------------|\n");

    int count = 0;

    /*
     * Como a lista é inserida na cabeça, a ordem natural é invertida.
     * Para imprimir na ordem de primeira aparição, coletamos em um array
     * auxiliar e percorremos de trás para frente.
     */
    SymbolNode *cur = symbol_table_head;
    SymbolNode *arr[1024];
    int n = 0;
    while (cur != NULL && n < 1024) {
        arr[n++] = cur;
        cur = cur->next;
    }

    for (int i = n - 1; i >= 0; i--) {
        count++;
        printf("  | %-4d | %-30s | %-10d |\n",
               count, arr[i]->name, arr[i]->first_line);
    }

    printf("  ============================================================\n");
    printf("  Total de identificadores unicos: %d\n", count);
}

/* ---------------------------------------------------------------------------
 * scan — O núcleo do Analisador Léxico
 *
 * Percorre o código-fonte caractere por caractere, classificando cada
 * sequência reconhecida em seu respectivo tipo de token. Ignora espaços
 * em branco irrelevantes e rastreia o número da linha atual.
 *
 * Categorias reconhecidas:
 *   KEYWORD     — palavras reservadas do Python
 *   IDENTIFIER  — nomes definidos pelo programador (variáveis, classes, funcs)
 *   NUMBER      — literais inteiros
 *   STRING      — literais delimitados por aspas simples ou duplas
 *   OPERATOR    — =, ==, !=, ., +, -, *, /, %, <, >, <=, >=
 *   DELIMITER   — :, ;, ,, (, ), [, ], {, }
 *   COMMENT     — linhas iniciadas por #
 * --------------------------------------------------------------------------- */
static void scan(const char *source)
{
    int i      = 0;
    int len    = (int)strlen(source);
    int line   = 1;       /* contador de linhas (para rastreamento) */
    char buf[MAX_LEXEME_LEN];
    int  bi;              /* índice do buffer */

    printf("  ============================================================\n");
    printf("  ||             FLUXO DE LEXEMAS E TOKENS                  ||\n");
    printf("  ============================================================\n\n");

    while (i < len) {
        char c = source[i];

        /* --- Controle de linha --- */
        if (c == '\n') {
            line++;
            i++;
            continue;
        }

        /* --- Ignora espaços em branco (espaço, tab, \r) --- */
        if (c == ' ' || c == '\t' || c == '\r') {
            i++;
            continue;
        }

        /* --- Comentários: do '#' até o fim da linha --- */
        if (c == '#') {
            bi = 0;
            while (i < len && source[i] != '\n') {
                if (bi < MAX_LEXEME_LEN - 1)
                    buf[bi++] = source[i];
                i++;
            }
            buf[bi] = '\0';
            emit_token(T_COMMENT, buf, line);
            continue;   /* o '\n' será tratado na próxima iteração */
        }

        /* --- Strings: delimitadas por " ou ' --- */
        if (c == '"' || c == '\'') {
            char quote = c;
            bi = 0;
            buf[bi++] = c;  /* inclui a aspa de abertura */
            i++;

            while (i < len && source[i] != quote) {
                /* Tratamento de sequências de escape (\n, \t, \", \\, etc.) */
                if (source[i] == '\\' && (i + 1) < len) {
                    if (bi < MAX_LEXEME_LEN - 2) {
                        buf[bi++] = source[i];
                        buf[bi++] = source[i + 1];
                    }
                    i += 2;
                    continue;
                }
                if (bi < MAX_LEXEME_LEN - 1)
                    buf[bi++] = source[i];
                i++;
            }

            if (i < len) {
                buf[bi++] = source[i];  /* inclui a aspa de fechamento */
                i++;
            }
            buf[bi] = '\0';
            emit_token(T_STRING, buf, line);
            continue;
        }

        /* --- Números inteiros --- */
        if (isdigit((unsigned char)c)) {
            bi = 0;
            while (i < len && isdigit((unsigned char)source[i])) {
                if (bi < MAX_LEXEME_LEN - 1)
                    buf[bi++] = source[i];
                i++;
            }
            /* Suporte a números com ponto decimal (ex: 3.14) */
            if (i < len && source[i] == '.' && (i + 1) < len
                && isdigit((unsigned char)source[i + 1])) {
                buf[bi++] = source[i++]; /* o '.' */
                while (i < len && isdigit((unsigned char)source[i])) {
                    if (bi < MAX_LEXEME_LEN - 1)
                        buf[bi++] = source[i];
                    i++;
                }
            }
            buf[bi] = '\0';
            emit_token(T_NUMBER, buf, line);
            continue;
        }

        /* --- Identificadores e Keywords --- */
        if (isalpha((unsigned char)c) || c == '_') {
            bi = 0;
            while (i < len &&
                   (isalnum((unsigned char)source[i]) || source[i] == '_')) {
                if (bi < MAX_LEXEME_LEN - 1)
                    buf[bi++] = source[i];
                i++;
            }
            buf[bi] = '\0';

            if (is_keyword(buf)) {
                emit_token(T_KEYWORD, buf, line);
            } else {
                emit_token(T_IDENTIFIER, buf, line);
                insert_symbol(buf, line);
            }
            continue;
        }

        /* --- Operadores compostos (dois caracteres) --- */
        if (i + 1 < len) {
            char next = source[i + 1];
            int matched_op = 0;

            /* ==, !=, <=, >=, +=, -=, *=, /=, ** */
            if ((c == '=' && next == '=') ||
                (c == '!' && next == '=') ||
                (c == '<' && next == '=') ||
                (c == '>' && next == '=') ||
                (c == '+' && next == '=') ||
                (c == '-' && next == '=') ||
                (c == '*' && next == '=') ||
                (c == '/' && next == '=') ||
                (c == '*' && next == '*')) {
                buf[0] = c;
                buf[1] = next;
                buf[2] = '\0';
                emit_token(T_OPERATOR, buf, line);
                i += 2;
                matched_op = 1;
            }

            if (matched_op)
                continue;
        }

        /* --- Operadores simples (um caractere) --- */
        if (strchr("=.+-*/%<>!", c)) {
            buf[0] = c;
            buf[1] = '\0';
            emit_token(T_OPERATOR, buf, line);
            i++;
            continue;
        }

        /* --- Delimitadores --- */
        if (strchr(":;,()[]{}",  c)) {
            buf[0] = c;
            buf[1] = '\0';
            emit_token(T_DELIMITER, buf, line);
            i++;
            continue;
        }

        /* --- Caractere não reconhecido --- */
        buf[0] = c;
        buf[1] = '\0';
        emit_token(T_UNKNOWN, buf, line);
        i++;
    }
}

/* ===========================================================================
 * main — Ponto de entrada do Analisador Léxico
 *
 * O código-fonte Python está hardcoded conforme requisito da avaliação.
 * =========================================================================== */
int main(void)
{
    /* -----------------------------------------------------------------
     * Código-fonte Python interceptado pela Colônia Espacial
     * (hardcoded conforme especificação da avaliação)
     * ----------------------------------------------------------------- */
    const char *source_code =
        "class Node:\n"
        "    def __init__(self, data):\n"
        "        self.data = data\n"
        "        self.next = None\n"
        "\n"
        "# Criando os nos\n"
        "head = Node(10)\n"
        "second = Node(20)\n"
        "third = Node(30)\n"
        "\n"
        "# Ligando os nos\n"
        "head.next = second\n"
        "second.next = third\n"
        "\n"
        "# Imprimindo a lista encadeada\n"
        "print(\"Lista Encadeada: \", end=\"\")\n"
        "temp = head\n"
        "while temp:\n"
        "    print(temp.data, end=\" \")\n"
        "    temp = temp.next\n"
        "print()\n";

    printf("\n");
    printf("  ************************************************************\n");
    printf("  *   ANALISADOR LEXICO — Colonia Espacial Ares-7            *\n");
    printf("  *   Linguagens, Automatos e Computabilidade                *\n");
    printf("  ************************************************************\n\n");

    /* Fase 1: Varredura e emissão do fluxo de tokens */
    scan(source_code);

    /* Fase 2: Impressão da Tabela de Símbolos */
    print_symbol_table();

    /* Limpeza de memória alocada dinamicamente */
    free_symbol_table();

    printf("\n");
    return 0;
}