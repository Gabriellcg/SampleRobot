#ifndef NEURALNETWORK_H
#define NEURALNETWORK_H
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>
#include "Sigmoid.h"
#include "ExpectedMovement.h"


#define PadroesValidacao 7
#define PadroesTreinamento 7
#define Sucesso 0.00009		    // 0.0004
#define NumeroCiclos 500000   // Exibir o progresso do treinamento a cada NumeroCiclos ciclos

//Sigmoide
#define TaxaAprendizado 0.2  //0.3 converge super rápido e com uma boa precisão (sigmoide na oculta).
#define Momentum 0.7            // Dificulta a convergencia da rede em minimos locais, fazendo com que convirja apenas quando realmente se tratar de um valor realmente significante.
#define MaximoPesoInicial 0.1


//Saidas da rede neural (Exemplo): Vocês precisam definir os intervalos entre 0 e 1 para cada uma das saídas de um mesmo neuronio.
//Alem disso, esses sao exemplos, voces podem ter mais tipos de saidas, por exemplo defni que o robo ira rotacionar para a direita, esquerda ou nao rotacionar, no geral isso nao teria como alterar, entao deixei como exemplo.

//Direcao de rotacao (Neuronio da camada de saida 1)
//   Direita              Reto            Esquerda
//0.125 - 0.375      0.375 - 0.625      0.625 - 0.875
//    0,25				  0,5                0,75
#define OUT_DR_DIREITA    0.25    // gira direita
#define OUT_DR_FRENTE     0.5     
#define OUT_DR_ESQUERDA   0.75    // gira esquerda
//#define OUT_DR_RE     0.70

//Para a direcao de movimento nao ha muita diferenca, entao acredito que voces possam adotar esses valores
//Direcao de movimento (Neuronio da camada de saida 2)

#define OUT_DM_FRENTE     0.5      
#define OUT_DM_RE         0.7

//O angulo nao possui receita de bolo, voces podem altera-lo em diferentes niveis, ou ate lidar com valores continuos
//Angulo de rotacao  (Neuronio da camada de saida 3)
#define OUT_AR_SEM_ROTACAO  0.1  // mantem direção
#define OUT_AR_LATERAL      0.2  // 
#define OUT_AR_DIAGONAL     0.5  // curva em canto amplo
#define  OUT_AR_FRONTAL     0.8  // evasão rapida ou mudança em 90 graus

//...

//Essa e uma sugestao, voces tambem podem trabalhar com a velocidade de movbvimento tambem sendo retornada pela rede neural, pois quanto mais proximo dos obstaculos, mais lento deveria ser o movimento
//Velocidade de movimento (Neuronio da camada de saida 4)

#define ALCANCE_MAX_SENSOR 6000

//Sobre o numero de neuronio das camadas, a camada de entrada ira refletir o numero de sensores, entao seriam esses 8. Se voces possuissem mais variaveis relevantes para essa operacao, poderiam utiliza-las. 
//Pensem que ate mesmo a velocidade de movimento atual do robo poderia ser utilizada como entrada para decidir no momento t+1
// Camada de entrada
#define NodosEntrada 8

//A quantidade de neuronios nessa camada esta fortemente vinculada a complexidade do problema, sendo uma boa pratica iniciar os esperimentos com pelo menos um neuronio a mais do que na camada de entrada.
// Camada oculta
#define NodosOcultos 9

//Essa camada ira definir a quantidade de diferentes variaveis de saida, nesse meu exemplo sao elas  direcao de rotacao (DR), direcao de movimento (DM) e angulo de rotacao (AR).
//Mas como eu disse no comentario acima, a rede poderia ter um quarto neuronio na camada de saida, para definir a velocidade de mopvimento do robo, ou ate outras saidas que voces condiderem importanes para a resolucao do problema.
// Camada de saída
#define NodosSaida 3


//Estrutura da rede neural, sintam-se livres para adicionar novas camadas intermediarias, alterar a funcao de ativacao, bias e etc.
class NeuralNetwork {
public:
    int i, j, p, q, r;
    int IntervaloTreinamentosPrintTela;
    int IndiceRandom[PadroesTreinamento];
    long CiclosDeTreinamento;
    float Rando;
    float Error;
    float AcumulaPeso;

    int esquerda = 0;
    int diagonal_esquerda_lateral = 0;
    int diagonal_esquerda_frontal = 0;
    int frente_esquerda = 0;
    int direita = 0;
    int diagonal_direita_lateral = 0;
    int diagonal_direita_frontal = 0;
    int frente_direita = 0;

    // Camada oculta
    float Oculto[NodosOcultos];
    float PesosCamadaOculta[NodosEntrada + 1][NodosOcultos];
    float OcultoDelta[NodosOcultos];
    float AlteracaoPesosOcultos[NodosEntrada + 1][NodosOcultos];
    ActivationFunction* activationFunctionCamadasOcultas;

    float Saida[NodosSaida];
    float SaidaDelta[NodosSaida];
    float PesosSaida[NodosOcultos + 1][NodosSaida];
    float AlterarPesosSaida[NodosOcultos + 1][NodosSaida];
    ActivationFunction* activationFunctionCamadaSaida;

    float ValoresSensores[1][NodosEntrada] = {{0, 0, 0, 0, 0, 0, 0, 0}};

    const float Input[PadroesTreinamento][NodosEntrada] = {
     // frente 
        {2000, 2000, 5000, 5000, 5000, 5000, 2000, 2000}, // 1 - tudo livre
        {1000, 1000, 1000, 4000, 4000, 1000, 1000, 1000}, // 2 - apenas frente livre        
               
        {1000, 1500, 2000, 4000, 5000, 5000, 5000, 5000}, // 3 esquerda com obstaculo 
        {5000, 5000, 5000, 5000, 4000, 2000, 1500, 1000}, // 4 direita com obstaculo 
        {5000, 5000, 5000, 2500, 1500, 1000, 1000, 5000}, // 5 
        //{3000, 2000, 2000, 1000, 1000, 2000, 2000, 3000},  // 6 - parede na frente
        
         // sonares laterais/diagonais
        {5000, 5000, 5000, 5000, 1000, 2000, 2000, 4000}, // 7 - obstaculo apenas de um lado (direita)
        {4000, 2000, 2000, 1000, 5000, 5000, 5000, 5000}, // 8 - - obstaculo apenas de um lado (esquerda)
        
    };
    float InputNormalizado[PadroesTreinamento][NodosEntrada];

    const float Objetivo[PadroesTreinamento][NodosSaida] = {
    
	    {OUT_DR_FRENTE, OUT_AR_SEM_ROTACAO, OUT_DM_FRENTE},   //1 - vai pra frente
	    {OUT_DR_FRENTE, OUT_AR_SEM_ROTACAO, OUT_DM_FRENTE},   //2 - vai frente
        {OUT_DR_DIREITA, OUT_AR_SEM_ROTACAO, OUT_DM_FRENTE},  //3 - vai frente
        {OUT_DR_ESQUERDA, OUT_AR_SEM_ROTACAO, OUT_DM_FRENTE}, //4 - vai frente
        {OUT_DR_DIREITA, OUT_AR_DIAGONAL, OUT_DM_FRENTE},     //5 - vira pra direita 
        //{OUT_DR_DIREITA, OUT_AR_FRONTAL, OUT_DM_RE},          //6 - da ré e vira  - RE MT FORTE e trava o giro
        {OUT_DR_ESQUERDA, OUT_AR_DIAGONAL, OUT_DM_FRENTE},     //7 - virar médio para esquerda 
        {OUT_DR_DIREITA, OUT_AR_DIAGONAL, OUT_DM_FRENTE}       //8 - virar médio para direita
    };  
    //Aqui eu utilizei os mesmos valores, mas o correto sera definir dados de validacao diferentes daqueles apresentados a rede em seu treinamento, para garantir que ela nao tenha apenas "decorado" as respostas.
    //Dados de validação
    const float InputValidacao[PadroesValidacao][NodosEntrada] = {
        // 1 - Tudo Livre (Variação do Padrão 1 de Treino)
        {5000, 5000, 5000, 5000, 5000, 5000, 5000, 5000}, 
        // 2 - Apenas Frente Livre (Variação do Padrão 2 de Treino)
        {1000, 1000, 1000, 5000, 5000, 1000, 1000, 1000}, 
        // 3 - Esquerda com Obstáculo (Variação do Padrão 3 de Treino)
        {1200, 1800, 2200, 4000, 5000, 5000, 5000, 5000}, 
        // 4 - Direita com Obstáculo (Variação do Padrão 4 de Treino)
        {5000, 5000, 5000, 4000, 2200, 1800, 1200, 1200}, 
        // 5 - Frente-Direita Obstáculo (Variação do Padrão 5 de Treino)
        {5000, 5000, 5000, 2700, 1700, 1100, 1100, 5000},  
        // 7 - Obstáculo Apenas Direita (Variação do Padrão 7 de Treino)
        {5000, 5000, 5000, 5000, 1500, 2500, 3500, 4500}, 
        // 8 - Obstáculo Apenas Esquerda (Variação do Padrão 8 de Treino)
        {4500, 3500, 2500, 1500, 5000, 5000, 5000, 5000}  
    };
float InputValidacaoNormalizado[PadroesValidacao][NodosEntrada];

    const float ObjetivoValidacao[PadroesValidacao][NodosSaida] = {
        // ESTES DEVEM CORRESPONDER EXATAMENTE AOS OBJETIVOS DO TREINAMENTO
        {OUT_DR_FRENTE, OUT_AR_SEM_ROTACAO, OUT_DM_FRENTE}, 
        {OUT_DR_FRENTE, OUT_AR_SEM_ROTACAO, OUT_DM_FRENTE},
        {OUT_DR_DIREITA, OUT_AR_SEM_ROTACAO, OUT_DM_FRENTE},
        {OUT_DR_ESQUERDA, OUT_AR_SEM_ROTACAO, OUT_DM_FRENTE},
        {OUT_DR_DIREITA, OUT_AR_DIAGONAL, OUT_DM_FRENTE},
        {OUT_DR_ESQUERDA, OUT_AR_DIAGONAL, OUT_DM_FRENTE},
        {OUT_DR_DIREITA, OUT_AR_DIAGONAL, OUT_DM_FRENTE}
    };
    
    //--

public:
    NeuralNetwork();
    void treinarRedeNeural();
    void inicializacaoPesos();
    int treinoInicialRede();
    void PrintarValores();
    ExpectedMovement testarValor();
    ExpectedMovement definirAcao(int sensor0, int sensor1, int sensor2, int sensor3, int sensor4, int sensor5, int sensor6, int sensor7);
    void validarRedeNeural();
    void treinarValidar();
    void normalizarEntradas();
    void setupCamadas() ;

    void salvarPesos(const char* nomeArquivo);
    void carregarPesos(const char* nomeArquivo);
};

#endif // NEURALNETWORK_H
