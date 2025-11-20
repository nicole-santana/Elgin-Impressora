#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <windows.h>

/* ======================= Config DLL ======================= */
static HMODULE g_hDll = NULL;

/* Convenção de chamada (Windows): __stdcall */
#ifndef CALLCONV
#  define CALLCONV WINAPI
#endif

/* ======================= Assinaturas da DLL ======================= */
typedef int (CALLCONV *AbreConexaoImpressora_t)(int, const char *, const char *, int);
typedef int (CALLCONV *FechaConexaoImpressora_t)(void);
typedef int (CALLCONV *ImpressaoTexto_t)(const char *, int, int, int);
typedef int (CALLCONV *Corte_t)(int);
typedef int (CALLCONV *ImpressaoQRCode_t)(const char *, int, int);
typedef int (CALLCONV *ImpressaoCodigoBarras_t)(int, const char *, int, int, int);
typedef int (CALLCONV *AvancaPapel_t)(int);
typedef int (CALLCONV *AbreGavetaElgin_t)(int, int, int);
typedef int (CALLCONV *AbreGaveta_t)(int, int, int);
typedef int (CALLCONV *SinalSonoro_t)(int, int, int);
typedef int (CALLCONV *ImprimeXMLSAT_t)(const char *, int);
typedef int (CALLCONV *ImprimeXMLCancelamentoSAT_t)(const char *, const char *, int);
typedef int (CALLCONV *InicializaImpressora_t)(void);

/* ======================= Ponteiros ======================= */
static AbreConexaoImpressora_t        AbreConexaoImpressora        = NULL;
static FechaConexaoImpressora_t       FechaConexaoImpressora       = NULL;
static ImpressaoTexto_t               ImpressaoTexto               = NULL;
static Corte_t                        Corte                        = NULL;
static ImpressaoQRCode_t              ImpressaoQRCode              = NULL;
static ImpressaoCodigoBarras_t        ImpressaoCodigoBarras        = NULL;
static AvancaPapel_t                  AvancaPapel                  = NULL;
static AbreGavetaElgin_t              AbreGavetaElgin              = NULL;
static AbreGaveta_t                   AbreGaveta                   = NULL;
static SinalSonoro_t                  SinalSonoro                  = NULL;
static ImprimeXMLSAT_t                ImprimeXMLSAT                = NULL;
static ImprimeXMLCancelamentoSAT_t    ImprimeXMLCancelamentoSAT    = NULL;
static InicializaImpressora_t         InicializaImpressora         = NULL;

/* ======================= Configuração ======================= */
static int   g_tipo      = 1;
static char  g_modelo[64] = "i9";
static char  g_conexao[128] = "USB";
static int   g_parametro = 0;
static int   g_conectada = 0;

/* ======================= Utilidades ======================= */
#define LOAD_FN(h, name)                                                         \
    do {                                                                         \
        name = (name##_t)GetProcAddress((HMODULE)(h), #name);                    \
        if (!(name)) {                                                           \
            fprintf(stderr, "Falha ao resolver símbolo %s (erro=%lu)\n",         \
                    #name, GetLastError());                                      \
            return 0;                                                            \
        }                                                                        \
    } while (0)

static void flush_entrada(void) {
    int c;
    while ((c = getchar()) != '\n' && c != EOF) { }
}

/* ======================= Funções para manipular a DLL ======================= */
static int carregarFuncoes(void)
{
    g_hDll = LoadLibraryA("E1_Impressora01.dll");
    if (!g_hDll) {
        fprintf(stderr, "Erro ao carregar E1_Impressora01.dll (erro=%lu)\n", GetLastError());
        return 0;
    }

    LOAD_FN(g_hDll, AbreConexaoImpressora);
    LOAD_FN(g_hDll, FechaConexaoImpressora);
    LOAD_FN(g_hDll, ImpressaoTexto);
    LOAD_FN(g_hDll, Corte);
    LOAD_FN(g_hDll, ImpressaoQRCode);
    LOAD_FN(g_hDll, ImpressaoCodigoBarras);
    LOAD_FN(g_hDll, AvancaPapel);
    LOAD_FN(g_hDll, AbreGavetaElgin);
    LOAD_FN(g_hDll, AbreGaveta);
    LOAD_FN(g_hDll, SinalSonoro);
    LOAD_FN(g_hDll, ImprimeXMLSAT);
    LOAD_FN(g_hDll, ImprimeXMLCancelamentoSAT);
    LOAD_FN(g_hDll, InicializaImpressora);

    return 1;
}

static void liberarBiblioteca(void)
{
    if (g_hDll) {
        FreeLibrary(g_hDll);
        g_hDll = NULL;
    }
}

/* ======================= Funções a serem implementadas pelos alunos ======================= */

static void exibirMenu(void)
{
    printf("\n=========== MENU ===========\n");
    printf("1 - Configurar Conexao\n");
    printf("2 - Abrir Conexao\n");
    printf("3 - Impressao Texto\n");
    printf("4 - Impressao QRCode\n");
    printf("5 - Impressao Cod Barras\n");
    printf("6 - Impressao XML SAT\n");
    printf("7 - Impressao XML Canc SAT\n");	
    printf("8 - Abrir Gaveta Elgin\n");
    printf("9 - Abrir Gaveta\n");
    printf("10 - Sinal Sonoro\n");
    printf("0 - Fechar Conexao e Sair\n");
    printf("----------------------------\n");
}

static void configurarConexao(void)
{
	printf("\n=== Configurar conexao ===\n");
	
	printf("Digite o tipo de entrada \n(1 - USB; 2 - RS232; 3 - TCP/IP; 4 - Bluetooth; 5 - Impressoras acopladas;)\n");
	scanf("%d", &g_tipo);
	
	if (g_tipo != 1) 
	{
		flush_entrada();
		printf("\nEntrada invalida para tipo. Usando o padrao: %d.\n", g_tipo);
	}

	flush_entrada(); //pra limpar o buffer

	printf("Digite o modelo \n(ex: i9; i7; i7 plus;)\n");
	scanf("%s", g_modelo);
		
	printf("Digite o tipo de conexao \n(ex: USB; COM2; IP; MAC;)\n");
	scanf("%s", g_conexao);
	
	printf("Digite o parametro \n(baudrate/portaTCP 0 - padrao)\n");
	scanf("%d", &g_parametro);
	
	flush_entrada();
	
	printf("\nConfiguracao salva: Tipo=%d, Modelo=%s, Conexao=%s, Parametro=%d.\n", g_tipo, g_modelo, g_conexao, g_parametro);
}

static void abrirConexao(void)
{
	if (g_conectada == 0)
	{
		int ret = AbreConexaoImpressora(g_tipo, g_modelo, g_conexao, g_parametro);
		
		if (ret == 0)
		{
			g_conectada = 1;
			printf("Conexao feita. Retorno: %d\n", ret);
		}
		else
		{
			printf("Falha na conexao. Valor ret: %d\n", ret);
		}
	
	}
	else
	{		
		printf("Ja existe uma conexao aberta.\n");
	}
}

static void imprimirTexto(void)
{
    // TODO: solicitar texto do usuário e chamar ImpressaoTexto
    // incluir AvancaPapel e Corte no final
    if(g_conectada == 1)
		{
    	
    	int ret = ImpressaoTexto("teste", 1,4,0);
    	AvancaPapel(5);
    	
    	if(ret == 0)
		{
    		printf("Impressao OK\n");
    		//AvancaPapel(2);
    		Corte(3);
		}
		else
		{
			printf("Erro. %d\n", ret);
		}
	}
	else
	{
		printf("Abra a conexao\n");
	}

}

static void imprimirQRCode(void)
{
    // TODO: solicitar conteúdo do QRCode e chamar ImpressaoQRCode(texto, 6, 4)
    // incluir AvancaPapel e Corte no final
    if (g_conexao == 1)
    {
    	int ret = ImpressaoQRCode("Teste de impressao", 6, 4);
    	
    	if(ret == 0)
    	{
    		printf("Impressao ok. Realizando avanço e corte.");
    		AvancaPapel(2);
    		Corte(2);
		}
		else
		{
			printf("Erro na ImpressaoQRCode. Retorno: %d\n", ret);
		}
	}
	else
	{
		printf("Abra a conexao primeiro.\n");
	}
}

static void imprimirCodigoBarras(void)
{
    // TODO: usar ImpressaoCodigoBarras(8, "{A012345678912", 100, 2, 3)
    // incluir AvancaPapel e Corte no final
    if (g_conectada == 1)
    {
    	int ret = ImpressaoCodigoBarras(8, "{A012345678912", 100, 2, 3);
    	
    	if(ret == 0)
    	{
    		printf("Impressao ok. Realizando avanço e corte.");
    		AvancaPapel(2);
    		Corte(2);
		}
		else
		{
			printf("Erro na ImpressaoQRCode. Retorno: %d\n", ret);
		}
	}
	else
	{
		printf("Abra a conexao primeiro.\n");
	}
}

static void imprimirXMLSAT(void)
{
    // TODO: ler o arquivo ./XMLSAT.xml e enviar via ImprimeXMLSAT
    // incluir AvancaPapel e Corte no final
    if (g_conectada == 1) 
	{
        char *xml = ("path=C:/Users/botelho_gabriel/Downloads/C Aluno/XMLSAT.xml");
        
        
        if (xml) 
		{
            
			printf("Arquivo XMLSAT.xml lido. Enviando para impressao\n");
            
            int ret = ImprimeXMLSAT(xml, 0);
            
            if (ret == 0) 
			{
                printf("Impressao XML SAT OK. Realizando avanco e corte\n");
                AvancaPapel(2); 
                Corte(2);
            } 
			else 
			{
                printf("Erro na ImprimeXMLSAT. Retorno: %d\n", ret);
            }
        } 
		else 
		{
            printf("Nao foi possivel carregar o XMLSAT.xml. Verifique o caminho e a existencia do arquivo.\n");
        }
        
    } 
	else 
	{
        printf("Abra a conexao primeiro.\n");
    }
}

static void imprimirXMLCancelamentoSAT(void)
{
    // TODO: ler o arquivo ./CANC_SAT.xml e chamar ImprimeXMLCancelamentoSAT
    // incluir AvancaPapel e Corte no final
    
	/*usar assinatura abaixo:
        "Q5DLkpdRijIRGY6YSSNsTWK1TztHL1vD0V1Jc4spo/CEUqICEb9SFy82ym8EhBRZ"
        "jbh3btsZhF+sjHqEMR159i4agru9x6KsepK/q0E2e5xlU5cv3m1woYfgHyOkWDNc"
        "SdMsS6bBh2Bpq6s89yJ9Q6qh/J8YHi306ce9Tqb/drKvN2XdE5noRSS32TAWuaQE"
        "Vd7u+TrvXlOQsE3fHR1D5f1saUwQLPSdIv01NF6Ny7jZwjCwv1uNDgGZONJdlTJ6"
        "p0ccqnZvuE70aHOI09elpjEO6Cd+orI7XHHrFCwhFhAcbalc+ZfO5b/+vkyAHS6C"
        "YVFCDtYR9Hi5qgdk31v23w==";
        */
        
        if(g_conectada == 1)
		{
			char *canc_xml = ("path=C:/Users/botelho_gabriel/Downloads/C Aluno/CANC_SAT.xml");
			const char *assQRCode = "Q5DLkpdRijIRGY6YSSNsTWK1TztHL1vD0V1Jc4spo/CEUqICEb9SFy82ym8EhBRZ"
        "jbh3btsZhF+sjHqEMR159i4agru9x6KsepK/q0E2e5xlU5cv3m1woYfgHyOkWDNc"
        "SdMsS6bBh2Bpq6s89yJ9Q6qh/J8YHi306ce9Tqb/drKvN2XdE5noRSS32TAWuaQE"
        "Vd7u+TrvXlOQsE3fHR1D5f1saUwQLPSdIv01NF6Ny7jZwjCwv1uNDgGZONJdlTJ6"
        "p0ccqnZvuE70aHOI09elpjEO6Cd+orI7XHHrFCwhFhAcbalc+ZfO5b/+vkyAHS6C"
        "YVFCDtYR9Hi5qgdk31v23w==";
        
        	if (canc_xml)
        	{
        		int ret = ImprimeXMLCancelamentoSAT(canc_xml, assQRCode, 0);
        	
        		if (ret == 0) 
				{
                	printf("Cancelamento ok. Realizando avanco e corte\n");
                	AvancaPapel(2); 
                	Corte(2);
            	} 
				else 
				{
                	printf("Erro na ImprimeXMLCancelamentoSAT. Retorno: %d\n", ret);
            	}
			}
        
		}
		
		else 
		{
        	printf("Abra a conexao primeiro.\n");
    	}
}

static void abrirGavetaElginOpc(void)
{
    // TODO: chamar AbreGavetaElgin(1, 50, 50)
    if (g_conectada == 1)
    {
    	int ret = AbreGavetaElgin(1, 50, 50);
    	
    	if(ret == 0)
    	{
    		printf("Abriu gaveta");
		}
		else
		{
			printf("Erro na Abertura. Retorno: %d\n", ret);
		}
	}
	else
	{
		printf("Abra a conexao primeiro.\n");
	}
    
}

static void abrirGavetaOpc(void)
{
    // TODO: chamar AbreGaveta(1, 5, 10)
    if (g_conectada == 1)
    {
    	int ret = AbreGaveta(1, 5, 10);
    	
    	if(ret == 0)
    	{
    		printf("Abriu gaveta");
		}
		else
		{
			printf("Erro na Abertura. Retorno: %d\n", ret);
		}
	}
	else
	{
		printf("Abra a conexao primeiro.\n");
	}
}

static void emitirSinalSonoro(void)
{
    // TODO: chamar SinalSonoro(4, 5, 5)
    if(g_conectada == 1 )
	{ 
		int ret = SinalSonoro(4, 5, 5);
		
		if(ret == 0)
		{
			printf("Sinal sonoro OK\n");
		}
		else
		{
			printf("Erro no SinalSonoro. Retorno: %d\n", ret);
		}
	}
	else
	{
		printf("Abra a conexao primeiro.\n");
	}
}

static void fecharConexao(void)
{
    // TODO: chamar FechaConexaoImpressora e tratar retorno
	int ret = -1;

	if (g_conectada == 1)
	{
		ret = FechaConexaoImpressora();
		if (ret == 0) 
		{
			g_conectada = 0;
			printf("Conexão Fechada. Retorno: %d\n", ret);
		} 
		else 
		{
			printf("Falha ao fechar conexao. Retorno: %d\n", ret);
		}
	}
	else
	{
		printf("Nenhuma conexao estava aberta.\n");
	}
}

/* ======================= Função principal ======================= */
int main(void)
{
    if (!carregarFuncoes()) {
        return 1;
    }

    int opc = 0;
    while (1) {
        
        //construir o menu e chamar as funçoes aqui!!!
        exibirMenu();
        printf("Digite sua opcao: \n");
        scanf("%d", &opc);
        
        switch (opc) 
		{
			case 0: 
			fecharConexao(); 
			liberarBiblioteca();
			printf("Saiu do programa.\n");
			return 0;
			
			case 1: configurarConexao();
			break;
			
			case 2: abrirConexao();
			break;
			
			case 3: imprimirTexto();
			break;
			
			case 4: imprimirQRCode();
			break;
			
			case 5: imprimirCodigoBarras();
			break;
			
			case 6: imprimirXMLSAT();
			break;
			
			case 7: imprimirXMLCancelamentoSAT();
			break;
			
			case 8: abrirGavetaElginOpc();
			break;
			
			case 9: abrirGavetaOpc();
			break;
			
			case 10: emitirSinalSonoro();
			break;
			
			default: printf("Opcao invalida.\n");
			break;
		}
                
        
    }
}

