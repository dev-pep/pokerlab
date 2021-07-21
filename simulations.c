static struct WLS _simHandVsHand(struct Carta cartas[4])
{
    // Recibe en 'cartas' el pocket del hero y del villano (h1, h2, v1, v2).
    // Retorna una estructura con datos (win, lose, tie).
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
                            resul.tie++;
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
    // range 'r'. Retorna una tupla con los valores (win, lose, tie).
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
                            resul.tie = resul.tie + resulParcial.tie;
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
                        resul.tie = resul.tie + resulParcial.tie;
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
                            resul.tie = resul.tie + resulParcial.tie;
                        }
                }
            }
    return resul;
}

static struct WLS _simRangeVsRange(_Bool rH[13][13], _Bool rV[13][13])
{
    int i, j, k, l, n, n1 = -1, n2 = -1;
    struct WLS resul = {0, 0, 0};
    char parH[4], parV[4];
    char *ranges[] = {"AA ", "AKs", "AKo", "AQs", "AQo", "AJs", "AJo", "ATs",
        "ATo", "A9s", "A9o", "A8s", "A8o", "A7s", "A7o", "A6s", "A6o", "A5s",
        "A5o", "A4s", "A4o", "A3s", "A3o", "A2s", "A2o", "KK ", "KQs", "KQo",
        "KJs", "KJo", "KTs", "KTo", "K9s", "K9o", "K8s", "K8o", "K7s", "K7o",
        "K6s", "K6o", "K5s", "K5o", "K4s", "K4o", "K3s", "K3o", "K2s", "K2o",
        "QQ ", "QJs", "QJo", "QTs", "QTo", "Q9s", "Q9o", "Q8s", "Q8o", "Q7s",
        "Q7o", "Q6s", "Q6o", "Q5s", "Q5o", "Q4s", "Q4o", "Q3s", "Q3o", "Q2s",
        "Q2o", "JJ ", "JTs", "JTo", "J9s", "J9o", "J8s", "J8o", "J7s", "J7o",
        "J6s", "J6o", "J5s", "J5o", "J4s", "J4o", "J3s", "J3o", "J2s", "J2o",
        "TT ", "T9s", "T9o", "T8s", "T8o", "T7s", "T7o", "T6s", "T6o", "T5s",
        "T5o", "T4s", "T4o", "T3s", "T3o", "T2s", "T2o", "99 ", "98s", "98o",
        "97s", "97o", "96s", "96o", "95s", "95o", "94s", "94o", "93s", "93o",
        "92s", "92o", "88 ", "87s", "87o", "86s", "86o", "85s", "85o", "84s",
        "84o", "83s", "83o", "82s", "82o", "77 ", "76s", "76o", "75s", "75o",
        "74s", "74o", "73s", "73o", "72s", "72o", "66 ", "65s", "65o", "64s",
        "64o", "63s", "63o", "62s", "62o", "55 ", "54s", "54o", "53s", "53o",
        "52s", "52o", "44 ", "43s", "43o", "42s", "42o", "33 ", "32s", "32o",
        "22 "};
    for(i = 0; i < 13; i++)
        for(j = 0; j < 13; j++)
        {
            _rangeCoordsToPar(i, j, parH);
            for(n = 0; n < 169; n++)
                if(!strcmp(ranges[n], parH))
                {
                    n1 = n;
                    break;
                }
            if(rH[i][j])
                for(k = 0; k < 13; k++)
                    for(l = 0; l < 13; l++)
                        if(rV[k][l])
                        {
                            _rangeCoordsToPar(k, l, parV);
                            for(n = 0; n < 169; n++)
                            if(!strcmp(ranges[n], parV))
                            {
                                n2 = n;
                                break;
                            }
                            resul.win = resul.win + _rangeTable[n1][n2][0];
                            resul.lose = resul.lose + _rangeTable[n1][n2][1];
                            resul.tie = resul.tie + _rangeTable[n1][n2][2];
                        }
        }
    return resul;
}

