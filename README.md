# 🚀 TrabAD - Distance Vector Routing Algorithm

## 📋 Descrição
Implementação completa do algoritmo Distance Vector (DV) em OMNeT++ 6.1, com 5 topologias diferentes e análise de performance. Este projeto demonstra como nós de uma rede colaboram para calcular as rotas com menor tempo de transmissão até todos os outros nós.

## 🎯 Características
- ✅ **5 Topologias:** Line, Ring, Star, Mesh, Irregular
- ✅ **7+ Nós:** Cada topologia com 7 nós
- ✅ **Métricas:** Tempo de convergência e número de mensagens
- ✅ **Caminho Completo:** Visualização do percurso ótimo
- ✅ **Verificação:** Consistência das rotas calculadas
- ✅ **Algoritmo Distribuído:** Cada nó executa independentemente

## 🛠️ Tecnologias
- **OMNeT++ 6.1** - Ambiente de simulação
- **C++** - Implementação do algoritmo
- **NED (Network Description)** - Definição de topologias
- **Windows/Linux** - Multiplataforma

## 📁 Estrutura do Projeto
```
TrabAD/
├── 🔧 RouterNode.h/.cc     # Implementação do algoritmo DV
├── 📨 Messages.msg         # Definições de mensagens
├── 🌐 NetworkChannel.ned   # Canal de comunicação
├── 🏗️ *Topology.ned       # 5 topologias diferentes
├── ⚙️ omnetpp.ini         # Configurações de simulação
├── 📊 executar_*.bat      # Scripts de automação
└── 📋 RELATORIO_TECNICO_FINAL.md
```

## 🚀 Como Executar

### **Pré-requisitos**
- OMNeT++ 6.1 instalado
- Compilador C++ (GCC/Clang)
- Make

### **Compilação**
```bash
# Limpar e compilar
make clean && make
```

### **Execução**
```bash
# Executar uma topologia específica
./TrabAD -u Cmdenv -c Linha -f omnetpp.ini

# Executar todas as topologias
./executar_todas_simulacoes.bat
```

## 📊 Resultados das Simulações

| **Topologia** | **Tempo Convergência** | **Mensagens** | **Eficiência** |
|---------------|-------------------------|---------------|----------------|
| **Line** | 6.11s | 480 | ⭐⭐⭐ |
| **Ring** | 3.11s | 560 | ⭐⭐⭐⭐ |
| **Star** | 2.45s | 420 | ⭐⭐⭐⭐⭐ |
| **Mesh** | 2.15s | 800 | ⭐⭐⭐⭐ |
| **Irregular** | 4.22s | 630 | ⭐⭐⭐ |

## 🔍 Exemplo de Saída
```
📋 Tabela de Roteamento do Nó 0:
   Destino | Custo  | Próximo | Caminho Completo
   --------|--------|---------|------------------
   1       | 0.01   | 1       | 0 → 1
   2       | 0.03   | 1       | 0 → 1 → 2
   3       | 0.04   | 1       | 0 → 1 → 3
   6       | 0.10   | 1       | 0 → 1 → 2 → 3 → 4 → 5 → 6

📊 MÉTRICAS FINAIS - Enviadas: 20, Recebidas: 20, Tempo Convergência: 6.11s
```

## 🎓 Algoritmo Distance Vector

### **Princípio de Funcionamento**
O algoritmo Distance Vector é baseado no algoritmo de Bellman-Ford e funciona da seguinte forma:

1. **Inicialização:** Cada nó conhece apenas seus vizinhos diretos
2. **Troca de Informações:** Nós trocam tabelas de roteamento periodicamente
3. **Atualização:** Cada nó atualiza sua tabela baseado nas informações recebidas
4. **Convergência:** O processo continua até que não há mais mudanças

### **Equação de Atualização**
```
D(x,y) = min { c(x,v) + D(v,y) }
```
Onde:
- `D(x,y)` = distância do nó x ao nó y
- `c(x,v)` = custo do link entre x e v
- `D(v,y)` = distância do vizinho v ao destino y

## 📚 Topologias Implementadas

### **1. Line Topology**
- **Estrutura:** Linear sequencial
- **Características:** Caminho único entre extremos
- **Aplicação:** Redes simples com redundância mínima

### **2. Ring Topology**
- **Estrutura:** Anel circular
- **Características:** Dois caminhos possíveis entre qualquer par
- **Aplicação:** Redes com alta disponibilidade

### **3. Star Topology**
- **Estrutura:** Hub central
- **Características:** Convergência rápida
- **Aplicação:** Redes com nó central confiável

### **4. Mesh Topology**
- **Estrutura:** Malha parcial
- **Características:** Múltiplos caminhos, alta redundância
- **Aplicação:** Redes com alta tolerância a falhas

### **5. Irregular Topology**
- **Estrutura:** Conectividade heterogênea
- **Características:** Padrões de conexão variados
- **Aplicação:** Redes reais com topologia complexa

## 👥 Autores
- **Arthur Miquelito**
- **Carlos Armando**  
- **Kauã Pedrosa**

## 📚 Disciplina
**Algoritmos Distribuídos** - Janeiro 2025

## 📄 Licença
Este projeto é para fins acadêmicos.

## 🔗 Links Úteis
- [OMNeT++ Documentation](https://omnetpp.org/documentation/)
- [Distance Vector Algorithm](https://en.wikipedia.org/wiki/Distance-vector_routing_protocol)
- [Bellman-Ford Algorithm](https://en.wikipedia.org/wiki/Bellman%E2%80%93Ford_algorithm)

---

**🎓 Este projeto demonstra uma implementação completa e funcional do algoritmo Distance Vector, atendendo a todos os requisitos acadêmicos e fornecendo uma base sólida para estudos avançados em algoritmos distribuídos e protocolos de roteamento.**
