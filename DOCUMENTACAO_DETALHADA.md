# Documentação Detalhada - Sistema de Roteamento Distribuído

## Índice
1. [Visão Geral](#visão-geral)
2. [Estrutura de Arquivos](#estrutura-de-arquivos)
3. [NoRoteador.h - Definições](#noroteadorh---definições)
4. [NoRoteador.cc - Implementação](#noroteadorcc---implementação)
5. [Algoritmo de Roteamento](#algoritmo-de-roteamento)
6. [Métricas e Coleta de Dados](#métricas-e-coleta-de-dados)
7. [Pseudocódigo Completo](#pseudocódigo-completo)

---

## Visão Geral

Este sistema implementa um **algoritmo de roteamento distribuído** baseado no **Distance Vector** (Bellman-Ford), onde cada nó da rede:
- Descobre seus vizinhos automaticamente
- Mantém uma tabela de roteamento local
- Troca informações de roteamento com vizinhos
- Detecta convergência quando a tabela se estabiliza
- Coleta métricas de performance

---

## Estrutura de Arquivos

### NoRoteador.h
- **Propósito**: Define a classe `NoRoteador` e suas estruturas de dados
- **Herança**: Estende `cSimpleModule` do OMNeT++
- **Funcionalidades**: Declarações de métodos e variáveis

### NoRoteador.cc
- **Propósito**: Implementa toda a lógica do algoritmo de roteamento
- **Funcionalidades**: 
  - Descoberta de vizinhos
  - Troca de mensagens
  - Atualização de tabelas
  - Detecção de convergência
  - Coleta de métricas

---

## NoRoteador.h - Definições

### Estrutura InformacaoRota
```cpp
struct InformacaoRota {
    double custo;           // Custo total até o destino
    int proximoVizinho;     // Próximo nó no caminho
};
```

### Variáveis de Instância (protected)
```cpp
// Identificação
int meuID;                  // ID único do nó

// Tabela de roteamento
std::map<int, InformacaoRota> tabelaRoteamento;

// Informações de vizinhos
std::vector<int> listaVizinhos;           // Lista de IDs dos vizinhos
std::map<int, double> custoVizinhos;      // Custo para cada vizinho

// Métricas individuais
int totalMensagensEnviadas;
int totalMensagensRecebidas;
simtime_t tempoInicioConvergencia;
simtime_t tempoFimConvergencia;
bool jaConvergiu;
int ciclosAposConvergencia;

// Detecção de convergência
std::map<int, InformacaoRota> tabelaAnterior;
int ciclosEstaveis;

// Timer
cMessage *temporizadorEnvio;
```

### Variáveis Estáticas (globais da rede)
```cpp
static int totalNosConvergiu;              // Total de nós que convergiram
static int totalMensagensEnviadasRede;     // Mensagens enviadas na rede
static int totalMensagensRecebidasRede;    // Mensagens recebidas na rede
static double tempoInicioRede;             // Tempo de início da rede
static double tempoFimConvergenciaRede;    // Tempo de fim da convergência
static bool redeConvergiu;                 // Se toda a rede convergiu
static int totalNosRede;                   // Total de nós na rede
static bool resumoGlobalMostrado;          // Controle de exibição
```

### Métodos Virtuais (OMNeT++)
```cpp
virtual void initialize() override;        // Inicialização do módulo
virtual void handleMessage(cMessage *msg) override;  // Processamento de mensagens
virtual void finish() override;            // Finalização do módulo
```

### Métodos Privados
```cpp
void descobrirVizinhos();                 // Descobre vizinhos conectados
void enviarMinhaTabela();                 // Envia tabela para vizinhos
void processarTabelaVizinho(cMessage *msg); // Processa tabela recebida
void verificarConvergencia();             // Verifica se convergiu
void mostrarTabelaRoteamento();           // Exibe tabela no log
void mostrarResumoGlobal();               // Exibe resumo final
```

---

## NoRoteador.cc - Implementação

### 1. Inicialização das Variáveis Estáticas
```cpp
// Inicialização das variáveis estáticas para métricas globais
int NoRoteador::totalNosConvergiu = 0;
int NoRoteador::totalMensagensEnviadasRede = 0;
int NoRoteador::totalMensagensRecebidasRede = 0;
double NoRoteador::tempoInicioRede = 0.0;
double NoRoteador::tempoFimConvergenciaRede = 0.0;
bool NoRoteador::redeConvergiu = false;
int NoRoteador::totalNosRede = 0;
bool NoRoteador::resumoGlobalMostrado = false;
```

**Explicação**: Estas variáveis são compartilhadas entre todas as instâncias da classe, permitindo coleta de métricas globais da rede.

### 2. Método initialize()
```cpp
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
        tempoFimConvergenciaRede = 0.0;
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
```

**Explicação**:
- `par("meuNumero")`: Lê o parâmetro "meuNumero" do arquivo NED
- `simTime()`: Retorna o tempo atual da simulação
- `simTime().dbl()`: Converte para double
- `getParentModule()`: Obtém o módulo pai (rede)
- `getSubmoduleVectorSize("no")`: Conta quantos nós existem na rede
- `scheduleAt()`: Agenda um evento para o futuro
- `EV <<`: Sistema de logging do OMNeT++

### 3. Método descobrirVizinhos()
```cpp
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
```

**Explicação**:
- `gateSize("porta")`: Conta quantas portas "porta" existem
- `gate("porta$o", i)`: Obtém a i-ésima porta de saída
- `getNextGate()`: Obtém a porta conectada
- `getOwnerModule()`: Obtém o módulo que possui a porta
- `getChannel()`: Obtém o canal conectado à porta
- `hasPar("delay")`: Verifica se o canal tem parâmetro "delay"
- `par("delay").doubleValue()`: Lê o valor do delay como double

### 4. Método handleMessage()
```cpp
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
```

**Explicação**:
- `dynamic_cast<TabelaRoteamento*>(msg)`: Verifica se a mensagem é do tipo TabelaRoteamento
- `delete msg`: Libera memória da mensagem não reconhecida
- Lógica de timer: Agenda novo envio a cada 1 segundo, até convergir ou atingir limite

### 5. Método enviarMinhaTabela()
```cpp
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
```

**Explicação**:
- `new TabelaRoteamento("minhaTabela")`: Cria nova mensagem
- `setDestinosArraySize()`: Define tamanho do array de destinos
- `setCustosArraySize()`: Define tamanho do array de custos
- `mensagem->dup()`: Duplica a mensagem para envio
- `send()`: Envia mensagem pela porta especificada
- `delete mensagem`: Libera memória da mensagem original

### 6. Método processarTabelaVizinho()
```cpp
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
```

**Explicação**:
- `check_and_cast<TabelaRoteamento*>(msg)`: Converte mensagem para tipo específico
- `getRemetente()`: Obtém ID do nó que enviou a tabela
- `find(destino)`: Busca destino na tabela local
- `std::abs()`: Valor absoluto para comparação de custos
- Lógica de atualização: Só atualiza se encontrar rota melhor ou nova

### 7. Método verificarConvergencia()
```cpp
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
            
            // Atualizar o tempo de fim da convergência da rede
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
```

**Explicação**:
- `recordScalar()`: Grava métrica para análise posterior
- Tolerância de 0.001: Evita problemas de precisão de ponto flutuante
- 3 ciclos estáveis: Garante que a convergência é real
- Métricas globais: Acumula dados de todos os nós

### 8. Método finish()
```cpp
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
```

**Explicação**:
- `cancelAndDelete()`: Cancela e libera memória do timer
- Verificação de consistência: Confirma se as rotas são realmente ótimas
- `std::fixed` e `std::setprecision(3)`: Formatação de saída
- Resumo global: Mostrado apenas pelo último nó

---

## Algoritmo de Roteamento

### Pseudocódigo do Algoritmo Distance Vector

```
ALGORITMO Distance Vector para Nó i
INICIALIZAÇÃO:
    tabela[i] = {destino: i, custo: 0, proximo: i}
    para cada vizinho j:
        tabela[j] = {destino: j, custo: custo(i,j), proximo: j}

LOOP PRINCIPAL:
    enquanto não convergiu:
        enviar tabela para todos os vizinhos
        para cada mensagem recebida de vizinho k:
            para cada entrada (destino, custo) na tabela de k:
                novo_custo = custo(i,k) + custo(k,destino)
                se novo_custo < tabela[destino].custo:
                    tabela[destino] = {destino, novo_custo, k}
                    tabela_mudou = true
        
        se tabela não mudou por 3 ciclos consecutivos:
            convergiu = true
```

### Fluxo de Execução

1. **Inicialização** (initialize)
   - Descobre vizinhos
   - Inicializa tabela local
   - Agenda primeiro envio

2. **Loop Principal** (handleMessage)
   - Timer dispara → enviarMinhaTabela()
   - Mensagem recebida → processarTabelaVizinho()
   - Verificar convergência → verificarConvergencia()

3. **Finalização** (finish)
   - Mostra tabela final
   - Verifica consistência
   - Exibe métricas
   - Mostra resumo global

---

## Métricas e Coleta de Dados

### Métricas Individuais
- **Mensagens enviadas**: Contador de mensagens enviadas
- **Mensagens recebidas**: Contador de mensagens recebidas
- **Tempo de convergência**: Tempo individual até convergir

### Métricas Globais
- **Total de nós convergiu**: Quantos nós convergiram
- **Tempo total de convergência**: Tempo até o último nó convergir
- **Total de mensagens na rede**: Soma de todas as mensagens

### Gravação de Métricas
```cpp
recordScalar("nome", valor);  // Grava para arquivo .sca
```

### Verificação de Consistência
- Compara custos calculados com custos diretos
- Verifica se as rotas são realmente ótimas
- Detecta inconsistências no algoritmo

---

## Pseudocódigo Completo

### Estrutura Principal
```
CLASSE NoRoteador:
    // Variáveis de instância
    meuID: inteiro
    tabelaRoteamento: mapa[destino -> InformacaoRota]
    listaVizinhos: vetor[inteiro]
    custoVizinhos: mapa[vizinho -> custo]
    
    // Métricas
    totalMensagensEnviadas: inteiro
    totalMensagensRecebidas: inteiro
    tempoInicioConvergencia: tempo
    tempoFimConvergencia: tempo
    jaConvergiu: booleano
    
    // Detecção de convergência
    tabelaAnterior: mapa[destino -> InformacaoRota]
    ciclosEstaveis: inteiro
    
    // Timer
    temporizadorEnvio: mensagem
    
    // Métodos principais
    initialize()
    handleMessage(mensagem)
    finish()
    
    // Métodos auxiliares
    descobrirVizinhos()
    enviarMinhaTabela()
    processarTabelaVizinho(mensagem)
    verificarConvergencia()
    mostrarTabelaRoteamento()
    mostrarResumoGlobal()
```

### Algoritmo de Descoberta de Vizinhos
```
PROCEDIMENTO descobrirVizinhos():
    numConexoes = tamanho_portas("porta")
    para i de 0 até numConexoes-1:
        minhaPorta = obter_porta("porta$o", i)
        se minhaPorta == null: continuar
        
        portaVizinho = minhaPorta.proximo_gate()
        se portaVizinho == null: continuar
        
        vizinho = portaVizinho.modulo_proprietario()
        se vizinho == null: continuar
        
        idVizinho = vizinho.parametro("meuNumero")
        canal = minhaPorta.canal()
        custoComunicacao = canal.parametro("delay")
        
        custoVizinhos[idVizinho] = custoComunicacao
        listaVizinhos.adicionar(idVizinho)
```

### Algoritmo de Envio de Tabela
```
PROCEDIMENTO enviarMinhaTabela():
    destinos = []
    custos = []
    
    para cada entrada em tabelaRoteamento:
        destinos.adicionar(entrada.destino)
        custos.adicionar(entrada.custo)
    
    para cada vizinho em listaVizinhos:
        mensagem = nova TabelaRoteamento()
        mensagem.remetente = meuID
        mensagem.destinos = destinos
        mensagem.custos = custos
        
        porta = encontrar_porta_para_vizinho(vizinho)
        enviar(mensagem.duplicar(), porta)
        totalMensagensEnviadas++
        
        deletar mensagem
```

### Algoritmo de Processamento de Tabela
```
PROCEDIMENTO processarTabelaVizinho(mensagem):
    vizinhoQueEnviou = mensagem.remetente
    totalMensagensRecebidas++
    custoParaVizinho = custoVizinhos[vizinhoQueEnviou]
    tabelaMudou = false
    
    para i de 0 até mensagem.num_destinos-1:
        destino = mensagem.destinos[i]
        custoVizinhoAteDestino = mensagem.custos[i]
        custoTotal = custoParaVizinho + custoVizinhoAteDestino
        
        se destino não existe em tabelaRoteamento:
            // Novo destino
            novaRota = {custo: custoTotal, proximo: vizinhoQueEnviou}
            tabelaRoteamento[destino] = novaRota
            tabelaMudou = true
        senão se custoTotal < tabelaRoteamento[destino].custo:
            // Caminho melhor
            tabelaRoteamento[destino].custo = custoTotal
            tabelaRoteamento[destino].proximo = vizinhoQueEnviou
            tabelaMudou = true
    
    se tabelaMudou:
        mostrarTabelaRoteamento()
    senão:
        verificarConvergencia()
    
    deletar mensagem
```

### Algoritmo de Verificação de Convergência
```
PROCEDIMENTO verificarConvergencia():
    tabelaEstavel = true
    
    se tabelaAnterior não está vazia:
        para cada entrada em tabelaRoteamento:
            destino = entrada.destino
            entradaAnterior = tabelaAnterior[destino]
            
            se entradaAnterior não existe OU
               |entrada.custo - entradaAnterior.custo| > 0.001:
                tabelaEstavel = false
                parar
    
    tabelaAnterior = copiar(tabelaRoteamento)
    
    se tabelaEstavel:
        ciclosEstaveis++
        se ciclosEstaveis >= 3 E não jaConvergiu:
            jaConvergiu = true
            tempoFimConvergencia = tempo_atual()
            
            // Gravar métricas
            gravar_escalar("tempoConvergencia", tempoFimConvergencia - tempoInicioConvergencia)
            gravar_escalar("mensagensEnviadas", totalMensagensEnviadas)
            gravar_escalar("mensagensRecebidas", totalMensagensRecebidas)
            
            // Atualizar métricas globais
            totalNosConvergiu++
            totalMensagensEnviadasRede += totalMensagensEnviadas
            totalMensagensRecebidasRede += totalMensagensRecebidas
            
            tempoAtual = tempo_atual()
            se tempoAtual > tempoFimConvergenciaRede:
                tempoFimConvergenciaRede = tempoAtual
            
            se totalNosConvergiu >= totalNosRede E não redeConvergiu:
                redeConvergiu = true
                tempoTotalRede = tempoFimConvergenciaRede - tempoInicioRede
                totalMensagensRede = totalMensagensEnviadasRede + totalMensagensRecebidasRede
                
                gravar_escalar("tempoConvergenciaRede", tempoTotalRede)
                gravar_escalar("totalMensagensRede", totalMensagensRede)
                gravar_escalar("mensagensEnviadasRede", totalMensagensEnviadasRede)
                gravar_escalar("mensagensRecebidasRede", totalMensagensRecebidasRede)
                gravar_escalar("nosConvergiu", totalNosConvergiu)
    senão:
        ciclosEstaveis = 0
```

### Algoritmo de Verificação de Consistência
```
PROCEDIMENTO verificarConsistencia():
    tabelaConsistente = true
    
    para cada entrada em tabelaRoteamento:
        destino = entrada.destino
        custoCalculado = entrada.custo
        proximoVizinho = entrada.proximo
        
        se proximoVizinho existe em custoVizinhos:
            custoDireto = custoVizinhos[proximoVizinho]
            
            se destino == proximoVizinho:
                se |custoCalculado - custoDireto| > 0.001:
                    tabelaConsistente = false
                    mostrar "Inconsistência detectada"
                senão:
                    mostrar "Rota direta consistente"
            senão:
                mostrar "Rota via " + proximoVizinho
    
    se tabelaConsistente:
        mostrar "Tabela CONSISTENTE"
    senão:
        mostrar "Tabela INCONSISTENTE"
```

---

## Comandos OMNeT++ Utilizados

### Sistema de Módulos
- `cSimpleModule`: Classe base para módulos simples
- `Define_Module(NoRoteador)`: Define o módulo para OMNeT++
- `getParentModule()`: Obtém módulo pai
- `getSubmoduleVectorSize()`: Conta submodules

### Sistema de Portas e Gates
- `gateSize("porta")`: Conta portas
- `gate("porta$o", i)`: Obtém porta de saída
- `getNextGate()`: Obtém porta conectada
- `getOwnerModule()`: Obtém módulo proprietário

### Sistema de Canais
- `getChannel()`: Obtém canal
- `hasPar("delay")`: Verifica parâmetro
- `par("delay").doubleValue()`: Lê valor do parâmetro

### Sistema de Mensagens
- `cMessage`: Classe base para mensagens
- `dynamic_cast<>()`: Conversão de tipo
- `check_and_cast<>()`: Conversão segura
- `send()`: Envia mensagem
- `scheduleAt()`: Agenda evento

### Sistema de Tempo
- `simTime()`: Tempo atual da simulação
- `simtime_t`: Tipo de tempo do OMNeT++

### Sistema de Logging
- `EV <<`: Stream de logging
- `recordScalar()`: Grava métrica

### Sistema de Parâmetros
- `par("nome")`: Lê parâmetro do NED
- `.intValue()`: Converte para inteiro
- `.doubleValue()`: Converte para double

---

## Conclusão

Este sistema implementa um algoritmo de roteamento distribuído completo, com:

1. **Descoberta automática de vizinhos**
2. **Troca de informações de roteamento**
3. **Detecção de convergência baseada em estabilidade**
4. **Coleta abrangente de métricas**
5. **Verificação de consistência**
6. **Logging detalhado**

O código é robusto, eficiente e adequado para análise de performance em diferentes topologias de rede.
