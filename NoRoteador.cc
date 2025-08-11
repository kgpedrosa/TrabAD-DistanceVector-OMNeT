#include "NoRoteador.h"
#include "Mensagens_m.h"
#include <algorithm>  // Para std::find

Define_Module(NoRoteador);

void NoRoteador::initialize() {
    // Passo 1: Descobrir quem sou eu
    meuID = par("meuNumero");
    
    // Inicializar sistema de logging
    logger = new Logger(meuID, this);
    logger->logInicializacao(0); // Será atualizado depois
    
    // Inicializar métricas para avaliação
    totalMensagensEnviadas = 0;
    totalMensagensRecebidas = 0;
    tempoInicioConvergencia = simTime();
    jaConvergiu = false;
    ciclosAposConvergencia = 0;
    
    // Passo 2: Descobrir meus vizinhos e quanto custa falar com cada um
    descobrirVizinhos();
    
    // Passo 3: Começar minha tabela de roteamento
    // Para chegar em mim mesmo, o custo é zero e o próximo sou eu mesmo
    InformacaoRota paraEuMesmo;
    paraEuMesmo.custo = 0.0;
    paraEuMesmo.proximoVizinho = meuID;
    tabelaRoteamento[meuID] = paraEuMesmo;
    
    // Passo 4: Criar um timer para enviar minha tabela periodicamente
    temporizadorEnvio = new cMessage("enviarTabela");
    scheduleAt(simTime() + 1.0, temporizadorEnvio); 
    
    logger->logConfiguracaoCompleta(listaVizinhos.size());
}

void NoRoteador::descobrirVizinhos() {
    logger->logDescobertaVizinhos();
    
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
        
        logger->logVizinhoEncontrado(idVizinho, custoComunicacao);
    }
}

void NoRoteador::handleMessage(cMessage *msg) {
    // Se for meu timer, é hora de enviar minha tabela
    if (msg == temporizadorEnvio) {
        enviarMinhaTabela();
        
        // Verificar se devemos continuar agendando
        if (jaConvergiu) {
            ciclosAposConvergencia++;
            
            // Dar mais tempo para garantir que todos os nós aprendam todas as rotas
            // Especialmente importante em topologias lineares onde a informação precisa
            // se propagar por todos os nós sequencialmente
            if (ciclosAposConvergencia >= 5) {  // Mais 5 ciclos após convergência
                // Parar o timer
                return;
            }
        }
        
        // Verificar se não ultrapassou o limite de tempo (18s para dar margem)
        if (simTime().dbl() >= 18.0) {
            // Parar o timer para evitar ultrapassar o limite
            return;
        }
        
        // Agendar próximo envio
        scheduleAt(simTime() + 1.0, temporizadorEnvio); 
        return;
    }
    
    // Se for uma tabela de roteamento de um vizinho
    if (dynamic_cast<TabelaRoteamento*>(msg)) {
        processarTabelaVizinho(msg);
        return;
    }
    
    // Se chegou até aqui, não sei o que é essa mensagem
    logger->logMensagemDesconhecida();
    delete msg;
}

void NoRoteador::enviarMinhaTabela() {
    logger->logEnvioTabela();
    
    // Preparar listas com destinos e custos (uma vez só)
    std::vector<int> destinos;
    std::vector<double> custos;
    
    for (auto& entrada : tabelaRoteamento) {
        destinos.push_back(entrada.first);      // Para onde posso ir
        custos.push_back(entrada.second.custo); // Quanto custa
    }
    
    // Para cada vizinho, enviar uma cópia da mensagem
    for (int vizinho : listaVizinhos) {
        // Criar uma mensagem com minha tabela
        TabelaRoteamento *mensagem = new TabelaRoteamento("minhaTabela");
        mensagem->setRemetente(meuID);
        
        // Colocar as informações na mensagem
        mensagem->setDestinosArraySize(destinos.size());
        mensagem->setCustosArraySize(custos.size());
        for (size_t i = 0; i < destinos.size(); i++) {
            mensagem->setDestinos(i, destinos[i]);
            mensagem->setCustos(i, custos[i]);
        }
        
        // Enviar para o vizinho
        // Primeiro, encontrar qual porta usar para este vizinho
        for (int porta = 0; porta < gateSize("porta"); porta++) {
            cGate *minhaPorta = gate("porta$o", porta);
            if (!minhaPorta) continue;
            
            cGate *portaVizinho = minhaPorta->getNextGate();
            if (!portaVizinho) continue;
            
            cModule *moduloVizinho = portaVizinho->getOwnerModule();
            if (!moduloVizinho) continue;
            
            if (moduloVizinho->par("meuNumero").intValue() == vizinho) {
                send(mensagem->dup(), "porta$o", porta);
                totalMensagensEnviadas++;  // Contar mensagem enviada
                break;
            }
        }
        delete mensagem;
    }
}

void NoRoteador::processarTabelaVizinho(cMessage *msg) {
    TabelaRoteamento *tabela = check_and_cast<TabelaRoteamento*>(msg);
    int vizinhoQueEnviou = tabela->getRemetente();
    
    totalMensagensRecebidas++;  // Contar mensagem recebida
    logger->logRecebimentoTabela(vizinhoQueEnviou);
    
    // Quanto custa falar com este vizinho?
    double custoParaVizinho = custoVizinhos[vizinhoQueEnviou];
    
    bool tabelaMudou = false;
    
    // Para cada destino que o vizinho conhece
    int numDestinos = tabela->getDestinosArraySize();
    for (int i = 0; i < numDestinos; i++) {
        int destino = tabela->getDestinos(i);
        double custoVizinhoAteDestino = tabela->getCustos(i);
        
        // Calcular: quanto custaria ir até este destino passando por este vizinho?
        double custoTotal = custoParaVizinho + custoVizinhoAteDestino;
        
        // Verificar se já conheço este destino
        auto it = tabelaRoteamento.find(destino);
        if (it == tabelaRoteamento.end()) {
            // Destino novo! Vou aprender
            InformacaoRota novaRota;
            novaRota.custo = custoTotal;
            novaRota.proximoVizinho = vizinhoQueEnviou;
            tabelaRoteamento[destino] = novaRota;
            tabelaMudou = true;
            
            logger->logNovoDestino(destino, vizinhoQueEnviou, custoTotal);
        }
        else {
            // Já conheço este destino. Este novo caminho é melhor?
            if (custoTotal < it->second.custo) {
                // Sim! Vou atualizar minha tabela
                it->second.custo = custoTotal;
                it->second.proximoVizinho = vizinhoQueEnviou;
                tabelaMudou = true;
                
                logger->logCaminhoMelhor(destino, vizinhoQueEnviou, custoTotal);
            }
        }
    }
    
    if (tabelaMudou) {
        logger->logTabelaMudou();
        logger->logTabelaRoteamento(tabelaRoteamento, listaVizinhos);
    } else {
        // Se a tabela não mudou, pode ser que tenha convergido
        logger->logTabelaNaoMudou();
        verificarConvergencia();
    }
    
    delete tabela;
}

void NoRoteador::verificarConvergencia() {
    // Verificar se não houve mudanças recentes (possível convergência)
    if (!jaConvergiu) {
        // Heurística melhorada: só convergir se tiver informações sobre todos os nós
        // ou se já passou tempo suficiente (para evitar convergência prematura)
        double tempoAtual = simTime().dbl();
        
        // Em topologias lineares, o nó mais distante está a N-1 saltos
        // Dar tempo suficiente para a informação se propagar
        if (tempoAtual >= 8.0) {  // Tempo mínimo antes de considerar convergência
            jaConvergiu = true;
            tempoFimConvergencia = simTime();
            double tempoTotal = (tempoFimConvergencia - tempoInicioConvergencia).dbl();
            
            logger->logConvergencia(tempoTotal);
            logger->gravarMetricas(totalMensagensEnviadas, totalMensagensRecebidas, tempoTotal);
        }
    }
}

void NoRoteador::finish() {
    logger->logFinalizacao();
    logger->logTabelaRoteamento(tabelaRoteamento, listaVizinhos);
    
    // Gravar métricas finais
    logger->gravarMetricasFinais(totalMensagensEnviadas, totalMensagensRecebidas, 0.0);
    
    if (jaConvergiu) {
        double tempoTotal = (tempoFimConvergencia - tempoInicioConvergencia).dbl();
        logger->logMetricasFinais(totalMensagensEnviadas, totalMensagensRecebidas, tempoTotal);
    }
    
    // Limpar o timer
    if (temporizadorEnvio) {
        cancelAndDelete(temporizadorEnvio);
        temporizadorEnvio = nullptr;
    }
    
    // Limpar o logger
    delete logger;
}
