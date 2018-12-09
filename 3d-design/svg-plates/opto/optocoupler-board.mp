beginfig(1);

numeric resistorH; resistorH = 9mm;
numeric pinR; pinR = 0.8mm;

pair plateDim; plateDim = (29.4cm, 7cm);
path plate; plate := unitsquare xyscaled plateDim;

def drawFixingHole(expr loc) =
    numeric fixingHoleR; fixingHoleR := 3mm;

    draw fullcircle scaled fixingHoleR shifted loc;
enddef;
def drawFixingHoles =
    pair fixingHoleDist; fixingHoleDist := (10mm, 5mm);

    numeric dx, dy;
    dx := xpart fixingHoleDist;
    dy := ypart fixingHoleDist;
    drawFixingHole(llcorner plate shifted ( dx,  dy));
    drawFixingHole(lrcorner plate shifted (-dx,  dy));
    drawFixingHole(urcorner plate shifted (-dx, -dy));
    drawFixingHole(ulcorner plate shifted ( dx, -dy));
enddef;

def drawResistorV(expr loc) =
    draw fullcircle scaled pinR shifted loc shifted (0,-0.5resistorH);
    draw fullcircle scaled pinR shifted loc shifted (0, 0.5resistorH);
enddef;
def drawOptoCoupler(expr locSet, locUnit, shouldDraw, name, num) =
    if shouldDraw:
        pair loc; loc := locSet shifted locUnit;
        pair optoCouplerDim; optoCouplerDim = (6mm, 13mm);
        pair optoCouplerPinDim; optoCouplerPinDim = (2.54mm, 10mm);
        path optoCouplerPinSquare;
        path optoCouplerSquare;

        pair resistorLoc; resistorLoc := (xpart loc, ypart locSet + 42.5mm - (.5 * ypart optoCouplerDim) + .5resistorH);

        optoCouplerPinSquare := fullsquare xyscaled optoCouplerPinDim shifted loc;
        optoCouplerSquare    := fullsquare xyscaled optoCouplerDim    shifted loc;
        
        draw textext(name & decimal(num)) scaled 0.8 shifted .5[llcorner optoCouplerSquare, urcorner optoCouplerSquare] withcolor blue;

        draw optoCouplerSquare;

        draw fullcircle scaled pinR shifted (llcorner optoCouplerPinSquare);
        draw fullcircle scaled pinR shifted (lrcorner optoCouplerPinSquare);
        draw fullcircle scaled pinR shifted (ulcorner optoCouplerPinSquare);
        draw fullcircle scaled pinR shifted (urcorner optoCouplerPinSquare);

        drawResistorV(resistorLoc);
    fi
enddef;
def drawTransistor(expr loc) =
    pair locB, locE, locC;
    locB = loc shifted ( 0mm,0mm);
    locE = loc shifted (-3mm,0mm);
    locC = loc shifted ( 0mm,3mm);
    draw fullcircle scaled pinR shifted locB;
    draw fullcircle scaled pinR shifted locE;
    draw fullcircle scaled pinR shifted locC;
enddef;
vardef optoCouplerSetLoc(expr i) =
    (22.5mm + i*2cm, 15mm + 0.5*13mm)
enddef;
def drawOptoCouplerSet(expr loc, a, b, c, d, tr, rr, li) =
    pair distA; distA = (16mm,28mm);
    pair distB; distB = (0mm,14mm);
    pair distC; distC = (12mm,7mm);
    pair distD; distD = (4mm,0mm);
    pair distE; distE = (8mm,35mm);

    pair distT; distT = (10mm, 30mm);
    pair distR; distR = (10mm, 22mm);
    pair distS; distS = (11mm, -8mm);

    boolean e; e = false;

    drawOptoCoupler(loc, distA, a, "A", li);
    drawOptoCoupler(loc, distB, b, "B", li);
    drawOptoCoupler(loc, distC, c, "C", li);
    drawOptoCoupler(loc, distD, d, "D", li - 1);
    drawOptoCoupler(loc, distE, false, "E", li - 2);

    %pair resistiorLoc; resistiorLoc := loc shifted distR;
    if tr:
        drawResistorV(loc shifted distR);
        drawTransistor(loc shifted distT);
    fi
    if rr:
        drawResistorV(loc shifted distS);
    fi

    numeric transistorD;

enddef;

def drawOptoCouplers =
    boolean t, F; t = true; F = false;
%    drawOptoCouplerSet(optoCouplerSetLoc( 0), t,t,t,F,F,t, 0);
%    drawOptoCouplerSet(optoCouplerSetLoc( 1), t,t,t,F,t,t, 1);

%    drawOptoCouplerSet(optoCouplerSetLoc( 2), t,t,t,t,t,t, 2);
%    drawOptoCouplerSet(optoCouplerSetLoc( 3), t,t,t,t,t,t, 3);
%    drawOptoCouplerSet(optoCouplerSetLoc( 4), t,t,t,t,t,t, 4);
%    drawOptoCouplerSet(optoCouplerSetLoc( 5), t,t,t,t,t,t, 5);
%    drawOptoCouplerSet(optoCouplerSetLoc( 6), t,t,t,t,t,t, 6);
%    drawOptoCouplerSet(optoCouplerSetLoc( 7), t,t,t,t,t,t, 7);
%    drawOptoCouplerSet(optoCouplerSetLoc( 8), t,t,t,t,t,t, 8);

%    drawOptoCouplerSet(optoCouplerSetLoc( 9), t,t,t,F,t,t, 9);
%    drawOptoCouplerSet(optoCouplerSetLoc(10), t,t,t,F,t,t, 10);
%    drawOptoCouplerSet(optoCouplerSetLoc(11), t,t,t,F,t,t, 11);
%    drawOptoCouplerSet(optoCouplerSetLoc(12), F,t,F,F,F,F, 12);

    for i =  0 upto  0: drawOptoCouplerSet(optoCouplerSetLoc(i), t,t,t,F,F,t, i); endfor
    for i =  1 upto  1: drawOptoCouplerSet(optoCouplerSetLoc(i), t,t,t,F,t,t, i); endfor
    for i =  2 upto  8: drawOptoCouplerSet(optoCouplerSetLoc(i), t,t,t,t,t,t, i); endfor
    for i =  9 upto 11: drawOptoCouplerSet(optoCouplerSetLoc(i), t,t,t,F,t,t, i); endfor
    for i = 12 upto 12: drawOptoCouplerSet(optoCouplerSetLoc(i), F,t,F,F,F,F, i); endfor
enddef;

draw plate;
drawFixingHoles;
drawOptoCouplers;

endfig;
end.
