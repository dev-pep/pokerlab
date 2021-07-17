static PyObject * _simHandVsHand(struct Carta cartas[4])
{
    // Recibe en 'cartas' el pocket del hero y del villano (h1, h2, v1, v2).
    // Retorna tupla con datos (win, lose, tie). Pasa ownership al caller.
    PyObject *resul;
    int c1, c2, c3, c4, c5;
    struct Carta c[7];
    int h1 = _cardToInt(cartas[0]), h2 = _cardToInt(cartas[1]);
    int v1 = _cardToInt(cartas[2]), v2 = _cardToInt(cartas[3]);
    struct Valor resulH, resulV;
    int resulHV;
    int win = 0, lose = 0, tie = 0;
    for(c1 = 0; c1 < 48; c1++)
    {
        if(c1 == h1 || c1 == h2 || c1 == v1 || c1 == v2)
            continue;
        for(c2 = c1 + 1; c2 < 49; c2++)
        {
            if(c2 == h1 || c2 == h2 || c2 == v1 || c2 == v2)
                continue;
            for(c3 = c2 + 1; c3 < 50; c3++)
            {
                if(c3 == h1 || c3 == h2 || c3 == v1 || c3 == v2)
                    continue;
                for(c4 = c3 + 1; c4 < 51; c4++)
                {
                    if(c4 == h1 || c4 == h2 || c4 == v1 || c4 == v2)
                        continue;
                    for(c5 = c4 + 1; c5 < 52; c5++)
                    {
                        if(c5 == h1 || c5 == h2 || c5 == v1 || c5 == v2)
                            continue;
                        c[0] = _intToCard(c1);
                        c[1] = _intToCard(c2);
                        c[2] = _intToCard(c3);
                        c[3] = _intToCard(c4);
                        c[4] = _intToCard(c5);
                        c[5] = cartas[0];
                        c[6] = cartas[1];
                        resulH = _valorMano(c, 7);
                        c[5] = cartas[2];
                        c[6] = cartas[3];
                        resulV = _valorMano(c, 7);
                        resulHV = _comparaValores(resulH, resulV);
                        if(resulHV > 0)
                            win++;
                        else if(resulHV < 0)
                            lose++;
                        else
                            tie++;
                    }
                }
            }
        }
    }
    resul = Py_BuildValue("iii", win, lose, tie);  // new reference
    return resul;  // pasamos tupla y ownership
}

