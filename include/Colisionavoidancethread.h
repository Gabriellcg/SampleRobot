#ifndef COLISIONAVOIDANCETHREAD_H
#define COLISIONAVOIDANCETHREAD_H

#include "Aria.h"
#include "ClassRobo.h"      // <-- ESSENCIAL: Contém a definição completa de PioneerRobot
#include "Config.h"
#include "NeuralNetwork.h"  // <-- ESSENCIAL: Para as constantes OUT_...

// 2. INCLUSÕES DESNECESSÁRIAS/PROBLEMÁTICAS (REMOVIDAS):
// #include "Colisionavoidancethread.h" <-- Causaria inclusão infinita
// #include <iostream>                  <-- Não necessário em arquivos .h
// class PioneerRobot                   <-- Sintaxe incorreta e desnecessária

class ColisionAvoidanceThread : public ArASyncTask
{
public:
    PioneerRobot *robo;
    ArCondition myCondition;
    ArMutex myMutex;
    int sonar[8];

public:
    ColisionAvoidanceThread(PioneerRobot *_robo);
    void *runThread(void *);
    void waitOnCondition();
    void lockMutex();
    void unlockMutex();
    void tratamentoSimples();
};

#endif // COLISIONAVOIDANCETHREAD_H