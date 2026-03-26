// WarNivelMestre.c
// Projeto: Sistema de Batalha entre Territórios com Modularização do Código

// 1 Modularizar completamento o código com funções especializadas, aplicando boas práticas.
// - Passagem por valor (para exibição) e por referência (para modificação).
// - A função main() se torna um "orquestrador", chamando as outras.
// 2 Criar um sistema de Missões para os exércitos.
// - Implementar uma função que sorteia uma missão no início do jogo e atribui ao exército.
// Refine suas funções para usar uma constante onde for apropriado 
// (Ex: tamanho de strings, número máximo de territórios, etc).

// Task: implementar a funcionalidade de missões estratégicas individuais para cada jogador, que deverá receber, 
// no início do jogo, uma missão sorteada de forma automática entre diversas descrições pré-definidas, 
// armazenadas em um vetor de strings. Essa missão será consultada durante o jogo para verificar se a 
// condição de vitória foi atingida. A nova camada de estratégia exige organização modular do 
// código, uso de ponteiros, passagem de parâmetros por valor e 
// referência e gerenciamento adequado da memória.

// Inclusão de Bibliotecas
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <stdbool.h>

// Constantes Globais
#define MAX_TERRITORIOS 5
#define TAM_STRING 30
#define TAM_MISS 100
#define COR_SIZE 10 // Tamanho para a string de cor do exército
#define MAX_MISSOES 5
#define INICIAL_TROPAS 3 // tropa inicial por território (ou por jogador).
#define MIN_TROPAS_PARA_ATACAR 2
#define CONQUER_LEAVE_TROOPS 1 // tropas deixadas ao conquistar (atualmente 1).

// Vetores Globais
const char *NOMES_TERRITORIOS[MAX_TERRITORIOS] = {
    "Américas", "Oceania", "África", "Europa", "Ásia"
};

const char *CORES_EXERCITOS[] = {
    "Azul", "Verde", "Vermelho", "Amarelo", "Preto"
};

// Struct Globais
typedef struct {
    int id;
    char nome[TAM_STRING];
    char cor[COR_SIZE];
    int tropas;
} Territorio;

typedef struct {
    char nome[TAM_STRING];
    char cor[COR_SIZE];
    int missaoID;
    
    // NOVO: Vetor dinâmico dos territórios conquistados
    int *territoriosConquistados;  // Armazena os IDs
    int numTerritariosConquistados;  // Quantidade
    int capacidade;  // Espaço alocado
} Jogador;

typedef struct {
    int id;
    char descricao[TAM_MISS]; // Tamanho das descrições das missões.
    bool completa; // Marca se a missão foi completada (pode ser útil para missões mais complexas)
    bool utilizada; // Marca se a missão já foi sorteada para algum jogador
} Missao;

typedef struct {
    int dacante;           // número do dado do atacante
    int defensor;          // número do dado do defensor
    int vencedor;          // armazena uma das 3 opções acima
    int tropas_perdidas;   // quantas tropas perdeu
} ResultadoBatalha;

typedef struct {
    Missao missoes[MAX_MISSOES];
    bool utilizada[MAX_MISSOES]; // Marca se a missão já foi sorteada
} SistemaMissoes;

typedef struct {
    Territorio *territorios;
    int num_territorios;
    
    Jogador *jogadores;
    int num_jogadores;
    
    Missao *missoes;
    int num_missoes;
    
    int turno_atual;
} Jogo;

// PROTÓTIPOS VERSÃO MESTRE + LÓGICA DE FILÍPE
// CONTROLE GERAL DO SISTEMA

void limparBufferEntrada(void); // Limpa o buffer de entrada (scanf / getchar)

// SETUP DO JOGO

int alocarMemoria(void); // Aloca memória para os territórios e jogador, retornando 1 para sucesso e 0 para falha
void inicializarTerritorios(void); // Inicializa os territórios com nomes, cores e tropas pré-definidas
void exibirMapa(const Territorio *territorios); // Exibe o mapa do mundo (apenas leitura — passagem por valor)

// JOGADOR HUMANO

void configurarJogador(Jogador *jogador); // Configura o jogador com nome, cor e missão padrão (será atualizado depois)
int menuPrincipal(void); // Exibe o menu principal de ações do jogador

// SISTEMA DE MISSÕES

void atribuirMissao(Jogador *jogador); // Atribui uma missão sorteada ao jogador
void exibirMissao(const Jogador *jogador); // Exibe a missão do jogador (somente se ele solicitar)

// SISTEMA DE BATALHA

int faseAtaque(Jogador *jogador); // Executa a fase de ataque entre territórios, retorna 1 se continua, 0 se encerra

// FLUXO PRINCIPAL DO JOGO

void executarJogo(void); // Controla o loop principal do jogo
int checarVitoria(const Jogador *jogador); // Verifica se o jogador venceu e encerra o jogo
void liberarMemoria(void); // Libera a memória alocada para os territórios e jogador, garantindo que os dados internos sejam liberados antes da struct

// UNÇÃO MAIN VERSÃO MESTRE + LÓGICA DE FILÍPE ======================================================= 
// FUNÇÃO PRINCIPAL

// 5. Variáveis globais (se existirem)
Territorio *territorios = NULL; // Uso de ponteiros para o vetor de territórios, alocado dinamicamente
Jogador *jogador = NULL;  // Uso de ponteiro para o jogador, alocado dinamicamente
int num_territorios = MAX_TERRITORIOS;  // O número de territórios é uma constante definida, e isso torna o código mais flexível e fácil de manter.


// Pré-definir as missões do jogo:
Missao missoes[MAX_MISSOES] = {
    {0, "Destruir o exército Verde", false, false},
    {1, "Conquistar 3 territórios", false, false},
    {2, "Destruir o exército Azul", false, false},
    {3, "Conquistar Europa", false, false},
    {4, "Eliminar o território Américas", false, false}
};


// FUNÇÃO PRINCIPAL com controle de erros na alocação de memória e limpeza adequada
int main(void) {
    int status = EXIT_FAILURE;

    if (!alocarMemoria()) {
        fprintf(stderr, "Erro crítico na alocação.\n");
        goto cleanup;
    }

    executarJogo();
    status = EXIT_SUCCESS;

cleanup:
    liberarMemoria();
    return status;
}

// Função para alocar memória dinamicamente para os territórios e jogador 
int alocarMemoria(void) {
    // Alocar territórios
    territorios = malloc(MAX_TERRITORIOS * sizeof(Territorio));
    if (territorios == NULL) {
        printf("Erro ao alocar memória para territórios!\n");
        return 0;  
    }
    printf("✓ %d Territórios alocados com sucesso.\n", MAX_TERRITORIOS);

    // Alocar jogador
    jogador = malloc(sizeof(Jogador));
    if (jogador == NULL) {
        printf("Erro ao alocar memória para o jogador!\n");
        free(territorios);  //free() → atua na memória
        territorios = NULL; //NULL → atua no ponteiro
        return 0;  // Retorna 0 para indicar falha na alocação do jogador
    }
    printf("✓ Jogador alocado com sucesso.\n");
    return 1; // Retorna 1 para indicar sucesso na alocação do  terrtorio e jogador
}

// Função de  Execução do Jogo
void executarJogo(void) { 
    printf("BEM-VINDO AO ÉPICO JOGO WAR!\n");

    // 1. Inicializar aleatoriedade
    srand(time(NULL));
        
    // 2. Inicializar territórios >>> ONDE ESTOU HOJE 06/02/2026 12H00
    inicializarTerritorios();

    // 3. Configurar jogador
    configurarJogador(jogador);  // Passa ponteiro para configurar o jogador (nome, cor, territórios iniciais)
        
    // 4. Sortear e atribuir missão
    atribuirMissao(jogador);
        
    // 5. Exibir mapa inicial
    exibirMapa(territorios);
        
    // 6. Loop principal do jogo
    int acao;
    int jogo_ativo = 1;
    do {
        acao = menuPrincipal();
        switch (acao) {
            case 1:
                if (!faseAtaque(jogador)) {
                    jogo_ativo = 0;
                    break;
                }
                break;
            case 2:
                exibirMissao(jogador);
                break;
            case 0:                    
                printf("Saindo do jogo...\n");
                 break;
            default:
                 printf("Ação inválida! Tente novamente.\n");
        }

        if (checarVitoria(jogador)) {
            jogo_ativo = 0;
        }
    } while (jogo_ativo && acao != 0);
        
    // 7. Liberar memória dos territórios e do jogador
    liberarMemoria();
    printf("Memória liberada. Jogo encerrado. Até a próxima!\n");
}

// FUNÇÕES DO JOGO

// -- Função para limpar o buffer de entrada ---
void limparBufferEntrada() {
    int c;
    while ((c = getchar()) != '\n' && c != EOF);
}

// Inicializa territórios com nomes, cores e tropas padrão
void inicializarTerritorios(void) {
  
    int ncores = sizeof(CORES_EXERCITOS) / sizeof(CORES_EXERCITOS[0]);

    for (int i = 0; i < MAX_TERRITORIOS; i++) {
        territorios[i].id = i;
        strncpy(territorios[i].nome, NOMES_TERRITORIOS[i], TAM_STRING - 1);
        territorios[i].nome[TAM_STRING - 1] = '\0';

        strncpy(territorios[i].cor, CORES_EXERCITOS[i % ncores], COR_SIZE - 1);
        territorios[i].cor[COR_SIZE - 1] = '\0';

        territorios[i].tropas = INICIAL_TROPAS;
    }
}

// Configura o jogador com nome, cor e missão padrão (será atualizado depois)
void configurarJogador(Jogador *jogador) {
    printf("\n--- CONFIGURAR JOGADOR ---\n");
    
    printf("Digite seu nome: ");
    fgets(jogador->nome, TAM_STRING, stdin);
    jogador->nome[strcspn(jogador->nome, "\n")] = 0;
    
    // NOVO: Menu de cores
    printf("\nEscolha a cor do seu exército:\n");
    int ncores = sizeof(CORES_EXERCITOS) / sizeof(CORES_EXERCITOS[0]);
    for (int i = 0; i < ncores; i++) {
        printf("%d. %s\n", i + 1, CORES_EXERCITOS[i]);
    }
    
    int opcao;
    do {
        printf("Escolha (1-%d): ", ncores);
        scanf("%d", &opcao);
        limparBufferEntrada();
        
        if (opcao < 1 || opcao > ncores) {
            printf("Opção inválida! Tente novamente.\n");
        }
    } while (opcao < 1 || opcao > ncores);
    
    strncpy(jogador->cor, CORES_EXERCITOS[opcao - 1], COR_SIZE - 1);
    jogador->cor[COR_SIZE - 1] = '\0';
    
    jogador->missaoID = -1;
    
    // Alocar vetor dinâmico
    jogador->territoriosConquistados = malloc(MAX_TERRITORIOS * sizeof(int));
    jogador->capacidade = MAX_TERRITORIOS;
    jogador->numTerritariosConquistados = 0;
    
    // Encontrar territórios iniciais
    for (int i = 0; i < MAX_TERRITORIOS; i++) {
        if (strcmp(territorios[i].cor, jogador->cor) == 0) {
            jogador->territoriosConquistados[jogador->numTerritariosConquistados] = i;
            jogador->numTerritariosConquistados++;
        }
    }
    
    printf("\nA cor do seu exército é %s e o seu território é %s.\n", 
           jogador->cor, territorios[jogador->territoriosConquistados[0]].nome);
    printf("Todos iniciam com 3 tropas.\n\n");
}

// Função para sortear uma missão disponível e marcar como utilizada

void atribuirMissao(Jogador *jogador) {
    int missionAvailable[MAX_MISSOES];
    int count = 0;
    int missaoID;
    bool compativel;
    
    // Resortear enquanto a missão não for compatível
    do {
        // Encontrar missões disponíveis
        count = 0;
        for (int i = 0; i < MAX_MISSOES; i++) {
            if (!missoes[i].utilizada) {
                missionAvailable[count] = i;
                count++;
            }
        }
        
        if (count == 0) {
            printf("Nenhuma missão disponível!\n");
            return;
        }
        
        // Sortear
        int indice = rand() % count;
        missaoID = missionAvailable[indice];
        
        // Verificar compatibilidade
        compativel = 1;  // true
        if (missaoID == 0 && strcmp(jogador->cor, "Verde") == 0) compativel = 0;
        if (missaoID == 2 && strcmp(jogador->cor, "Azul") == 0) compativel = 0;
        if (missaoID == 4 && strcmp(jogador->cor, "Azul") == 0) compativel = 0;  // Exemplo
        
    } while (!compativel);
    
    jogador->missaoID = missaoID;
    missoes[missaoID].utilizada = true;
    
    printf("-- SUA MISSÃO (%s) : ", jogador->cor);
    printf("%s\n", missoes[missaoID].descricao);
}

// Função para exibir o mapa do mundo (apenas leitura — passagem por valor)
void exibirMapa(const Territorio *territorios) {
    if (territorios == NULL) return;
    
    printf("   📍 MAPA DO MUNDO - ESTADO ATUAL\n");
    printf("====================================");
    
    for (int i = 0; i < MAX_TERRITORIOS; i++) {
        printf("\n%d. %s\n", i + 1, territorios[i].nome);
        printf("   Exército: %s\n", territorios[i].cor);
        printf("   Tropas: %d\n", territorios[i].tropas);
    }
    printf("\n");
}

// Função para exibir o menu principal de ações do jogador
int menuPrincipal(void) {
    printf("-- MENU DE AÇÕES --\n");
    printf("1. Atacar um território\n");
    printf("2. Verificar minha missão\n");
    printf("0. Sair do jogo\n");
    
    int acao;
    do {
        printf("Escolha sua ação (0-2): ");
        scanf("%d", &acao);
        limparBufferEntrada();
        
        if (acao < 0 || acao > 2) {
            printf("Opção inválida! Digite 0, 1 ou 2.\n");
        }
    } while (acao < 0 || acao > 2);
    
    return acao;
}

// Laço para Fase de Ataque até digitação 0 para sair
int faseAtaque(Jogador *jogador) {
    
    // ✅ Sempre usar o primeiro território
    int atacante = jogador->territoriosConquistados[0];
    
    do {
        printf("\n--- ATAQUE ---\n");
        if (territorios[atacante].tropas <= 1) {
            printf("Tropas insuficientes para atacar! Mínimo: 2\n");
            printf("Pressione enter para encerrar o jogo\n");
            limparBufferEntrada();
            return 0; // Encerra o jogo
        }
        printf("%s com %d tropas pronta para atacar!\n", 
               territorios[atacante].nome, territorios[atacante].tropas);
        
        // ✅ Escolher defensor
        printf("\nEscolha o território que será atacado (1-%d, ou 0 para sair): ", MAX_TERRITORIOS);
        int defensor;
        scanf("%d", &defensor);
        limparBufferEntrada();
        
        if (defensor == 0) break;
        defensor--;
        
        // ✅ Validações
        if (defensor < 0 || defensor >= MAX_TERRITORIOS) {
            printf("Índice inválido!\n");
            continue;
        }
        
        if (atacante == defensor) {
            printf("Não é possível atacar a si mesmo!\n");
            continue;
        }
        
        // ✅ BATALHA
        printf("Dados rolaram... de 1 a 6!\n");
        printf("\n--- RESULTADO DA BATALHA ---\n");
        int dado_atacante = rand() % 6 + 1;
        int dado_defensor = rand() % 6 + 1;
        
        printf("Ataque (%s): %d | Defesa (%s): %d\n", 
               territorios[atacante].nome, dado_atacante,
               territorios[defensor].nome, dado_defensor);
        
        if (dado_atacante > dado_defensor) {
            territorios[defensor].tropas--;
            printf("VITÓRIA! %s perdeu 1 tropa.\n", territorios[defensor].nome);
            
            // ✅ Se conquistou
            if (territorios[defensor].tropas <= 0) {
                printf("CONQUISTA! %s agora é seu!\n", territorios[defensor].nome);
                
                territorios[defensor].tropas = 1;
                strncpy(territorios[defensor].cor, jogador->cor, COR_SIZE - 1);
                territorios[defensor].cor[COR_SIZE - 1] = '\0';
                territorios[atacante].tropas--;
                
                if (jogador->numTerritariosConquistados < jogador->capacidade) {
                    jogador->territoriosConquistados[jogador->numTerritariosConquistados] = defensor;
                    jogador->numTerritariosConquistados++;
                }
            }
        } else {
            territorios[atacante].tropas--;
            printf("DERROTA! Você perdeu 1 tropa.\n");
        }
        
        printf("\nPressione Enter para continuar...\n");
        limparBufferEntrada();
        exibirMapa(territorios);
        
    } while (1);
    
    return 1; // Continua o jogo
}

// Função para exibir a missão do jogador (somente se ele solicitar)
void exibirMissao(const Jogador *jogador) {
    if (jogador == NULL) return;
    
    // Validar se tem missão
    if (jogador->missaoID < 0 || jogador->missaoID >= MAX_MISSOES) {
        printf("Erro: Nenhuma missão atribuída!\n");
        return;
    }
    
    printf("-- SUA MISSÃO (%s) -- ", jogador->cor);
    printf("%s\n", missoes[jogador->missaoID].descricao);
}

// Verifica se o jogador cumpriu sua missão e exibe relatório final se venceu
int checarVitoria(const Jogador *jogador) {
    int venceu = 0;
    
    // De acordo com a missão atribuída
    switch (jogador->missaoID) {
        
        // Missão 0: Destruir o exército Verde
        case 0: {
            int verdes_restantes = 0;
            for (int i = 0; i < MAX_TERRITORIOS; i++) {
                if (strcmp(territorios[i].cor, "Verde") == 0) {
                    verdes_restantes++;
                }
            }
            if (verdes_restantes == 0) {
                printf("MISSÃO CUMPRIDA! Todos os territórios Verdes foram eliminados!\n");
                venceu = 1;
            }
            break;
        }
        
        // Missão 1: Conquistar 3 territórios
        case 1: {
            if (jogador->numTerritariosConquistados >= 3) {
                printf("MISSÃO CUMPRIDA! Você conquistou 3 territórios!\n");
                venceu = 1;
            }
            break;
        }

        // Missão 2: Destruir o exército Azul
        case 2: {
            int azuis_restantes = 0;
            for (int i = 0; i < MAX_TERRITORIOS; i++) {
                if (strcmp(territorios[i].cor, "Azul") == 0) {
                    azuis_restantes++;
                }
            }
            if (azuis_restantes == 0) {
                printf("MISSÃO CUMPRIDA! Todos os territórios Azuis foram eliminados!\n");
                venceu = 1;
            }
            break;
        }
        
        // Missão 3: Conquistar Europa (id=3)
        case 3: {
            int conquistou_europa = 0;
            for (int i = 0; i < jogador->numTerritariosConquistados; i++) {
                if (jogador->territoriosConquistados[i] == 3) {  // Europa é id=3
                    conquistou_europa = 1;
                    break;
                }
            }
            if (conquistou_europa) {
                printf("MISSÃO CUMPRIDA! Você conquistou a Europa!\n");
                venceu = 1;
            }
            break;
        }
        
        // Missão 4: Eliminar o território Américas (id=0)
        case 4: {
            int conquistou_americas = 0;
            for (int i = 0; i < jogador->numTerritariosConquistados; i++) {
                if (jogador->territoriosConquistados[i] == 0) {  // Américas é id=0
                    conquistou_americas = 1;
                    break;
                }
            }
            if (conquistou_americas) {
                printf("MISSÃO CUMPRIDA! Você conquistou o território Américas!\n");
                venceu = 1;
            }
            break;
        }
    }
    
    // ✅ Se venceu, exibe relatório
    if (venceu) {
        printf("\n");
        printf("========================================\n");
        printf("🎉 PARABÉNS! VOCÊ VENCEU! 🎉\n");
        printf("========================================\n");
        printf("\n-- RELATÓRIO FINAL --\n");
        printf("Jogador: %s\n", jogador->nome);
        printf("Exército: %s\n", jogador->cor);
        printf("Territórios Conquistados: %d\n", jogador->numTerritariosConquistados);
        
        printf("\nTerritórios:\n");
        for (int i = 0; i < jogador->numTerritariosConquistados; i++) {
            int id = jogador->territoriosConquistados[i];
            printf("  • %s\n", territorios[id].nome);
        }
        
        printf("\nMissão: %s\n", missoes[jogador->missaoID].descricao);
        printf("Status: ✓ CUMPRIDA!\n");
        printf("========================================\n");
    }
    
    return venceu;
}

// Função para liberar a memória alocada para os territórios e jogador, garantindo que os dados internos sejam liberados antes da struct
void liberarMemoria(void) {
    if (jogador != NULL) {
        // ✅ PRIMEIRO: Liberar dados dentro da struct
        if (jogador->territoriosConquistados != NULL) {
            free(jogador->territoriosConquistados);
            jogador->territoriosConquistados = NULL;
            printf("✓ Vetor de territórios do jogador liberado.\n");
        }
        
        // ✅ DEPOIS: Liberar a struct
        free(jogador);
        jogador = NULL;
        printf("✓ Memória do jogador liberada.\n");
    }

    if (territorios != NULL) {
        free(territorios);
        territorios = NULL;
        printf("✓ Memória de territórios liberada.\n");
    }
}
