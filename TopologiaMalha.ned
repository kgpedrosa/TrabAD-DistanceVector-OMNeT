// Topologia 4: Malha Parcial com 7 Nós
// Forma: Linha principal com alguns atalhos estratégicos
// Mais complexa que linha, menos que malha completa

network TopologiaMalha
{
    submodules:
        // Criar 7 nós com conexões em malha parcial
        no[7]: NoRoteador { 
            meuNumero = index;
        }
        
    connections:
        // Linha principal
        no[0].porta++ <--> ned.DelayChannel { delay = 0.01s; } <--> no[1].porta++;
        no[1].porta++ <--> ned.DelayChannel { delay = 0.015s; } <--> no[2].porta++;
        no[2].porta++ <--> ned.DelayChannel { delay = 0.02s; } <--> no[3].porta++;
        no[3].porta++ <--> ned.DelayChannel { delay = 0.018s; } <--> no[4].porta++;
        no[4].porta++ <--> ned.DelayChannel { delay = 0.022s; } <--> no[5].porta++;
        no[5].porta++ <--> ned.DelayChannel { delay = 0.016s; } <--> no[6].porta++;
        
        // Atalhos estratégicos
        no[0].porta++ <--> ned.DelayChannel { delay = 0.05s; } <--> no[3].porta++;   // Atalho longo
        no[1].porta++ <--> ned.DelayChannel { delay = 0.04s; } <--> no[4].porta++;   // Atalho médio
        no[2].porta++ <--> ned.DelayChannel { delay = 0.012s; } <--> no[5].porta++;  // Atalho rápido
        no[3].porta++ <--> ned.DelayChannel { delay = 0.03s; } <--> no[6].porta++;   // Atalho final
}
