// Topologia 5: Irregular Complexa com 7 Nós
// Forma: Conexões irregulares com gargalos e caminhos alternativos
// A mais complexa de todas - simula rede real

network TopologiaIrregular
{
    submodules:
        // Criar 7 nós com topologia irregular complexa
        no[7]: NoRoteador { 
            meuNumero = index;
        }
        

    
    connections:
        // Grupo 1: Nós 0,1,2 bem conectados
        no[0].porta++ <--> ned.DelayChannel { delay = 0.01s; } <--> no[1].porta++;
        no[1].porta++ <--> ned.DelayChannel { delay = 0.015s; } <--> no[2].porta++;
        no[0].porta++ <--> ned.DelayChannel { delay = 0.02s; } <--> no[2].porta++;
        
        // Gargalo: apenas nó 2 conecta aos outros grupos
        no[2].porta++ <--> ned.DelayChannel { delay = 0.05s; } <--> no[3].porta++;  // Gargalo lento!
        
        // Grupo 2: Nós 3,4,5 formam triângulo
        no[3].porta++ <--> ned.DelayChannel { delay = 0.01s; } <--> no[4].porta++;
        no[4].porta++ <--> ned.DelayChannel { delay = 0.015s; } <--> no[5].porta++;
        no[5].porta++ <--> ned.DelayChannel { delay = 0.012s; } <--> no[3].porta++;
        
        // Nó 6 isolado: apenas conectado ao nó 5
        no[5].porta++ <--> ned.DelayChannel { delay = 0.025s; } <--> no[6].porta++;
        
        // Atalho de emergência: conecta os dois grupos diretamente
        no[1].porta++ <--> ned.DelayChannel { delay = 0.08s; } <--> no[4].porta++;  // Atalho muito lento
}
