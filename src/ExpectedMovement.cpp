#include "ExpectedMovement.h"
#include "Config.h"

ExpectedMovement::ExpectedMovement(float _direcaoRotacao, float _direcaoMovimento, float _anguloRotacao)
{
    this->DirecaoRotacao = _direcaoRotacao;
    this->DirecaoMovimento = _direcaoMovimento;
    this->AnguloRotacao = _anguloRotacao;
}

void ExpectedMovement::ProcessarMovimento()
{
    // -------- DIREÇÃO DA ROTAÇÃO --------
    if (this->DirecaoRotacao >= 0.125 && this->DirecaoRotacao < 0.375)
        this->DirecaoRotacaoProcessada = 2;   // Direita

    else if (this->DirecaoRotacao >= 0.375 && this->DirecaoRotacao < 0.625)
        this->DirecaoRotacaoProcessada = 0;   // Reto (não rotaciona)

    else if (this->DirecaoRotacao >= 0.625 && this->DirecaoRotacao < 0.875)
        this->DirecaoRotacaoProcessada = 1;   // Esquerda

    else
        this->DirecaoRotacaoProcessada = -999;


    // -------- ANGULO DE ROTAÇÃO --------
    if (this->AnguloRotacao >= 0.1 && this->AnguloRotacao < 0.3)
        this->AnguloRotacaoProcessado = 0;   // sem rotação

    else if (this->AnguloRotacao >= 0.3 && this->AnguloRotacao < 0.5)
        this->AnguloRotacaoProcessado = 5;   // lateral

    else if (this->AnguloRotacao >= 0.5 && this->AnguloRotacao < 0.7)
        this->AnguloRotacaoProcessado = 15;  // diagonal

    else if (this->AnguloRotacao >= 0.7 && this->AnguloRotacao < 0.9)
        this->AnguloRotacaoProcessado = 45;  // frontal

    else
        this->AnguloRotacaoProcessado = -999;


    // -------- DIREÇÃO DO MOVIMENTO --------
    if (this->DirecaoMovimento >= 0.1 && this->DirecaoMovimento < 0.5)
        this->DirecaoMovimentoProcessada = VELOCIDADEDESLOCAMENTO; // frente

    else if (this->DirecaoMovimento >= 0.5 && this->DirecaoMovimento < 0.9)
        this->DirecaoMovimentoProcessada = -VELOCIDADEDESLOCAMENTO; // ré

    else
        this->DirecaoMovimentoProcessada = -999;
}

