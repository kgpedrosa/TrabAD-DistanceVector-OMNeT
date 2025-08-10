// Definição de um Nó Simples para Roteamento
// Este é um "molde" que define como cada nó da rede funciona

simple RouterNode
{
    parameters:
        int meuNumero;                    // Número único que identifica este nó (como um RG)
        @display("i=device/router");      // Ícone de roteador na interface gráfica
        @class(RouterNode);                // Qual código C++ usar para este nó
    
    gates:
        inout porta[];                    // Portas para conectar com outros nós (como tomadas)
}
