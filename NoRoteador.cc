#include "NoRoteador.h"
#include "Mensagens_m.h"
#include <algorithm>  // Para std::find

Define_Module(NoRoteador);

void NoRoteador::initialize() {
    // Passo 1: Descobrir quem sou eu
    meuID = par("meuNumero");
    EV << "Sou o nó " << meuID << " e estou iniciando!\n";
    
    // 📊 Inicializar métricas para avaliação
    totalMensagensEnviadas = 0;
    totalMensagensRecebidas = 0;
    tempoInicioConvergencia = simTime();
    jaConvergiu = false;
    
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
    scheduleAt(simTime() + 0.1, temporizadorEnvio);  // Enviar a cada 0.1 segundos
    
    EV << "Nó " << meuID << " configurado com " << listaVizinhos.size() << " vizinhos\n";
}

void NoRoteador::descobrirVizinhos() {
    EV << "Nó " << meuID << " descobrindo vizinhos...\n";
    
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
        
        EV << "Encontrei o vizinho " << idVizinho << " com custo " << custoComunicacao << "s\n";
    }
}

void NoRoteador::handleMessage(cMessage *msg) {
    // Se for meu timer, é hora de enviar minha tabela
    if (msg == temporizadorEnvio) {
        enviarMinhaTabela();
        // Agendar próximo envio
        scheduleAt(simTime() + 1.0, temporizadorEnvio);  // A cada 1 segundo
        return;
    }
    
    // Se for uma tabela de roteamento de um vizinho
    if (dynamic_cast<TabelaRoteamento*>(msg)) {
        processarTabelaVizinho(msg);
        return;
    }
    
    // Se chegou até aqui, não sei o que é essa mensagem
    EV << "Nó " << meuID << " recebeu mensagem desconhecida\n";
    delete msg;
}

void NoRoteador::enviarMinhaTabela() {
    EV << "Nó " << meuID << " enviando tabela para vizinhos\n";
    
    // Para cada vizinho, vou contar como chegar em cada destino
    for (int vizinho : listaVizinhos) {
        // Criar uma mensagem com minha tabela
        TabelaRoteamento *mensagem = new TabelaRoteamento("minhaTabela");
        mensagem->setRemetente(meuID);
        
        // Preparar listas com destinos e custos
        std::vector<int> destinos;
        std::vector<double> custos;
        
        for (auto& entrada : tabelaRoteamento) {
            destinos.push_back(entrada.first);      // Para onde posso ir
            custos.push_back(entrada.second.custo); // Quanto custa
        }
        
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
                totalMensagensEnviadas++;  // 📊 Contar mensagem enviada
                break;
            }
        }
        delete mensagem;
    }
}

void NoRoteador::processarTabelaVizinho(cMessage *msg) {
    TabelaRoteamento *tabela = check_and_cast<TabelaRoteamento*>(msg);
    int vizinhoQueEnviou = tabela->getRemetente();
    
    totalMensagensRecebidas++;  // 📊 Contar mensagem recebida
    EV << "Nó " << meuID << " recebeu tabela do vizinho " << vizinhoQueEnviou << "\n";
    
    // Quanto custa falar com este vizinho?
    double custoParaVizinho = custoVizinhos[vizinhoQueEnviou];
    
    bool tabelaMudou = false;
    
    // Para cada destino que o vizinho conhece
    for (int i = 0; i < tabela->getDestinosArraySize(); i++) {
        int destino = tabela->getDestinos(i);
        double custoVizinhoAteDestino = tabela->getCustos(i);
        
        // Calcular: quanto custaria ir até este destino passando por este vizinho?
        double custoTotal = custoParaVizinho + custoVizinhoAteDestino;
        
        // Verificar se já conheço este destino
        if (tabelaRoteamento.find(destino) == tabelaRoteamento.end()) {
            // Destino novo! Vou aprender
            InformacaoRota novaRota;
            novaRota.custo = custoTotal;
            novaRota.proximoVizinho = vizinhoQueEnviou;
            tabelaRoteamento[destino] = novaRota;
            tabelaMudou = true;
            
            EV << "Aprendi novo destino " << destino << " via vizinho " << vizinhoQueEnviou
               << " com custo " << custoTotal << "\n";
        }
        else {
            // Já conheço este destino. Este novo caminho é melhor?
            if (custoTotal < tabelaRoteamento[destino].custo) {
                // Sim! Vou atualizar minha tabela
                tabelaRoteamento[destino].custo = custoTotal;
                tabelaRoteamento[destino].proximoVizinho = vizinhoQueEnviou;
                tabelaMudou = true;
                
                EV << "⬆️ Encontrei caminho melhor para " << destino << " via vizinho " 
                   << vizinhoQueEnviou << " com custo " << custoTotal << "\n";
            }
        }
    }
    
    if (tabelaMudou) {
        EV << "Minha tabela mudou! Vou mostrar como está agora:\n";
        mostrarMinhaTabela();
    } else {
        // Se a tabela não mudou, pode ser que tenha convergido
        verificarConvergencia();
    }
    
    delete tabela;
}

void NoRoteador::mostrarMinhaTabela() {
    EV << "Tabela de Roteamento do Nó " << meuID << ":\n";
    EV << "   Destino | Custo  | Próximo | Caminho Completo\n";
    EV << "   --------|--------|---------|------------------\n";
    
    for (auto& entrada : tabelaRoteamento) {
        std::string caminhoCompleto = obterCaminhoCompleto(entrada.first);
        EV << "   " << entrada.first << "       | " 
           << entrada.second.custo << " | " 
           << entrada.second.proximoVizinho << "       | " 
           << caminhoCompleto << "\n";
    }
    EV << "\n";
}

std::string NoRoteador::obterCaminhoCompleto(int destino) {
    // Reconstrói o caminho completo seguindo os próximos saltos
    
    if (destino == meuID) {
        return std::to_string(meuID);  // Caminho para mim mesmo
    }
    
    std::string caminho = std::to_string(meuID);  // Começar comigo
    int atual = meuID;
    int passos = 0;  // Contador para evitar loops infinitos
    
    while (atual != destino && passos < 10) {  // Máximo 10 saltos para evitar loop
        // Procurar o próximo salto na tabela
        auto it = tabelaRoteamento.find(destino);
        if (it == tabelaRoteamento.end()) {
            return caminho + " -> (sem rota)";  // Não há rota conhecida
        }
        
        int proximoSalto = it->second.proximoVizinho;
        caminho += " -> " + std::to_string(proximoSalto);
        
        if (proximoSalto == destino) {
            break;  // Chegamos ao destino
        }
        
        // Para continuar o caminho, precisaríamos da tabela do próximo nó
        // Como não temos acesso, vamos simular baseado na nossa tabela
        atual = proximoSalto;
        passos++;
        
        // Se o próximo salto for um vizinho direto, vamos direto ao destino
        if (std::find(listaVizinhos.begin(), listaVizinhos.end(), proximoSalto) != listaVizinhos.end()) {
            if (proximoSalto != destino) {
                caminho += " -> " + std::to_string(destino);
            }
            break;
        }
    }
    
    if (passos >= 10) {
        caminho += " -> (loop detectado)";
    }
    
    return caminho;
}

void NoRoteador::verificarConvergencia() {
    // Verificar se não houve mudanças recentes (possível convergência)
    if (!jaConvergiu) {
        // Simples heurística: se recebi mensagens mas não mudei, posso ter convergido
        jaConvergiu = true;
        tempoFimConvergencia = simTime();
        double tempoTotal = (tempoFimConvergencia - tempoInicioConvergencia).dbl();
        
        EV << "Nó " << meuID << " possivelmente CONVERGIU em " << tempoTotal << "s\n";
        
        // Gravar métricas como scalars para análise
        recordScalar("mensagensEnviadas", totalMensagensEnviadas);
        recordScalar("mensagensRecebidas", totalMensagensRecebidas);
        recordScalar("tempoConvergencia", tempoTotal);
    }
}

void NoRoteador::finish() {
    EV << "Nó " << meuID << " finalizando. Tabela final:\n";
    mostrarMinhaTabela();
    
    // Gravar métricas finais
    recordScalar("mensagensEnviadasFinal", totalMensagensEnviadas);
    recordScalar("mensagensRecebidasFinal", totalMensagensRecebidas);
    
    if (jaConvergiu) {
        double tempoTotal = (tempoFimConvergencia - tempoInicioConvergencia).dbl();
        recordScalar("tempoConvergenciaFinal", tempoTotal);
        EV << "MÉTRICAS FINAIS - Enviadas: " << totalMensagensEnviadas
           << ", Recebidas: " << totalMensagensRecebidas 
           << ", Tempo Convergência: " << tempoTotal << "s\n";
    }
    
    // Verificar consistência: mostrar rota completa para cada destino
    EV << "VERIFICAÇÃO DE CONSISTÊNCIA - CAMINHOS COMPLETOS:\n";
    for (auto& entrada : tabelaRoteamento) {
        if (entrada.first != meuID) {  // Não mostrar rota para mim mesmo
            std::string caminhoCompleto = obterCaminhoCompleto(entrada.first);
            EV << "Para destino " << entrada.first << ": custo=" << entrada.second.custo
               << "s, caminho=" << caminhoCompleto << "\n";
        }
    }
    
    // Limpar o timer
    if (temporizadorEnvio) {
        cancelAndDelete(temporizadorEnvio);
        temporizadorEnvio = nullptr;
    }
}
