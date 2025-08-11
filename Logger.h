#ifndef LOGGER_H
#define LOGGER_H

#include <omnetpp.h>
#include <string>
#include <map>
#include <vector>
#include "Estruturas.h"

using namespace omnetpp;

class Logger {
private:
    int nodeID;
    std::string nodeName;
    cSimpleModule* parentModule;  // Referência para o módulo pai
    
public:
    Logger(int id, cSimpleModule* parent);
    
    // ===== LOGS DE INICIALIZAÇÃO =====
    void logInicializacao(int numVizinhos);
    void logDescobertaVizinhos();
    void logVizinhoEncontrado(int idVizinho, double custo);
    void logConfiguracaoCompleta(int numVizinhos);
    
    // ===== LOGS DE COMUNICAÇÃO =====
    void logEnvioTabela();
    void logRecebimentoTabela(int vizinhoRemetente);
    void logMensagemDesconhecida();
    
    // ===== LOGS DE ROTEAMENTO =====
    void logNovoDestino(int destino, int viaVizinho, double custo);
    void logCaminhoMelhor(int destino, int viaVizinho, double custo);
    void logTabelaMudou();
    void logTabelaNaoMudou();
    
    // ===== LOGS DE TABELA DE ROTEAMENTO =====
    void logTabelaRoteamento(const std::map<int, InformacaoRota>& tabela, 
                           const std::vector<int>& listaVizinhos);
    void logCaminhoCompleto(int destino, double custo, const std::string& caminho);
    
    // ===== LOGS DE CONVERGÊNCIA =====
    void logConvergencia(double tempoTotal);
    void logConvergenciaDetectada(double tempoTotal);
    
    // ===== LOGS DE MÉTRICAS =====
    void logMetricasFinais(int mensagensEnviadas, int mensagensRecebidas, double tempoConvergencia);
    void logMetricasConvergencia(int mensagensEnviadas, int mensagensRecebidas, double tempoTotal);
    
    // ===== LOGS DE FINALIZAÇÃO =====
    void logFinalizacao();
    void logVerificacaoConsistencia();
    void logRotaFinal(int destino, double custo, const std::string& caminho);

    
    // ===== GRAVAÇÃO DE MÉTRICAS =====
    void gravarMetricas(int mensagensEnviadas, int mensagensRecebidas, double tempoConvergencia);
    void gravarMetricasFinais(int mensagensEnviadas, int mensagensRecebidas, double tempoConvergencia);
    
    // ===== UTILITÁRIOS =====
    std::string formatarTempo(double tempo);
    std::string formatarCusto(double custo);
};

#endif
