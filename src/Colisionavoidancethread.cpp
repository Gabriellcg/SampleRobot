#include "Colisionavoidancethread.h"
#include "Config.h"
#include <iostream>
#include "NeuralNetwork.h"

ColisionAvoidanceThread::ColisionAvoidanceThread(PioneerRobot *_robo)
{
      this->robo = _robo;
}

void *ColisionAvoidanceThread::runThread(void *)
{
      while (this->getRunningWithLock())
      {
            myMutex.lock();
            robo->getAllSonar(sonar);
            tratamentoSimples();
            // ArUtil::sleep(1000);
            myMutex.unlock();
      }

      ArLog::log(ArLog::Normal, "Colision Avoidance.");
      return NULL;
}

void ColisionAvoidanceThread::waitOnCondition() { myCondition.wait(); }

void ColisionAvoidanceThread::lockMutex() { myMutex.lock(); }

void ColisionAvoidanceThread::unlockMutex() { myMutex.unlock(); }


void ColisionAvoidanceThread::tratamentoSimples()
{
    // Variáveis para armazenar os 3 TARGETS (saídas ideais para a RN)
    float target_DR = -1.0; // Direção de Rotação
    float target_AR = -1.0; // Ângulo de Rotação
    float target_DM = -1.0; // Direção de Movimento

    int sumD = (sonar[3] * LIMIARFRENTE) + ((sonar[2] + sonar[1]) * LIMIARDIAGONAIS) + (sonar[0] * LIMIARLATERAIS);
    int sumE = (sonar[4] * LIMIARFRENTE) + ((sonar[5] + sonar[6]) * LIMIARDIAGONAIS) + (sonar[7] * LIMIARLATERAIS);
    int dirMov = 1; // 1 = Esquerda, 2 = Direita (Padrão Aria)

    if (robo->robot.isHeadingDone())
    {
        // Decide a melhor direção para desviar
        if (sumD > sumE)
            dirMov = 2; // Virar para a Direita é a melhor opção

        // Inicializa TARGETS para o caso de ROTA LIVRE (Padrão)
        target_DM = OUT_DM_FRENTE;        // 0.3
        target_DR = OUT_DR_FRENTE;        // 0.75
        target_AR = OUT_AR_SEM_ROTACAO;   // 0.1 (Para 0 graus)

        // --- LÓGICA HEURÍSTICA E MAPPING PARA TARGETS ---
        // (Aqui traduzimos a decisão do 'Professor' para o formato numérico da RN)

        if (sonar[3] <= LIMIARFRENTE / 5 || sonar[4] <= LIMIARFRENTE / 5)
        {
            // FRENTE PERTO - Ação: Recuar
            robo->Move(-VELOCIDADEDESLOCAMENTO, -VELOCIDADEDESLOCAMENTO);
            target_DM = OUT_DM_RE; // 0.7
            target_DR = OUT_DR_FRENTE; // Não rotacionar
        }
        else if (sonar[0] <= LIMIARLATERAIS)
        {
            // ESQUERDA PERTO - Ação: Rotacionar Suavemente p/ Direita
            robo->Rotaciona(5, 2, VELOCIDADEROTACAO); // 2 = Direita no Aria
            target_DR = OUT_DR_DIREITA; // 0.25
            target_AR = OUT_AR_LATERAL; // 5 graus (aprox. 0.3)
        }
        else if (sonar[1] <= LIMIARDIAGONAIS || sonar[2] <= LIMIARDIAGONAIS)
        {
            // ESQUERDA DIAGONAL - Ação: Rotacionar Médio p/ Direita
            robo->Rotaciona(15, 2, VELOCIDADEROTACAO);
            target_DR = OUT_DR_DIREITA; // 0.25
            target_AR = OUT_AR_DIAGONAL; // 15 graus (aprox. 0.5)
        }
        else if (sonar[3] <= LIMIARFRENTE || sonar[4] <= LIMIARFRENTE)
        {
            // FRENTE AFASTADO - Ação: Rotacionar Bruscamente
            robo->Rotaciona(45, dirMov, VELOCIDADEROTACAO);
            target_AR = OUT_AR_FRONTAL; // 45 graus (0.7)
            if (dirMov == 1) target_DR = OUT_DR_ESQUERDA; // 0.5 (Virar à Esquerda)
            else target_DR = OUT_DR_DIREITA; // 0.25 (Virar à Direita)
        }
        else if (sonar[5] <= LIMIARDIAGONAIS || sonar[6] <= LIMIARDIAGONAIS)
        {
            // DIREITA DIAGONAL - Ação: Rotacionar Médio p/ Esquerda
            robo->Rotaciona(15, 1, VELOCIDADEROTACAO); // 1 = Esquerda no Aria
            target_DR = OUT_DR_ESQUERDA; // 0.5
            target_AR = OUT_AR_DIAGONAL; // 15 graus (aprox. 0.5)
        }
        else if (sonar[7] <= LIMIARLATERAIS)
        {
            // DIREITA PERTO - Ação: Rotacionar Suavemente p/ Esquerda
            robo->Rotaciona(5, 1, VELOCIDADEROTACAO);
            target_DR = OUT_DR_ESQUERDA; // 0.5
            target_AR = OUT_AR_LATERAL; // 5 graus (aprox. 0.3)
        }
        else
        {
            // ROTA LIVRE - Ação: Seguir em Frente (Valores Padrão já definidos acima)
            robo->Move(VELOCIDADEDESLOCAMENTO, VELOCIDADEDESLOCAMENTO);
        }
        
        // --- IMPRESSÃO DOS DADOS DE TREINAMENTO (O RESULTADO FINAL!) ---

        printf("DATA: %d,%d,%d,%d,%d,%d,%d,%d | %.4f,%.4f,%.4f\n", 
            sonar[0], sonar[1], sonar[2], sonar[3], sonar[4], sonar[5], sonar[6], sonar[7],
            target_DR, target_AR, target_DM);
    }
}



/*
void ColisionAvoidanceThread::tratamentoSimples()
{
      int sumD = (sonar[3] * LIMIARFRENTE) + ((sonar[2] + sonar[1]) * LIMIARDIAGONAIS) + (sonar[0] * LIMIARLATERAIS); // 2
      int sumE = (sonar[4] * LIMIARFRENTE) + ((sonar[5] + sonar[6]) * LIMIARDIAGONAIS) + (sonar[7] * LIMIARLATERAIS); // 1
      int dirMov = 1;

      if (robo->robot.isHeadingDone())
      {
            std::cout << "A ultima rotacao foi concluida \n";
            if (sumD > sumE)
                  dirMov = 2;
            if (sonar[3] <= LIMIARFRENTE / 5 || sonar[4] <= LIMIARFRENTE / 5)
            {
                  robo->Move(-VELOCIDADEDESLOCAMENTO, -VELOCIDADEDESLOCAMENTO);
                  std::cout << "Frente perto \n";
            }
            else if (sonar[0] <= LIMIARLATERAIS)
            {
                  robo->Rotaciona(5, dirMov, VELOCIDADEROTACAO);
                  std::cout << "Esquerda perto \n";
            }
            else if (sonar[1] <= LIMIARDIAGONAIS || sonar[2] <= LIMIARDIAGONAIS)
            {
                  robo->Rotaciona(15, dirMov, VELOCIDADEROTACAO);
                  std::cout << "DDE perto \n";
            }
            else if (sonar[3] <= LIMIARFRENTE || sonar[4] <= LIMIARFRENTE)
            {
                  robo->Rotaciona(45, dirMov, VELOCIDADEROTACAO);
                  std::cout << "Frente afastado \n";
            }
            else if (sonar[5] <= LIMIARDIAGONAIS || sonar[6] <= LIMIARDIAGONAIS)
            {
                  robo->Rotaciona(15, dirMov, VELOCIDADEROTACAO);
                  std::cout << "DDD perto \n";
            }
            else if (sonar[7] <= LIMIARLATERAIS)
            {
                  robo->Rotaciona(5, dirMov, VELOCIDADEROTACAO);
                  std::cout << "Direita perto \n";
            }
            else
            {
                  robo->Move(VELOCIDADEDESLOCAMENTO, VELOCIDADEDESLOCAMENTO);
                  std::cout << "Seguir em frente \n";
            }
      }
      else
            std::cout << "Executando rotacao previa \n";
}
*/