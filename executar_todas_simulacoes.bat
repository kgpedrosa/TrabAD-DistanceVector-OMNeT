@echo off
REM Script para executar todas as 5 topologias e coletar resultados
REM Execute este arquivo após compilar o projeto

echo 🚀 Iniciando simulações de todas as topologias...
echo.

echo 📊 Simulation 1: Line Topology
TrabAD.exe -u Cmdenv -c Linha -f omnetpp.ini > resultados_linha.txt
echo ✅ Line completed

echo 📊 Simulation 2: Ring Topology  
TrabAD.exe -u Cmdenv -c Anel -f omnetpp.ini > resultados_anel.txt
echo ✅ Ring completed

echo 📊 Simulation 3: Star Topology
TrabAD.exe -u Cmdenv -c Estrela -f omnetpp.ini > resultados_estrela.txt
echo ✅ Star completed

echo 📊 Simulation 4: Mesh Topology
TrabAD.exe -u Cmdenv -c Malha -f omnetpp.ini > resultados_malha.txt
echo ✅ Mesh completed

echo 📊 Simulation 5: Irregular Topology
TrabAD.exe -u Cmdenv -c Irregular -f omnetpp.ini > resultados_irregular.txt
echo ✅ Irregular completed

echo.
echo 🎉 All simulations completed!
echo 📁 Results saved in: resultados_*.txt
echo 📊 Metrics saved in: results/*.sca
echo.
pause