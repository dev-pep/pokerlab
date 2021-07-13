void _rangeSetPercent(_Bool r[13][13], int n, int val)
{
    // Establece (o borra) la tabla preflop a partir de un porcentaje de combos
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
    int n_combos = n * 1326 / 100;
    int contadorCombos = 0, contadorPares = 0;
    int x = 0, y = 0;    // inicializamos para evitar warnings
    while(contadorCombos < n_combos)
    {
        _rangeParToCoords(pockets[contadorPares++], &x, &y);
        r[x][y] = val;
        if(x == y)
            contadorCombos = contadorCombos + 6;  // pocket pair
        else if(x > y)
            contadorCombos = contadorCombos + 4;  // suited hand
        else
            contadorCombos = contadorCombos + 12;  // offsuit hand
    }
}

static void _rangeClear(_Bool r[13][13])    // resetea tabla preflop
{
    for(int i = 0; i < 13; i++)
        for(int j = 0; j < 13; j++)
            r[i][j] = 0;
}

static void _rangeInvert(_Bool r[13][13])    // invierte tabla preflop
{
    for(int i = 0; i < 13; i++)
        for(int j = 0; j < 13; j++)
            r[i][j] = r[i][j] - 1;  // como es _Bool cambia valor
}

static void _rangeCopy(const _Bool source[13][13], _Bool dest[13][13])
{
    for(int i = 0; i < 13; i++)
        for(int j = 0; j < 13; j++)
            dest[i][j] = source[i][j];
}

static void _rangePrintTable(_Bool r[13][13])
{
    // Muestra la tabla preflop en un formato de tabla
    int i, j;
    char par[4];
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
            if(r[j][i])
            {
                _rangeCoordsToPar(j, i, par);
                printf(" %s ", par);
            }
            else
                printf("     ");
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

static PyObject* _rangeGetTable(_Bool r[13][13])
{
    // Retorna el range especificado en 'r' (hero o villain)
    // en una tupla Python (cede la ownership).
    PyObject *range = PyTuple_New(13);    // new reference
    for(int i = 0; i < 13; i++)
    {
        PyObject *fila = PyTuple_New(13);    // new reference 'fila'
        for(int j = 0; j < 13; j++)
            PyTuple_SetItem(fila, j, PyLong_FromLong(r[j][i]));  // New ref. stolen
        PyTuple_SetItem(range, i, fila);    // ref. a 'fila' stolen
    }
    return range;    // pasamos ownership de 'range' al llamador
}

static int _rangeWriteString(int x0, int y0, int x1, int y1,
    char *buffer, int index)
{
    // Función auxiliar para _rangeGetString(): escribe un fragmento
    // de range en el buffer.
    // Retorna la posición del nuevo índice.
    int delta;
    if(index)    // no es el primer strip, con lo que hay que escribir coma
        buffer[index++] = ',';
    if(x0 == y0)    // se trata de un rango de pocket pairs
        delta = 2;
    else
        delta = 3;
    if(x0 == x1 && y0 == y1)    // no es un strip sino una mano solitaria
    {
        _rangeCoordsToPar(x0, y0, &buffer[index]);
        index = index + delta;
    }
    else    // es un strip
    {
        _rangeCoordsToPar(x0, y0, &buffer[index]);
        index = index + delta;
        buffer[index++] = '-';
        _rangeCoordsToPar(x1, y1, &buffer[index]);
        index = index + delta;
    }
    return index;
}

static PyObject* _rangeGetString(_Bool r[13][13])
{
    // Retorna el range especificado en 'r' (hero o villain)
    // en un string Python (cede la ownership).
    PyObject *range;
    char buffer[500];
    int i, j, index = 0;
    int xini, yini, xfin, yfin;
    _Bool inStrip = 0;
    // Pocket pairs:
    for(i = 0; i < 13; i++)
    {
        if(r[i][i])
        {
            if(inStrip)
            {
                xfin = i;
                yfin = i;
            }
            else
            {
                inStrip = 1;
                xini = i;
                yini = i;
                xfin = i;
                yfin = i;
            }
        }
        else if(inStrip)
        {
            inStrip = 0;
            index = _rangeWriteString(xini, yini, xfin, yfin, buffer, index);
        }
    }
    if(inStrip)
    {
        inStrip = 0;
        index = _rangeWriteString(xini, yini, xfin, yfin, buffer, index);
    }
    // Suited hands
    for(j = 0; j < 13; j++)
    {
        for(i = j + 1; i < 13; i++)
        {
            if(r[i][j])
            {
                if(inStrip)
                {
                    xfin = i;
                    yfin = j;
                }
                else
                {
                    inStrip = 1;
                    xini = i;
                    yini = j;
                    xfin = i;
                    yfin = j;
                }
            }
            else if(inStrip)
            {
                inStrip = 0;
                index = _rangeWriteString(xini, yini, xfin, yfin, buffer, index);
            }
        }
        if(inStrip)
        {
            inStrip = 0;
            index = _rangeWriteString(xini, yini, xfin, yfin, buffer, index);
        }
    }
    // Offsuit hands
    for(j = 0; j < 13; j++)
    {
        for(i = j + 1; i < 13; i++)
        {
            if(r[j][i])
            {
                if(inStrip)
                {
                    xfin = j;
                    yfin = i;
                }
                else
                {
                    inStrip = 1;
                    xini = j;
                    yini = i;
                    xfin = j;
                    yfin = i;
                }
            }
            else if(inStrip)
            {
                inStrip = 0;
                index = _rangeWriteString(xini, yini, xfin, yfin, buffer, index);
            }
        }
        if(inStrip)
        {
            inStrip = 0;
            index = _rangeWriteString(xini, yini, xfin, yfin, buffer, index);
        }
    }
    buffer[index] = '\x0';
    range = PyUnicode_FromString(buffer);  // new reference
    return range;    // pasamos ownership de 'range' al llamador
}

static _Bool _rangeParToCoords(const char *par, int *xptr, int *yptr)
{
    // Coloca en (*xptr, *yptr) las coordenadas en tabla preflop
    // a partir de un string tipo "AKs". Si hay error, retorna 0 (si no 1).
    if(strlen(par) == 2)
    {
        int x;
        if(par[0] != par[1])
            return 0;
        x = _rankToNum(par[0]);
        if(x == -1)
            return 0;
        *xptr = 12 - x;
        *yptr = 12 - x;
    }
    else if(strlen(par) == 3)
    {
        int x, y;
        x = _rankToNum(par[0]);
        y = _rankToNum(par[1]);
        if(x == -1 || y == -1)
            return 0;
        if(par[2] == 'o')
        {
            *xptr = 12 - x;
            *yptr = 12 - y;
        }
        else if(par[2] == 's')
        {
            *xptr = 12 - y;
            *yptr = 12 - x;
        }
        else
            return 0;    
    }
    else
        return 0;
    return 1;
}

static _Bool _rangeCoordsToPar(int x, int y, char *par)
{
    // A partir de las coordenadas en la tabla preflop, establece el valor del
    // par correspondiente en 'par', que debe tener 4 caracteres reservados.
    // Si es un pocket pair pondrá un espacio de padding a la derecha.
    // Si hay error retorna 0 (si no 1).
    const char ranks[] = "AKQJT98765432";
    if(x < 0 || x > 12 || y < 0 || y > 12)
        return 0;
    if(x == y)  // pocket pair
    {
        par[0] = ranks[x];
        par[1] = ranks[y];
        par[2] = ' ';
    }
    else if(x > y)  // suited
    {
        par[0] = ranks[y];
        par[1] = ranks[x];
        par[2] = 's';
    }
    else  // offsuit
    {
        par[0] = ranks[x];
        par[1] = ranks[y];
        par[2] = 'o';
    }
    par[3] = '\x0';
    return 1;
}

