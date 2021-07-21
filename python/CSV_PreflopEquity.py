#!/usr/bin/env python3

import pokerlab

ranks = "AKQJT98765432"

def procesa(pocket, h1, h2):
    win = lose = tie = 0
    for v1 in range(52):
        if v1 == h1 or v1 == h2:
            continue
        for v2 in range(v1+1, 52):
            if v2 == h1 or v2 == h2:
                continue
            for a1 in range(52):
                if a1 == v1 or a1 == v2 or a1 == h1 or a1 == h2:
                    continue
                for a2 in range(a1+1, 52):
                    if a2 == v1 or a2 == v2 or a2 == h1 or a2 == h2:
                        continue
                    for a3 in range(a2+1, 52):
                        if a3 == v1 or a3 == v2 or a3 == h1 or a3 == h2:
                            continue
                        for a4 in range(a3+1, 52):
                            if a4 == v1 or a4 == v2 or a4 == h1 or a4 == h2:
                                continue
                            for a5 in range(a4+1, 52):
                                if a5 == v1 or a5 == v2 or a5 == h1 or a5 == h2:
                                    continue
                                hero = pokerlab.valorMano(a1, a2, a3, a4, a5, h1, h2)
                                vill = pokerlab.valorMano(a1, a2, a3, a4, a5, v1, v2)
                                if hero > vill:
                                    win += 1
                                elif vill > hero:
                                    lose += 1
                                else:
                                    tie += 1
    print(pocket + ";" + str(win) + ";" + str(lose) + ";" + str(tie))

for c1 in range(13):
    for c2 in range(13):
        if c1 == c2:
            pocket = ranks[c1] + ranks[c2]
            n1 = 13 - c1
            if n1 == 13:
                n1 = 0
            n2 = 13 - c2
            if n2 == 13:
                n2 = 0
            n2 += 13
            procesa(pocket, n1, n2)
        elif c2 < c1:
            continue
        else:
            n1 = 13 - c1
            if n1 == 13:
                n1 = 0
            n2 = 13 - c2
            if n2 == 13:
                n2 = 0
            pocket = ranks[c1] + ranks[c2] + "s"
            procesa(pocket, n1, n2)
            n2 += 13
            pocket = ranks[c1] + ranks[c2] + "o"
            procesa(pocket, n1, n2)

