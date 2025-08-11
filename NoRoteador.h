#ifndef NO_SIMPLES_H
#define NO_SIMPLES_H

#include <omnetpp.h>
#include <map>
#include <vector>

using namespace omnetpp;

// Esta estrutura guarda as informações de roteamento para cada destino
struct InformacaoRota {
    double custo;           // Quanto "custa" chegar ao destino (em segundos)
    int proximoVizinho;     // Para qual vizinho devo enviar o pacote
};

class NoRoteador : public cSimpleModule {
private:
    // Informações básicas do nó
    int meuID;                                    // Quem sou eu?
    std::map<int, InformacaoRota> tabelaRoteamento;     // Como chegar em cada lugar?
    std::map<int, double> custoVizinhos;          // Quanto custa falar com cada vizinho?
    std::vector<int> listaVizinhos;               // Quem são meus vizinhos?
    
    // Timer para enviar informações periodicamente
    cMessage *temporizadorEnvio;

protected:
    virtual void initialize() override;           // Configuração inicial
    virtual void handleMessage(cMessage *msg) override;  // Processa mensagens
    virtual void finish() override;               // Limpeza final

    // 📊 MÉTRICAS PARA AVALIAÇÃO (conforme especificação)
    int totalMensagensEnviadas;                   // Quantas mensagens enviei
    int totalMensagensRecebidas;                  // Quantas mensagens recebi
    simtime_t tempoInicioConvergencia;            // Quando começou a convergir
    simtime_t tempoFimConvergencia;               // Quando terminou de convergir
    bool jaConvergiu;                             // Flag para controlar convergência

    // Funções auxiliares (bem simples!)
    void descobrirVizinhos();                     // Descobre quem são os vizinhos
    void enviarMinhaTabela();                     // Conta para os vizinhos como chegar em cada lugar
    void processarTabelaVizinho(cMessage *msg);   // Aprende com a tabela de um vizinho
    void mostrarMinhaTabela();                    // Mostra minha tabela no console
    void verificarConvergencia();                 // 📊 Verifica se convergiu (para métricas)
    std::string obterCaminhoCompleto(int destino); // 🛤️ Reconstrói o caminho completo até o destino
};

#endif
