#include <iostream>
using namespace std;

class Personagem {

    private: 
        string nome;
        int vida;
        int armadura;
        int arma;
        
    public:

    Personagem(string nome, int vida, int armadura, int arma){
        this->nome = nome; 
        this->vida = vida; 
        this-> armadura = armadura;
        this-> arma = arma;
    }
    
    virtual void receberDano(int dano) = 0;
    virtual void causarDano(Personagem* alvo, int danoBase) = 0;
    
    virtual bool ehJogador() = 0;
    virtual bool ehInimigo() = 0;

    int getVida() {
        return vida;
    }

    string getNome() {
        return nome;
    }

    int getArmadura() {
        return armadura;
    }

    int getArma() {
        return arma;
    }

    void setVida(int vida) {
        this->vida = vida;
    }

    virtual ~Personagem() {}
};


class Jogador : public Personagem {
public:
    Jogador(string nome, int vida, int armadura, int arma)
        : Personagem(nome, vida, armadura, arma) {}

    bool ehJogador() { return true; }
    bool ehInimigo() { return false; }

    void receberDano(int dano) {

        int danoFinal = dano - getArmadura();
        if (danoFinal < 0) danoFinal = 0;

        setVida(getVida() - danoFinal);

        cout << getNome() << " recebeu "<< danoFinal << " de dano. Vida: "<< getVida() << endl;
    }

    void causarDano(Personagem* alvo, int danoBase) {

        if (alvo->ehJogador()) {
            cout << getNome() << " nao pode atacar outro jogador!\n";
            return;
        }

        int danoTotal = danoBase + getArma();

        cout << getNome() << " atacou "<< alvo->getNome() << " causando "<< danoTotal << " de dano.\n";

        alvo->receberDano(danoTotal);
    }
};


class Inimigo : public Personagem {
public:
    Inimigo(string nome, int vida, int armadura, int arma)
        : Personagem(nome, vida, armadura, arma) {}

    bool ehJogador() { return false; }
    bool ehInimigo() { return true; }

    void receberDano(int dano) {

        int danoFinal = dano - getArmadura();
        if (danoFinal < 0) danoFinal = 0;

        setVida(getVida() - danoFinal);

        cout << getNome() << " recebeu "<< danoFinal << " de dano. Vida: "<< getVida() << endl;
    }

    void causarDano(Personagem* alvo, int danoBase) {

        if (alvo->ehInimigo()) {
            cout << getNome() << " nao pode atacar outro inimigo!\n";
            return;
        }

        int danoTotal = danoBase + getArma();

        cout << getNome() << " atacou "<< alvo->getNome() << " causando "<< danoTotal << " de dano.\n";

        alvo->receberDano(danoTotal);
    }
};

// Guerreiro tem mais defesa (dobro de armadura)
class Guerreiro : public Jogador {
public:
    Guerreiro(string nome, int vida, int armadura, int arma)
        : Jogador(nome, vida, armadura, arma) {}

    void receberDano(int dano) {

        int armaduraDobro = getArmadura() * 2;

        int danoFinal = dano - armaduraDobro;
        if (danoFinal < 0) danoFinal = 0;

        setVida(getVida() - danoFinal);

        cout << getNome() << " (Guerreiro) recebeu "<< danoFinal << " de dano. Vida: "<< getVida() << endl;
    }
};

// Bárbaro causa o dobro de dano
class Barbaro : public Jogador {
public:
    Barbaro(string nome, int vida, int armadura, int arma)
        : Jogador(nome, vida, armadura, arma) {}

    void causarDano(Personagem* alvo, int danoBase) {

        if (alvo->ehJogador()) {
            cout << getNome() << " nao pode atacar outro jogador!\n";
            return;
        }

        int danoTotal = (danoBase + getArma()) * 2;

        cout << getNome() << " (Barbaro) causou "<< danoTotal << " de dano!\n";

        alvo->receberDano(danoTotal);
    }
    
};

int main () {
    // ordem dos dados ("nome", vida, armadura, arma)
    
    Personagem* j1 = new Guerreiro("Batman", 500, 50, 40);
    Personagem* j2 = new Barbaro("Conan", 500, 30, 80);

    Personagem* inimigo1 = new Inimigo("Coringa", 400, 20, 50);
    Personagem* inimigo2 = new Inimigo("Bane", 600, 40, 70);

    cout << "\n COMBATE \n\n";

    j1->causarDano(inimigo1, 30);
    inimigo1->causarDano(j1, 20);

    cout << "\n";

    j2->causarDano(inimigo2, 25);
    inimigo2->causarDano(j2, 15);

    cout << "\n";

    // TESTES DAS REGRAS
    j1->causarDano(j2, 10);            // proibido
    inimigo1->causarDano(inimigo2, 10); // proibido

    return 0;
}