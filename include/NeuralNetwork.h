#ifndef NEURALNETWORK_H
#define NEURALNETWORK_H
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>
#include "Sigmoid.h"
#include "ExpectedMovement.h"


// Ajustei para mais padrões se quiser aumentar, mantenho 4 para compatibilidade com seu código original
#define PadroesValidacao 4
#define PadroesTreinamento 4

// Meta de erro
#define Sucesso 0.00007
#define NumeroCiclos 100000

// Hiperparâmetros ajustados para maior estabilidade
#define TaxaAprendizado 0.05   // menor para evitar oscilações
#define Momentum 0.5           // menos momentum até garantir convergência
#define MaximoPesoInicial 0.03 // pesos iniciais menores para evitar saturação

// Limite de clipping dos pesos para evitar explosão
#define LIMITE_PESO 5.0f

#define OUT_DR_DIREITA    0.25    
#define OUT_DR_ESQUERDA   0.5   
#define OUT_DR_FRENTE     0.75

#define OUT_DM_FRENTE     0.3      
#define OUT_DM_RE         0.7

#define OUT_AR_SEM_ROTACAO  0.1
#define OUT_AR_FRONTAL     0.2
#define OUT_AR_LATERAL      0.3
#define OUT_AR_DIAGONAL     0.5

#define ALCANCE_MAX_SENSOR 6000

#define NodosEntrada 8
#define NodosOcultos 9
#define NodosSaida 3


class NeuralNetwork {
public:
    int i, j, p, q, r;
    int IntervaloTreinamentosPrintTela;
    int IndiceRandom[PadroesTreinamento];
    long CiclosDeTreinamento;
    float Rando;
    float Error;
    float AcumulaPeso;

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
        {5000, 5000, 5000, 5000, 5000, 5000, 5000, 5000}, 
        {1000, 1000, 2000, 4000, 5000, 5000, 5000, 5000}, 
        {5000, 5000, 5000, 4000, 2000, 1000, 1000, 5000},
        {1000, 1000, 1000, 500, 500, 1000, 1000, 1000}
    };
    float InputNormalizado[PadroesTreinamento][NodosEntrada];

    const float Objetivo[PadroesTreinamento][NodosSaida] = {
        {OUT_DR_FRENTE, OUT_AR_SEM_ROTACAO, OUT_DM_FRENTE}, 
        {OUT_DR_DIREITA, OUT_AR_FRONTAL, OUT_DM_FRENTE},
        {OUT_DR_ESQUERDA, OUT_AR_FRONTAL, OUT_DM_FRENTE},
        {OUT_DR_FRENTE, OUT_AR_FRONTAL, OUT_DM_RE}
    };

    const float InputValidacao[PadroesValidacao][NodosEntrada] = {
        {5000, 5000, 5000, 5000, 5000, 5000, 5000, 5000}, 
        {1000, 1000, 2000, 4000, 5000, 5000, 5000, 5000}, 
        {5000, 5000, 5000, 4000, 2000, 1000, 1000, 5000},
        {1000, 1000, 1000, 500, 500, 1000, 1000, 1000}
    };
    float InputValidacaoNormalizado[PadroesValidacao][NodosEntrada];

    const float ObjetivoValidacao[PadroesValidacao][NodosSaida]{
        {OUT_DR_FRENTE, OUT_AR_SEM_ROTACAO, OUT_DM_FRENTE}, 
        {OUT_DR_DIREITA, OUT_AR_FRONTAL, OUT_DM_FRENTE},
        {OUT_DR_ESQUERDA, OUT_AR_FRONTAL, OUT_DM_FRENTE},
        {OUT_DR_FRENTE, OUT_AR_FRONTAL, OUT_DM_RE}            
    };

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
