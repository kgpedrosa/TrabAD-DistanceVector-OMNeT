#ifndef NOROTEADOR_H
#define NOROTEADOR_H

#include <omnetpp.h>
#include <map>
#include <vector>
#include <string>

using namespace omnetpp;

// Estrutura para armazenar informações de uma rota
struct InformacaoRota {
    double custo;           // Custo total até o destino
    int proximoVizinho;     // Próximo nó no caminho
};

class NoRoteador : public cSimpleModule {
protected:
    // Identificação do nó
    int meuID;
    
    // Informações dos vizinhos
    std::vector<int> listaVizinhos;
    std::map<int, double> custoVizinhos;
    
    // Tabela de roteamento: destino -> informações da rota
    std::map<int, InformacaoRota> tabelaRoteamento;
    
    // Timer para envio periódico
    cMessage *temporizadorEnvio;
    
    // Métricas para avaliação
    int totalMensagensEnviadas;
    int totalMensagensRecebidas;
    simtime_t tempoInicioConvergencia;
    simtime_t tempoFimConvergencia;
    bool jaConvergiu;
    int ciclosAposConvergencia;
    
    // Variáveis para detecção de convergência baseada em estabilidade
    std::map<int, InformacaoRota> tabelaAnterior;
    int ciclosEstaveis;
    
    // Variáveis estáticas para métricas globais da rede
    static int totalNosConvergiu;
    static int totalMensagensEnviadasRede;
    static int totalMensagensRecebidasRede;
    static double tempoInicioRede;
    static double tempoFimConvergenciaRede;
    static bool redeConvergiu;
    static int totalNosRede;
    static bool resumoGlobalMostrado;

protected:
    // Funções virtuais do Omnet++
    virtual void initialize() override;
    virtual void handleMessage(cMessage *msg) override;
    virtual void finish() override;

private:
    // Funções auxiliares
    void descobrirVizinhos();
    void enviarMinhaTabela();
    void processarTabelaVizinho(cMessage *msg);
    void verificarConvergencia();
    void mostrarTabelaRoteamento();
    void mostrarResumoGlobal();
};

#endif
