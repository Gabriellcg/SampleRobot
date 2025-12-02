#include "ColisionAvoidanceNeuralNetworkThread.h"
#include "Config.h"
#include <iostream>
#include <ctime>

ColisionAvoidanceNeuralNetworkThread::ColisionAvoidanceNeuralNetworkThread(PioneerRobot *_robo, NeuralNetwork *_neuralNetwork)
{
    this->robo = _robo;
    this->neuralNetwork = _neuralNetwork;
}

void* ColisionAvoidanceNeuralNetworkThread::runThread(void*)
{
    while(this->getRunningWithLock())
    {
        myMutex.lock();
        robo->getAllSonar(sonar);
        tratamentoRna();
        ArUtil::sleep(500); //Esse delay pode ser importante, principalmente dependendo da velocidade do computador em que o codigo estiver rodando, pois pode acabar empilhando muitas acoes para o robo realizar.
        myMutex.unlock();
    }

    ArLog::log(ArLog::Normal, "Colision Avoidance.");
    return NULL;
}

void ColisionAvoidanceNeuralNetworkThread::waitOnCondition() { myCondition.wait(); }

void ColisionAvoidanceNeuralNetworkThread::lockMutex() { myMutex.lock(); }

void ColisionAvoidanceNeuralNetworkThread::unlockMutex() { myMutex.unlock(); }

//Essa funcao pode ser alterada completamente, essa e apenas uma sugestao de como os dados de saida da rede poderiam ser convertidos em acoes do robo
// ColisionAvoidanceNeuralNetworkThread.cpp -> tratamentoRna()

void ColisionAvoidanceNeuralNetworkThread::tratamentoRna()
{
    if(robo->robot.isHeadingDone() && robo->robot.isMoveDone())
    {
        ExpectedMovement movement =  neuralNetwork->definirAcao(sonar[0], sonar[1], sonar[2], sonar[3], sonar[4], sonar[5], sonar[6], sonar[7]);
        
        // Processar os valores para que DirecaoRotacaoProcessada seja 1, 2 ou 0
        movement.ProcessarMovimento(); 

        printf("\nDirecaoRotacao %f DirecaoMovimentoProcessada %f AnguloRotacaoProcessado %f", 
               movement.DirecaoRotacao, movement.DirecaoMovimentoProcessada, movement.AnguloRotacaoProcessado);

        if(movement.DirecaoRotacaoProcessada == -999 || movement.AnguloRotacaoProcessado == -999 || movement.DirecaoMovimentoProcessada == -999)
        {
            robo->pararMovimento();
        }
        else
        {
            // Se DirecaoRotacaoProcessada é 0 (Frente), ele apenas move.
            if(movement.DirecaoRotacaoProcessada == 0) 
            {
                // Move: Usa a velocidade/direção linear
                robo->Move(movement.DirecaoMovimentoProcessada, movement.DirecaoMovimentoProcessada);
            }
            else
            {
                // Rotaciona: Usa a DIREÇÃO DE ROTAÇÃO (1 ou 2) no campo Sentido.
                // movement.DirecaoMovimentoProcessada é a VELOCIDADE DE DESLOCAMENTO (100 ou -100).
                robo->Rotaciona(movement.AnguloRotacaoProcessado, (int)movement.DirecaoRotacaoProcessada, VELOCIDADEROTACAO);
            }
        }
    }
}
