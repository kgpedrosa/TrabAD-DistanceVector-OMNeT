@echo off
REM Script para executar todas as 5 topologias e coletar resultados
REM Execute este arquivo após compilar o projeto

echo 🚀 Iniciando simulações de todas as topologias...
echo.

echo 📊 Simulação 1: Topologia em Linha
TrabAD.exe -u Cmdenv -c Linha -f omnetpp.ini > resultados_linha.txt
echo ✅ Linha concluída

echo 📊 Simulação 2: Topologia em Anel  
TrabAD.exe -u Cmdenv -c Anel -f omnetpp.ini > resultados_anel.txt
echo ✅ Anel concluído

echo 📊 Simulação 3: Topologia em Estrela
TrabAD.exe -u Cmdenv -c Estrela -f omnetpp.ini > resultados_estrela.txt
echo ✅ Estrela concluída

echo 📊 Simulação 4: Topologia em Malha
TrabAD.exe -u Cmdenv -c Malha -f omnetpp.ini > resultados_malha.txt
echo ✅ Malha concluída

echo 📊 Simulação 5: Topologia Irregular
TrabAD.exe -u Cmdenv -c Irregular -f omnetpp.ini > resultados_irregular.txt
echo ✅ Irregular concluída

echo.
echo 🎉 Todas as simulações concluídas!
echo 📁 Resultados salvos em: resultados_*.txt
echo 📊 Métricas salvas em: results/*.sca
echo.
pause