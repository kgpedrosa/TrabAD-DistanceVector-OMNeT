// Topologia 1: Linha com 7 Nós
// Forma: 0 - 1 - 2 - 3 - 4 - 5 - 6
// Esta é a topologia mais simples: uma linha reta

network TopologiaLinha
{
    submodules:
        no[7]: NoRoteador { 
            meuNumero = index;  
        }
        

    
    connections:
        // Conectar em linha com tempos variados
        no[0].porta++ <--> ned.DelayChannel { delay = 0.01s; } <--> no[1].porta++;
        no[1].porta++ <--> ned.DelayChannel { delay = 0.02s; } <--> no[2].porta++;
        no[2].porta++ <--> ned.DelayChannel { delay = 0.01s; } <--> no[3].porta++;
        no[3].porta++ <--> ned.DelayChannel { delay = 0.03s; } <--> no[4].porta++;
        no[4].porta++ <--> ned.DelayChannel { delay = 0.01s; } <--> no[5].porta++;
        no[5].porta++ <--> ned.DelayChannel { delay = 0.02s; } <--> no[6].porta++;
}
