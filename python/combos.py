#!/usr/bin/env python3

import pokerlab

def factorial(n):
    """Una simple función factorial."""
    if n > 0:
        return n * factorial(n - 1)
    if n == 0:
        return 1
    return None

def C(nElem, nTomados):
    """Número de combinaciones (sin repetición) de 'nElem', tomados en grupos de 
    'nTomados' elementos (número combinatorio)."""
    return factorial(nElem) // (factorial(nElem - nTomados) * factorial(nTomados))

def combos(nElem, nTomados, exclude=()):
    """Combinaciones (sin repetición) de 'nElem', tomados en grupos de 
    'nTomados' elementos. Los elementos serán enteros entre 0 y 'nElem'-1.
    Se puede especificar una tupla con enteros a excluir ('exclude')."""
    inexclude = False
    lista = [i for i in range(nTomados)]
    for v in lista:
        if v in exclude:
            inexclude = True
            break
    if inexclude:
        inexclude = False
    else:
        yield lista
    while True:
        i = len(lista) - 1
        lista[i] = lista[i] + 1
        if lista[i] < nElem - (len(lista) - i - 1):
            for v in lista:
                if v in exclude:
                    inexclude = True
                    break
            if inexclude:
                inexclude = False
            else:
                yield lista
        else:
            while lista[i] >= nElem - (len(lista) - i - 1):
                if i == 0:
                    return    # hemos terminado
                else:
                    i = i - 1
                    lista[i] = lista[i] + 1
            valor = lista[i]
            for j in range(i + 1, len(lista)):
                valor += 1
                lista[j] = valor
            for v in lista:
                if v in exclude:
                    inexclude = True
                    break
            if inexclude:
                inexclude = False
            else:
                yield lista


manos = ["Carta alta", "Pareja", "Doble pareja", "Trío", "Escalera",
         "Color", "Full", "Póker", "Escalera de color", "Escalera real"]
frecuencias = [0, 0, 0, 0, 0, 0, 0, 0, 0, 0]
num = 1
for a in combos(52, 5):
    val = pokerlab.pl_valorMano(*a)
    frecuencias[val[0]] += 1
    print(num, "-", a, "==>", val)
    num += 1
print("Calculado: ", C(52, 5))
for i in range(10):
    print(manos[i], frecuencias[i])

