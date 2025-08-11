// Topologia 2: Anel com 7 Nós
// Forma: 0 - 1 - 2 - 3 - 4 - 5 - 6 - 0 (volta ao início)
// Cada nó tem exatamente 2 vizinhos

network TopologiaAnel
{
    submodules:
        // Criar 7 nós em anel
        no[7]: NoRoteador { 
            meuNumero = index;
        }
    
    connections:
        // Formar um anel completo
        no[0].porta++ <--> ned.DelayChannel { delay = 0.01s; } <--> no[1].porta++;
        no[1].porta++ <--> ned.DelayChannel { delay = 0.02s; } <--> no[2].porta++;
        no[2].porta++ <--> ned.DelayChannel { delay = 0.015s; } <--> no[3].porta++;
        no[3].porta++ <--> ned.DelayChannel { delay = 0.025s; } <--> no[4].porta++;
        no[4].porta++ <--> ned.DelayChannel { delay = 0.01s; } <--> no[5].porta++;
        no[5].porta++ <--> ned.DelayChannel { delay = 0.03s; } <--> no[6].porta++;
        no[6].porta++ <--> ned.DelayChannel { delay = 0.02s; } <--> no[0].porta++;  // Fecha o anel
}
