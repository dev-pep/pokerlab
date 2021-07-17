static int _rankToCoord(char c)
{
    // Retorna número (0-12) a partir de carácter de rank (A, K,..., 3, 2)
    // para coordenada en la tabla preflop, o -1 en caso de error.
    char ranks[] = "AKQJT98765432";
    for(int i = 0; i < 13; i++)
        if(c == ranks[i])
            return i;
    return -1;
}

static struct Carta _intToCard(int carta)
{
    // Recibe carta numérica (0-51) y retorna struct Carta.
    struct Carta resul;
    if((carta > 51) || (carta < 0))
    {
        resul.rank = -1;
        return resul;
    }
    resul.suit = carta / 13;
    resul.rank = carta % 13;
    return resul;
}

static int _cardToInt(struct Carta carta)
{
    return carta.rank + carta.suit * 13;
}

static struct Carta _strToCard(PyObject *carta)
{
    // Recibe carta tipo "Ts" (en un string Python) y retorna struct Carta.
    struct Carta resul = {-1};  // valor de error, por defecto
    if(PyUnicode_GetLength(carta) != 2)
        return resul;    // formato incorrecto
    char ranks[] = "A23456789TJQK";
    char suits[] = "cdhs";
    int i;
    Py_UCS4 caracter = PyUnicode_ReadChar(carta, 0);
    for(i = 0; i < 13; i++)
        if(caracter == ranks[i])
        {
            resul.rank = i;
            break;
        }
    if(i == 13)
        return resul;    // rank incorrecto
    caracter = PyUnicode_ReadChar(carta, 1);
    for(i = 0; i < 4; i++)
        if(caracter == suits[i])
        {
            resul.suit = i;
            break;
        }
    if(i == 4)
        resul.rank = -1;    // suit incorrecto
    return resul;
}

static int _comparaValores(struct Valor valH, struct Valor valV)
{
    // Compara los valores resultantes de dos manos. Retorna 1 si valH > valV,
    // -1 si valH < valV, o 0 si son iguales
    if(valH.valor > valV.valor)
        return 1;
    if(valH.valor < valV.valor)
        return -1;
    for(int i = 0; i < 5; i++)
    {
        if(valH.kickers[i] > valV.kickers[i])
            return 1;
        if(valH.kickers[i] < valV.kickers[i])
            return -1;
    }
    return 0;
}

static struct Valor _valorMano(struct Carta *cartas, int n)
{
    // Recibe un apuntador a 5 o 7 cartas y retorna su valor como mano de póker.
    struct Valor resul = {-1, {-1, -1, -1, -1, -1}};
    int i, j, kickers[5], count;
    int count3 = 0, count2 = 0, count1 = 0;
    int val3 = 0, val2[2], val1[5];    // inicializamos val3 para evitar warning
    // Frecuencias de ranks por palo:
    // Para cada rango hay un array de 4 elementos (1 por palo).
    int rankFreq[14][4];
    for(i = 0; i < 13; i++)    // no hace falta inicializar el último (14º) array
        for(j = 0; j < 4; j++)
            rankFreq[i][j] = 0;
    for(i = 0; i < n; i++)
        rankFreq[cartas[i].rank][cartas[i].suit] = 1;
    for(i =0; i < 4; i++)
        rankFreq[13][i] = rankFreq[0][i];    // acehigh = ace
    // Vamos a buscar escaleras:
    int steps = 0, rankFin;    // longitud y rango final de posible(s) escalera(s)
    for(i = 0; i < 14; i++)
    {
        if(rankFreq[i][0] + rankFreq[i][1] + rankFreq[i][2] + rankFreq[i][3])
        {
            rankFin = i;
            steps++;
        }
        else
        {
            if(steps > 4)
                break;
            steps = 0;
            if(i >= 9)
                break;
        }
    }
    // Vamos a almacenar repeticiones:
    for(i = 13; i > 0; i--)
    {
        int freq = rankFreq[i][0] + rankFreq[i][1] + rankFreq[i][2] + rankFreq[i][3];
        if(freq == 3)
        {
            if(count3 == 0)
            {
                val3 = i;
                count3++;
            }
            else if(count2 < 2)   // un segundo trío contará como pareja del full, si procede
                val2[count2++] = i;
        }
        else if(freq == 2)
        {
            if(count2 < 2)
                val2[count2++] = i;
            else
                val1[count1++] = i;    // una tercera pareja contará como kicker de la doble pareja
        }
        else if((freq == 1) && (count1 < 5))
            val1[count1++] = i;
    }
    // ROYAL FLUSH Y STRAIGHT FLUSH:
    if(steps > 4)
    {
        for(i = 0; i <= steps - 5; i++)    // puede haber entre 5 y 7 valores consecutivos
        {
            for(int k = 0; k < 4; k++)
                if(rankFreq[rankFin-i][k] *
                   rankFreq[rankFin-i-1][k] * rankFreq[rankFin-i-2][k] *
                   rankFreq[rankFin-i-3][k] * rankFreq[rankFin-i-4][k] == 1)
                {
                    if(rankFin - i == 13)
                    {
                        resul.valor = ROYALFLUSH;
                        return resul;
                    }
                    resul.valor = STRAIGHTFLUSH;
                    resul.kickers[0] = rankFin - i;
                    return resul;
                }
        }
    }
    // 4 OF A KIND:
    for(i = 13; i > 0; i--)
        if(rankFreq[i][0] + rankFreq[i][1] + rankFreq[i][2] + rankFreq[i][3] == 4)
            for(j = 13; j > 0; j--)
                if((i != j) && (rankFreq[j][0] + rankFreq[j][1] + rankFreq[j][2] + rankFreq[j][3] > 0))
                {
                    resul.valor = FOUROAK;
                    resul.kickers[0] = i;
                    resul.kickers[1] = j;
                    return resul;
                }
    // FULL HOUSE:
    if(count3 && count2)
    {
        resul.valor = FULLHOUSE;
        resul.kickers[0] = val3;
        resul.kickers[1] = val2[0];
        return resul;
    }
    // FLUSH:
    for(i = 0; i < 4; i++)
    {
        count = 0;
        for(j = 13; j > 0; j--)
            if(rankFreq[j][i])
            {
                kickers[count] = j;
                count++;
                if(count == 5)
                {
                    resul.valor = FLUSH;
                    for(int k = 0; k < 5; k++)
                        resul.kickers[k] = kickers[k];
                    return resul;
                }
            }
    }
    // STRAIGHT:
    if(steps > 4)
    {
        resul.valor = STRAIGHT;
        resul.kickers[0] = rankFin;
        return resul;
    }
    // 3 OF A KIND, 2 PAIR, PAIR:
    if(count3)    // 3 OF A KIND
    {
        resul.valor = THREEOAK;
        resul.kickers[0] = val3;
        resul.kickers[1] = val1[0];
        resul.kickers[2] = val1[1];
        return resul;
    }
    else if(count2 == 2)    // 2 PAIR
    {
        resul.valor = TWOPAIR;
        resul.kickers[0] = val2[0];
        resul.kickers[1] = val2[1];
        resul.kickers[2] = val1[0];
        return resul;
    }
    else if(count2 == 1)    // PAIR
    {
        resul.valor = PAIR;
        resul.kickers[0] = val2[0];
        resul.kickers[1] = val1[0];
        resul.kickers[2] = val1[1];
        resul.kickers[3] = val1[2];
        return resul;
    }
    // HIGH CARD:
    resul.valor = HIGHCARD;
    resul.kickers[0] = val1[0];
    resul.kickers[1] = val1[1];
    resul.kickers[2] = val1[2];
    resul.kickers[3] = val1[3];
    resul.kickers[4] = val1[4];
    return resul;
}

