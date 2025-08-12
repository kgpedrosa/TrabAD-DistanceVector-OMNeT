// Topologia 3: Estrela com 7 Nós
// Forma: Nó 0 no centro conectado a todos os outros (1,2,3,4,5,6)
// Nó central tem 6 vizinhos, outros têm apenas 1

network TopologiaEstrela
{
    submodules:
        // Criar 7 nós - nó 0 será o centro
        no[7]: NoRoteador { 
            meuNumero = index;
        }
        

    
    connections:
        // Nó 0 (centro) conectado a todos os outros
        no[0].porta++ <--> ned.DelayChannel { delay = 0.01s; } <--> no[1].porta++;
        no[0].porta++ <--> ned.DelayChannel { delay = 0.015s; } <--> no[2].porta++;
        no[0].porta++ <--> ned.DelayChannel { delay = 0.02s; } <--> no[3].porta++;
        no[0].porta++ <--> ned.DelayChannel { delay = 0.012s; } <--> no[4].porta++;
        no[0].porta++ <--> ned.DelayChannel { delay = 0.025s; } <--> no[5].porta++;
        no[0].porta++ <--> ned.DelayChannel { delay = 0.018s; } <--> no[6].porta++;
}
