#ifndef ESTRUTURAS_H
#define ESTRUTURAS_H

// Estrutura para armazenar informações de roteamento para cada destino
struct InformacaoRota {
    double custo;           // Quanto "custa" chegar ao destino (em segundos)
    int proximoVizinho;     // Para qual vizinho devo enviar o pacote
};

#endif
