static int _rangeParseNextDelim(const char *buffer, int index)
{
    // Retorna la posición en la que se encuentra el siguiente delimitador
    // (',', '%', '+', '-' o nulo terminal).
    while(buffer[index] != ',' && buffer[index] != '\x0' && 
        buffer[index] != '%' && buffer[index] != '+' && 
        buffer[index] != '-')
        index++;
    return index;
}

static _Bool _rangeParse(const char *rangeStr, _Bool val)
{
    // Función principal del parsing. Retorna 1 si tiene éxito
    // y 0 en caso contrario.
    // Rellena la tabla preflop temporal (_rangeTMP).
    int index = 0, newindex;
    int i = 0;
    int x1, y1, x2, y2, x3, y3, x4, y4;
    char *buffer, backup;
    _Bool parseError = 0;
    // Vamos a copiar todo el string sin espacios, en nuestro buffer (compactado):
    buffer = calloc(strlen(rangeStr) + 1, sizeof(char));
    while(rangeStr[index] != '\x0')
    {
        if(rangeStr[index] != ' ' && rangeStr[index] != '\n' &&
            rangeStr[index] != '\t')
            buffer[i++] = rangeStr[index];
        index++;
    }
    buffer[i] = '\x0';
    // Ya tenemos el buffer listo. Empecemos el parsing:
    index = 0;
    while(buffer[index] != '\x0' && !parseError)
    {
        newindex = _rangeParseNextDelim(buffer, index);
        switch(buffer[newindex])
        {
            case '%': // Número de porcentaje:
                      i = atoi(&buffer[index]);
                      if(i < 0 || i > 100)
                      {
                          parseError = 1;
                          break;
                      }
                      _rangeSetPercent(_rangeTMP, i, val);
                      index = newindex + 1;
                      break;
            case '+': // Strip hacia arriba (es una mano seguida de '+'):
                      buffer[newindex] = '\x0';
                      if(!_rangeParToCoords(&buffer[index], &x1, &y1, &x2, &y2))
                      {
                          parseError = 1;
                          break;
                      }
                      _rangeParsePintaUp(x1, y1, val);
                      if(x2 > -1)
                          _rangeParsePintaUp(x2, y2, val);
                      index = newindex + 1;
                      break;
            case '-': // Strip hacia abajo o delimitado:
                      if(buffer[newindex + 1] == ',' || buffer[newindex + 1] == '\x0')
                      {    // Hacia abajo
                          buffer[newindex] = '\x0';
                          if(!_rangeParToCoords(&buffer[index], &x1, &y1, &x2, &y2))
                          {
                              parseError = 1;
                              break;
                          }
                          _rangeParsePintaDown(x1, y1, val);
                          if(x2 > -1)
                              _rangeParsePintaDown(x2, y2, val);
                          index = newindex + 1;
                      }
                      else
                      {   // Delimitado. A parte de que los pares se especifican con
                          // la carta alta primero, los strips se especifican con el
                          // par más alto primero.
                          // Vamos a por el primer par:
                          buffer[newindex] = '\x0';
                          if(!_rangeParToCoords(&buffer[index], &x1, &y1, &x2, &y2))
                          {
                              parseError = 1;
                              break;
                          }
                          index = newindex + 1;
                          // Ahora deberíamos estar a punto de leer el segundo par:
                          newindex = _rangeParseNextDelim(buffer, index);
                          backup = buffer[newindex];    // para restaurar después
                          buffer[newindex] = '\x0';
                          if(!_rangeParToCoords(&buffer[index], &x3, &y3, &x4, &y4))
                          {
                              parseError = 1;
                              break;
                          }
                          buffer[newindex] = backup;    // restauramos valor anterior
                          if((x2 > -1 && x4 == -1) || (x4 > -1 && x2 == -1))
                          {
                              parseError = 1;
                              break;
                          }
                          if(!_rangeParsePintaLine(x1, y1, x3, y3, val))
                          {
                              parseError = 1;
                              break;
                          }
                          if(x2 > -1)
                              if(!_rangeParsePintaLine(x2, y2, x4, y4, val))
                              {
                                  parseError = 1;
                                  break;
                              }
                          index = newindex;
                      }
                      break;
            default : // Fin de buffer '\x0' o ',' (se trata de una mano solitaria):
                      backup = buffer[newindex];    // para restaurar después
                      buffer[newindex] = '\x0';
                      if(!_rangeParToCoords(&buffer[index], &x1, &y1, &x2, &y2))
                      {
                          parseError = 1;
                          break;
                      }
                      buffer[newindex] = backup;    // restauramos valor anterior
                      _rangeTMP[x1][y1] = val;
                      if(x2 > -1)
                          _rangeTMP[x2][y2] = val;
                      index = newindex;
        }
        if(!parseError && buffer[index] == ',')
            index++;
    }
    free(buffer);
    return !parseError;
}

static _Bool _rangeParsePintaLine(int x0, int y0, int x1, int y1, _Bool val)
{
    // Pinta una línea (horizontal, vertical o diagonal, siempre hacia la
    // derecha y hacia abajo) con el valor especificado en 'val' en la tabla
    // temporal. Retorna 1 si tiene éxito (0 si no).
    int deltax = x1 - x0, deltay = y1 - y0;
    int x, y;
    if(deltax < 0 || deltay < 0)    // siempre ascendente
        return 0;
    if(deltax == 0 && deltay == 0)    // hay que avanzar
        return 0;
    if(deltax != 0 && deltay != 0 && deltax != deltay)    // diagonal perfecta
        return 0;
    if(x0 >= y0 && deltax == 0)    // pair o suited no puede ser vertical
        return 0;
    if(x0 <= y0 && deltay == 0)    // pair o offsuit no puede ser horizontal
        return 0;
    if(deltax > 1)
        deltax = 1;
    if(deltay > 1)
        deltay = 1;
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

static _Bool _rangeParsePintaUp(int x, int y, _Bool val)
{
    // Strip del tipo "AB+".
    _Bool resul;
    if(x == y)    // pocket pair
        resul = _rangeParsePintaLine(0, 0, x, y, val);
    else if(x == y + 1)    // suited connector
        resul = _rangeParsePintaLine(1, 0, x, y, val);
    else if(y == x + 1)    // offsuit connector
        resul = _rangeParsePintaLine(0, 1, x, y, val);
    else if(x > y)   // mano suited
        resul = _rangeParsePintaLine(y + 1, y, x, y, val);
    else    // mano offsuit (y > x)
        resul = _rangeParsePintaLine(x, x + 1, x, y, val);
    return resul;
}

static _Bool _rangeParsePintaDown(int x, int y, _Bool val)
{
    // Strip del tipo "AB-".
    _Bool resul;
    if(x == y)    // pocket pair
        resul = _rangeParsePintaLine(x, y, 12, 12, val);
    else if(x == y + 1)    // suited connector
        resul = _rangeParsePintaLine(x, y, 12, 11, val);
    else if(y == x + 1)    // offsuit connector
        resul = _rangeParsePintaLine(x, y, 11, 12, val);
    else if(x > y)   // mano suited
        resul = _rangeParsePintaLine(x, y, 12, y, val);
    else    // mano offsuit (y > x)
        resul = _rangeParsePintaLine(x, y, x, 12, val);
    return resul;
}

