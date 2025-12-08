#include "ColisionAvoidanceNeuralNetworkThread.h"
#include "Config.h"
#include <iostream>

ColisionAvoidanceNeuralNetworkThread::ColisionAvoidanceNeuralNetworkThread(
    PioneerRobot *_robo, NeuralNetwork *_neuralNetwork)
{
    this->robo = _robo;
    this->neuralNetwork = _neuralNetwork;
}

void* ColisionAvoidanceNeuralNetworkThread::runThread(void*)
{
    while (this->getRunningWithLock())
    {
        myMutex.lock();

        robo->getAllSonar(sonar);
        tratamentoRna();

        ArUtil::sleep(300);

        myMutex.unlock();
    }

    ArLog::log(ArLog::Normal, "Colision Avoidance Thread Finalizada.");
    return NULL;
}

void ColisionAvoidanceNeuralNetworkThread::waitOnCondition() { myCondition.wait(); }
void ColisionAvoidanceNeuralNetworkThread::lockMutex() { myMutex.lock(); }
void ColisionAvoidanceNeuralNetworkThread::unlockMutex() { myMutex.unlock(); }


void ColisionAvoidanceNeuralNetworkThread::tratamentoRna()
{
    // Só envia nova ação quando o robô terminou a anterior
    if (!robo->robot.isHeadingDone() || !robo->robot.isMoveDone())
        return;

    ExpectedMovement movement = neuralNetwork->definirAcao(
        sonar[0], sonar[1], sonar[2], sonar[3],
        sonar[4], sonar[5], sonar[6], sonar[7]
    );

    movement.ProcessarMovimento();

    printf(
        "\nDirecaoRot: %d | Movimento: %d | Angulo: %d",
        movement.DirecaoRotacaoProcessada,
        movement.DirecaoMovimentoProcessada,
        movement.AnguloRotacaoProcessado
    );

    // ------------------------
    // FALHA NA DECISÃO
    // ------------------------
    if (movement.DirecaoRotacaoProcessada == -1 ||
        movement.AnguloRotacaoProcessado == -1)
    {
        robo->pararMovimento();
        return;
    }

    // ------------------------
    // DECISÃO: SEGUINDO EM FRENTE
    // ------------------------
    if (movement.DirecaoRotacaoProcessada == 0)
    {
        robo->Move(
            movement.DirecaoMovimentoProcessada,
            movement.DirecaoMovimentoProcessada
        );
        return;
    }

    // ------------------------
    // DECISÃO: ROTACIONANDO
    // ------------------------
    int sentido = movement.DirecaoRotacaoProcessada; // 1 esquerda | 2 direita
    robo->Rotaciona(
        movement.AnguloRotacaoProcessado,
        sentido,
        VELOCIDADEROTACAO
    );
}
