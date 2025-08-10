# 🚀 GUIA COMPLETO: Como Fazer Upload do Projeto para o GitHub

---

## 📋 **PRÉ-REQUISITOS**

### **1. Conta no GitHub**
- Acesse [github.com](https://github.com) e crie uma conta (se ainda não tiver)
- Confirme seu email

### **2. Git Instalado**
- **Windows:** Baixe em [git-scm.com](https://git-scm.com/download/win)
- **Verificar se está instalado:** Abra o terminal e digite `git --version`

### **3. Configuração Inicial do Git**
```bash
# Configurar seu nome e email
git config --global user.name "Seu Nome"
git config --global user.email "seu.email@exemplo.com"

# Verificar configuração
git config --list
```

---

## 🔧 **PASSO A PASSO - UPLOAD PARA GITHUB**

### **Passo 1: Criar Repositório no GitHub**

1. **Acesse o GitHub** e faça login
2. **Clique no botão "+"** no canto superior direito
3. **Selecione "New repository"**
4. **Preencha as informações:**
   - **Repository name:** `TrabAD-DistanceVector-OMNeT`
   - **Description:** `Implementação do algoritmo Distance Vector em OMNeT++ com 5 topologias diferentes`
   - **Visibility:** Public (ou Private se preferir)
   - **NÃO marque** "Add a README file" (vamos criar um personalizado)
   - **NÃO marque** "Add .gitignore" (vamos criar um personalizado)
5. **Clique em "Create repository"**

### **Passo 2: Preparar o Projeto Local**

#### **2.1 Criar .gitignore**
Crie um arquivo `.gitignore` na raiz do projeto:

```gitignore
# OMNeT++ specific
out/
results/
*.vec
*.sca
*.elog
*.vci
*.anf
*.tkenvrc
*.nedb
*.nedbak
*.bak
*.tmp

# Build files
*.o
*.exe
*.dll
*.so
*.dylib
*.a
*.lib

# IDE files
.vscode/
.idea/
*.swp
*.swo
*~

# OS files
.DS_Store
Thumbs.db
desktop.ini

# Logs
*.log
resultados_*.txt

# Temporary files
*.tmp
*.temp
```

#### **2.2 Criar README.md**
Crie um arquivo `README.md` na raiz do projeto:

```markdown
# 🚀 TrabAD - Distance Vector Routing Algorithm

## 📋 Descrição
Implementação completa do algoritmo Distance Vector (DV) em OMNeT++ 6.1, com 5 topologias diferentes e análise de performance.

## 🎯 Características
- ✅ **5 Topologias:** Line, Ring, Star, Mesh, Irregular
- ✅ **7+ Nós:** Cada topologia com 7 nós
- ✅ **Métricas:** Tempo de convergência e número de mensagens
- ✅ **Caminho Completo:** Visualização do percurso ótimo
- ✅ **Verificação:** Consistência das rotas calculadas

## 🛠️ Tecnologias
- **OMNeT++ 6.1**
- **C++**
- **NED (Network Description)**
- **Windows/Linux**

## 📁 Estrutura do Projeto
```
TrabAD/
├── 🔧 RouterNode.h/.cc     # Implementação do algoritmo DV
├── 📨 Messages.msg         # Definições de mensagens
├── 🌐 NetworkChannel.ned   # Canal de comunicação
├── 🏗️ *Topology.ned       # 5 topologias diferentes
├── ⚙️ omnetpp.ini         # Configurações de simulação
├── 📊 executar_*.bat      # Scripts de automação
└── 📋 RELATORIO_TECNICO_FINAL.md
```

## 🚀 Como Executar

### **Pré-requisitos**
- OMNeT++ 6.1 instalado
- Compilador C++ (GCC/Clang)
- Make

### **Compilação**
```bash
# Limpar e compilar
make clean && make
```

### **Execução**
```bash
# Executar uma topologia específica
./TrabAD -u Cmdenv -c Linha -f omnetpp.ini

# Executar todas as topologias
./executar_todas_simulacoes.bat
```

## 📊 Resultados
- **Line Topology:** 6.11s convergência
- **Ring Topology:** 3.11s convergência  
- **Star Topology:** 2.45s convergência
- **Mesh Topology:** 2.15s convergência
- **Irregular Topology:** 4.22s convergência

## 👥 Autores
- Arthur Miquelito
- Carlos Armando  
- Kauã Pedrosa

## 📚 Disciplina
Algoritmos Distribuídos - Janeiro 2025

## 📄 Licença
Este projeto é para fins acadêmicos.
```

### **Passo 3: Inicializar Git Local**

Abra o terminal na pasta do projeto (`C:\Users\silva\Downloads\omnetpp-6.1\samples\TrabAD`) e execute:

```bash
# Inicializar repositório Git
git init

# Adicionar todos os arquivos
git add .

# Fazer o primeiro commit
git commit -m "🎉 Initial commit: Distance Vector implementation with 5 topologies

- Implementação completa do algoritmo Distance Vector
- 5 topologias diferentes (Line, Ring, Star, Mesh, Irregular)
- Sistema de métricas e verificação de consistência
- Scripts de automação para execução
- Relatório técnico completo"

# Verificar status
git status
```

### **Passo 4: Conectar ao GitHub**

```bash
# Adicionar o repositório remoto (substitua pela URL do seu repositório)
git remote add origin https://github.com/SEU_USUARIO/TrabAD-DistanceVector-OMNeT.git

# Verificar se foi adicionado
git remote -v

# Fazer push para o GitHub
git push -u origin main
```

**⚠️ IMPORTANTE:** Substitua `SEU_USUARIO` pelo seu nome de usuário do GitHub.

---

## 🔐 **AUTENTICAÇÃO NO GITHUB**

### **Opção 1: Personal Access Token (Recomendado)**

1. **No GitHub:** Settings → Developer settings → Personal access tokens → Tokens (classic)
2. **Generate new token** → **Generate new token (classic)**
3. **Configure:**
   - **Note:** `TrabAD Project Access`
   - **Expiration:** 90 days
   - **Scopes:** Marque `repo` (todos os subitens)
4. **Generate token** e **copie o token**
5. **No terminal:**
   ```bash
   # Quando pedir username: seu usuário do GitHub
   # Quando pedir password: cole o token (não sua senha)
   ```

### **Opção 2: GitHub CLI**
```bash
# Instalar GitHub CLI
# Windows: winget install GitHub.cli

# Autenticar
gh auth login

# Seguir as instruções na tela
```

---

## 📤 **COMANDOS COMPLETOS PARA UPLOAD**

```bash
# 1. Navegar para a pasta do projeto
cd C:\Users\silva\Downloads\omnetpp-6.1\samples\TrabAD

# 2. Inicializar Git
git init

# 3. Adicionar arquivos
git add .

# 4. Primeiro commit
git commit -m "🎉 Initial commit: Distance Vector implementation"

# 5. Adicionar repositório remoto
git remote add origin https://github.com/SEU_USUARIO/TrabAD-DistanceVector-OMNeT.git

# 6. Fazer push
git push -u origin main
```

---

## 🔄 **ATUALIZAÇÕES FUTURAS**

### **Para fazer alterações e enviar para o GitHub:**

```bash
# 1. Fazer alterações nos arquivos

# 2. Adicionar mudanças
git add .

# 3. Commit com mensagem descritiva
git commit -m "✨ Adicionar nova funcionalidade X"

# 4. Enviar para o GitHub
git push
```

### **Para baixar alterações do GitHub:**

```bash
# Baixar e aplicar mudanças
git pull origin main
```

---

## 🎯 **DICAS IMPORTANTES**

### **✅ Boas Práticas:**
- **Commits frequentes:** Faça commits pequenos e frequentes
- **Mensagens descritivas:** Use mensagens claras que expliquem a mudança
- **Branches:** Use branches para novas funcionalidades
- **README atualizado:** Mantenha a documentação sempre atualizada

### **❌ Evitar:**
- Commits muito grandes
- Mensagens genéricas como "fix" ou "update"
- Enviar arquivos desnecessários (use .gitignore)
- Commits com bugs conhecidos

---

## 🆘 **SOLUÇÃO DE PROBLEMAS**

### **Erro: "fatal: remote origin already exists"**
```bash
# Remover e adicionar novamente
git remote remove origin
git remote add origin https://github.com/SEU_USUARIO/TrabAD-DistanceVector-OMNeT.git
```

### **Erro: "fatal: refusing to merge unrelated histories"**
```bash
# Forçar merge
git pull origin main --allow-unrelated-histories
```

### **Erro de autenticação**
- Verifique se o token está correto
- Tente usar GitHub CLI: `gh auth login`

---

## 🎉 **PRÓXIMOS PASSOS**

1. **✅ Criar repositório no GitHub**
2. **✅ Fazer upload inicial**
3. **📝 Adicionar colaboradores** (se for trabalho em grupo)
4. **🔗 Compartilhar o link** do repositório
5. **📊 Configurar GitHub Pages** (opcional, para documentação)

---

**🚀 Seu projeto estará disponível em:**
`https://github.com/SEU_USUARIO/TrabAD-DistanceVector-OMNeT`

**💡 Dica:** Depois do upload, você pode adicionar um badge no README mostrando o status do build, número de downloads, etc.
