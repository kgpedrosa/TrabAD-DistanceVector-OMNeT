#include "NoRoteador.h"
#include "Mensagens_m.h"

#include <algorithm>
#include <iomanip>
#include <sstream>

Define_Module(NoRoteador);

// Inicialização das variáveis estáticas para métricas globais
int NoRoteador::totalNosConvergiu = 0;
int NoRoteador::totalMensagensEnviadasRede = 0;
int NoRoteador::totalMensagensRecebidasRede = 0;
double NoRoteador::tempoInicioRede = 0.0;
double NoRoteador::tempoFimConvergenciaRede = 0.0;
bool NoRoteador::redeConvergiu = false;
int NoRoteador::totalNosRede = 0;
bool NoRoteador::resumoGlobalMostrado = false;

void NoRoteador::initialize() {
    // Passo 1: Descobrir quem sou eu
    meuID = par("meuNumero");
    
    // Inicializar métricas
    totalMensagensEnviadas = 0;
    totalMensagensRecebidas = 0;
    tempoInicioConvergencia = simTime();
    jaConvergiu = false;
    ciclosAposConvergencia = 0;
    
    // Inicializar variáveis de convergência
    ciclosEstaveis = 0;
    tabelaAnterior.clear();
    
    // Inicializar métricas globais (apenas no primeiro nó)
    if (meuID == 0) {
        // Reinicializar TODAS as variáveis estáticas para nova simulação
        totalNosConvergiu = 0;
        totalMensagensEnviadasRede = 0;
        totalMensagensRecebidasRede = 0;
        tempoInicioRede = simTime().dbl();
        tempoFimConvergenciaRede = 0.0; // IMPORTANTE: reinicializar!
        redeConvergiu = false;
        resumoGlobalMostrado = false;
        
        // Descobrir total de nós na rede
        cModule *network = getParentModule();
        totalNosRede = network->getSubmoduleVectorSize("no");
        
        EV << "=== INICIALIZACAO DA REDE ===\n";
        EV << "Total de nos na rede: " << totalNosRede << "\n";
        EV << "=============================\n\n";
    }
    
    // Passo 2: Descobrir meus vizinhos
    descobrirVizinhos();
    
    // Passo 3: Inicializar tabela de roteamento
    InformacaoRota paraEuMesmo;
    paraEuMesmo.custo = 0.0;
    paraEuMesmo.proximoVizinho = meuID;
    tabelaRoteamento[meuID] = paraEuMesmo;
    
    // Passo 4: Criar um timer para enviar minha tabela periodicamente
    temporizadorEnvio = new cMessage("enviarTabela");
    scheduleAt(simTime() + 1.0, temporizadorEnvio);
    
    EV << "Nó " << meuID << " inicializado com " << listaVizinhos.size() << " vizinhos\n";
}

void NoRoteador::descobrirVizinhos() {
    
    // Verificar quantas conexões eu tenho
    int numConexoes = gateSize("porta");
    
    for (int i = 0; i < numConexoes; i++) {
        // Pegar a porta de saída
        cGate *minhaPorta = gate("porta$o", i);
        if (!minhaPorta) continue;
        
        // Ver para onde ela está conectada
        cGate *portaVizinho = minhaPorta->getNextGate();
        if (!portaVizinho) continue;
        
        // Descobrir qual nó está do outro lado
        cModule *vizinho = portaVizinho->getOwnerModule();
        if (!vizinho) continue;
        
        int idVizinho = vizinho->par("meuNumero");
        
        // Descobrir quanto custa enviar uma mensagem para este vizinho
        double custoComunicacao = 0.0;
        cChannel *canal = minhaPorta->getChannel();
        if (canal && canal->hasPar("delay")) {
            custoComunicacao = canal->par("delay").doubleValue();
        }
        
        // Guardar as informações do vizinho
        custoVizinhos[idVizinho] = custoComunicacao;
        listaVizinhos.push_back(idVizinho);
        
        EV << "Nó " << meuID << " descobriu vizinho " << idVizinho 
           << " (custo: " << std::fixed << std::setprecision(3) << custoComunicacao << "s)\n";
    }
}

void NoRoteador::handleMessage(cMessage *msg) {
    // Se for meu timer, é hora de enviar minha tabela
    if (msg == temporizadorEnvio) {
        enviarMinhaTabela();
        
        // Verificar se devemos continuar agendando
        if (jaConvergiu) {
            ciclosAposConvergencia++;
            if (ciclosAposConvergencia >= 5) {
                return;
            }
        }
        
        // Verificar limite de tempo
        if (simTime().dbl() >= 18.0) {
            return;
        }
        
        scheduleAt(simTime() + 1.0, temporizadorEnvio);
        return;
    }
    
    // Tabela de roteamento de vizinho
    if (dynamic_cast<TabelaRoteamento*>(msg)) {
        processarTabelaVizinho(msg);
        return;
    }
    
    // Mensagem desconhecida
    delete msg;
}

void NoRoteador::enviarMinhaTabela() {
    // Preparar dados da tabela
    std::vector<int> destinos;
    std::vector<double> custos;
    
    for (auto& entrada : tabelaRoteamento) {
        destinos.push_back(entrada.first);
        custos.push_back(entrada.second.custo);
    }
    
    // Enviar para cada vizinho
    for (int vizinho : listaVizinhos) {
        TabelaRoteamento *mensagem = new TabelaRoteamento("minhaTabela");
        mensagem->setRemetente(meuID);
        
        mensagem->setDestinosArraySize(destinos.size());
        mensagem->setCustosArraySize(custos.size());
        for (size_t i = 0; i < destinos.size(); i++) {
            mensagem->setDestinos(i, destinos[i]);
            mensagem->setCustos(i, custos[i]);
        }
        
        // Encontrar porta para o vizinho
        for (int porta = 0; porta < gateSize("porta"); porta++) {
            cGate *minhaPorta = gate("porta$o", porta);
            if (!minhaPorta) continue;
            
            cGate *portaVizinho = minhaPorta->getNextGate();
            if (!portaVizinho) continue;
            
            cModule *moduloVizinho = portaVizinho->getOwnerModule();
            if (!moduloVizinho) continue;
            
            if (moduloVizinho->par("meuNumero").intValue() == vizinho) {
                send(mensagem->dup(), "porta$o", porta);
                totalMensagensEnviadas++;
                break;
            }
        }
        delete mensagem;
    }
}

void NoRoteador::processarTabelaVizinho(cMessage *msg) {
    TabelaRoteamento *tabela = check_and_cast<TabelaRoteamento*>(msg);
    int vizinhoQueEnviou = tabela->getRemetente();
    
    totalMensagensRecebidas++;
    double custoParaVizinho = custoVizinhos[vizinhoQueEnviou];
    
    bool tabelaMudou = false;
    
    // Processar cada destino da tabela do vizinho
    int numDestinos = tabela->getDestinosArraySize();
    for (int i = 0; i < numDestinos; i++) {
        int destino = tabela->getDestinos(i);
        double custoVizinhoAteDestino = tabela->getCustos(i);
        double custoTotal = custoParaVizinho + custoVizinhoAteDestino;
        
        auto it = tabelaRoteamento.find(destino);
        if (it == tabelaRoteamento.end()) {
            // Novo destino
            InformacaoRota novaRota;
            novaRota.custo = custoTotal;
            novaRota.proximoVizinho = vizinhoQueEnviou;
            tabelaRoteamento[destino] = novaRota;
            tabelaMudou = true;
            
            EV << "Nó " << meuID << " aprendeu rota para " << destino 
               << " via " << vizinhoQueEnviou << " (custo: " 
               << std::fixed << std::setprecision(3) << custoTotal << "s)\n";
        }
        else if (custoTotal < it->second.custo) {
            // Caminho melhor encontrado
            it->second.custo = custoTotal;
            it->second.proximoVizinho = vizinhoQueEnviou;
            tabelaMudou = true;
            
            EV << "Nó " << meuID << " melhorou rota para " << destino 
               << " via " << vizinhoQueEnviou << " (custo: " 
               << std::fixed << std::setprecision(3) << custoTotal << "s)\n";
        }
    }
    
    if (tabelaMudou) {
        mostrarTabelaRoteamento();
    } else {
        verificarConvergencia();
    }
    
    delete tabela;
}

void NoRoteador::verificarConvergencia() {
    // Verificar se a tabela de roteamento está estável
    bool tabelaEstavel = true;
    
    // Comparar com a tabela anterior
    if (!tabelaAnterior.empty()) {
        for (auto& entrada : tabelaRoteamento) {
            int destino = entrada.first;
            auto entradaAnterior = tabelaAnterior.find(destino);
            
            if (entradaAnterior == tabelaAnterior.end() || 
                std::abs(entrada.second.custo - entradaAnterior->second.custo) > 0.001) {
                tabelaEstavel = false;
                break;
            }
        }
    } else {
        tabelaEstavel = false; // Primeira verificação
    }
    
    // Atualizar tabela anterior
    tabelaAnterior = tabelaRoteamento;
    
    // Se a tabela está estável por 3 ciclos consecutivos, considerar convergido
    if (tabelaEstavel) {
        ciclosEstaveis++;
        if (ciclosEstaveis >= 3 && !jaConvergiu) {
            jaConvergiu = true;
            tempoFimConvergencia = simTime();
            double tempoTotal = (tempoFimConvergencia - tempoInicioConvergencia).dbl();
            
            // Gravar métricas (sem mostrar no log)
            recordScalar("tempoConvergencia", tempoTotal);
            recordScalar("mensagensEnviadas", totalMensagensEnviadas);
            recordScalar("mensagensRecebidas", totalMensagensRecebidas);
            
            // Atualizar métricas globais
            totalNosConvergiu++;
            totalMensagensEnviadasRede += totalMensagensEnviadas;
            totalMensagensRecebidasRede += totalMensagensRecebidas;
            
            // Atualizar o tempo de fim da convergência da rede (usar o tempo atual da simulação)
            double tempoAtual = simTime().dbl();
            if (tempoAtual > tempoFimConvergenciaRede) {
                tempoFimConvergenciaRede = tempoAtual;
            }
            
            // Verificar se TODOS os nós convergiram
            if (totalNosConvergiu >= totalNosRede && !redeConvergiu) {
                redeConvergiu = true;
                
                // Gravar métricas globais
                double tempoTotalRede = tempoFimConvergenciaRede - tempoInicioRede;
                int totalMensagensRede = totalMensagensEnviadasRede + totalMensagensRecebidasRede;
                

                
                recordScalar("tempoConvergenciaRede", tempoTotalRede);
                recordScalar("totalMensagensRede", totalMensagensRede);
                recordScalar("mensagensEnviadasRede", totalMensagensEnviadasRede);
                recordScalar("mensagensRecebidasRede", totalMensagensRecebidasRede);
                recordScalar("nosConvergiu", totalNosConvergiu);
            }
        }
    } else {
        ciclosEstaveis = 0; // Reset contador se tabela mudou
    }
}

void NoRoteador::mostrarTabelaRoteamento() {
    EV << "Nó " << meuID << " - Tabela atualizada:\n";
    EV << "Destino | Custo   | Próximo\n";
    EV << "--------|---------|--------\n";
    
    for (auto& entrada : tabelaRoteamento) {
        EV << std::setw(7) << entrada.first << " | "
           << std::setw(7) << std::fixed << std::setprecision(3) << entrada.second.custo << " | "
           << std::setw(7) << entrada.second.proximoVizinho << "\n";
    }
    EV << "\n";
}

void NoRoteador::finish() {
    EV << "Nó " << meuID << " finalizado\n";
    
    // MÉTRICA 3: Verificação de Consistência da Tabela de Roteamento
    EV << "\n=== VERIFICAÇÃO DE CONSISTÊNCIA - Nó " << meuID << " ===\n";
    EV << "Tabela de roteamento final:\n";
    mostrarTabelaRoteamento();
    
    // Verificar se as rotas são consistentes (caminhos mínimos)
    EV << "Análise de consistência:\n";
    bool tabelaConsistente = true;
    
    for (auto& entrada : tabelaRoteamento) {
        int destino = entrada.first;
        double custoCalculado = entrada.second.custo;
        int proximoVizinho = entrada.second.proximoVizinho;
        
        // Verificar se o custo para o próximo vizinho é conhecido
        auto custoVizinho = custoVizinhos.find(proximoVizinho);
        if (custoVizinho != custoVizinhos.end()) {
            double custoDireto = custoVizinho->second;
            if (destino == proximoVizinho) {
                // Para o próprio vizinho, o custo deve ser o custo direto
                if (std::abs(custoCalculado - custoDireto) > 0.001) {
                    EV << "Inconsistência: Nó " << destino << " - custo calculado (" 
                       << custoCalculado << "s) != custo direto (" << custoDireto << "s)\n";
                    tabelaConsistente = false;
                } else {
                    EV << "Nó " << destino << " - rota direta consistente (" 
                       << std::fixed << std::setprecision(3) << custoCalculado << "s)\n";
                }
            } else {
                EV << "Nó " << destino << " - rota via " << proximoVizinho 
                   << " (" << std::fixed << std::setprecision(3) << custoCalculado << "s)\n";
            }
        }
    }
    
    if (tabelaConsistente) {
        EV << "Tabela de roteamento CONSISTENTE - todos os caminhos são ótimos!\n";
    } else {
        EV << "Tabela de roteamento INCONSISTENTE - alguns caminhos podem não ser ótimos!\n";
    }
    EV << "==========================================\n\n";
    
    // MÉTRICAS FINAIS SIMPLES
    EV << "\n=== MÉTRICAS FINAIS - Nó " << meuID << " ===\n";
    int totalMensagens = totalMensagensEnviadas + totalMensagensRecebidas;
    EV << "Mensagens enviadas: " << totalMensagensEnviadas << "\n";
    EV << "Mensagens recebidas: " << totalMensagensRecebidas << "\n";
    EV << "Total de mensagens trocadas: " << totalMensagens << "\n";
    
    if (jaConvergiu) {
        double tempoTotal = (tempoFimConvergencia - tempoInicioConvergencia).dbl();
        EV << "Tempo de convergência: " << std::fixed << std::setprecision(3) << tempoTotal << "s\n";
    } else {
        EV << "Tempo de convergência: N/A (não convergiu)\n";
    }
    EV << "==========================================\n\n";
    
    // Gravar métricas finais
    recordScalar("mensagensEnviadasFinal", totalMensagensEnviadas);
    recordScalar("mensagensRecebidasFinal", totalMensagensRecebidas);
    
    if (jaConvergiu) {
        double tempoTotal = (tempoFimConvergencia - tempoInicioConvergencia).dbl();
        recordScalar("tempoConvergenciaFinal", tempoTotal);
    }
    
    // Limpar timer
    if (temporizadorEnvio) {
        cancelAndDelete(temporizadorEnvio);
        temporizadorEnvio = nullptr;
    }
    
    // Mostrar resumo global no final (apenas no último nó)
    if (meuID == totalNosRede - 1) {
        mostrarResumoGlobal();
    }
}

void NoRoteador::mostrarResumoGlobal() {
    // Evitar mostrar múltiplas vezes
    if (resumoGlobalMostrado) {
        return;
    }
    resumoGlobalMostrado = true;
    
    EV << "\n";
    EV << "==========================================\n";
    EV << "RESUMO GLOBAL DA SIMULAÇÃO\n";
    EV << "==========================================\n";
    
    if (redeConvergiu) {
        double tempoTotalRede = tempoFimConvergenciaRede - tempoInicioRede;
        int totalMensagensRede = totalMensagensEnviadasRede + totalMensagensRecebidasRede;
        

        
        EV << "REDE COMPLETAMENTE CONVERGIDA!\n";
        EV << "Tempo total de convergência da rede: " << std::fixed << std::setprecision(3) << tempoTotalRede << "s\n";
        EV << "Total de mensagens trocadas na rede: " << totalMensagensRede << "\n";
        EV << "Mensagens enviadas na rede: " << totalMensagensEnviadasRede << "\n";
        EV << "Mensagens recebidas na rede: " << totalMensagensRecebidasRede << "\n";
        EV << "Nós que convergiram: " << totalNosConvergiu << "/" << totalNosRede << "\n";
    } else {
        EV << "REDE NAO CONVERGIU COMPLETAMENTE\n";
        EV << "Nós que convergiram: " << totalNosConvergiu << "/" << totalNosRede << "\n";
        EV << "Tempo de simulação: " << simTime().dbl() << "s\n";
        EV << "Total de mensagens trocadas na rede: " << (totalMensagensEnviadasRede + totalMensagensRecebidasRede) << "\n";
    }
    
    EV << "==========================================\n\n";
}

