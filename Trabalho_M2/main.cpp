// Trabalho de Programacao Orientada a Objetos, M2.
// Sistema de contratacao de servicos (marido de aluguel).
// Integrantes do Grupo: Dhavi Ioshua Grando Da Silva e Felipe Scremin Bochnia.

#include <iostream>
#include <string>
#include <fstream>
#include <cstdio>
using namespace std;

// converte float para string com 2 casas decimais
string f2s(float v) {
    char buf[32];
    snprintf(buf, sizeof(buf), "%.2f", v);
    return string(buf);
}

#define MAX 20         // quantidade maxima de clientes, trabalhadores e contratacoes
#define MAX_HAB 10     // quantidade maxima de habilidades por trabalhador
#define MAX_DATAS 30   // quantidade maxima de datas ocupadas por trabalhador


// interface totalmente abstrata: tudo que pode ser impresso no console
class Imprimivel {
public:
    virtual void imprime() const = 0;
    virtual ~Imprimivel() = default;
};


// armazena telefone e email da pessoa
class Contato : public Imprimivel {

    string telefone;
    string email;

public:
    // construtor completo
    Contato(string telefone, string email) {
        this->telefone = telefone;
        this->email = email;
    }

    // sobrecarga: contato sem email
    Contato(string telefone) {
        this->telefone = telefone;
        this->email = "";
    }

    string getTelefone() const { return telefone; }
    string getEmail() const { return email; }

    void setTelefone(string t) { telefone = t; }
    void setEmail(string e) { email = e; }

    void imprime() const override {
        cout << telefone;
        if (email != "") cout << ", " << email;
    }
};


// controla as datas ocupadas do trabalhador
class Agenda {

    int dia[MAX_DATAS];
    int mes[MAX_DATAS];
    int ano[MAX_DATAS];
    int quantidade;

public:
    Agenda() {
        quantidade = 0; // comeca sem datas ocupadas
    }

    bool estaDisponivel(int d, int m, int a) const {
        for (int i = 0; i < quantidade; i++) {
            if (dia[i] == d && mes[i] == m && ano[i] == a) {
                return false;
            }
        }
        return true;
    }

    bool reservar(int d, int m, int a) {
        if (quantidade < MAX_DATAS && estaDisponivel(d, m, a)) {
            dia[quantidade] = d;
            mes[quantidade] = m;
            ano[quantidade] = a;
            quantidade++;
            return true;
        }
        return false;
    }

    void imprime() const {
        if (quantidade == 0) {
            cout << "Nenhuma data ocupada." << endl;
        } else {
            for (int i = 0; i < quantidade; i++) {
                cout << dia[i] << "/" << mes[i] << "/" << ano[i] << endl;
            }
        }
    }
};


// classe abstrata: base para Cliente e Trabalhador
class Pessoa : public Imprimivel {

    static int totalPessoas; // contador global de pessoas cadastradas

    int id;
    string nome;
    string cpf;
    string dataNascimento;
    Contato contato;

public:
    Pessoa(string nome, string cpf, string dataNascimento, Contato contato)
        : contato(contato) {
        this->id = proximoId();
        this->nome = nome;
        this->cpf = cpf;
        this->dataNascimento = dataNascimento;
    }

    // metodos static
    static int proximoId() { return ++totalPessoas; }
    static int getTotal() { return totalPessoas; }

    int getId() const { return id; }
    string getNome() const { return nome; }
    string getCpf() const { return cpf; }
    string getDataNascimento() const { return dataNascimento; }
    Contato getContato() const { return contato; }

    void setNome(string n) { nome = n; }
    void setCpf(string c) { cpf = c; }
    void setDataNascimento(string d) { dataNascimento = d; }
    void setContato(Contato c) { contato = c; }

    // metodo virtual implementado: imprime dados comuns de Pessoa
    virtual void imprime() const override {
        cout << "#" << id << " " << nome << ", " << cpf << ", " << dataNascimento << ", ";
        contato.imprime();
    }

    // metodo puro: forca Pessoa a ser abstrata
    virtual string tipo() const = 0;
};

int Pessoa::totalPessoas = 0;


// representa quem solicita o servico
class Cliente : public Pessoa {

    string endereco;
    string necessidadeServico;

public:
    Cliente(string nome, string cpf, string dataNascimento, Contato contato,
            string endereco, string necessidadeServico)
        : Pessoa(nome, cpf, dataNascimento, contato) {
        this->endereco = endereco;
        this->necessidadeServico = necessidadeServico;
    }

    string getEndereco() const { return endereco; }
    string getNecessidadeServico() const { return necessidadeServico; }

    void setEndereco(string e) { endereco = e; }
    void setNecessidadeServico(string n) { necessidadeServico = n; }

    string tipo() const override { return "Cliente"; }

    void imprime() const override {
        Pessoa::imprime(); // reaproveita o print da Pessoa
        cout << ", " << endereco << ", " << necessidadeServico << endl;
    }

    // gera card HTML do cliente
    string toHTML() const {
        return string("<div class='card cliente'>")
             + "<h3>" + getNome() + " <small>#" + to_string(getId()) + "</small></h3>"
             + "<p><b>CPF:</b> " + getCpf() + "</p>"
             + "<p><b>Nascimento:</b> " + getDataNascimento() + "</p>"
             + "<p><b>Telefone:</b> " + getContato().getTelefone() + "</p>"
             + "<p><b>Email:</b> " + getContato().getEmail() + "</p>"
             + "<p><b>Endereco:</b> " + endereco + "</p>"
             + "<p><b>Necessidade:</b> " + necessidadeServico + "</p>"
             + "</div>";
    }
};


// representa a habilidade do trabalhador
class Habilidade : public Imprimivel {

    string nome;
    string descricao;

public:
    Habilidade(string nome, string descricao) {
        this->nome = nome;
        this->descricao = descricao;
    }

    string getNome() const { return nome; }
    string getDescricao() const { return descricao; }

    void imprime() const override {
        cout << nome << ", " << descricao;
    }
};


// representa quem executa o servico
class Trabalhador : public Pessoa {

    string profissao;
    float valorHora;
    Habilidade* habilidades[MAX_HAB];
    int qtdHabilidades;
    Agenda agenda;

public:
    Trabalhador(string nome, string cpf, string dataNascimento, Contato contato,
                string profissao, float valorHora)
        : Pessoa(nome, cpf, dataNascimento, contato) {
        this->profissao = profissao;
        this->valorHora = valorHora;
        qtdHabilidades = 0;
    }

    string getProfissao() const { return profissao; }
    float getValorHora() const { return valorHora; }
    int getQtdHabilidades() const { return qtdHabilidades; }
    Habilidade* getHabilidade(int i) const { return habilidades[i]; }

    void setProfissao(string p) { profissao = p; }
    void setValorHora(float v) { valorHora = v; }

    void adicionarHabilidade(Habilidade* habilidade) {
        if (qtdHabilidades < MAX_HAB) {
            habilidades[qtdHabilidades] = habilidade;
            qtdHabilidades++;
        }
    }

    bool contratar(int d, int m, int a) {
        return agenda.reservar(d, m, a);
    }

    bool estaDisponivel(int d, int m, int a) const {
        return agenda.estaDisponivel(d, m, a);
    }

    void imprimeAgenda() const {
        agenda.imprime();
    }

    string tipo() const override { return "Trabalhador"; }

    void imprime() const override {
        Pessoa::imprime();
        cout << ", " << profissao << ", R$" << valorHora << endl;

        cout << "Habilidades: " << endl;
        for (int i = 0; i < qtdHabilidades; i++) {
            cout << "- ";
            habilidades[i]->imprime();
            cout << endl;
        }
    }

    // gera card HTML do trabalhador, com habilidades embutidas inline
    string toHTML() const {
        string s = string("<div class='card trabalhador'>")
                 + "<h3>" + getNome() + " <small>#" + to_string(getId()) + "</small></h3>"
                 + "<p><b>CPF:</b> " + getCpf() + "</p>"
                 + "<p><b>Nascimento:</b> " + getDataNascimento() + "</p>"
                 + "<p><b>Telefone:</b> " + getContato().getTelefone() + "</p>"
                 + "<p><b>Email:</b> " + getContato().getEmail() + "</p>"
                 + "<p><b>Profissao:</b> " + profissao + "</p>"
                 + "<p><b>Valor/hora:</b> R$ " + f2s(valorHora) + "</p>"
                 + "<p><b>Habilidades:</b></p><ul>";
        for (int i = 0; i < qtdHabilidades; i++) {
            s += "<li>" + habilidades[i]->getNome() + " - " + habilidades[i]->getDescricao() + "</li>";
        }
        s += "</ul></div>";
        return s;
    }

    ~Trabalhador() {
        for (int i = 0; i < qtdHabilidades; i++) {
            delete habilidades[i];
        }
    }
};


// armazena os dados da contratacao realizada
class Contratacao : public Imprimivel {

    string nomeCliente;
    string nomeTrabalhador;
    string descricaoServico;
    int dia;
    int mes;
    int ano;
    string status;

public:
    Contratacao(string nomeCliente, string nomeTrabalhador, string descricaoServico,
                int dia, int mes, int ano, string status) {
        this->nomeCliente = nomeCliente;
        this->nomeTrabalhador = nomeTrabalhador;
        this->descricaoServico = descricaoServico;
        this->dia = dia;
        this->mes = mes;
        this->ano = ano;
        this->status = status;
    }

    void imprime() const override {
        cout << "Cliente: " << nomeCliente
             << " | Trabalhador: " << nomeTrabalhador
             << " | Servico: " << descricaoServico
             << " | Data: " << dia << "/" << mes << "/" << ano
             << " | Status: " << status << endl << endl;
    }

    string toHTML() const {
        return string("<div class='card contratacao'>")
             + "<h3>" + descricaoServico + "</h3>"
             + "<p><b>Cliente:</b> " + nomeCliente + "</p>"
             + "<p><b>Trabalhador:</b> " + nomeTrabalhador + "</p>"
             + "<p><b>Data:</b> " + to_string(dia) + "/" + to_string(mes) + "/" + to_string(ano) + "</p>"
             + "<p><b>Status:</b> " + status + "</p>"
             + "</div>";
    }

    // getters para salvar em arquivo
    string getNomeCliente() const     { return nomeCliente; }
    string getNomeTrabalhador() const { return nomeTrabalhador; }
    string getDescricaoServico() const{ return descricaoServico; }
    int getDia() const                { return dia; }
    int getMes() const                { return mes; }
    int getAno() const                { return ano; }
    string getStatus() const          { return status; }
};


// ====================== sobrecarga: pesquisar por nome OU por id ======================

// pesquisa cliente por nome, retorna a posicao ou -1
int pesquisarCliente(Cliente* v[], int qtd, string nome) {
    for (int i = 0; i < qtd; i++) {
        if (v[i]->getNome() == nome) return i;
    }
    return -1;
}

// sobrecarga: mesma funcao, mas pesquisa por id
int pesquisarCliente(Cliente* v[], int qtd, int id) {
    for (int i = 0; i < qtd; i++) {
        if (v[i]->getId() == id) return i;
    }
    return -1;
}

int pesquisarTrabalhador(Trabalhador* v[], int qtd, string nome) {
    for (int i = 0; i < qtd; i++) {
        if (v[i]->getNome() == nome) return i;
    }
    return -1;
}

int pesquisarTrabalhador(Trabalhador* v[], int qtd, int id) {
    for (int i = 0; i < qtd; i++) {
        if (v[i]->getId() == id) return i;
    }
    return -1;
}


// ====================== exportacao para HTML ======================

// escreve o cabecalho HTML comum em todas as paginas
void escreveCabecalho(ofstream& f, string titulo) {
    f << "<!DOCTYPE html>\n<html lang='pt-BR'>\n<head>\n"
      << "<meta charset='UTF-8'>\n"
      << "<title>" << titulo << " - Marido de Aluguel</title>\n"
      << "<link rel='stylesheet' href='style.css'>\n"
      << "</head>\n<body>\n"
      << "<header><h1>Marido de Aluguel</h1>\n"
      << "<nav>\n"
      << "  <a href='index.html'>Inicio</a>\n"
      << "  <a href='clientes.html'>Clientes</a>\n"
      << "  <a href='trabalhadores.html'>Trabalhadores</a>\n"
      << "  <a href='contratacoes.html'>Contratacoes</a>\n"
      << "</nav>\n</header>\n<main>\n"
      << "<h2>" << titulo << "</h2>\n";
}

void escreveRodape(ofstream& f) {
    f << "</main>\n</body>\n</html>\n";
}

void gerarHTML(Cliente* clientes[], int qtdClientes,
               Trabalhador* trabalhadores[], int qtdTrabalhadores,
               Contratacao* contratacoes[], int qtdContratacoes) {

    // index.html
    ofstream fi("web/index.html");
    if (!fi) {
        cout << "Erro ao abrir web/index.html. Verifique se a pasta 'web' existe." << endl;
        return;
    }
    escreveCabecalho(fi, "Inicio");
    fi << "<p>Sistema de contratacao de servicos.</p>\n";
    fi << "<ul>"
       << "<li>Clientes cadastrados: " << qtdClientes << "</li>"
       << "<li>Trabalhadores cadastrados: " << qtdTrabalhadores << "</li>"
       << "<li>Contratacoes realizadas: " << qtdContratacoes << "</li>"
       << "<li>Total de pessoas (static): " << Pessoa::getTotal() << "</li>"
       << "</ul>\n";
    escreveRodape(fi);
    fi.close();

    // clientes.html
    ofstream fc("web/clientes.html");
    escreveCabecalho(fc, "Clientes");
    if (qtdClientes == 0) {
        fc << "<p>Nenhum cliente cadastrado.</p>\n";
    } else {
        for (int i = 0; i < qtdClientes; i++) fc << clientes[i]->toHTML() << "\n";
    }
    escreveRodape(fc);
    fc.close();

    // trabalhadores.html
    ofstream ft("web/trabalhadores.html");
    escreveCabecalho(ft, "Trabalhadores");
    if (qtdTrabalhadores == 0) {
        ft << "<p>Nenhum trabalhador cadastrado.</p>\n";
    } else {
        for (int i = 0; i < qtdTrabalhadores; i++) ft << trabalhadores[i]->toHTML() << "\n";
    }
    escreveRodape(ft);
    ft.close();

    // contratacoes.html
    ofstream fco("web/contratacoes.html");
    escreveCabecalho(fco, "Contratacoes");
    if (qtdContratacoes == 0) {
        fco << "<p>Nenhuma contratacao realizada.</p>\n";
    } else {
        for (int i = 0; i < qtdContratacoes; i++) fco << contratacoes[i]->toHTML() << "\n";
    }
    escreveRodape(fco);
    fco.close();

    cout << "Arquivos HTML gerados em ./web/ (abra web/index.html no navegador)." << endl;
}


// ====================== persistencia em arquivo .txt ======================

// ---------- SALVAR ----------

void salvarClientes(Cliente* v[], int qtd) {
    ofstream f("clientes.txt");
    for (int i = 0; i < qtd; i++) {
        Cliente* c = v[i];
        f << c->getNome()                  << endl;
        f << c->getCpf()                   << endl;
        f << c->getDataNascimento()        << endl;
        f << c->getContato().getTelefone() << endl;
        f << c->getContato().getEmail()    << endl;
        f << c->getEndereco()              << endl;
        f << c->getNecessidadeServico()    << endl;
    }
    f.close();
}

void salvarTrabalhadores(Trabalhador* v[], int qtd) {
    ofstream f("trabalhadores.txt");
    for (int i = 0; i < qtd; i++) {
        Trabalhador* t = v[i];
        f << t->getNome()                  << endl;
        f << t->getCpf()                   << endl;
        f << t->getDataNascimento()        << endl;
        f << t->getContato().getTelefone() << endl;
        f << t->getContato().getEmail()    << endl;
        f << t->getProfissao()             << endl;
        f << t->getValorHora()             << endl;
    }
    f.close();
}

void salvarHabilidades(Trabalhador* v[], int qtd) {
    ofstream f("habilidades.txt");
    for (int i = 0; i < qtd; i++) {
        Trabalhador* t = v[i];
        for (int j = 0; j < t->getQtdHabilidades(); j++) {
            Habilidade* h = t->getHabilidade(j);
            f << t->getNome()      << endl;
            f << h->getNome()      << endl;
            f << h->getDescricao() << endl;
        }
    }
    f.close();
}

void salvarContratacoes(Contratacao* v[], int qtd) {
    ofstream f("contratacoes.txt");
    for (int i = 0; i < qtd; i++) {
        Contratacao* c = v[i];
        f << c->getNomeCliente()      << endl;
        f << c->getNomeTrabalhador()  << endl;
        f << c->getDescricaoServico() << endl;
        f << c->getDia()              << endl;
        f << c->getMes()              << endl;
        f << c->getAno()              << endl;
        f << c->getStatus()           << endl;
    }
    f.close();
}

// ---------- CARREGAR ----------

void carregarClientes(Cliente* v[], int& qtd) {
    ifstream f("clientes.txt");
    if (!f.is_open()) return; // se nao existe, segue vazio
    string nome, cpf, dn, tel, email, end, nec;
    while (getline(f, nome) && qtd < MAX) {
        getline(f, cpf);
        getline(f, dn);
        getline(f, tel);
        getline(f, email);
        getline(f, end);
        getline(f, nec);
        v[qtd++] = new Cliente(nome, cpf, dn, Contato(tel, email), end, nec);
    }
    f.close();
}

void carregarTrabalhadores(Trabalhador* v[], int& qtd) {
    ifstream f("trabalhadores.txt");
    if (!f.is_open()) return;
    string nome, cpf, dn, tel, email, prof;
    float valorHora;
    while (getline(f, nome) && qtd < MAX) {
        getline(f, cpf);
        getline(f, dn);
        getline(f, tel);
        getline(f, email);
        getline(f, prof);
        f >> valorHora;  // le o float (igual ao cin>>media)
        f.ignore();       // descarta o '\n' depois do numero
        v[qtd++] = new Trabalhador(nome, cpf, dn, Contato(tel, email), prof, valorHora);
    }
    f.close();
}

void carregarHabilidades(Trabalhador* v[], int qtd) {
    ifstream f("habilidades.txt");
    if (!f.is_open()) return;
    string nomeT, nomeH, desc;
    while (getline(f, nomeT)) {
        getline(f, nomeH);
        getline(f, desc);
        int pos = pesquisarTrabalhador(v, qtd, nomeT);
        if (pos != -1) v[pos]->adicionarHabilidade(new Habilidade(nomeH, desc));
    }
    f.close();
}

void carregarContratacoes(Contratacao* v[], int& qtd, Trabalhador* trabs[], int qtdT) {
    ifstream f("contratacoes.txt");
    if (!f.is_open()) return;
    string nomeC, nomeT, desc, status;
    int dia, mes, ano;
    while (getline(f, nomeC) && qtd < MAX) {
        getline(f, nomeT);
        getline(f, desc);
        f >> dia; f.ignore();
        f >> mes; f.ignore();
        f >> ano; f.ignore();
        getline(f, status);
        v[qtd++] = new Contratacao(nomeC, nomeT, desc, dia, mes, ano, status);
        // tambem reserva a data na agenda do trabalhador correspondente
        int pos = pesquisarTrabalhador(trabs, qtdT, nomeT);
        if (pos != -1) trabs[pos]->contratar(dia, mes, ano);
    }
    f.close();
}

// ---------- ORQUESTRADORES ----------

void salvarTudo(Cliente* c[], int qc, Trabalhador* t[], int qt,
                Contratacao* ct[], int qct) {
    salvarClientes(c, qc);
    salvarTrabalhadores(t, qt);
    salvarHabilidades(t, qt);
    salvarContratacoes(ct, qct);
}

void carregarTudo(Cliente* c[], int& qc, Trabalhador* t[], int& qt,
                  Contratacao* ct[], int& qct) {
    carregarClientes(c, qc);
    carregarTrabalhadores(t, qt);
    carregarHabilidades(t, qt);          // depende de trabalhadores estarem carregados
    carregarContratacoes(ct, qct, t, qt); // depende de trabalhadores tambem
}


// ====================== sub-menu de edicao ======================

void editarCliente(Cliente* c) {
    int op;
    do {
        cout << endl << "Editando cliente #" << c->getId() << " (" << c->getNome() << ")" << endl;
        cout << "[1] Nome" << endl
             << "[2] CPF" << endl
             << "[3] Data de nascimento" << endl
             << "[4] Telefone" << endl
             << "[5] Email" << endl
             << "[6] Endereco" << endl
             << "[7] Necessidade do servico" << endl
             << "[0] Voltar" << endl
             << "Escolha o campo: ";
        cin >> op;
        cin.ignore();

        string nov;
        switch (op) {
            case 1: cout << "Novo nome: ";          getline(cin, nov); c->setNome(nov);          break;
            case 2: cout << "Novo CPF: ";           getline(cin, nov); c->setCpf(nov);           break;
            case 3: cout << "Nova data nasc.: ";    getline(cin, nov); c->setDataNascimento(nov);break;
            case 4: {
                cout << "Novo telefone: "; getline(cin, nov);
                Contato cont = c->getContato();
                cont.setTelefone(nov);
                c->setContato(cont);
                break;
            }
            case 5: {
                cout << "Novo email: "; getline(cin, nov);
                Contato cont = c->getContato();
                cont.setEmail(nov);
                c->setContato(cont);
                break;
            }
            case 6: cout << "Novo endereco: ";     getline(cin, nov); c->setEndereco(nov);     break;
            case 7: cout << "Nova necessidade: ";  getline(cin, nov); c->setNecessidadeServico(nov); break;
            case 0: cout << "Voltando." << endl;   break;
            default: cout << "Opcao invalida." << endl;
        }
    } while (op != 0);
}

void editarTrabalhador(Trabalhador* t) {
    int op;
    do {
        cout << endl << "Editando trabalhador #" << t->getId() << " (" << t->getNome() << ")" << endl;
        cout << "[1] Nome" << endl
             << "[2] CPF" << endl
             << "[3] Data de nascimento" << endl
             << "[4] Telefone" << endl
             << "[5] Email" << endl
             << "[6] Profissao" << endl
             << "[7] Valor por hora" << endl
             << "[0] Voltar" << endl
             << "Escolha o campo: ";
        cin >> op;
        cin.ignore();

        string nov;
        float val;
        switch (op) {
            case 1: cout << "Novo nome: ";       getline(cin, nov); t->setNome(nov);          break;
            case 2: cout << "Novo CPF: ";        getline(cin, nov); t->setCpf(nov);           break;
            case 3: cout << "Nova data nasc.: "; getline(cin, nov); t->setDataNascimento(nov);break;
            case 4: {
                cout << "Novo telefone: "; getline(cin, nov);
                Contato cont = t->getContato();
                cont.setTelefone(nov);
                t->setContato(cont);
                break;
            }
            case 5: {
                cout << "Novo email: "; getline(cin, nov);
                Contato cont = t->getContato();
                cont.setEmail(nov);
                t->setContato(cont);
                break;
            }
            case 6: cout << "Nova profissao: ";  getline(cin, nov); t->setProfissao(nov);     break;
            case 7: cout << "Novo valor/h: ";    cin >> val; cin.ignore(); t->setValorHora(val); break;
            case 0: cout << "Voltando." << endl;   break;
            default: cout << "Opcao invalida." << endl;
        }
    } while (op != 0);
}


// ====================== main ======================

int main() {

    Cliente* clientes[MAX];
    Trabalhador* trabalhadores[MAX];
    Contratacao* contratacoes[MAX];

    int qtdClientes = 0;
    int qtdTrabalhadores = 0;
    int qtdContratacoes = 0;
    int opcao;

    // carrega tudo dos arquivos .txt (se existirem)
    carregarTudo(clientes, qtdClientes, trabalhadores, qtdTrabalhadores,
                 contratacoes, qtdContratacoes);

    do {
        cout << endl << "===== Total de pessoas cadastradas (static): " << Pessoa::getTotal() << " =====" << endl;
        cout << "[1]  Inserir cliente"          << endl
             << "[2]  Remover cliente"          << endl
             << "[3]  Inserir trabalhador"      << endl
             << "[4]  Remover trabalhador"      << endl
             << "[5]  Listar clientes"          << endl
             << "[6]  Listar trabalhadores"     << endl
             << "[7]  Contratar trabalhador"    << endl
             << "[8]  Mostrar agenda"           << endl
             << "[9]  Mostrar contratacoes"     << endl
             << "[10] Pesquisar cliente"        << endl
             << "[11] Pesquisar trabalhador"    << endl
             << "[12] Editar cliente"           << endl
             << "[13] Editar trabalhador"       << endl
             << "[14] Exportar para HTML"       << endl
             << "[0]  Sair"                     << endl;
        cout << "Escolha: ";
        cin >> opcao;
        cin.ignore();

        switch (opcao) {

            case 1: { // INSERIR CLIENTE
                if (qtdClientes < MAX) {
                    string nome, cpf, dataNascimento, telefone, email, endereco, necessidadeServico;

                    cout << endl << "Nome do cliente: ";   getline(cin, nome);
                    cout << "CPF: ";                       getline(cin, cpf);
                    cout << "Data de nascimento: ";        getline(cin, dataNascimento);
                    cout << "Telefone: ";                  getline(cin, telefone);
                    cout << "Email: ";                     getline(cin, email);
                    cout << "Endereco: ";                  getline(cin, endereco);
                    cout << "Necessidade do servico: " << endl;
                    getline(cin, necessidadeServico);

                    clientes[qtdClientes++] = new Cliente(nome, cpf, dataNascimento,
                        Contato(telefone, email), endereco, necessidadeServico);

                    cout << "Cliente inserido com sucesso." << endl << endl;
                } else {
                    cout << "Limite de clientes atingido." << endl << endl;
                }
                salvarTudo(clientes, qtdClientes, trabalhadores, qtdTrabalhadores, contratacoes, qtdContratacoes);
                break;
            }

            case 2: { // REMOVER CLIENTE
                string nome;
                cout << endl << "Nome do cliente para remover: ";
                getline(cin, nome);

                int pos = pesquisarCliente(clientes, qtdClientes, nome);
                if (pos == -1) {
                    cout << "Cliente nao encontrado." << endl << endl;
                } else {
                    delete clientes[pos];
                    for (int j = pos; j < qtdClientes - 1; j++) clientes[j] = clientes[j + 1];
                    qtdClientes--;
                    cout << "Cliente removido com sucesso." << endl << endl;
                }
                salvarTudo(clientes, qtdClientes, trabalhadores, qtdTrabalhadores, contratacoes, qtdContratacoes);
                break;
            }

            case 3: { // INSERIR TRABALHADOR
                if (qtdTrabalhadores < MAX) {
                    string nome, cpf, dataNascimento, telefone, email, profissao;
                    float valorHora;
                    int qtdHab;

                    cout << endl << "Nome do trabalhador: "; getline(cin, nome);
                    cout << "CPF: ";                         getline(cin, cpf);
                    cout << "Data de nascimento: ";          getline(cin, dataNascimento);
                    cout << "Telefone: ";                    getline(cin, telefone);
                    cout << "Email: ";                       getline(cin, email);
                    cout << "Profissao: ";                   getline(cin, profissao);
                    cout << "Valor por hora: " << endl;
                    cin >> valorHora; cin.ignore();

                    trabalhadores[qtdTrabalhadores] = new Trabalhador(nome, cpf, dataNascimento,
                        Contato(telefone, email), profissao, valorHora);

                    cout << "Quantas habilidades deseja adicionar? ";
                    cin >> qtdHab; cin.ignore();

                    for (int i = 0; i < qtdHab && i < MAX_HAB; i++) {
                        string nomeHabilidade, descricaoHabilidade;
                        cout << "Nome da habilidade " << i + 1 << ": ";
                        getline(cin, nomeHabilidade);
                        cout << "Descricao da habilidade " << i + 1 << ": ";
                        getline(cin, descricaoHabilidade);
                        trabalhadores[qtdTrabalhadores]->adicionarHabilidade(
                            new Habilidade(nomeHabilidade, descricaoHabilidade));
                    }

                    qtdTrabalhadores++;
                    cout << "Trabalhador inserido com sucesso." << endl << endl;
                } else {
                    cout << "Limite de trabalhadores atingido." << endl << endl;
                }
                salvarTudo(clientes, qtdClientes, trabalhadores, qtdTrabalhadores, contratacoes, qtdContratacoes);
                break;
            }

            case 4: { // REMOVER TRABALHADOR
                string nome;
                cout << endl << "Nome do trabalhador para remover: ";
                getline(cin, nome);

                int pos = pesquisarTrabalhador(trabalhadores, qtdTrabalhadores, nome);
                if (pos == -1) {
                    cout << "Trabalhador nao encontrado." << endl << endl;
                } else {
                    delete trabalhadores[pos];
                    for (int j = pos; j < qtdTrabalhadores - 1; j++) trabalhadores[j] = trabalhadores[j + 1];
                    qtdTrabalhadores--;
                    cout << "Trabalhador removido com sucesso." << endl << endl;
                }
                salvarTudo(clientes, qtdClientes, trabalhadores, qtdTrabalhadores, contratacoes, qtdContratacoes);
                break;
            }

            case 5: { // LISTAR CLIENTES
                if (qtdClientes == 0) {
                    cout << "Nenhum cliente cadastrado." << endl << endl;
                } else {
                    cout << endl;
                    for (int i = 0; i < qtdClientes; i++) {
                        clientes[i]->imprime();
                        cout << endl;
                    }
                }
                break;
            }

            case 6: { // LISTAR TRABALHADORES
                if (qtdTrabalhadores == 0) {
                    cout << "Nenhum trabalhador cadastrado." << endl << endl;
                } else {
                    cout << endl;
                    for (int i = 0; i < qtdTrabalhadores; i++) {
                        trabalhadores[i]->imprime();
                        cout << endl;
                    }
                }
                break;
            }

            case 7: { // CONTRATAR TRABALHADOR
                string nomeCliente, nomeTrabalhador, descricaoServico;
                int dia, mes, ano;

                cout << endl << "Nome do cliente: ";    getline(cin, nomeCliente);
                cout << "Nome do trabalhador: ";        getline(cin, nomeTrabalhador);
                cout << "Descricao do servico: ";       getline(cin, descricaoServico);
                cout << "Dia: ";  cin >> dia;
                cout << "Mes: ";  cin >> mes;
                cout << "Ano: ";  cin >> ano;
                cin.ignore();

                int posCliente = pesquisarCliente(clientes, qtdClientes, nomeCliente);
                int posTrabalhador = pesquisarTrabalhador(trabalhadores, qtdTrabalhadores, nomeTrabalhador);

                if (posCliente == -1) {
                    cout << "Cliente nao encontrado." << endl << endl;
                } else if (posTrabalhador == -1) {
                    cout << "Trabalhador nao encontrado." << endl << endl;
                } else {
                    if (trabalhadores[posTrabalhador]->contratar(dia, mes, ano)) {
                        contratacoes[qtdContratacoes++] = new Contratacao(
                            nomeCliente, nomeTrabalhador, descricaoServico,
                            dia, mes, ano, "Confirmada");
                        cout << "Contratacao realizada com sucesso." << endl << endl;
                    } else {
                        cout << "Nao pode contratar, trabalhador ja esta ocupado nessa data." << endl << endl;
                    }
                }
                salvarTudo(clientes, qtdClientes, trabalhadores, qtdTrabalhadores, contratacoes, qtdContratacoes);
                break;
            }

            case 8: { // MOSTRAR AGENDA
                string nomeTrabalhador;
                cout << endl << "Nome do trabalhador: ";
                getline(cin, nomeTrabalhador);

                int pos = pesquisarTrabalhador(trabalhadores, qtdTrabalhadores, nomeTrabalhador);
                if (pos == -1) {
                    cout << "Trabalhador nao encontrado." << endl << endl;
                } else {
                    trabalhadores[pos]->imprimeAgenda();
                }
                break;
            }

            case 9: { // MOSTRAR CONTRATACOES
                if (qtdContratacoes == 0) {
                    cout << "Nenhuma contratacao realizada." << endl << endl;
                } else {
                    for (int i = 0; i < qtdContratacoes; i++) contratacoes[i]->imprime();
                }
                break;
            }

            case 10: { // PESQUISAR CLIENTE (usa sobrecarga)
                int sub;
                cout << endl << "Pesquisar por: [1] Nome  [2] ID  -> ";
                cin >> sub; cin.ignore();

                int pos = -1;
                if (sub == 1) {
                    string nome;
                    cout << "Nome: "; getline(cin, nome);
                    pos = pesquisarCliente(clientes, qtdClientes, nome); // versao string
                } else if (sub == 2) {
                    int id;
                    cout << "ID: "; cin >> id; cin.ignore();
                    pos = pesquisarCliente(clientes, qtdClientes, id);   // versao int (sobrecarga)
                } else {
                    cout << "Opcao invalida." << endl;
                    break;
                }

                if (pos == -1) cout << "Cliente nao encontrado." << endl << endl;
                else { clientes[pos]->imprime(); cout << endl; }
                break;
            }

            case 11: { // PESQUISAR TRABALHADOR (usa sobrecarga)
                int sub;
                cout << endl << "Pesquisar por: [1] Nome  [2] ID  -> ";
                cin >> sub; cin.ignore();

                int pos = -1;
                if (sub == 1) {
                    string nome;
                    cout << "Nome: "; getline(cin, nome);
                    pos = pesquisarTrabalhador(trabalhadores, qtdTrabalhadores, nome);
                } else if (sub == 2) {
                    int id;
                    cout << "ID: "; cin >> id; cin.ignore();
                    pos = pesquisarTrabalhador(trabalhadores, qtdTrabalhadores, id);
                } else {
                    cout << "Opcao invalida." << endl;
                    break;
                }

                if (pos == -1) cout << "Trabalhador nao encontrado." << endl << endl;
                else { trabalhadores[pos]->imprime(); cout << endl; }
                break;
            }

            case 12: { // EDITAR CLIENTE
                string nome;
                cout << endl << "Nome do cliente para editar: ";
                getline(cin, nome);
                int pos = pesquisarCliente(clientes, qtdClientes, nome);
                if (pos == -1) cout << "Cliente nao encontrado." << endl << endl;
                else editarCliente(clientes[pos]);
                salvarTudo(clientes, qtdClientes, trabalhadores, qtdTrabalhadores, contratacoes, qtdContratacoes);
                break;
            }

            case 13: { // EDITAR TRABALHADOR
                string nome;
                cout << endl << "Nome do trabalhador para editar: ";
                getline(cin, nome);
                int pos = pesquisarTrabalhador(trabalhadores, qtdTrabalhadores, nome);
                if (pos == -1) cout << "Trabalhador nao encontrado." << endl << endl;
                else editarTrabalhador(trabalhadores[pos]);
                salvarTudo(clientes, qtdClientes, trabalhadores, qtdTrabalhadores, contratacoes, qtdContratacoes);
                break;
            }

            case 14: { // EXPORTAR PARA HTML
                gerarHTML(clientes, qtdClientes, trabalhadores, qtdTrabalhadores,
                          contratacoes, qtdContratacoes);
                break;
            }

            case 0: {
                cout << "Voce saiu do programa." << endl << endl;
                break;
            }

            default: {
                cout << "Digite um numero valido." << endl << endl;
            }
        }

    } while (opcao != 0);

    for (int i = 0; i < qtdClientes; i++)      delete clientes[i];
    for (int i = 0; i < qtdTrabalhadores; i++) delete trabalhadores[i];
    for (int i = 0; i < qtdContratacoes; i++)  delete contratacoes[i];

    return 0;
}