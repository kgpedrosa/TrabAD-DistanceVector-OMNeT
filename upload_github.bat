@echo off
echo 🚀 SCRIPT DE UPLOAD PARA GITHUB - TrabAD Project
echo ================================================
echo.

echo 📋 Verificando se Git está instalado...
git --version >nul 2>&1
if errorlevel 1 (
    echo ❌ Git não está instalado! 
    echo 📥 Baixe em: https://git-scm.com/download/win
    pause
    exit /b 1
)
echo ✅ Git encontrado!

echo.
echo 🔧 Configurando Git...
echo ⚠️  IMPORTANTE: Configure seu nome e email se ainda não fez:
echo    git config --global user.name "Seu Nome"
echo    git config --global user.email "seu.email@exemplo.com"
echo.

echo 📁 Inicializando repositório Git...
git init

echo 📦 Adicionando arquivos ao Git...
git add .

echo 💾 Fazendo primeiro commit...
git commit -m "🎉 Initial commit: Distance Vector implementation with 5 topologies

- Implementação completa do algoritmo Distance Vector
- 5 topologias diferentes (Line, Ring, Star, Mesh, Irregular)
- Sistema de métricas e verificação de consistência
- Scripts de automação para execução
- Relatório técnico completo"

echo.
echo 🔗 CONFIGURAÇÃO DO REPOSITÓRIO REMOTO
echo =====================================
echo.
echo ⚠️  ANTES DE CONTINUAR:
echo 1. Crie um repositório no GitHub chamado: TrabAD-DistanceVector-OMNeT
echo 2. NÃO marque "Add README" ou "Add .gitignore"
echo 3. Copie a URL do repositório
echo.
echo 📝 Digite a URL do seu repositório (ex: https://github.com/SEU_USUARIO/TrabAD-DistanceVector-OMNeT.git):
set /p repo_url="URL: "

echo.
echo 🔗 Adicionando repositório remoto...
git remote add origin %repo_url%

echo 📤 Fazendo push para o GitHub...
echo ⚠️  Se pedir autenticação:
echo    - Username: seu usuário do GitHub
echo    - Password: use um Personal Access Token (não sua senha)
echo.
git push -u origin main

echo.
echo 🎉 UPLOAD CONCLUÍDO!
echo ===================
echo.
echo ✅ Seu projeto está disponível em: %repo_url%
echo.
echo 📋 PRÓXIMOS PASSOS:
echo 1. Verifique se todos os arquivos foram enviados
echo 2. Adicione colaboradores (se necessário)
echo 3. Configure GitHub Pages (opcional)
echo 4. Compartilhe o link do repositório
echo.
echo 💡 Dica: Para futuras atualizações, use:
echo    git add .
echo    git commit -m "Sua mensagem"
echo    git push
echo.
pause
