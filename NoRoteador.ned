// Definição do nó para Roteamento

simple NoRoteador
{
    parameters:
        int meuNumero;                    // Número único que identifica este nó (como um RG)
        
        @display("i=abstract/router");
    gates:
        inout porta[];                    // Portas para conectar com outros nós (como tomadas)
}
