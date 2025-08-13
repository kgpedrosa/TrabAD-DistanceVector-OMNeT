// Mensagem contendo a tabela de roteamento de um nó

packet TabelaRoteamento

{
    int remetente;          // Quem está enviando esta tabela
    int destinos[];         // Lista de lugares que eu sei chegar
    double custos[];        // Quanto custa chegar em cada lugar
}
