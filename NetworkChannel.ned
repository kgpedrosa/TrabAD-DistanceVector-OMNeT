// Canal de Comunicação Simples
// É como um "fio" que conecta dois nós, com um tempo de demora para a mensagem chegar

channel NetworkChannel extends ned.DelayChannel
{
    // O parâmetro 'delay' já existe no DelayChannel
    // Ele representa quanto tempo demora para uma mensagem atravessar este "fio"
}
