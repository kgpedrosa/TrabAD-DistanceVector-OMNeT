#ifndef NO_SIMPLES_H
#define NO_SIMPLES_H

#include <omnetpp.h>
#include <map>
#include <vector>
#include "Estruturas.h"
#include "Logger.h"

using namespace omnetpp;

class NoRoteador : public cSimpleModule {
private:
    // Informações básicas do nó
    int meuID;                                    // Quem sou eu?
    std::map<int, InformacaoRota> tabelaRoteamento; // Como chegar em cada lugar?
    std::map<int, double> custoVizinhos;          // Quanto custa falar com cada vizinho?
    std::vector<int> listaVizinhos;               // Quem são meus vizinhos?
    

    
    // Timer para enviar informações periodicamente
    cMessage *temporizadorEnvio;

protected:
    virtual void initialize() override;           // Configuração inicial
    virtual void handleMessage(cMessage *msg) override;  // Processa mensagens
    virtual void finish() override;               // Limpeza final

    // MÉTRICAS PARA AVALIAÇÃO (conforme especificação)
    int totalMensagensEnviadas;                   // Quantas mensagens enviei
    int totalMensagensRecebidas;                  // Quantas mensagens recebi
    simtime_t tempoInicioConvergencia;            // Quando começou a convergir
    simtime_t tempoFimConvergencia;               // Quando terminou de convergir
    bool jaConvergiu;                             // Flag para controlar convergência
    int ciclosAposConvergencia;                   // Contador de ciclos após convergência

    // Funções auxiliares (bem simples!)
    void descobrirVizinhos();                     // Descobre quem são os vizinhos
    void enviarMinhaTabela();                     // Conta para os vizinhos como chegar em cada lugar
    void processarTabelaVizinho(cMessage *msg);   // Aprende com a tabela de um vizinho
    void verificarConvergencia();                 // Verifica se convergiu (para métricas)

    
    // Sistema de logging
    Logger* logger;                               // Logger para mensagens e métricas
};

#endif
