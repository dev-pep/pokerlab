#!/usr/bin/env python3

# genera todas las estadísticas (win, lose, tie) de enfrentamientos entre
# ranges simples preflop (entradas únicas en la tabla preflop son unos
# rangos con 4, 6 o 12 combos: AKs, AKo, AA,...).
# Con 0 argumentos genera toda la tabla. Con 1 argumento genera los datos
# de ese range. Con dos argumentos genera desde el primero hasta el segundo
# (el segundo debe ser posterior al primero en la lista).

import pokerlab
import sys

def rangeVsRange(rangeH, rangeV):
    # suit combinations:
    combosPairs = ('cd', 'ch', 'cs', 'dh', 'ds', 'hs')
    combosSuited = ('cc', 'dd', 'hh', 'ss')
    combosOffsuit = ('cd', 'ch', 'cs', 'dc', 'dh', 'ds', 'hc', 'hd', 'hs', 'sc', 'sd', 'sh')
    win = lose = split = 0
    # Veamos qué combos se deben considerar:
    if rangeH[0] == rangeH[1]:    # pocket pair
        combos = combosPairs
    elif rangeH[2] == 'o':    # offsuit
        combos = combosOffsuit
    else:    # suited
        combos = combosSuited
    # Empecemos el bucle:
    for combo in combos:
        # Generemos las 2 cartas:
        carta1 = rangeH[0] + combo[0]
        carta2 = rangeH[1] + combo[1]
        resul = pokerlab.simHandVsRange(carta1, carta2, rangeV)
        win += resul[0]
        lose += resul[1]
        split += resul[2]
    return win, lose, split

ranges = ('AA ', 'AKs', 'AKo', 'AQs', 'AQo', 'AJs', 'AJo', 'ATs', 'ATo', 'A9s', 'A9o', 'A8s', 'A8o',
          'A7s', 'A7o', 'A6s', 'A6o', 'A5s', 'A5o', 'A4s', 'A4o', 'A3s', 'A3o', 'A2s', 'A2o', 'KK ',
          'KQs', 'KQo', 'KJs', 'KJo', 'KTs', 'KTo', 'K9s', 'K9o', 'K8s', 'K8o', 'K7s', 'K7o', 'K6s',
          'K6o', 'K5s', 'K5o', 'K4s', 'K4o', 'K3s', 'K3o', 'K2s', 'K2o', 'QQ ', 'QJs', 'QJo', 'QTs',
          'QTo', 'Q9s', 'Q9o', 'Q8s', 'Q8o', 'Q7s', 'Q7o', 'Q6s', 'Q6o', 'Q5s', 'Q5o', 'Q4s', 'Q4o',
          'Q3s', 'Q3o', 'Q2s', 'Q2o', 'JJ ', 'JTs', 'JTo', 'J9s', 'J9o', 'J8s', 'J8o', 'J7s', 'J7o',
          'J6s', 'J6o', 'J5s', 'J5o', 'J4s', 'J4o', 'J3s', 'J3o', 'J2s', 'J2o', 'TT ', 'T9s', 'T9o',
          'T8s', 'T8o', 'T7s', 'T7o', 'T6s', 'T6o', 'T5s', 'T5o', 'T4s', 'T4o', 'T3s', 'T3o', 'T2s',
          'T2o', '99 ', '98s', '98o', '97s', '97o', '96s', '96o', '95s', '95o', '94s', '94o', '93s',
          '93o', '92s', '92o', '88 ', '87s', '87o', '86s', '86o', '85s', '85o', '84s', '84o', '83s',
          '83o', '82s', '82o', '77 ', '76s', '76o', '75s', '75o', '74s', '74o', '73s', '73o', '72s',
          '72o', '66 ', '65s', '65o', '64s', '64o', '63s', '63o', '62s', '62o', '55 ', '54s', '54o',
          '53s', '53o', '52s', '52o', '44 ', '43s', '43o', '42s', '42o', '33 ', '32s', '32o', '22 ')

inicial = final = -1
if len(sys.argv) > 3:
    print("Se esperan 0, 1 o 2 argumentos del tipo AKs, JJ, o Q9o con el par inicial y final.\n" +
        "0 argumentos equivale a AA 22. 1 argumento lo contará como inicial y final.\n")
elif len(sys.argv) == 1:
    inicial = 0
    final = 168
else:
    arg1 = sys.argv[1]
    if len(sys.argv) == 3:
        arg2 = sys.argv[2]
    else:
        arg2 = arg1
    for i in range(169):
        if arg1 == ranges[i].strip():
            inicial = i
        if arg2 == ranges[i].strip():
            final = i
if inicial == -1 or final == -1 or final < inicial:
    print("Formato incorrecto.\n")
else:
    if inicial != final:
        filename = f"range_vs_range_{ranges[inicial].strip()}-{ranges[final].strip()}.csv"
    else:
        filename = f"range_vs_range_{ranges[inicial].strip()}.csv"
    f = open(filename, "wt")
    f.write("Hero;Villain;Win;Lose;Tie\n")
    print("Hero;Villain;Win;Lose;Tie")
    f.close()
    for r1 in range(inicial, final + 1):
        for r2 in range(r1, 169):
            resul = rangeVsRange(ranges[r1], ranges[r2])
            f = open(filename, "at")
            f.write(f"{ranges[r1].strip()};{ranges[r2].strip()};{resul[0]};{resul[1]};{resul[2]}\n")
            print(f"{ranges[r1].strip()};{ranges[r2].strip()};{resul[0]};{resul[1]};{resul[2]}")
            if r1 != r2:
                f.write(f"{ranges[r2].strip()};{ranges[r1].strip()};{resul[1]};{resul[0]};{resul[2]}\n")
                print(f"{ranges[r2].strip()};{ranges[r1].strip()};{resul[1]};{resul[0]};{resul[2]}")
            f.close()

