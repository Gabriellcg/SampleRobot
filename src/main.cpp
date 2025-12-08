#include "ClassRobo.h"
#include "Aria.h"
#include <iostream>
#include "Config.h"
#include "Colisionavoidancethread.h"
#include "Wallfollowerthread.h"
#include "Sonarthread.h"
#include "Laserthread.h"
#include "ColisionAvoidanceNeuralNetworkThread.h"

PioneerRobot *robo;
NeuralNetwork *neuralNetwork;

int main(int argc, char **argv)
{
    int sucesso;

    // Inicialização do robô e da rede neural
    robo = new PioneerRobot(ConexaoSimulacao, "", &sucesso);
    neuralNetwork = new NeuralNetwork();

    ArLog::log(ArLog::Normal, "Criando as threads...");

    // Threads disponíveis
    ColisionAvoidanceThread colisionAvoidanceThread(robo);  // ← NÃO SERÁ USADA
    ColisionAvoidanceNeuralNetworkThread colisionAvoidanceNeuralNetworkThread(robo, neuralNetwork); // ← USADA
    WallFollowerThread wallFollowerThread(robo);
    SonarThread sonarReadingThread(robo); 
    // LaserThread laserReadingThread(robo);

    // THREAD DE LEITURA DO SONAR
    ArLog::log(ArLog::Normal, "Sonar Readings thread ...");
    sonarReadingThread.runAsync();

    // ArLog::log(ArLog::Normal, "Laser Readings thread ...");
    // laserReadingThread.runAsync();

    // ---------------------------
    // DESATIVA qualquer controle NÃO neural
    // ---------------------------
    // ArLog::log(ArLog::Normal, "Colision Avoidance thread ...");
    // colisionAvoidanceThread.runAsync();

    // ArLog::log(ArLog::Normal, "Wall Following thread ...");
    // wallFollowerThread.runAsync();


    // ---------------------------
    // ATIVA CONTROLE VIA REDE NEURAL
    // ---------------------------
    ArLog::log(ArLog::Normal, "Colision Avoidance Neural Network thread ...");
    colisionAvoidanceNeuralNetworkThread.runAsync();
    // ---------------------------

    // Aguarda terminar
    robo->robot.waitForRunExit();

    Aria::exit(0);
}
