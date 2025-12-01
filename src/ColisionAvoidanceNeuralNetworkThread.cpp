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
    // Variável estática para rastrear o tempo desde o último comando. 
    // É estática para manter o valor entre chamadas da função.
    static time_t ultima_acao_tempo = time(NULL);
    const int LIMITE_STALL = 5; // 5 segundos

    if(robo->robot.isHeadingDone() && robo->robot.isMoveDone()) 
    {
        // VERIFICAÇÃO DO FAIL-SAFE:
        // Se o robô estiver parado (MoveDone e HeadingDone) por mais de 5 segundos
        if (std::difftime(time(NULL), ultima_acao_tempo) > LIMITE_STALL)
        {
            printf("\n*** FAIL-SAFE ATIVADO: ROBÔ PRESO POR > %ds. DANDO RÉ! ***\n", LIMITE_STALL);
            // Força o robô a dar ré por 1 segundo e reseta o timer
            robo->Move(-VELOCIDADEDESLOCAMENTO, -VELOCIDADEDESLOCAMENTO);
            ArUtil::sleep(1000); // Dá tempo para o robô sair do canto
            ultima_acao_tempo = time(NULL); // Reseta o timer
            return; // Sai da função para que o comando de ré seja executado
        }
        
        // Lógica da Rede Neural (executada somente se o robô não estiver em stall)
        ExpectedMovement movement =  neuralNetwork->definirAcao(sonar[0], sonar[1], sonar[2], sonar[3], sonar[4], sonar[5], sonar[6], sonar[7]);
        printf("\nDirecaoRotacaoProcessada %f DirecaoMovimento %f AnguloRotacao %f", movement.DirecaoRotacaoProcessada, movement.DirecaoMovimento, movement.AnguloRotacao);
        movement.ProcessarMovimento(); 
        printf("\nDirecaoRotacao %f DirecaoMovimentoProcessada %f AnguloRotacaoProcessado %f", movement.DirecaoRotacao, movement.DirecaoMovimentoProcessada, movement.AnguloRotacaoProcessado);

        if(movement.DirecaoRotacaoProcessada == 999 || movement.AnguloRotacaoProcessado == 999 || movement.DirecaoMovimentoProcessada == 999)
        {
            robo->pararMovimento();
        }
        else
        {
            // Lógica de execução RN (prioriza Move se Angulo=0, senão Rotaciona)
            if(movement.AnguloRotacaoProcessado == 0)
            {
                robo->Move(movement.DirecaoMovimentoProcessada, movement.DirecaoMovimentoProcessada);
            }
            else
            {
                 robo->Rotaciona(movement.AnguloRotacaoProcessado, movement.DirecaoMovimentoProcessada, VELOCIDADEROTACAO);
            }
        }
        
        // Se a RN emitiu um comando de movimento válido, resetamos o timer
        ultima_acao_tempo = time(NULL);
    }
    // Se o robô estiver em movimento, o timer não é checado nem resetado.
}

