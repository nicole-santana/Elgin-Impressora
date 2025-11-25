# Elgin-Impressora

Projeto em C para controlar impressoras **Elgin** usando a DLL **E1_Impressora01.dll**.
A aplicação é um menu interativo que permite testar todas as funções principais da impressora: conexão, texto, QR Code, código de barras, XML SAT, gaveta e sinais sonoros.

---

## **📦 Requisitos**

* Windows
* DLL **E1_Impressora01.dll** no mesmo diretório do executável
* Impressora Elgin conectada (USB / Serial / TCP / Bluetooth)

---

## **🔧 Como funciona**

O programa carrega dinamicamente as funções da DLL usando `LoadLibrary` e `GetProcAddress`.
Depois disso, expõe um menu com chamadas diretas para cada função impressora.

---

# **📜 Menu e comandos**

### **1 — Configurar Conexão**

Define os parâmetros usados para abrir a impressora:

* Tipo:

  * `1 = USB`
  * `2 = Serial`
  * `3 = TCP/IP`
  * `4 = Bluetooth`
  * `5 = Acoplada`
* Modelo (ex: `i9`, `i7`)
* Conexão (ex: `USB`, `COM2`, `192.168.0.2`, MAC)
* Parâmetro adicional (baudrate ou porta TCP)

**Atualiza:** `g_tipo`, `g_modelo`, `g_conexao`, `g_parametro`.

---

### **2 — Abrir Conexão**

Chama:

```c
AbreConexaoImpressora(g_tipo, g_modelo, g_conexao, g_parametro);
```

Se retorno = 0 → conexão ativa.
Se != 0 → erro.

---

### **3 — Impressão de Texto**

Imprime o texto `"teste"` usando:

```c
ImpressaoTexto("teste", 1, 4, 0);
AvancaPapel(5);
Corte(3);
```

Fluxo:

1. Envia o texto fixo
2. Avança papel
3. Corta

---

### **4 — Impressão de QRCode**

(Obs: seu código está com bug: compara `g_conexao == 1`, mas deveria ser `g_conectada`.)

Chama:

```c
ImpressaoQRCode("Teste de impressao", 6, 4);
AvancaPapel(2);
Corte(2);
```

---

### **5 — Código de Barras**

Gera um Code128 usando o padrão `{A`:

```c
ImpressaoCodigoBarras(8, "{A012345678912", 100, 2, 3);
AvancaPapel(2);
Corte(2);
```

---

### **6 — Imprimir XML SAT**

Carrega:

`C:/Users/.../XMLSAT.xml`

E chama:

```c
ImprimeXMLSAT(xmlPath, 0);
AvancaPapel(2);
Corte(2);
```

Usado para testar DANFE SAT.

---

### **7 — Imprimir XML Cancelamento SAT**

Usa:

```c
ImprimeXMLCancelamentoSAT(caminhoCANCXML, assinaturaQRCode, 0);
```

Depois:

* Avança papel
* Corta

---

### **8 — Abrir Gaveta Elgin**

Chamando:

```c
AbreGavetaElgin(1, 50, 50);
```

Pulso para gaveta acoplada à Elgin.

---

### **9 — Abrir Gaveta Genérica**

Chamando:

```c
AbreGaveta(1, 5, 10);
```

Funciona com gavetas que não são Elgin, ligadas via pulso.

---

### **10 — Sinal Sonoro**

Chama:

```c
SinalSonoro(4, 5, 5);
```

Emite beeps na impressora.

---

### **0 — Fechar Conexão e Sair**

Executa:

```c
FechaConexaoImpressora();
FreeLibrary(g_hDll);
```

Fecha tudo e encerra o programa.

---

# **📁 Estrutura dos Arquivos (sugestão)**

```
/src
  main.c
  E1_Impressora01.dll
  XMLSAT.xml
  CANC_SAT.xml
README.md
```

---

# **⚠️ Observações Importantes**

* O QR Code só funciona se a conexão estiver aberta (corrigir `if (g_conexao == 1)` → `if (g_conectada == 1)`).
* Os caminhos de XML estão hardcoded (não é ideal).
* O menu é síncrono: a impressora sempre bloqueia até concluir.
