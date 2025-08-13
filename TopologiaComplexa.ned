// Topologia Complexa: Estrela + Anel + Cruzamentos
// Forma: Nó 0 como centro da estrela, conectado a todos os outros
// Anéis internos entre nós 1-4 e 5-9, com conexões cruzadas
// Total: 10 nós com topologia híbrida complexa

network TopologiaComplexa
{
    submodules:
        // Criar 10 nós
        no[10]: NoRoteador { 
            meuNumero = index; 
        }
        

    
    connections:
        // Conexões em estrela a partir do centro (nó 0)
        no[0].porta++ <--> ned.DelayChannel { delay = 0.010s; } <--> no[1].porta++;
        no[0].porta++ <--> ned.DelayChannel { delay = 0.015s; } <--> no[2].porta++;
        no[0].porta++ <--> ned.DelayChannel { delay = 0.020s; } <--> no[3].porta++;
        no[0].porta++ <--> ned.DelayChannel { delay = 0.025s; } <--> no[4].porta++;
        no[0].porta++ <--> ned.DelayChannel { delay = 0.030s; } <--> no[5].porta++;
        no[0].porta++ <--> ned.DelayChannel { delay = 0.035s; } <--> no[6].porta++;
        no[0].porta++ <--> ned.DelayChannel { delay = 0.040s; } <--> no[7].porta++;
        no[0].porta++ <--> ned.DelayChannel { delay = 0.045s; } <--> no[8].porta++;
        no[0].porta++ <--> ned.DelayChannel { delay = 0.050s; } <--> no[9].porta++;
        
        // Primeiro anel: 1 -> 3 -> 5 -> 9 -> 7 -> 1
        no[1].porta++ <--> ned.DelayChannel { delay = 0.055s; } <--> no[3].porta++;
        no[3].porta++ <--> ned.DelayChannel { delay = 0.060s; } <--> no[5].porta++;
        no[5].porta++ <--> ned.DelayChannel { delay = 0.065s; } <--> no[9].porta++;
        no[9].porta++ <--> ned.DelayChannel { delay = 0.070s; } <--> no[7].porta++;
        no[7].porta++ <--> ned.DelayChannel { delay = 0.075s; } <--> no[1].porta++;
        
        // Segundo anel: 2 -> 4 -> 6 -> 9 -> 8 -> 2
        no[2].porta++ <--> ned.DelayChannel { delay = 0.080s; } <--> no[4].porta++;
        no[4].porta++ <--> ned.DelayChannel { delay = 0.085s; } <--> no[6].porta++;
        no[6].porta++ <--> ned.DelayChannel { delay = 0.090s; } <--> no[9].porta++;
        no[9].porta++ <--> ned.DelayChannel { delay = 0.095s; } <--> no[8].porta++;
        no[8].porta++ <--> ned.DelayChannel { delay = 0.100s; } <--> no[2].porta++;
        
        // Conexões cruzadas
        no[1].porta++ <--> ned.DelayChannel { delay = 0.105s; } <--> no[2].porta++;
        no[3].porta++ <--> ned.DelayChannel { delay = 0.110s; } <--> no[4].porta++;
        no[5].porta++ <--> ned.DelayChannel { delay = 0.115s; } <--> no[6].porta++;
}
