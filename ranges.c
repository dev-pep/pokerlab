void _rangeSetPercent(int n, int val)
{
    // Establece (o borra) la tabla preflop a partir de un porcentaje
    const char *pockets[] = {"AA", "KK", "QQ", "JJ", "TT", "99", "88", "AKs", "77", 
        "AQs", "AJs", "AKo", "ATs", "AQo", "AJo", "KQs", "66", "A9s", "ATo", "KJs", 
        "A8s", "KTs", "KQo", "A7s", "A9o", "KJo", "55", "QJs", "K9s", "A5s", "A6s", 
        "A8o", "KTo", "QTs", "A4s", "A7o", "K8s", "A3s", "QJo", "K9o", "A5o", "A6o", 
        "Q9s", "K7s", "JTs", "A2s", "QTo", "44", "A4o", "K6s", "K8o", "Q8s", "A3o", 
        "K5s", "J9s", "Q9o", "JTo", "K7o", "A2o", "K4s", "Q7s", "K6o", "K3s", "T9s", 
        "J8s", "33", "Q6s", "Q8o", "K5o", "J9o", "K2s", "Q5s", "T8s", "K4o", "J7s", 
        "Q4s", "Q7o", "T9o", "J8o", "K3o", "Q6o", "Q3s", "98s", "T7s", "J6s", "K2o", 
        "22", "Q2s", "Q5o", "J5s", "T8o", "J7o", "Q4o", "97s", "J4s", "T6s", "J3s", 
        "Q3o", "98o", "87s", "T7o", "J6o", "96s", "J2s", "Q2o", "T5s", "J5o", "T4s", 
        "97o", "86s", "J4o", "T6o", "95s", "T3s", "76s", "J3o", "87o", "T2s", "85s", 
        "96o", "J2o", "T5o", "94s", "75s", "T4o", "93s", "86o", "65s", "84s", "95o", 
        "T3o", "92s", "76o", "74s", "T2o", "54s", "85o", "64s", "83s", "94o", "75o", 
        "82s", "73s", "93o", "65o", "53s", "63s", "84o", "92o", "43s", "74o", "72s", 
        "54o", "64o", "52s", "62s", "83o", "42s", "82o", "73o", "53o", "63o", "32s", 
        "43o", "72o", "52o", "62o", "42o", "32o"};
    int n_manos = n * 169 / 100;
    for(int i = 0; i < n_manos; i++)
    {
        struct Coords point = _rangeParToCoords(pockets[i]);
        _range[point.x][point.y] = val;
    }
}

static void _rangeClear()    // resetea tabla preflop
{
    for(int i = 0; i < 13; i++)
        for(int j = 0; j < 13; j++)
            _range[i][j] = 0;
}

static void _rangeInvert()    // invierte tabla preflop
{
    for(int i = 0; i < 13; i++)
        for(int j = 0; j < 13; j++)
        {
            int v = _range[i][j] - 1;
            _range[i][j] = v && v;
        }
}

static void _rangePrintTable()
{
    // Muestra la tabla preflop en un formato de tabla
    int i, j;
    char par[4];
    struct Coords c;
    // Borde superior:
    printf("%lc", 0x2554);
    for(i = 0; i < 5; i++)
        printf("%lc", 0x2550);
    for(i = 0; i < 12; i++)
    {
        printf("%lc", 0x2564);
        for(j = 0; j < 5; j++)
            printf("%lc", 0x2550);
    }
    printf("%lc\n", 0x2557);
    // BUCLE:
    for(i = 0; i < 13; i++)
    {
        // Parejas:
        printf("%lc", 0x2551);
        for(j = 0; j < 13; j++)
        {
            c.x = j;
            c.y = i;
            _rangeCoordsToPar(c, par, 1);
            printf(" %s ", par);
            if(j < 12)
                printf("%lc", 0x2502);
        }
        printf("%lc\n", 0x2551);
        // Divisoria horizontal (excepto última fila):
        if(i < 12)
        {
            printf("%lc", 0x255F);
            for(j = 0; j < 5; j++)
                printf("%lc", 0x2500);
            for(j = 0; j < 12; j++)
            {
                printf("%lc", 0x253C);
                for(int k = 0; k < 5; k++)
                    printf("%lc", 0x2500);
            }
            printf("%lc\n", 0x2562);
        }
    }
    // Borde inferior:
    printf("%lc", 0x255A);
    for(i = 0; i < 5; i++)
        printf("%lc", 0x2550);
    for(i = 0; i < 12; i++)
    {
        printf("%lc", 0x2567);
        for(j = 0; j < 5; j++)
            printf("%lc", 0x2550);
    }
    printf("%lc\n", 0x255D);
}

static void _rangePrintSimpleTable()
{
    // Muestra la tabla preflop mediante simples ceros y unos
    for(int y = 0; y < 13; y++)
    {
        for(int x = 0; x < 13; x++)
            printf(" %d", _range[x][y]);
        printf("\n");
    }
}

static void _rangePrintRange()
{
    // Muestra el rango correspondiente a la tabla preflop actual
    printf("RANGE\n");
}

static int _rangeRankToCoord(char c)
{
    // Retorna número de coordenada de la tabla preflop (0-12)
    // a partir de carácter de rank (A, K, Q,...)
    char ranks[] = "AKQJT98765432";
    for(int i = 0; i < 13; i++)
        if(c == ranks[i])
            return i;
    return -1;
}

static struct Coords _rangeParToCoords(const char *par)
{
    // Retorna coordenadas de tabla preflop a partir de string tipo "AKs".
    // Si hay error, la coordenada x retornada será -1.
    struct Coords resul;
    if(strlen(par) == 2)
    {
        resul.x = _rangeRankToCoord(par[0]);
        if(resul.x == -1)
            return resul;
        if(par[0] != par[1])
        {
            resul.x = -1;
            return resul;
        }
        resul.y = resul.x;
        return resul;
    }
    else if(strlen(par) == 3)
    {
        int x, y;
        x = _rangeRankToCoord(par[0]);
        y = _rangeRankToCoord(par[1]);
        if(x == -1 || y == -1)
            resul.x = -1;
        else if(par[2] == 'o')
        {
            resul.x = x;
            resul.y = y;
        }
        else if(par[2] == 's')
        {
            resul.x = y;
            resul.y = x;
        }
        else
            resul.x = -1;
        return resul;    
    }
    resul.x = -1;
    return resul;
}

static void _rangeCoordsToPar(struct Coords c, char *par, _Bool onlyInRange)
{
    // A partir de las coordenadas en la tabla preflop, establece el valor del
    // par correspondiente en 'par', que debe tener 4 caracteres reservados.
    // Si 'onlyInRange' es verdadero, los pares que no pertenezcan al range
    // actual, quedaran en blanco (3 espacios).
    const char ranks[] = "AKQJT98765432";
    if(!_range[c.x][c.y] && onlyInRange)  // no forma parte del range actual
    {
        par[0] = ' ';
        par[1] = ' ';
        par[2] = ' ';
    }
    else if(c.x == c.y)  // pocket pair
    {
        par[0] = ranks[c.x];
        par[1] = ranks[c.y];
        par[2] = ' ';
    }
    else if(c.x > c.y)  // suited
    {
        par[0] = ranks[c.y];
        par[1] = ranks[c.x];
        par[2] = 's';
    }
    else  // offsuit
    {
        par[0] = ranks[c.x];
        par[1] = ranks[c.y];
        par[2] = 'o';
    }
    par[3] = '\x0';
}

