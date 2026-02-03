// WarNivelAventureiro.c
// Projeto: Sistema de Batalha entre Territórios com Ponteiros e Alocação

// Task: Implementar a funcionalidade de ataque entre territórios. 
// Com base nos territórios já cadastrados, o sistema deverá permitir 
// que um jogador selecione um território como atacante, e outro como defensor. 
// O ataque será resolvido por meio de uma simulação com dados aleatórios (como rolagem de dados), 
// e o resultado alterará o controle e as tropas do território atacado. 

// Essa etapa deve utilizar ponteiros para manipular os dados 
// dos territórios e a alocação dinâmica de memória para armazenar os 
// territórios cadastrados, fornecendo maior flexibilidade ao sistema.

// Inclusão de Bibliotecas
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>

// Constantes Globais
#define MAX_TERRITORIOS 5
#define TAM_STRING 30

// Definição da Estrutura (Struct) Globalmente
typedef struct {
    char nome[TAM_STRING];
    char cor[10];
    int tropas;
} Territorio;

Territorio *territorios = NULL;

// ★ PROTÓTIPOS (no topo)
void limparBufferEntrada();
void cadastrarTerritorios();
void exibirTerritorios();
void faseAtaque();

// -- Função para limpar o buffer de entrada ---
void limparBufferEntrada() {
    int c;
    while ((c = getchar()) != '\n' && c != EOF);
}

// Laço para preencher os territórios com ponteiros.
void cadastrarTerritorios() {
    for (int i = 0; i < MAX_TERRITORIOS; i++) {
        printf("\n--- Cadastrando Território %d ---\n", i + 1);

        printf("Nome do Território: ");
        fgets(territorios[i].nome, TAM_STRING, stdin); // Guardar o nome do território
        territorios[i].nome[strcspn(territorios[i].nome, "\n")] = 0; // Remover o '\n'

        printf("Cor do Exército (ex: Azul, Verde): ");
        fgets(territorios[i].cor, 10, stdin); // Guardar a cor do exército
        territorios[i].cor[strcspn(territorios[i].cor, "\n")] = 0; // Remover o '\n'

        printf("Número de Tropas: ");
        scanf("%d", &territorios[i].tropas); // Guardar o número de tropas
        limparBufferEntrada(); // Limpar o buffer após scanf
    }
}

// Laço para exibir os territórios
void exibirTerritorios() {
    // Exibir o mapa do mundo
    printf("\n====================================\n");
    printf("   MAPA DO MUNDO - ESTADO ATUAL\n");
    printf("====================================\n");
    for (int i = 0; i < MAX_TERRITORIOS; i++) {
        printf("\n  %d. %s (Exército %s, Tropas: %d)", i + 1, territorios[i].nome, territorios[i].cor, territorios[i].tropas);
    }
    printf("\n");
}

// Laço para Fase de Ataque até digitação 0 para sair
void faseAtaque() {
    do{
        printf("\n--- FASE DE ATAQUE ---\n");
        printf("Escolha o território atacante (1 a %d, ou 0 para sair): ", MAX_TERRITORIOS);
        int atacante;
        scanf("%d", &atacante);
        limparBufferEntrada(); // Limpar o buffer após scanf
        if(atacante == 0){
            break;
        }
        atacante--;

        printf("Escolha o território defensor (1 a %d, ou 0 para sair): ", MAX_TERRITORIOS);
        int defensor;
        scanf("%d", &defensor);
        limparBufferEntrada(); // Limpar o buffer após scanf
        if(defensor == 0){
            break;
        }
        defensor--;

        // Verificar se os índices são válidos
        if (atacante < 0 || atacante >= MAX_TERRITORIOS || defensor < 0 || defensor >= MAX_TERRITORIOS) {
            printf("Índice inválido. Tente novamente.\n");
            continue;
        }

        // Verificar se o atacante tem tropas suficientes
        if (territorios[atacante].tropas < 2) {
            printf("O território atacante precisa ter pelo menos 2 tropas para atacar.\n");
            continue;
        }

        printf("\n--- RESULTADO DA BATALHA ---\n");
        int numero = rand() % 6 + 1;  // Número entre 1 e 6
        printf("O atacante %s rolou um dado e tirou: %d\n", territorios[atacante].nome, numero);
        int numero2 = rand() % 6 + 1;  // Número entre 1 e 6
        printf("O defensor %s rolou um dado e tirou: %d\n", territorios[defensor].nome, numero2);
        if (numero > numero2) {
            territorios[defensor].tropas -= 1;
            printf("VITÓRIA DO ATACANTE! O defensor perdeu 1 tropa.\n");
            if (territorios[defensor].tropas <= 0) {
                printf("CONQUISTA! O território %s foi dominado pelo Exército %s!\n", territorios[defensor].nome, territorios[atacante].cor);
                // Atualizar o território conquistado
                territorios[defensor].tropas = 1; // O conquistador deixa 1 tropa
                strcpy(territorios[defensor].cor, territorios[atacante].cor); // Muda a cor do exército
                territorios[atacante].tropas -= 1; // O atacante perde 1 tropa ao conquistar
            }
        } else {
            printf("DEFENSOR VENCEU! Atacante perde 1 tropa.\n");
            // Atacante perde 1 tropa
            territorios[atacante].tropas -= 1;
        }
        printf("\nPressione Enter para continuar para o próximo turno...\n");
        limparBufferEntrada(); // Limpar o buffer após getchar
        exibirTerritorios();
    } while (1);
}

// --- Função Principal ---
int main() {

    // Declaração de Ponteiro para Vetor de Territórios
    // ALOCAÇÃO DINÂMICA DE MEMÓRIA
    // Usamos malloc para o array de territórios. malloc(tamanho_total_em_bytes)
    territorios = malloc(MAX_TERRITORIOS * sizeof(Territorio));

    // VERIFICAÇÃO: É crucial verificar se a alocação de memória deu certo.
    if (territorios == NULL) {
        printf("Erro ao alocar memória para os territórios!\n");
        return 1;
    }

    // Cadastrar os territórios com ponteiros
    cadastrarTerritorios();

    // Exibir os territórios cadastrados com ponteiros
    exibirTerritorios();

    // Fase de ataque entre territórios
    srand(time(NULL));  // Inicializa o gerador de números aleatórios (faça isso uma vez)
    faseAtaque();
    
    // 3. LIBERAÇÃO DA MEMÓRIA ALOCADA
    // Antes de terminar, é ESSENCIAL liberar a memória que foi alocada dinamicamente.
    // Isso evita "memory leaks" (vazamentos de memória).
    free(territorios);
    printf("Memória liberada. Programa encerrado.\n");
    return 0;
}
