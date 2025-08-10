//
// Mensagens Simples para o Projeto de Roteamento
// Estas são as "cartas" que os nós trocam entre si
//

// Esta mensagem contém a tabela de roteamento de um nó
// É como se fosse uma "carta" dizendo: "Ei vizinho, eu sei chegar nestes lugares!"
packet TabelaRoteamento
{
    int remetente;          // Quem está enviando esta tabela
    int destinos[];         // Lista de lugares que eu sei chegar
    double custos[];        // Quanto custa chegar em cada lugar
}
