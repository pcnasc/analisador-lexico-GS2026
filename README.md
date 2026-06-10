# 🛸 Analisador Léxico — Colônia Espacial Ares-7

> **Disciplina:** Linguagens, Autômatos e Computabilidade  
> **Linguagem:** C (C99)  
> **Compilador:** GCC

---

## 📡 Cenário

Um script Python foi interceptado pela Colônia Espacial **Ares-7** durante uma varredura de sinais no perímetro do cinturão de asteroides. A equipe de decodificação conseguiu extrair o código-fonte, mas precisa de um **analisador léxico** para classificar cada cadeia de caracteres antes de prosseguir com a análise sintática.

Este projeto implementa esse analisador em **C**, realizando a varredura caractere por caractere do código interceptado e classificando os lexemas em seus respectivos tokens.

---

## 🧬 O Que o Analisador Faz

```
Código Python (input) → Varredura caractere a caractere → Lexemas + Tokens + Tabela de Símbolos
```

O analisador reconhece **7 categorias** de tokens:

| Token | Exemplos |
|---|---|
| `KEYWORD` | `class`, `def`, `while`, `None`, `self`, `print` |
| `IDENTIFIER` | `Node`, `head`, `__init__`, `data`, `temp` |
| `NUMBER` | `10`, `20`, `30` |
| `STRING` | `"Lista Encadeada: "`, `" "` |
| `OPERATOR` | `=`, `.`, `==`, `!=`, `<=` |
| `DELIMITER` | `:`, `,`, `(`, `)`, `[`, `]` |
| `COMMENT` | `# Criando os nós` |

---

## 🔧 Compilação e Execução

### Pré-requisitos
- GCC (qualquer versão com suporte a C99)

### Compilar
```bash
gcc -Wall -Wextra -pedantic -o analyzer analyzer.c
```

### Executar
```bash
./analyzer
```

### Rodar os Testes Automatizados
```bash
bash test_analyzer.sh
```

---

## 📂 Estrutura do Projeto

```
.
├── analyzer.c          # Código-fonte do analisador léxico (arquivo único)
├── test_analyzer.sh    # Suite de testes automatizados (49 asserts)
└── README.md
```

---

## 📊 Exemplo de Saída

### Fluxo de Tokens
```
  Linha   1 | [KEYWORD   ] Lexema: class
  Linha   1 | [IDENTIFIER] Lexema: Node
  Linha   1 | [DELIMITER ] Lexema: :
  Linha   2 | [KEYWORD   ] Lexema: def
  Linha   2 | [IDENTIFIER] Lexema: __init__
  ...
```

### Tabela de Símbolos
```
  | #    | Identificador                  | Linha      |
  |------|--------------------------------|------------|
  | 1    | Node                           | 1          |
  | 2    | __init__                       | 2          |
  | 3    | data                           | 2          |
  | 4    | next                           | 4          |
  | 5    | head                           | 7          |
  | 6    | second                         | 8          |
  | 7    | third                          | 9          |
  | 8    | temp                           | 17         |
```

> **Nota:** Apenas identificadores (variáveis, classes, funções, atributos) entram na tabela. Palavras reservadas (`class`, `def`, `self`, `None`, etc.) e literais (`10`, `"texto"`) são excluídos.

---

## 🏗️ Requisitos Técnicos Atendidos

- [x] Varredura caractere por caractere
- [x] Classificação em 7 categorias de tokens
- [x] Uso de `struct` para modelar a Tabela de Símbolos
- [x] Alocação dinâmica com `malloc` para inserção na tabela
- [x] Prevenção de duplicatas na Tabela de Símbolos
- [x] Keywords e literais **excluídos** da Tabela de Símbolos
- [x] Saída formatada: fluxo de tokens + tabela de símbolos
- [x] Liberação de memória com `free` ao final
- [x] Compilação limpa com `-Wall -Wextra -pedantic` (zero warnings)
- [x] Suite de testes automatizados (49/49 passing)

---

## 🧪 Testes

A suite de testes valida automaticamente:

| Categoria | Qtd de Asserts |
|---|---|
| Keywords corretas | 7 |
| Identificadores corretos | 8 |
| Números corretos | 3 |
| Strings corretas | 3 |
| Operadores corretos | 2 |
| Delimitadores corretos | 5 |
| Comentários corretos | 4 |
| Tabela de Símbolos — presença | 9 |
| Tabela de Símbolos — exclusões | 6 |
| Robustez (sem UNKNOWN) | 1 |
| **Total** | **49** |

---

## 📜 Licença

Projeto acadêmico — uso educacional.
