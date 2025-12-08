#ifndef NEURALNETWORK_H
#define NEURALNETWORK_H
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>
#include "Sigmoid.h"
#include "ExpectedMovement.h"


#define PadroesValidacao 18
#define PadroesTreinamento 18 
#define Sucesso 0.02		    // 0.0004
#define NumeroCiclos 500000     //100000 Exibir o progresso do treinamento a cada NumeroCiclos ciclos

//Sigmoide
#define TaxaAprendizado 0.2     //0.3 converge super rápido e com uma boa precisão (sigmoide na oculta).
#define Momentum 0.7            //0.9 Dificulta a convergencia da rede em minimos locais, fazendo com que convirja apenas quando realmente se tratar de um valor realmente significante.
#define MaximoPesoInicial 0.5


//Saidas da rede neural (Exemplo): Vocês precisam definir os intervalos entre 0 e 1 para cada uma das saídas de um mesmo neuronio.
//Alem disso, esses sao exemplos, voces podem ter mais tipos de saidas, por exemplo defni que o robo ira rotacionar para a direita, esquerda ou nao rotacionar, no geral isso nao teria como alterar, entao deixei como exemplo.

//Direcao de rotacao (Neuronio da camada de saida 1)
//   Direita              Reto            Esquerda
//0.125 - 0.375      0.375 - 0.625      0.625 - 0.875
//    0,25				  0,5                0,75
#define OUT_DR_DIREITA    0.25    
#define OUT_DR_ESQUERDA   0.5   
#define OUT_DR_FRENTE     0.75

//Para a direcao de movimento nao ha muita diferenca, entao acredito que voces possam adotar esses valores
//Direcao de movimento (Neuronio da camada de saida 2)
//	  Frente		    Re
//   0.1 - 0.5      0.5 - 0.9
#define OUT_DM_FRENTE     0.3     // Vai para frente
#define OUT_DM_PARADO     0.5     // Para
#define OUT_DM_RE         0.7     // Ré

//O angulo nao possui receita de bolo, voces podem altera-lo em diferentes niveis, ou ate lidar com valores continuos
//Angulo de rotacao  (Neuronio da camada de saida 3)
#define OUT_AR_SEM_ROTACAO  0.1   // Mantém direção
#define OUT_AR_LATERAL      0.2   // Curva lateral suave
#define OUT_AR_DIAGONAL     0.3   // Curva em canto amplo
#define OUT_AR_FRONTAL      0.5   // Evasão rápida

//Essa e uma sugestao, voces tambem podem trabalhar com a velocidade de movbvimento tambem sendo retornada pela rede neural, pois quanto mais proximo dos obstaculos, mais lento deveria ser o movimento
//Velocidade de movimento (Neuronio da camada de saida 4)

#define ALCANCE_MAX_SENSOR 5000

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

    // Camada de saída
    float Saida[NodosSaida];
    float SaidaDelta[NodosSaida];
    float PesosSaida[NodosOcultos + 1][NodosSaida];
    float AlterarPesosSaida[NodosOcultos + 1][NodosSaida];
    ActivationFunction* activationFunctionCamadaSaida;

    float ValoresSensores[1][NodosEntrada] = {{0, 0, 0, 0, 0, 0, 0, 0}};

    //Exemplo de dadod de treinamento, cada um representando a distancia lida por um sensor
    const float InputTreinamento[PadroesTreinamento][NodosEntrada] = {
    // 1- Tudo livre
    {5000, 5000, 5000, 5000, 5000, 5000, 5000, 5000}, 
    // 2- Apenas frente livre
    {1000, 1000, 4000, 4000, 4000, 1000, 1000, 1000}, 
    // 3- Esquerda com obstáculo
    {1000, 1500, 2000, 4000, 5000, 5000, 5000, 5000}, 
    // 4- Direita com obstáculo
    {5000, 5000, 5000, 4000, 2000, 1500, 1000, 5000},
    // 5- Frente ligeiramente obstruída
    {5000, 5000, 5000, 2500, 1500, 1000, 1000, 5000}, 
    // 6- Parede na frente (CRÍTICO: MUDANÇA PARA RÉ) deixar desativado por hora
    // {3000, 2000, 1000, 1000, 2000, 3000, 3000, 3000}, 
    // 7- Obstáculo apenas de um lado (direita)
    {5000, 5000, 5000, 1500, 2000, 3000, 4000, 5000},
    // 8- Obstáculo apenas de um lado (esquerda)
    {5000, 4000, 3000, 2000, 1500, 5000, 5000, 5000},
    // 9- Curva Ampla Direita
    {5000, 5000, 5000, 3000, 1500, 1000, 1000, 5000},
    // 10- Curva Ampla Esquerda
    {5000, 1000, 1000, 1500, 3000, 5000, 5000, 5000},
    // 11- Parede 90° Esquerda (Ré e vira)
    {5000, 5000, 5000, 5000, 5000, 5000, 2000, 1000},
    // 12- Objeto Pequeno Central
    {5000, 5000, 5000, 1500, 1500, 5000, 5000, 5000},
    // 13- Corredor Estreito (Alinhamento)
    {2000, 2000, 2000, 5000, 5000, 2000, 2000, 2000},
    // 14- Corredor Estreito (Movendo p/ Direita, corrigir p/ Esquerda)
    {1500, 1500, 1500, 5000, 5000, 2500, 2500, 2500},
    // 15- Beco Sem Saída (Parada)
    {500, 500, 500, 500, 500, 500, 500, 500},
    // 16- Desvio de Última Hora Esquerda
    {5000, 5000, 5000, 1000, 5000, 5000, 5000, 5000},
    // 17- Desvio de Última Hora Direita
    {5000, 5000, 5000, 5000, 1000, 5000, 5000, 5000},
    // 18- Canto Aberto (Livre)
    {6000, 6000, 6000, 6000, 6000, 6000, 6000, 6000}
};

const float ObjetivoTreinamento[PadroesTreinamento][NodosSaida] = {				
	// 1- Livre
    {OUT_DR_FRENTE, OUT_AR_SEM_ROTACAO, OUT_DM_FRENTE}, 
    // 2- Frente livre
    {OUT_DR_FRENTE, OUT_AR_SEM_ROTACAO, OUT_DM_FRENTE},
    // 3- Esquerda com obstáculo
    {OUT_DR_DIREITA, OUT_AR_FRONTAL, OUT_DM_FRENTE},
    // 4- Direita com obstáculo
    {OUT_DR_ESQUERDA, OUT_AR_FRONTAL, OUT_DM_FRENTE},
    // 5- Vira médio para a direita
    {OUT_DR_DIREITA, OUT_AR_DIAGONAL, OUT_DM_FRENTE},
    // 6- Ré e vira para a esquerda (Correcao) deixar desativado 
    // {OUT_DR_ESQUERDA, OUT_AR_FRONTAL, OUT_DM_RE}, 
    // 7- Vira médio para a esquerda
    {OUT_DR_ESQUERDA, OUT_AR_FRONTAL, OUT_DM_FRENTE},
    // 8- Vira médio para a direita
    {OUT_DR_DIREITA, OUT_AR_FRONTAL, OUT_DM_FRENTE},
    // 9- Curva Ampla Direita
    {OUT_DR_DIREITA, OUT_AR_DIAGONAL, OUT_DM_FRENTE},
    // 10- Curva Ampla Esquerda
    {OUT_DR_ESQUERDA, OUT_AR_DIAGONAL, OUT_DM_FRENTE},
    // 11- Ré e vira para a direita
    {OUT_DR_DIREITA, OUT_AR_FRONTAL, OUT_DM_RE}, 
    // 12- Desvio lateral suave para a direita
    {OUT_DR_DIREITA, OUT_AR_LATERAL, OUT_DM_FRENTE},
    // 13- Alinhamento
    {OUT_DR_FRENTE, OUT_AR_SEM_ROTACAO, OUT_DM_FRENTE},
    // 14- Centralizar (Gira p/ Esquerda)
    {OUT_DR_ESQUERDA, OUT_AR_LATERAL, OUT_DM_FRENTE},
    // 15- Parada de Emergência
    {OUT_DR_FRENTE, OUT_AR_FRONTAL, OUT_DM_PARADO}, 
    // 16- Desvio rápido Esquerda
    {OUT_DR_ESQUERDA, OUT_AR_FRONTAL, OUT_DM_FRENTE},
    // 17- Desvio rápido Direita
    {OUT_DR_DIREITA, OUT_AR_FRONTAL, OUT_DM_FRENTE},
    // 18- Livre
    {OUT_DR_FRENTE, OUT_AR_SEM_ROTACAO, OUT_DM_FRENTE}
};    
    //Aqui eu utilizei os mesmos valores, mas o correto sera definir dados de validacao diferentes daqueles apresentados a rede em seu treinamento, para garantir que ela nao tenha apenas "decorado" as respostas.
    //Dados de validação
    const float InputValidacao[PadroesValidacao][NodosEntrada] = {
        {5000,      5000,       5000,     900,     800,      5000,       5000,       5000}    
};
    float InputValidacaoNormalizado[PadroesValidacao][NodosEntrada];
    
    const float ObjetivoValidacao[PadroesValidacao][NodosSaida] = {
        {OUT_DR_ESQUERDA, OUT_AR_FRONTAL, OUT_DM_FRENTE}
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
};

#endif // NEURALNETWORK_H
