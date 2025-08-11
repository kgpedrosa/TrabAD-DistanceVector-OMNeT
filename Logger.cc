#include "Logger.h"
#include <algorithm>
#include <iomanip>
#include <sstream>

Logger::Logger(int id, cSimpleModule* parent) : nodeID(id), parentModule(parent) {
    nodeName = "Nó " + std::to_string(id);
}

// ===== LOGS DE INICIALIZAÇÃO =====

void Logger::logInicializacao(int numVizinhos) {
    EV << nodeName << " iniciando...\n";
}

void Logger::logDescobertaVizinhos() {
    // Log silencioso - não é necessário mostrar
}

void Logger::logVizinhoEncontrado(int idVizinho, double custo) {
    EV << "  - Vizinho " << idVizinho << " (custo: " << formatarCusto(custo) << "s)\n";
}

void Logger::logConfiguracaoCompleta(int numVizinhos) {
    EV << nodeName << " pronto com " << numVizinhos << " vizinhos\n\n";
}

// ===== LOGS DE COMUNICAÇÃO =====

void Logger::logEnvioTabela() {
    // Log silencioso - não é necessário mostrar
}

void Logger::logRecebimentoTabela(int vizinhoRemetente) {
    // Log silencioso - não é necessário mostrar
}

void Logger::logMensagemDesconhecida() {
    EV << nodeName << " recebeu mensagem desconhecida\n";
}

// ===== LOGS DE ROTEAMENTO =====

void Logger::logNovoDestino(int destino, int viaVizinho, double custo) {
    EV << nodeName << " aprendeu rota para " << destino << " via " << viaVizinho 
       << " (custo: " << formatarCusto(custo) << "s)\n";
}

void Logger::logCaminhoMelhor(int destino, int viaVizinho, double custo) {
    EV << nodeName << " encontrou caminho melhor para " << destino << " via " 
       << viaVizinho << " (custo: " << formatarCusto(custo) << "s)\n";
}

void Logger::logTabelaMudou() {
    EV << nodeName << " atualizou tabela de roteamento:\n";
}

void Logger::logTabelaNaoMudou() {
    // Log silencioso - apenas para controle interno
}

// ===== LOGS DE TABELA DE ROTEAMENTO =====

void Logger::logTabelaRoteamento(const std::map<int, InformacaoRota>& tabela, 
                                const std::vector<int>& listaVizinhos) {
    EV << " Destino | Custo | Próximo\n";
    EV << " --------|-------|--------\n";
    
    for (auto& entrada : tabela) {
        EV << "  " << entrada.first << "      | " 
           << formatarCusto(entrada.second.custo) << " | " 
           << entrada.second.proximoVizinho << "\n";
    }
    EV << "\n";
}

void Logger::logCaminhoCompleto(int destino, double custo, const std::string& caminho) {
    EV << "  Para " << destino << ": " << formatarCusto(custo) << "s via " << caminho << "\n";
}

// ===== LOGS DE CONVERGÊNCIA =====

void Logger::logConvergencia(double tempoTotal) {
    EV << nodeName << " convergiu em " << formatarTempo(tempoTotal) << "\n";
}

void Logger::logConvergenciaDetectada(double tempoTotal) {
    EV << nodeName << " convergiu em " << formatarTempo(tempoTotal) << "\n";
}

// ===== LOGS DE MÉTRICAS =====

void Logger::logMetricasFinais(int mensagensEnviadas, int mensagensRecebidas, double tempoConvergencia) {
    EV << "Métricas: " << mensagensEnviadas << " enviadas, " 
       << mensagensRecebidas << " recebidas, convergência em " << formatarTempo(tempoConvergencia) << "\n";
}

void Logger::logMetricasConvergencia(int mensagensEnviadas, int mensagensRecebidas, double tempoTotal) {
    EV << "Métricas: " << mensagensEnviadas << " enviadas, " 
       << mensagensRecebidas << " recebidas, tempo " << formatarTempo(tempoTotal) << "\n";
}

// ===== LOGS DE FINALIZAÇÃO =====

void Logger::logFinalizacao() {
    EV << "\n" << nodeName << " - Tabela final:\n";
}

void Logger::logVerificacaoConsistencia() {
    // Log silencioso - não é necessário mostrar
}

void Logger::logRotaFinal(int destino, double custo, const std::string& caminho) {
    // Log silencioso - não é necessário mostrar
}

// ===== GRAVAÇÃO DE MÉTRICAS =====

void Logger::gravarMetricas(int mensagensEnviadas, int mensagensRecebidas, double tempoConvergencia) {
    parentModule->recordScalar("mensagensEnviadas", mensagensEnviadas);
    parentModule->recordScalar("mensagensRecebidas", mensagensRecebidas);
    parentModule->recordScalar("tempoConvergencia", tempoConvergencia);
}

void Logger::gravarMetricasFinais(int mensagensEnviadas, int mensagensRecebidas, double tempoConvergencia) {
    parentModule->recordScalar("mensagensEnviadasFinal", mensagensEnviadas);
    parentModule->recordScalar("mensagensRecebidasFinal", mensagensRecebidas);
    parentModule->recordScalar("tempoConvergenciaFinal", tempoConvergencia);
}

// ===== UTILITÁRIOS =====

std::string Logger::formatarTempo(double tempo) {
    std::ostringstream oss;
    oss << std::fixed << std::setprecision(3) << tempo << "s";
    return oss.str();
}

std::string Logger::formatarCusto(double custo) {
    std::ostringstream oss;
    oss << std::fixed << std::setprecision(3) << custo;
    return oss.str();
}
