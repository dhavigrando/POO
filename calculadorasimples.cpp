#include <iostream>
using namespace std;

class Calculadora {
public:
    virtual float calcular(float a, float b) {
        return 0;
    }
};

class Soma : public Calculadora {
public:
    float calcular(float a, float b) {
        return a + b;
    }
};

class Subtracao : public Calculadora {
public:
    float calcular(float a, float b) {
        return a - b;
    }
};

class Multiplicacao : public Calculadora {
public:
    float calcular(float a, float b) {
        return a * b;
    }
};

class Divisao : public Calculadora {
public:
    float calcular(float a, float b) {
        if (b == 0) {
            cout << "Erro: divisao por zero" << endl;
            return 0;
        }
        return a / b;
    }
};


class CalculadoraSimples {
public:

    int soma(int a, int b) {
        return a + b;
    }

    float soma(float a, float b) {
        return a + b;
    }

    float soma(float a, float b, float c) {
        return a + b + c;
    }
};


int main() {

    float a, b;
    int opcao;

    cout << "Digite dois numeros: ";
    cin >> a >> b;

    cout <<"Escolha a operacao:"<<endl;
    cout << "1-Soma "<< "   2-Subtracao "<< "    3-Multiplicacao "<< "   4-Divisao "<<endl;
    cin >> opcao;

    Calculadora* calc;
 
    switch (opcao) {
        case 1:
            calc = new Soma();
            break;
        case 2:
            calc = new Subtracao();
            break;
        case 3:
            calc = new Multiplicacao();
            break;
        case 4:
            calc = new Divisao();
            break;
        default:
            cout << "Opcao invalida!" << endl;
            return 0;
    }

    cout << "Resultado: " << calc->calcular(a, b) << endl;

    delete calc;

    // testando a sobrecarga
    CalculadoraSimples c;

    cout << "Testando sobrecarga:";
    cout << "Soma int: " << c.soma(2, 3) << endl;
    cout << "Soma float: " << c.soma(2.5f, 3.5f) << endl;
    cout << "Soma 3 valores: " << c.soma(1.0f, 2.0f, 3.0f) << endl;

    return 0;
}