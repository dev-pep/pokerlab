static struct WLS _simHandVsHand(struct Carta cartas[4])
{
    // Recibe en 'cartas' el pocket del hero y del villano (h1, h2, v1, v2).
    // Retorna una estructura con datos (win, lose, split).
    struct WLS resul = {0, 0, 0};
    int c1, c2, c3, c4, c5;
    struct Carta c[7];
    int h1 = _cardToInt(cartas[0]), h2 = _cardToInt(cartas[1]);
    int v1 = _cardToInt(cartas[2]), v2 = _cardToInt(cartas[3]);
    struct Valor resulH, resulV;
    int resulHV;
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
                            resul.win++;
                        else if(resulHV < 0)
                            resul.lose++;
                        else
                            resul.split++;
                    }
                }
            }
        }
    }
    return resul;
}

static struct WLS _simHandVsRange(struct Carta cartas[2], _Bool r[13][13])
{
    // Recibe en 'cartas' dos simples cartas que se enfrentarán al
    // range 'r'. Retorna una tupla con los valores (win, lose, split).
    // Pasa ownership de la nueva tupla al caller.
    struct WLS resul = {0,0,0}, resulParcial;
    int x, y, i, j;
    char suits[] = {'c', 'd', 'h', 's'}, par[4];
    char cartaStr[2];
    struct Carta c[4];
    // Mano en c[0], c[1]:
    c[0] = cartas[0];
    c[1] = cartas[1];
    // Vamos a generar todos los combos posibles (palos) por cada entrada
    // de la tabla del range (los iremos colocando en c[2], c[3]):
    for(x = 0; x < 13; x++)
        for(y = 0; y < 13; y++)
            if(r[x][y])
            {
                _rangeCoordsToPar(x, y, par);
                if(par[0] == par[1])    // pocket pair
                {
                    for(i = 0; i < 3; i++)    // combos (6)
                        for(j = i + 1; j < 4; j++)
                        {
                            // Primera carta del combo:
                            cartaStr[0] = par[0];
                            cartaStr[1] = suits[i];
                            c[2] = _charsToCard(cartaStr);
                            // Segunda carta del combo:
                            cartaStr[0] = par[1];
                            cartaStr[1] = suits[j];
                            c[3] = _charsToCard(cartaStr);
                            if(_cartasRepetidas(c, 4))
                                continue;
                            resulParcial = _simHandVsHand(c);
                            resul.win = resul.win + resulParcial.win;
                            resul.lose = resul.lose + resulParcial.lose;
                            resul.split = resul.split + resulParcial.split;
                        }
                }
                else if(par[2] == 's')    // suited
                {
                    for(i = 0; i < 4; i++)    // combos (4)
                    {
                        // Primera carta del combo:
                        cartaStr[0] = par[0];
                        cartaStr[1] = suits[i];
                        c[2] = _charsToCard(cartaStr);
                        // Segunda carta del combo:
                        cartaStr[0] = par[1];
                        cartaStr[1] = suits[i];
                        c[3] = _charsToCard(cartaStr);
                        if(_cartasRepetidas(c, 4))
                            continue;
                        resulParcial = _simHandVsHand(c);
                        resul.win = resul.win + resulParcial.win;
                        resul.lose = resul.lose + resulParcial.lose;
                        resul.split = resul.split + resulParcial.split;
                    }
                }
                else    // offsuit
                {
                    for(i = 0; i < 4; i++)    // combos (12)
                        for(j = 0; j < 4; j++)
                        {
                            if(i == j)
                                continue;
                            // Primera carta del combo:
                            cartaStr[0] = par[0];
                            cartaStr[1] = suits[i];
                            c[2] = _charsToCard(cartaStr);
                            // Segunda carta del combo:
                            cartaStr[0] = par[1];
                            cartaStr[1] = suits[j];
                            c[3] = _charsToCard(cartaStr);
                            if(_cartasRepetidas(c, 4))
                                continue;
                            resulParcial = _simHandVsHand(c);
                            resul.win = resul.win + resulParcial.win;
                            resul.lose = resul.lose + resulParcial.lose;
                            resul.split = resul.split + resulParcial.split;
                        }
                }
            }
    return resul;
}

