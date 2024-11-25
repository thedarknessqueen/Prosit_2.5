#include <iostream>
#include <cmath>
#include <string>

using namespace std;

// Définition des types pour les coefficients et les exposants
typedef float term_t;         // type pour les coefficients (float)
typedef unsigned int exp_t;   // type pour les exposants (unsigned int)

// Déclaration anticipée de la classe Monomial (forward declaration)
// Cela permet au namespace mono d'utiliser Monomial<N> sans voir son implémentation complète.
template <int N>
class Monomial;

// Namespace mono : contient des fonctions utilitaires liées aux monômes.
namespace mono {
    // Fonction de différenciation d'un monôme
    // - source : le monôme d'origine
    // - deriv : le monôme dérivé
    // - mask : masque qui indique les variables à différencier
    template <int N>
    void differentiate(const Monomial<N>& source, Monomial<N>& deriv, exp_t mask[N]) {
        deriv = source; // Copier le monôme source dans le monôme dérivé
        bool isConstant = true; // Variable pour vérifier si le résultat est une constante

        for (int i = 0; i < N; ++i) {
            // Si la variable i est dans le masque et que l'exposant est non nul
            if (mask[i] != 0 && source.exps[i] != 0) {
                deriv.coeff *= source.exps[i];  // Multiplier le coefficient par l'exposant
                deriv.exps[i] -= 1;            // Réduire l'exposant de 1
                isConstant = false;            // Ce n'est pas une constante
            }
        }

        if (isConstant) {
            deriv.nullify(); // Si aucune variable n'a été différenciée, le résultat est nul
        }
    }
}

// Classe abstraite Expression : représente une expression générique
template <int N>
class Expression {
public:
    // Méthode pour évaluer l'expression avec des valeurs données
    virtual term_t solve(term_t val[N]) = 0;

    // Méthode pour différencier l'expression par rapport à certaines variables
    virtual void differentiate(Expression<N>& exp, exp_t mask[N]) = 0;

    // Méthode pour convertir l'expression en chaîne de caractères
    virtual string toString() = 0;

    // Destructeur virtuel pour permettre la suppression via des pointeurs vers Expression
    virtual ~Expression() = default;
};

// Classe Monomial : représente un monôme (par exemple, 3x^2y^1)
template <int N>
class Monomial : public Expression<N> {
protected:
    term_t coeff;  // Coefficient du monôme
    exp_t exps[N]; // Tableau des exposants pour chaque variable

    constexpr static exp_t null_exps[N] = {0}; // Tableau d'exposants nuls

public:
    // Constructeur par défaut : initialise le monôme à zéro
    Monomial() { nullify(); }

    // Constructeur avec un coefficient
    Monomial(term_t coeff) : coeff(coeff) { setExponents(null_exps); }

    // Constructeur avec un coefficient et des exposants
    Monomial(term_t coeff, const exp_t exps[N]) : coeff(coeff) {
        setExponents(exps);
    }

    // Méthode pour évaluer le monôme avec des valeurs données
    term_t solve(term_t vals[N]) override {
        term_t result = coeff; // Initialiser le résultat au coefficient
        for (int i = 0; i < N; ++i) {
            result *= pow(vals[i], exps[i]); // Multiplier par chaque variable élevée à son exposant
        }
        return result;
    }

    // Méthode pour différencier le monôme
    void differentiate(Expression<N>& exp, exp_t mask[N]) override {
        auto& deriv = dynamic_cast<Monomial<N>&>(exp); // Convertir l'expression en Monomial
        mono::differentiate<N>(*this, deriv, mask);    // Appeler la fonction de différenciation dans mono
    }

    // Méthode pour convertir le monôme en chaîne de caractères
    string toString() override {
        string result = to_string(coeff); // Ajouter le coefficient
        for (int i = 0; i < N; ++i) {
            if (exps[i] != 0) {
                // Ajouter chaque variable et son exposant si non nul
                result += "x_" + to_string(i + 1) + "^" + to_string(exps[i]);
            }
        }
        return result;
    }

    // Méthode pour rendre le monôme nul (coefficient à 0)
    void nullify() {
        coeff = 0.0f;
        for (int i = 0; i < N; ++i) {
            exps[i] = 0;
        }
    }

    // Méthode pour définir les exposants
    void setExponents(const exp_t exps[N]) {
        for (int i = 0; i < N; ++i) {
            this->exps[i] = exps[i];
        }
    }

    // Déclare mono::differentiate comme "ami" pour accéder aux membres protégés
    friend void mono::differentiate<N>(const Monomial<N>& source, Monomial<N>& deriv, exp_t mask[N]);
};

// Classe Monomial1D : spécialisation de Monomial pour une seule variable
class Monomial1D : public Monomial<1> {
public:
    using Monomial<1>::Monomial; // Hérite des constructeurs de Monomial
};

int main() {
    const exp_t exps[1] = {2}; // Exposant pour x^2
    Monomial1D m(3.0f, exps);  // Créer un monôme 3x^2

    exp_t mask[1] = {1};       // Masque pour différencier par rapport à x
    Monomial1D deriv;          // Monôme pour stocker le résultat de la dérivation
    m.differentiate(deriv, mask); // Différencier m par rapport à x

    cout << "Original: " << m.toString() << endl;  // Affiche : 3x_1^2
    cout << "Derivee: " << deriv.toString() << endl; // Affiche : 6x_1^1

    return 0;
}
