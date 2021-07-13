// Características de las funciones de parsing (excepto _rangeParse):
//     - al entrar, el índice no apunta a un espacio,
//     - retornan el nuevo índice dentro del string,
//     - en caso de error retornan -1,
//     - debe hacer un skip de espacios antes de retornar, y
//     - escriben en la tabla temporal (_rangeTMP).

static int _rangeParsePAIR(const char *rangeStr, int index, _Bool val)
{
    // Parsing rango de pocket pairs.
    char ppIni[3], ppFin[3];
    int xini, yini, xfin, yfin;
    ppIni[0] = rangeStr[index++];
    ppIni[1] = rangeStr[index++];
    ppIni[2] = '\x0';
    index = _rangeParseSkip(rangeStr, index);
    if(rangeStr[index] == '+')
    {
        index++;
        ppFin[0] = 'A';
        ppFin[1] = 'A';
        ppFin[2] = '\x0';
    }
    else if(rangeStr[index] == '-')
    {
        index++;
        index = _rangeParseSkip(rangeStr, index);
        if(rangeStr[index] != ',' && rangeStr[index] != '\x0')
        {
            ppFin[0] = rangeStr[index++];
            ppFin[1] = rangeStr[index++];
            if(_rankToNum(ppFin[0]) < 0 || _rankToNum(ppFin[1]) < 0 ||
                ppFin[0] != ppFin[1] || _rankToNum(ppFin[0]) >= _rankToNum(ppIni[0]))
                return -1;
            ppFin[2] = '\x0';
        }
        else
        {
            ppFin[0] = '2';
            ppFin[1] = '2';
            ppFin[2] = '\x0';
        }
    }
    else if(rangeStr[index] == ',' || rangeStr[index] == '\x0')
    {
        ppFin[0] = ppIni[0];
        ppFin[1] = ppIni[1];
        ppFin[2] = '\x0';
    }
    _rangeParToCoords(ppIni, &xini, &yini);
    _rangeParToCoords(ppFin, &xfin, &yfin);
    _rangeParsePinta(xini, yini, xfin, yfin, val);
    index = _rangeParseSkip(rangeStr, index);
    return index;
}

static int _rangeParseCONN(const char *rangeStr, int index, _Bool val)
{
    // Parsing rango de connectors.
    char connIni[4], connFin[4];
    int xini, yini, xfin, yfin;
    connIni[0] = rangeStr[index++];
    connIni[1] = rangeStr[index++];
    if(rangeStr[index] == 'o' || rangeStr[index] == 's')
    {
        connIni[2] = rangeStr[index++];
        connIni[3] = '\x0';
    }
    else
    {
        connIni[2] = '\x0';
        connIni[3] = '\x0';
    }
    index = _rangeParseSkip(rangeStr, index);
    if(rangeStr[index] == '+')
    {
        index++;
        connFin[0] = 'A';
        connFin[1] = 'K';
        connFin[2] = connIni[2];
        connFin[3] = '\x0';
    }
    else if(rangeStr[index] == '-')
    {
        index++;
        index = _rangeParseSkip(rangeStr, index);
        if(rangeStr[index] != ',' && rangeStr[index] != '\x0')
        {
            connFin[0] = rangeStr[index++];
            connFin[1] = rangeStr[index++];
            if(rangeStr[index] == 'o' || rangeStr[index] == 's')
            {
                connFin[2] = rangeStr[index++];
                connFin[3] = '\x0';
            }
            else
            {
                connFin[2] = '\x0';
                connFin[3] = '\x0';
            }
            if(_rankToNum(connFin[0]) < 0 || _rankToNum(connFin[1]) < 0 ||
                _rankToNum(connFin[0]) != _rankToNum(connFin[1]) + 1 || 
                _rankToNum(connFin[0]) >= _rankToNum(connIni[0]) ||
                connFin[2] != connIni[2])
                return -1;
        }
        else
        {
            connFin[0] = '3';
            connFin[1] = '2';
            connFin[2] = connIni[2];
            connFin[3] = '\x0';
        }
    }
    else if(rangeStr[index] == ',' || rangeStr[index] == '\x0')
    {
        connFin[0] = connIni[0];
        connFin[1] = connIni[1];
        connFin[2] = connIni[2];
        connFin[3] = connIni[3];
    }
    if(connIni[2] == '\x0')    // o + s
    {
        connIni[2] = 'o';
        connFin[2] = 'o';
        _rangeParToCoords(connIni, &xini, &yini);
        _rangeParToCoords(connFin, &xfin, &yfin);
        _rangeParsePinta(xini, yini, xfin, yfin, val);
        connIni[2] = 's';
        connFin[2] = 's';
        _rangeParToCoords(connIni, &xini, &yini);
        _rangeParToCoords(connFin, &xfin, &yfin);
        _rangeParsePinta(xini, yini, xfin, yfin, val);
    }
    else
    {
        _rangeParToCoords(connIni, &xini, &yini);
        _rangeParToCoords(connFin, &xfin, &yfin);
        _rangeParsePinta(xini, yini, xfin, yfin, val);
    }
    index = _rangeParseSkip(rangeStr, index);
    return index;
}

static int _rangeParseOTHER(const char *rangeStr, int index, _Bool val)
{
    // Parsing rango de manos no conector ni pareja.
    char handIni[4], handFin[4];
    int xini, yini, xfin, yfin;
    handIni[0] = rangeStr[index++];
    handIni[1] = rangeStr[index++];
    if(rangeStr[index] == 'o' || rangeStr[index] == 's')
    {
        handIni[2] = rangeStr[index++];
        handIni[3] = '\x0';
    }
    else
    {
        handIni[2] = '\x0';
        handIni[3] = '\x0';
    }
    index = _rangeParseSkip(rangeStr, index);
    if(rangeStr[index] == '+')
    {
        index++;
        handFin[0] = handIni[0];
        handFin[1] = _numToRank(_rankToNum(handIni[0]) - 1);
        handFin[2] = handIni[2];
        handFin[3] = '\x0';
    }
    else if(rangeStr[index] == '-')
    {
        index++;
        index = _rangeParseSkip(rangeStr, index);
        if(rangeStr[index] != ',' && rangeStr[index] != '\x0')
        {
            handFin[0] = rangeStr[index++];
            handFin[1] = rangeStr[index++];
            if(rangeStr[index] == 'o' || rangeStr[index] == 's')
            {
                handFin[2] = rangeStr[index++];
                handFin[3] = '\x0';
            }
            else
            {
                handFin[2] = '\x0';
                handFin[3] = '\x0';
            }
            if(_rankToNum(handFin[0]) < 0 || _rankToNum(handFin[1]) < 0 ||
                _rankToNum(handFin[0]) <= _rankToNum(handFin[1]) || 
                _rankToNum(handFin[0]) != _rankToNum(handIni[0]) ||
                handFin[2] != handIni[2])
            {
                printf("ini %d %d - fin %d %d - quals %d %d\n",
                       _rankToNum(handIni[0]), _rankToNum(handIni[1]),
                       _rankToNum(handFin[0]), _rankToNum(handFin[1]),
                       (int) handIni[2], (int) handFin[2]);
                return -1;
            }
        }
        else
        {
            handFin[0] = handIni[0];
            handFin[1] = '2';
            handFin[2] = handIni[2];
            handFin[3] = '\x0';
        }
    }
    else if(rangeStr[index] == ',' || rangeStr[index] == '\x0')
    {
        handFin[0] = handIni[0];
        handFin[1] = handIni[1];
        handFin[2] = handIni[2];
        handFin[3] = handIni[3];
    }
    if(handIni[2] == '\x0')    // o + s
    {
        handIni[2] = 'o';
        handFin[2] = 'o';
        _rangeParToCoords(handIni, &xini, &yini);
        _rangeParToCoords(handFin, &xfin, &yfin);
        _rangeParsePinta(xini, yini, xfin, yfin, val);
        handIni[2] = 's';
        handFin[2] = 's';
        _rangeParToCoords(handIni, &xini, &yini);
        _rangeParToCoords(handFin, &xfin, &yfin);
        _rangeParsePinta(xini, yini, xfin, yfin, val);
    }
    else
    {
        _rangeParToCoords(handIni, &xini, &yini);
        _rangeParToCoords(handFin, &xfin, &yfin);
        _rangeParsePinta(xini, yini, xfin, yfin, val);
    }
    index = _rangeParseSkip(rangeStr, index);
    return index;
}

static int _rangeParsePERCENT(const char *rangeStr, int index, _Bool val)
{
    int num, numsize = 0;
    while(rangeStr[index + numsize] >= '0' &&
        rangeStr[index + numsize] <= '9')
        numsize++;
    num = atoi(&rangeStr[index]);
    index = index + numsize;
    index = _rangeParseSkip(rangeStr, index);
    if(rangeStr[index] != '%')
        return -1;
    _rangeSetPercent(_rangeTMP, num, val);
    index++;
    index = _rangeParseSkip(rangeStr, index);
    return index;
}

static int _rangeParseRANGO(const char *rangeStr, int index, _Bool val)
{
    int i;
    // Probemos si es un porcentaje:
    i = _rangeParsePERCENT(rangeStr, index, val);
    if(i > index)    // era un porcentaje
        index = i;
    else             // no era un porcentaje
    {
        int r0 = _rankToNum(rangeStr[index]);
        int r1 = _rankToNum(rangeStr[index + 1]);
        if(r0 == -1 || r1 == -1)
            return -1;
        if(r1 == r0)    // pocket pair
            index = _rangeParsePAIR(rangeStr, index, val);
        else if(r1 == r0 - 1)    // connector
            index = _rangeParseCONN(rangeStr, index, val);
        else if(r1 < r0 -1)    // other hand
            index = _rangeParseOTHER(rangeStr, index, val);
        else
            index = -1;
        if(index == -1)
            return -1;
    }
    index = _rangeParseSkip(rangeStr, index); 
    return index;
}

static _Bool _rangeParse(const char *rangeStr, _Bool val)
{
    // Función directora del parsing. Retorna 1 si tiene éxito
    // y 0 en caso contrario.
    int index = _rangeParseSkip(rangeStr, 0);
    while(rangeStr[index] != '\x0')
    {
        index = _rangeParseRANGO(rangeStr, index, val);
        if(index < 0)
            return 0;
        if(rangeStr[index] == ',')
            index++;
        index = _rangeParseSkip(rangeStr, index);
    }
    return 1;
}

static int _rangeParseSkip(const char *rangeStr, int index)
{
    // Adelanta (retorna) el índice de parsing si hay espacio en blanco.
    while(rangeStr[index] == ' ' || rangeStr[index] == '\n' ||
        rangeStr[index] == '\t')
        index++;
    return index;
}

static _Bool _rangeParsePinta(int x0, int y0, int x1, int y1, _Bool val)
{
    // Pinta una línea (horizontal, vertical o diagonal descendente)
    // con el valor especificado en 'val' en la tabla temporal.
    // Retorna 1 si tiene éxito (0 si no).
    int deltax = x1 - x0, deltay = y1 - y0;
    int x, y;
    if(deltax < 0)
        deltax = -1;
    else if(deltax > 0)
        deltax = 1;
    if(deltay < 0)
        deltay = -1;
    else if(deltay > 0)
        deltay = 1;
    if(deltax != 0 && deltay != 0 && deltax != deltay)
        return 0;
    x = x0;
    y = y0;
    while(x != x1 || y != y1)
    {
        _rangeTMP[x][y] = val;
        x = x + deltax;
        y = y + deltay;
    }
    _rangeTMP[x][y] = val;
    return 1;
}

