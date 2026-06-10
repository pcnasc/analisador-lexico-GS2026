#!/bin/bash
# ============================================================================
#  Testes automatizados para o Analisador Léxico
#  Executa o analyzer e valida a saída contra padrões esperados.
# ============================================================================

PASS=0
FAIL=0
TOTAL=0

# Cores para output
GREEN="\033[0;32m"
RED="\033[0;31m"
YELLOW="\033[1;33m"
CYAN="\033[0;36m"
RESET="\033[0m"

# Compila o analyzer
echo -e "${CYAN}Compilando analyzer.c...${RESET}"
gcc -Wall -Wextra -pedantic -o analyzer analyzer.c 2>&1
if [ $? -ne 0 ]; then
    echo -e "${RED}ERRO: Falha na compilacao!${RESET}"
    exit 1
fi
echo -e "${GREEN}Compilacao OK (zero warnings com -Wall -Wextra -pedantic)${RESET}\n"

# Captura a saída
OUTPUT=$(./analyzer)

# --------------------------------------------------------------------------
# Função auxiliar para testar se um padrão existe na saída
# --------------------------------------------------------------------------
assert_contains() {
    TOTAL=$((TOTAL + 1))
    local description="$1"
    local pattern="$2"

    if echo "$OUTPUT" | grep -qF "$pattern"; then
        PASS=$((PASS + 1))
        echo -e "  ${GREEN}✓ PASS${RESET} — $description"
    else
        FAIL=$((FAIL + 1))
        echo -e "  ${RED}✗ FAIL${RESET} — $description"
        echo -e "         Esperado conter: ${YELLOW}$pattern${RESET}"
    fi
}

# --------------------------------------------------------------------------
# Função para testar se um padrão NÃO existe na saída
# --------------------------------------------------------------------------
assert_not_contains() {
    TOTAL=$((TOTAL + 1))
    local description="$1"
    local pattern="$2"

    if echo "$OUTPUT" | grep -qF "$pattern"; then
        FAIL=$((FAIL + 1))
        echo -e "  ${RED}✗ FAIL${RESET} — $description"
        echo -e "         Nao deveria conter: ${YELLOW}$pattern${RESET}"
    else
        PASS=$((PASS + 1))
        echo -e "  ${GREEN}✓ PASS${RESET} — $description"
    fi
}

# --------------------------------------------------------------------------
# Função para contar ocorrências
# --------------------------------------------------------------------------
assert_count() {
    TOTAL=$((TOTAL + 1))
    local description="$1"
    local pattern="$2"
    local expected="$3"

    local actual
    actual=$(echo "$OUTPUT" | grep -cF "$pattern")

    if [ "$actual" -eq "$expected" ]; then
        PASS=$((PASS + 1))
        echo -e "  ${GREEN}✓ PASS${RESET} — $description (count=$actual)"
    else
        FAIL=$((FAIL + 1))
        echo -e "  ${RED}✗ FAIL${RESET} — $description"
        echo -e "         Esperado: ${YELLOW}$expected${RESET} ocorrencias, encontrado: ${RED}$actual${RESET}"
    fi
}

echo "============================================================"
echo "  SUITE DE TESTES — Analisador Lexico"
echo "============================================================"
echo ""

# ==========================================================================
# TESTE 1: Keywords
# ==========================================================================
echo -e "${CYAN}[1] Keywords${RESET}"
assert_contains "class reconhecido como KEYWORD"       "[KEYWORD   ] Lexema: class"
assert_contains "def reconhecido como KEYWORD"         "[KEYWORD   ] Lexema: def"
assert_contains "None reconhecido como KEYWORD"        "[KEYWORD   ] Lexema: None"
assert_contains "while reconhecido como KEYWORD"       "[KEYWORD   ] Lexema: while"
assert_contains "self reconhecido como KEYWORD"        "[KEYWORD   ] Lexema: self"
assert_contains "print reconhecido como KEYWORD"       "[KEYWORD   ] Lexema: print"
assert_contains "end reconhecido como KEYWORD"         "[KEYWORD   ] Lexema: end"
echo ""

# ==========================================================================
# TESTE 2: Identificadores
# ==========================================================================
echo -e "${CYAN}[2] Identificadores${RESET}"
assert_contains "Node reconhecido como IDENTIFIER"     "[IDENTIFIER] Lexema: Node"
assert_contains "__init__ reconhecido como IDENTIFIER" "[IDENTIFIER] Lexema: __init__"
assert_contains "data reconhecido como IDENTIFIER"     "[IDENTIFIER] Lexema: data"
assert_contains "next reconhecido como IDENTIFIER"     "[IDENTIFIER] Lexema: next"
assert_contains "head reconhecido como IDENTIFIER"     "[IDENTIFIER] Lexema: head"
assert_contains "second reconhecido como IDENTIFIER"   "[IDENTIFIER] Lexema: second"
assert_contains "third reconhecido como IDENTIFIER"    "[IDENTIFIER] Lexema: third"
assert_contains "temp reconhecido como IDENTIFIER"     "[IDENTIFIER] Lexema: temp"
echo ""

# ==========================================================================
# TESTE 3: Numeros
# ==========================================================================
echo -e "${CYAN}[3] Numeros${RESET}"
assert_contains "10 reconhecido como NUMBER"           "[NUMBER    ] Lexema: 10"
assert_contains "20 reconhecido como NUMBER"           "[NUMBER    ] Lexema: 20"
assert_contains "30 reconhecido como NUMBER"           "[NUMBER    ] Lexema: 30"
echo ""

# ==========================================================================
# TESTE 4: Strings
# ==========================================================================
echo -e "${CYAN}[4] Strings${RESET}"
assert_contains "String com texto reconhecida"         '[STRING    ] Lexema: "Lista Encadeada: "'
assert_contains "String vazia reconhecida"             '[STRING    ] Lexema: ""'
assert_contains "String com espaco reconhecida"        '[STRING    ] Lexema: " "'
echo ""

# ==========================================================================
# TESTE 5: Operadores
# ==========================================================================
echo -e "${CYAN}[5] Operadores${RESET}"
assert_contains "= reconhecido como OPERATOR"          "[OPERATOR  ] Lexema: ="
assert_contains ". reconhecido como OPERATOR"          "[OPERATOR  ] Lexema: ."
echo ""

# ==========================================================================
# TESTE 6: Delimitadores
# ==========================================================================
echo -e "${CYAN}[6] Delimitadores${RESET}"
assert_contains ": reconhecido como DELIMITER"         "[DELIMITER ] Lexema: :"
assert_contains "( reconhecido como DELIMITER"         "[DELIMITER ] Lexema: ("
assert_contains ") reconhecido como DELIMITER"         "[DELIMITER ] Lexema: )"
assert_contains ", reconhecido como DELIMITER"         "[DELIMITER ] Lexema: ,"
assert_not_contains ": NAO e operador"                 "[OPERATOR  ] Lexema: :"
echo ""

# ==========================================================================
# TESTE 7: Comentarios
# ==========================================================================
echo -e "${CYAN}[7] Comentarios${RESET}"
assert_contains "Comentario 'Criando' detectado"       "[COMMENT   ] Lexema: # Criando os nos"
assert_contains "Comentario 'Ligando' detectado"       "[COMMENT   ] Lexema: # Ligando os nos"
assert_contains "Comentario 'Imprimindo' detectado"    "[COMMENT   ] Lexema: # Imprimindo a lista encadeada"
assert_count    "Exatamente 3 comentarios"             "[COMMENT   ]" 3
echo ""

# ==========================================================================
# TESTE 8: Tabela de Simbolos
# ==========================================================================
echo -e "${CYAN}[8] Tabela de Simbolos${RESET}"
assert_contains "Tabela de Simbolos impressa"          "TABELA DE SIMBOLOS"
assert_contains "Node na tabela de simbolos"           "| Node"
assert_contains "__init__ na tabela de simbolos"       "| __init__"
assert_contains "data na tabela de simbolos"           "| data"
assert_contains "next na tabela de simbolos"           "| next"
assert_contains "head na tabela de simbolos"           "| head"
assert_contains "second na tabela de simbolos"         "| second"
assert_contains "third na tabela de simbolos"          "| third"
assert_contains "temp na tabela de simbolos"           "| temp"
assert_contains "Total de 8 identificadores"           "Total de identificadores unicos: 8"
echo ""

# ==========================================================================
# TESTE 9: Exclusoes da Tabela de Simbolos
# ==========================================================================
echo -e "${CYAN}[9] Exclusoes da Tabela de Simbolos (keywords e literais fora)${RESET}"

# Extrai apenas a seção da tabela de símbolos
SYMBOL_SECTION=$(echo "$OUTPUT" | sed -n '/TABELA DE SIMBOLOS/,$ p')

assert_keyword_not_in_table() {
    TOTAL=$((TOTAL + 1))
    local kw="$1"
    # Procura por " | kw " na seção da tabela
    if echo "$SYMBOL_SECTION" | grep -qw "$kw"; then
        # Precisa verificar se está como valor na coluna Identificador
        if echo "$SYMBOL_SECTION" | grep -qP "\|\s+$kw\s+\|"; then
            FAIL=$((FAIL + 1))
            echo -e "  ${RED}✗ FAIL${RESET} — '$kw' NAO deveria estar na Tabela de Simbolos"
        else
            PASS=$((PASS + 1))
            echo -e "  ${GREEN}✓ PASS${RESET} — '$kw' corretamente ausente da Tabela de Simbolos"
        fi
    else
        PASS=$((PASS + 1))
        echo -e "  ${GREEN}✓ PASS${RESET} — '$kw' corretamente ausente da Tabela de Simbolos"
    fi
}

assert_keyword_not_in_table "class"
assert_keyword_not_in_table "def"
assert_keyword_not_in_table "None"
assert_keyword_not_in_table "while"
assert_keyword_not_in_table "self"
assert_keyword_not_in_table "print"
echo ""

# ==========================================================================
# TESTE 10: Contagem de tokens por tipo
# ==========================================================================
echo -e "${CYAN}[10] Contagem de tokens${RESET}"
KW_COUNT=$(echo "$OUTPUT" | grep -c "\[KEYWORD")
ID_COUNT=$(echo "$OUTPUT" | grep -c "\[IDENTIFIER")
NUM_COUNT=$(echo "$OUTPUT" | grep -c "\[NUMBER")
STR_COUNT=$(echo "$OUTPUT" | grep -c "\[STRING")
OP_COUNT=$(echo "$OUTPUT" | grep -c "\[OPERATOR")
DL_COUNT=$(echo "$OUTPUT" | grep -c "\[DELIMITER")
CM_COUNT=$(echo "$OUTPUT" | grep -c "\[COMMENT")

echo -e "  Keywords:      $KW_COUNT"
echo -e "  Identifiers:   $ID_COUNT"
echo -e "  Numbers:       $NUM_COUNT"
echo -e "  Strings:       $STR_COUNT"
echo -e "  Operators:     $OP_COUNT"
echo -e "  Delimiters:    $DL_COUNT"
echo -e "  Comments:      $CM_COUNT"
TOTAL_TOKENS=$((KW_COUNT + ID_COUNT + NUM_COUNT + STR_COUNT + OP_COUNT + DL_COUNT + CM_COUNT))
echo -e "  ${CYAN}Total tokens:  $TOTAL_TOKENS${RESET}"
echo ""

# ==========================================================================
# TESTE 11: Sem tokens UNKNOWN
# ==========================================================================
echo -e "${CYAN}[11] Robustez${RESET}"
assert_not_contains "Nenhum token UNKNOWN gerado" "[UNKNOWN"
echo ""

# ==========================================================================
# RESULTADO FINAL
# ==========================================================================
echo "============================================================"
if [ $FAIL -eq 0 ]; then
    echo -e "  ${GREEN}RESULTADO: TODOS OS TESTES PASSARAM! ($PASS/$TOTAL)${RESET}"
else
    echo -e "  ${RED}RESULTADO: $FAIL FALHA(S) em $TOTAL testes${RESET}"
    echo -e "  ${GREEN}Passaram: $PASS${RESET} | ${RED}Falharam: $FAIL${RESET}"
fi
echo "============================================================"
echo ""

# Cleanup
rm -f analyzer

exit $FAIL
